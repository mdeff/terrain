
#include "terrain.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "terrain_vshader.h"
#include "terrain_fshader.h"


Terrain::Terrain(unsigned int windowWidth, unsigned int windowHeight) :
    RenderingContext(windowWidth, windowHeight) {
}


GLuint Terrain::init(Vertices* vertices, GLuint heightMapTexID, GLuint shadowMapTexID) {

    /// Common initialization.
    /// Render to FBO by default.
    preinit(vertices, terrain_vshader, terrain_fshader, NULL, "vertexPosition2DWorld");

    /// Allow to manually set a clip distance in the vertex shader (used to cut
    /// under water level geometry when rendering to texture). This could have
    /// been used instead of the clip uniform. But some drivers ignore it.
    glEnable(GL_CLIP_DISTANCE0);

    /// Bind the heightmap and shadowmap to textures 0 and 1.
    set_texture(0, heightMapTexID, "heightMapTex", GL_TEXTURE_2D);
    set_texture(1, shadowMapTexID, "shadowMapTex", GL_TEXTURE_2D);

    /// Load material textures and bind them to textures 2 - 7.
    set_texture(2, -1, "sandTex", GL_TEXTURE_2D);
    load_texture("../../textures/sand.tga");
    set_texture(3, -1, "iceMoutainTex", GL_TEXTURE_2D);
    load_texture("../../textures/dordona_range.tga");
    set_texture(4, -1, "treeTex", GL_TEXTURE_2D);
    load_texture("../../textures/Mossy_Rock.tga");
    set_texture(5, -1, "stoneTex", GL_TEXTURE_2D);
    load_texture("../../textures/Fault_Zone.tga");
    set_texture(6, -1, "underWaterTex", GL_TEXTURE_2D);
    load_texture("../../textures/under_water.tga");
    set_texture(7, -1, "snowTex", GL_TEXTURE_2D);
	load_texture("../../textures/snow.tga");
  
    /// Define light properties and pass them to the shaders.
    vec3 Ia(1.0f, 1.0f, 1.0f);
    vec3 Id(1.0f, 1.0f, 1.0f);
    vec3 Is(1.0f, 1.0f, 1.0f);
    GLuint _IaID = glGetUniformLocation(_programID, "Ia");
    GLuint _IdID = glGetUniformLocation(_programID, "Id");
    GLuint _IsID = glGetUniformLocation(_programID, "Is");
    glUniform3fv( _IaID, 1, Ia.data());
    glUniform3fv( _IdID, 1, Id.data());
    glUniform3fv( _IsID, 1, Is.data());

    /// Set uniform IDs.
    _projectionID = glGetUniformLocation(_programID, "projection");
    _viewID = glGetUniformLocation(_programID, "view");
    _lightViewProjectionID = glGetUniformLocation(_programID, "lightViewProjection");
    _lightPositionWorldID = glGetUniformLocation(_programID, "lightPositionWorld");
    _clipID = glGetUniformLocation(_programID, "clip");

	return 1;
}


void Terrain::draw(const mat4& projection, const mat4 views[],
                   const mat4& lightViewProjection, const vec3& lightPositionWorld) const {

    /// Common drawing. 
    predraw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv(_lightViewProjectionID, 1, GL_FALSE, lightViewProjection.data());
    glUniform3fv(_lightPositionWorldID, 1, lightPositionWorld.data());

    /// Render from camera point of view to 'normal' FBOs.
    glUniform1f(_clipID, 0.0);
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, views[0].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["controllerView"]);
    _vertices->draw();
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, views[1].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["cameraView"]);
    _vertices->draw();

    /// Flip the terrain by multiplying the Z coordinate by -1 in world space.
    mat4 flip = mat4::Identity();
    flip(2,2) = -1.0f;
    mat4 viewFlip[] = {views[0]*flip, views[1]*flip};

    /// Render from flipped camera point of view to 'reflection' FBOs.
    glUniform1f(_clipID, 1.0);
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, viewFlip[0].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["controllerViewReflected"]);
    _vertices->draw();
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, viewFlip[1].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["cameraViewReflected"]);
    _vertices->draw();

}

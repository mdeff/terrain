
#include "water.h"
#include "vertices.h"

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "water_vshader.h"
#include "water_fshader.h"


Water::Water(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


void Water::init(Vertices* vertices, GLuint flippedTerrainTexID) {

    /// Common initialization.
//    RenderingContext::init(vertices, particles_control_vshader, water_fshader, NULL, "vertexPosition2DWorld", 0);
    preinit(vertices, water_vshader, water_fshader, NULL, "vertexPosition2DWorld");

    //bind the reflection tex to texture 0
    set_texture(0, flippedTerrainTexID, "flippedTerrainTex", GL_TEXTURE_2D);

//    /* Load texture for water surface */
//    set_texture(1, -1, "waterNormalMap", GL_TEXTURE_2D);
//    load_texture("../../textures/water_normal_map_2.tga");

//    set_texture(2, -1, "riverSurfaceMap", GL_TEXTURE_2D);
//    load_texture("../../textures/water_2.tga");


    /// Define light properties and pass them to the shaders.
//    vec3 Ia(1.0f, 1.0f, 1.0f);
//    vec3 Id(1.0f, 1.0f, 1.0f);
//    vec3 Is(1.0f, 1.0f, 1.0f);
//    GLuint _IaID = glGetUniformLocation(_programID, "Ia");
//    GLuint _IdID = glGetUniformLocation(_programID, "Id");
//    GLuint _IsID = glGetUniformLocation(_programID, "Is");
//    glUniform3fv( _IaID, 1, Ia.data());
//    glUniform3fv( _IdID, 1, Id.data());
//    glUniform3fv( _IsID, 1, Is.data());

    /// Set uniform IDs.
    _viewID = glGetUniformLocation(_programID, "view");
    _projectionID = glGetUniformLocation(_programID, "projection");
    _lightViewProjectionID = glGetUniformLocation(_programID, "lightViewProjection");
    _lightPositionWorldID = glGetUniformLocation(_programID, "lightPositionWorld");
    _timeID = glGetUniformLocation(_programID, "time");

}


void Water::draw(const mat4& projection, const mat4 views[],
                 const mat4& lightViewProjection, const vec3& lightPositionWorld) const {

    /// Common drawing.
    predraw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv(_lightViewProjectionID, 1, GL_FALSE, lightViewProjection.data());
    glUniform3fv(_lightPositionWorldID, 1, lightPositionWorld.data());

    /// Time value which animates water
//    static float time = 0;
//    glUniform1f(_timeID, int(time++)%5000);

    /// Blending for water transparency.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /// Render from camera point of view to 'normal' FBOs.
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, views[0].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["controllerView"]);
    _vertices->draw();
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, views[1].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["cameraView"]);
    _vertices->draw();

    /// Disable blending : other primitives are opaque.
    glDisable(GL_BLEND);

}


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


void Water::init(Vertices* vertices, GLuint renderedTexIDs[]) {

    /// Common initialization.
    preinit(vertices, water_vshader, water_fshader, NULL, "vertexPosition2DWorld");

    /// Store the reflection texture IDs.
    _reflectionTexID[0] = renderedTexIDs[2];
    _reflectionTexID[1] = renderedTexIDs[3];

    /// The Sampler uniform always refer to texture index 0.
    /// The binding to texture 0 is however changed.
    GLuint uniformID = glGetUniformLocation(_programID, "reflectionTex");
    glUniform1i(uniformID, 0);

    /// Water normal map.
    set_texture(1, -1, "waterNormalMap", GL_TEXTURE_2D);
    load_texture("../../textures/water_normal_map_2.tga");

    /// Set uniform IDs.
    _viewID = glGetUniformLocation(_programID, "view");
    _projectionID = glGetUniformLocation(_programID, "projection");
    _lightViewProjectionID = glGetUniformLocation(_programID, "lightViewProjection");
    _lightPositionWorldID = glGetUniformLocation(_programID, "lightPositionWorld");
    _deltaTID = glGetUniformLocation(_programID, "deltaT");

}


void Water::draw(const mat4& projection, const mat4 views[],
                 const mat4& lightViewProjection, const vec3& lightPositionWorld, float deltaT) const {

    /// Common drawing.
    predraw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv(_lightViewProjectionID, 1, GL_FALSE, lightViewProjection.data());
    glUniform3fv(_lightPositionWorldID, 1, lightPositionWorld.data());
    glUniform1f(_deltaTID, deltaT);

    /// Blending for water transparency.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);

    /// Render from camera point of view to 'normal' FBOs.
    glBindTexture(GL_TEXTURE_2D, _reflectionTexID[0]);
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, views[0].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["controllerView"]);
    _vertices->draw();
    glBindTexture(GL_TEXTURE_2D, _reflectionTexID[1]);
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, views[1].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["cameraView"]);
    _vertices->draw();

    /// Disable blending : other primitives are opaque.
    glDisable(GL_BLEND);

}

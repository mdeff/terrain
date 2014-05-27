
#include "camera_path_controls.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "camera_path_controls_vshader.h"
#include "rendering_simple_fshader.h"
#include "camera_path_controls_gshader.h"
    

CameraPathControls::CameraPathControls(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


void CameraPathControls::init(Vertices* vertices) {

    /// Common initialization.
    preinit(vertices, camera_path_controls_vshader, rendering_simple_fshader, camera_path_controls_gshader, "vertexPosition3DWorld");

    /// Set uniform IDs.
    _projectionID = glGetUniformLocation(_programID, "projection");
    _viewID = glGetUniformLocation(_programID, "view");
    _lightPositionWorldID = glGetUniformLocation(_programID, "lightPositionWorld");
    _selectedControlPointID = glGetUniformLocation(_programID, "selectedControlPoint");
    _rotationMatrixID = glGetUniformLocation(_programID, "rotationMatrix");

}


void CameraPathControls::draw(const mat4& projection, const mat4 views[],
                                   const vec3& lightPositionWorld,
                                   const int& selectedControlPoint,
                                   float deltaT) const {

    /// Common drawing.
    predraw();
	
    /// Update the model matrix.
    static float angle = 1.0f;
    angle += deltaT * 1.0f;
    mat4 rotationMatrix = mat4::Identity();
    rotationMatrix(0,0) = +std::cos(angle);
    rotationMatrix(0,1) = -std::sin(angle);
    rotationMatrix(1,0) = +std::sin(angle);
    rotationMatrix(1,1) = +std::cos(angle);

    /// Update the content of the uniforms.
    glUniformMatrix4fv( _projectionID, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv( _viewID, 1, GL_FALSE, views[0].data());
    glUniform3fv(_lightPositionWorldID, 1, lightPositionWorld.data());
    glUniform1i( _selectedControlPointID, selectedControlPoint);
    glUniformMatrix4fv(_rotationMatrixID, 1, GL_FALSE, rotationMatrix.data());

    /// Render from camera point of view to 'normal' FBOs.
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["controllerView"]);
    _vertices->draw();

}

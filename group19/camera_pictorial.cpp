
#include "camera_pictorial.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "camera_pictorial_vshader.h"
#include "camera_pictorial_fshader.h"


CameraPictorial::CameraPictorial(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


void CameraPictorial::init(Vertices* vertices) {

    /// Common initialization.
    RenderingContext::init(vertices, camera_pictorial_vshader, camera_pictorial_fshader, "vertexPosition3DModel", 0);

    /// Set uniform IDs.
    _projectionID = glGetUniformLocation(_programID, "projection");
    _viewID = glGetUniformLocation(_programID, "view");
    _modelID = glGetUniformLocation(_programID, "model");

}


void CameraPictorial::draw(const mat4& projection, const mat4& view, const mat4& model) const {

    /// Common drawing. 
    RenderingContext::draw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, view.data());
    glUniformMatrix4fv(_modelID, 1, GL_FALSE, model.data());

    /// Do not clear the default framebuffer (screen) : done by Terrain.
    /// Otherwise already drawn pixels will be cleared.

    /// Render the terrain from camera point of view to default framebuffer.
    _vertices->draw();

}

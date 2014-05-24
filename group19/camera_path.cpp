
#include "camera_path.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "camera_path_vshader.h"
#include "camera_path_fshader.h"


CameraPath::CameraPath(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


void CameraPath::init(Vertices* vertices) {

    /// Common initialization.
    RenderingContext::init(vertices, camera_path_vshader, camera_path_fshader, "vertexPosition3DModel", 0);

    /// Set uniform IDs.
    _modelviewID = glGetUniformLocation(_programID, "modelview");
    _projectionID = glGetUniformLocation(_programID, "projection");

}


void CameraPath::draw(const mat4& projection, const mat4& modelview) const {

    /// Common drawing. 
    RenderingContext::draw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_modelviewID, 1, GL_FALSE, modelview.data());
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());

    /// Do not clear the default framebuffer (screen) : done by Terrain.
    /// Otherwise already drawn pixels will be cleared.

    /// Render the terrain from camera point of view to default framebuffer.
    _vertices->draw();

}


#include "camera_path_control_points.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "camera_path_control_points_vshader.h"
#include "rendering_simple_fshader.h"
#include "camera_path_control_points_gshader.h"
    

CameraPathControlPoints::CameraPathControlPoints(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


void CameraPathControlPoints::init(Vertices* vertices) {

    /// Common initialization.
    RenderingContext::init(vertices, camera_path_control_points_vshader, rendering_simple_fshader, camera_path_control_points_gshader, "vertexPosition3DWorld", 0);

    /// Set uniform IDs.
    _projectionID = glGetUniformLocation(_programID, "projection");
    _viewID = glGetUniformLocation(_programID, "view");
    _selectedControlPointID = glGetUniformLocation(_programID, "selectedControlPoint");

}


void CameraPathControlPoints::draw(const mat4& projection, const mat4& view, const int& selectedControlPoint) const {

    /// Common drawing.
    RenderingContext::draw();
	
    /// Update the content of the uniforms.
    glUniformMatrix4fv( _projectionID, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv( _viewID, 1, GL_FALSE, view.data());
    glUniform1i( _selectedControlPointID, selectedControlPoint);

    /// Do not clear the default framebuffer (screen) : done by Terrain.
    /// Otherwise already drawn pixels will be cleared.

    /// Render to default framebuffer.
    _vertices->draw();

}

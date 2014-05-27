#include "duck.h"
#include "vertices.h"


#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "duck_vshader.h"
#include "duck_fshader.h"

RenderedDuck::RenderedDuck(unsigned int width, unsigned int height):
	RenderingContext(width, height) {

}

void RenderedDuck::init(Vertices* vertices)
{
	/// Common initialization.
    preinit(vertices, duck_vshader, duck_fshader, NULL, "vertexPosition3DWorld", 0);

    /// Set uniform IDs.
    _viewID = glGetUniformLocation(_programID, "view");
    _projectionID = glGetUniformLocation(_programID, "projection");
}


void RenderedDuck::draw(const mat4& projection, const mat4& view)
{	
	 /// Common drawing.
    predraw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv( _projectionID, 1, GL_FALSE, projection.data());

    /// Do not clear the framebuffers : done by Terrain.
    /// Otherwise already drawn pixels will be cleared.

    /// Flip the terrain by multiplying the Z coordinate by -1 in world space.
    //mat4 flip = mat4::Identity();
    //flip(2,2) = -1.0f;
    //mat4 viewFlip = view * flip;

    /// Render the terrain from camera point of view to the reflection FBO.
 /*   glUniformMatrix4fv(_viewID, 1, GL_FALSE, viewFlip.data());
    _vertices->draw();*/

    /// Render the skybox from camera point of view to default framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, view.data());
    _vertices->draw();

}

#include "screen_display.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "passthrough_vshader.h"
#include "screen_display_fshader.h"


ScreenDisplay::ScreenDisplay(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


void ScreenDisplay::init(Vertices* vertices, GLuint framebufferIDs[]) {

    /// Common initialization.
    preinit(vertices, passthrough_vshader, screen_display_fshader, NULL, NULL, 0);

    _framebufferIDs.push_back(framebufferIDs[0]);

    /// Bind the control and camera views to textures 0 and 1.
//    set_texture(0, renderedTexIDs[0], "controllerViewTex", GL_TEXTURE_2D);
//    set_texture(1, renderedTexIDs[1], "cameraViewTex", GL_TEXTURE_2D);


    /// Set uniform IDs.

}


void ScreenDisplay::draw() {

    /// Common drawing. 
    predraw();

    /// Update the content of the uniforms.


    /// Render to real screen.
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    _vertices->draw();

    /// Resolve the multi-sampled renderbuffer.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);   // Make sure no FBO is set as the draw framebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _framebufferIDs[0]); // Make sure your multisampled FBO is the read framebuffer
    glDrawBuffer(GL_BACK);                       // Set the back buffer as the draw buffer
    glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _framebufferIDs[0]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

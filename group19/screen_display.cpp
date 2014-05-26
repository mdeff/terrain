
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


void ScreenDisplay::init(Vertices* vertices, GLuint renderedTexIDs[]) {

    /// Common initialization.
    preinit(vertices, passthrough_vshader, screen_display_fshader, NULL, NULL, 0);

    /// Bind the control and camera views to textures 0 and 1.
    set_texture(0, renderedTexIDs[0], "controllerViewTex", GL_TEXTURE_2D);
    set_texture(1, renderedTexIDs[1], "cameraViewTex", GL_TEXTURE_2D);


    /// Set uniform IDs.

}


void ScreenDisplay::draw() {

    /// Common drawing. 
    predraw();

    /// Update the content of the uniforms.


    /// Render to real screen.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _vertices->draw();

}

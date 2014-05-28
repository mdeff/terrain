
#include "post_processing.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "passthrough_vshader.h"
#include "post_processing_fshader.h"


PostProcessing::PostProcessing(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


void PostProcessing::init(Vertices* vertices, GLuint renderedTexIDs[]) {

    /// Common initialization.
    preinit(vertices, passthrough_vshader, post_processing_fshader, NULL, NULL);

    /// Bind the control and camera views to textures 0 and 1.
//    set_texture(0, renderedTexIDs[0], "controllerViewTex", GL_TEXTURE_2D);
//    set_texture(1, renderedTexIDs[1], "cameraViewTex", GL_TEXTURE_2D);

    set_texture(0, renderedTexIDs[0], "mainviewTex", GL_TEXTURE_2D_MULTISAMPLE);
    set_texture(1, renderedTexIDs[0], "previewTex", GL_TEXTURE_2D_MULTISAMPLE);

    /// Set uniform IDs.

}


void PostProcessing::draw() {

    /// Common drawing. 
    predraw();

    /// Update the content of the uniforms.


    /// Render to real screen.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    _vertices->draw();
    glEnable(GL_DEPTH_TEST);


    /// Resolve the multi-sampled renderbuffer.
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);   // Make sure no FBO is set as the draw framebuffer
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferIDs["controllerView"]); // Make sure your multisampled FBO is the read framebuffer
//    glDrawBuffer(GL_BACK);                       // Set the back buffer as the draw buffer
//    glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);



    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferIDs["controllerView"]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

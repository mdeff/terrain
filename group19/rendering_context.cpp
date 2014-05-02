
#include "rendering_context.h"

void RenderingContext::init(const char* vshader, const char* fshader) {

    // Vertex array.
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    // Compile and install the rendering shaders.
    _programID = opengp::compile_shaders(vshader, fshader, 0, 0, 0);
    if(!_programID)
        exit(EXIT_FAILURE);
    glUseProgram(_programID);

}

void RenderingContext::draw() const {

    //--- Tell which shader we want to use
    glUseProgram(_programID);

    /*
     * Load vertex array
     * A vertex array object holds references to the vertex buffers, the index
     * buffer and the layout specification of the vertex itself. At runtime,
     * you can just glBindVertexArray to recall all of these information.
     */
    glBindVertexArray(_vertexArrayID);

}

void RenderingContext::clean() {

    glDeleteProgram(_programID);
    glDeleteVertexArrays(1, &_vertexArrayID);

}

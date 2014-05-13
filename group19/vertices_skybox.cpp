
#include "vertices_skybox.h"
#include <GL/glew.h>


const float size = 5.0f;
const float vertices[] = {
    -size,-size,-size, // triangle 1 : begin
    -size,-size, size,
    -size, size, size, // triangle 1 : end
     size, size,-size, // triangle 2 : begin
    -size,-size,-size,
    -size, size,-size, // triangle 2 : end
     size,-size, size,
    -size,-size,-size,
     size,-size,-size,
     size, size,-size,
     size,-size,-size,
    -size,-size,-size,
    -size,-size,-size,
    -size, size, size,
    -size, size,-size,
     size,-size, size,
    -size,-size, size,
    -size,-size,-size,
    -size, size, size,
    -size,-size, size,
     size,-size, size,
     size, size, size,
     size,-size,-size,
     size, size,-size,
     size,-size,-size,
     size, size, size,
     size,-size, size,
     size, size, size,
     size, size,-size,
    -size, size,-size,
     size, size, size,
    -size, size,-size,
    -size, size, size,
     size, size, size,
    -size, size, size,
     size,-size, size
};

const unsigned int nVertices = 36;
//const unsigned int nVertices = sizeof(vertices) / sizeof(float) / 3;


/// Copy box vertices to GPU.
void VerticesSkybox::generate() {

    /// Vertex array object.
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    /// Copy the vertices to GPU in a vertex buffer (VBO).
    glGenBuffers(1, &_vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

}


void VerticesSkybox::draw(GLuint vertexAttribID) const {

    /*
     * Bind vertex array
     * A vertex array object holds references to the vertex buffers, the index
     * buffer and the layout specification of the vertex itself. At runtime,
     * you can just glBindVertexArray to recall all of these information.
     */
    glBindVertexArray(_vertexArrayID);

    /// Vertex attribute points to data from the currently binded array buffer.
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glEnableVertexAttribArray(vertexAttribID);
    // vec3: 3 floats per vertex for the attribute.
    glVertexAttribPointer(vertexAttribID, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /// Draw the Skybox vertices.
    glDrawArrays(GL_TRIANGLES, 0, nVertices);

}


void VerticesSkybox::clean() {
    glDeleteBuffers(1, &_vertexBufferID);
    glDeleteVertexArrays(1, &_vertexArrayID);
}

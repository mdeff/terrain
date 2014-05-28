
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
const unsigned int nVertices = sizeof(vertices) / sizeof(float) / 3;


void VerticesSkybox::generate() {

    /// Vertex array object (VAO).
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    /// Copy the vertices to GPU in a vertex buffer (VBO).
    glGenBuffers(1, _vertexBufferIDs);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferIDs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

}


void VerticesSkybox::bind(GLuint vertexAttribIDs[]) const {

    /// Vertex attribute points to data from the currently binded array buffer.
    /// The binding is part of the binded VAO state.
    glBindVertexArray(_vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferIDs[0]);
    glEnableVertexAttribArray(vertexAttribIDs[0]);
    // vec3: 3 floats per vertex for the attribute.
    glVertexAttribPointer(vertexAttribIDs[0], 3, GL_FLOAT, GL_FALSE, 0, 0);

}


void VerticesSkybox::draw() const {

    /// The GL_ELEMENT_ARRAY_BUFFER binding is stored within the VAO.
    /// The GL_ARRAY_BUFFER​ binding is NOT part of the VAO state.
    /// But the vertex attribute binding to the VBO is retained.
    glBindVertexArray(_vertexArrayID);

    /// Draw the Skybox vertices.
    glDrawArrays(GL_TRIANGLES, 0, nVertices);

}


void VerticesSkybox::clean() {
    glDeleteBuffers(1, _vertexBufferIDs);
    glDeleteVertexArrays(1, &_vertexArrayID);
}


#include "vertices_camera_pictorial.h"
#include <GL/glew.h>


const GLfloat width  = 0.025f;
const GLfloat height = 0.05f;
const GLfloat depth  = 0.1f;

#define point1  0.00f,  0.000f, 0.0f
#define point2 +width, +height, depth
#define point3 +width, -height, depth
#define point4 -width, -height, depth
#define point5 -width, +height, depth

/// Camera pictorial vertices, who define lines.
const GLfloat vertices[] = {
    point1, point2,
    point1, point3,
    point1, point4,
    point1, point5,
    point2, point3,
    point3, point4,
    point4, point5,
    point5, point2,
};

const unsigned int nVertices = sizeof(vertices) / sizeof(GLfloat) / 3;


void VerticesCameraPictorial::generate() {

    /// Vertex array object (VAO).
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    /// Copy the vertices to GPU in a vertex buffer (VBO).
    glGenBuffers(1, _vertexBufferIDs);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferIDs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

}


void VerticesCameraPictorial::bind(GLuint vertexAttribIDs[]) const {

    /// Vertex attribute points to data from the currently binded array buffer.
    /// The binding is part of the binded VAO state.
    glBindVertexArray(_vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferIDs[0]);
    glEnableVertexAttribArray(vertexAttribIDs[0]);
    // vec3: 3 floats per vertex for the attribute.
    glVertexAttribPointer(vertexAttribIDs[0], 3, GL_FLOAT, GL_FALSE, 0, 0);

}


void VerticesCameraPictorial::draw() const {

    /// The GL_ELEMENT_ARRAY_BUFFER binding is stored within the VAO.
    /// The GL_ARRAY_BUFFER​ binding is NOT part of the VAO state.
    /// But the vertex attribute binding to the VBO is retained.
    glBindVertexArray(_vertexArrayID);

    /// Render the camera pictorial.
    glDrawArrays(GL_LINES, 0, nVertices);

}


void VerticesCameraPictorial::clean() {
    glDeleteBuffers(1, _vertexBufferIDs);
    glDeleteVertexArrays(1, &_vertexArrayID);
}

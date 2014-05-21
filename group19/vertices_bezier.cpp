#include <iostream>

#include "vertices_bezier.h"
#include <cmath>
#include <GL/glew.h>

/* control from project subject
double b0X = -1.10f;
double b0Y =  0.97f;
double b0Z =  0.40f;

double b1X = -0.58f;
double b1Y =  1.43f;
double b1Z =  0.40f;

double b2X = 0.65f;
double b2Y = 1.37f;
double b2Z = 0.40f;

double b3X = 1.11f;
double b3Y = 0.97f;
double b3Z = 0.40f;*/

/// Bézier curve control points.

const float b0X = -0.51f;
const float b0Y =  1.09f;
const float b0Z =  0.40f;

const float b1X =  0.57f;
const float b1Y =  1.26f;
const float b1Z =  0.40f;

const float b2X =  1.31f;
const float b2Y =  0.92f;
const float b2Z =  0.40f;

const float b3X =  1.25f;
const float b3Y = -0.41f;
const float b3Z =  0.40f;


/// Choose the resolution.
const unsigned int nVertices = 1000;


/// Copy Bézier curve vertices to GPU.
void VerticesBezier::generate() {

    /// Vertex array object (VAO).
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    /// Generate nVertices along a Bézier curve.
    float vertices[nVertices*3];
    for(int i=0; i<nVertices; ++i) {
        float t = float(i) / 1000.0;
        vertices[i+0] = std::pow((1-t),3)*b0X + 3*t*std::pow((1-t),2)*b1X + 3*std::pow(t,2)*(1-t)*b2X + std::pow(t,3)*b3X;
        vertices[i+1] = std::pow((1-t),3)*b0Y + 3*t*std::pow((1-t),2)*b1Y + 3*std::pow(t,2)*(1-t)*b2Y + std::pow(t,3)*b3Y;
        vertices[i+2] = std::pow((1-t),3)*b0Z + 3*t*std::pow((1-t),2)*b1Z + 3*std::pow(t,2)*(1-t)*b2Z + std::pow(t,3)*b3Z;
    }

    /// Copy the vertices to GPU in a vertex buffer (VBO).
    glGenBuffers(1, &_vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

}


void VerticesBezier::bind(GLuint vertexAttribID) const {

    /// Vertex attribute points to data from the currently binded array buffer.
    /// The binding is part of the binded VAO state.
    glBindVertexArray(_vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glEnableVertexAttribArray(vertexAttribID);
    // vec3: 3 floats per vertex for the attribute.
    glVertexAttribPointer(vertexAttribID, 3, GL_FLOAT, GL_FALSE, 0, 0);

}


void VerticesBezier::draw() const {

    /// The GL_ELEMENT_ARRAY_BUFFER binding is stored within the VAO.
    /// The GL_ARRAY_BUFFER​ binding is NOT part of the VAO state.
    /// But the vertex attribute binding to the VBO is retained.
    glBindVertexArray(_vertexArrayID);

    /// Draw the grid vertices : either points or lines.
    glDrawArrays(GL_POINTS, 0, nVertices);
//    glDrawArrays(GL_LINES, 0, nVertices);

}


void VerticesBezier::clean() {
    glDeleteBuffers(1, &_vertexBufferID);
    glDeleteVertexArrays(1, &_vertexArrayID);
}

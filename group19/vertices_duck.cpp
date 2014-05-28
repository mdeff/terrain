#include "vertices_duck.h"

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"


/// Load vertices list from a mesh.
void VerticesDuck::generate() {

    /// Load Mesh from path.
	read("../../duck/duck.obj");
    triangulate();
	update_vertex_normals();
    std::cout << "Number of vertices: " << n_vertices() << " num faces: " << n_faces() << std::endl;

    /// Vertex Array.
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    glGenBuffers(2, _vertexBufferIDs);

    /// Vertex buffer for points
    Surface_mesh::Vertex_property<opengp::Point> vpoints = get_vertex_property<opengp::Point>("v:point");
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferIDs[0]);
    glBufferData(GL_ARRAY_BUFFER, n_vertices() * sizeof(vec3), vpoints.data(), GL_STATIC_DRAW);

    /// Vertex buffer for normals.
    Surface_mesh::Vertex_property<opengp::Normal> vnormals = get_vertex_property<opengp::Normal>("v:normal");
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferIDs[1]);
    glBufferData(GL_ARRAY_BUFFER, n_vertices() * sizeof(vec3), vnormals.data(), GL_STATIC_DRAW);

    /// Looping around the mesh to index the vertices.
    unsigned int* indices = new unsigned int[100000]; //buffer holding the index of the mesh
    _nIndices = 0;
    // Copy index of the mesh .
    for(Surface_mesh::Face_iterator fit = faces_begin(); fit != faces_end(); ++fit) {
        unsigned int n = valence(*fit);
        // Loop around one face to get surrounding vertices.
        Surface_mesh::Vertex_around_face_circulator vit = vertices(*fit);
        for(unsigned int v = 0; v < n; ++v) {
            indices[_nIndices] = (*vit).idx();
            ++vit;
            ++_nIndices;
        }
    }

    glGenBuffers(1, &_elementBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _nIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    delete[] indices;

}


void VerticesDuck::bind(GLuint vertexAttribIDs[]) const {

    /// Vertex attribute points to data from the currently binded array buffer.
    /// The binding is part of the binded VAO state.
    glBindVertexArray(_vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferIDs[0]);
    glEnableVertexAttribArray(vertexAttribIDs[0]);
    glVertexAttribPointer(vertexAttribIDs[0], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferIDs[1]);
    glEnableVertexAttribArray(vertexAttribIDs[1]);
    glVertexAttribPointer(vertexAttribIDs[1], 3, GL_FLOAT, GL_FALSE, 0, 0);

}


void VerticesDuck::draw() const {

    /// The GL_ELEMENT_ARRAY_BUFFER binding is stored within the VAO.
    /// The GL_ARRAY_BUFFER​ binding is NOT part of the VAO state.
    /// But the vertex attribute binding to the VBO is retained.
	glBindVertexArray(_vertexArrayID);

    /// Draw the duck.
    glDrawElements(GL_TRIANGLES, _nIndices, GL_UNSIGNED_INT, 0);

}


void VerticesDuck::clean() {
    glDeleteBuffers(2, _vertexBufferIDs);
	glDeleteBuffers(1, &_elementBufferID);
	glDeleteVertexArrays(1, &_vertexArrayID);
}

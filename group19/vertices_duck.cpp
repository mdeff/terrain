#include "vertices_duck.h"


#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"



//load vertices list from a mesh, then bind it
void VerticesDuck::generate(){
	//load Mesh from path
	read("../../duck/duck.obj");
	triangulate();
	//update vertices normal
	update_vertex_normals();
	cout << "Number of vertices: " << n_vertices() << " num faces: " << n_faces() << endl;

	/// Vertex Array
    glGenVertexArrays(1, &_vertexArrayID);
    glBindVertexArray(_vertexArrayID);

    ///// Vertex Buffer
    Surface_mesh::Vertex_property<Point> vpoints = get_vertex_property<Point>("v:point");
    //GLuint vertexbuffer;
    glGenBuffers(1, &_vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, n_vertices() * sizeof(vec3), vpoints.data(), GL_STATIC_DRAW);

   
	 ///// Normal Buffer
    Surface_mesh::Vertex_property<Normal> vnormals = get_vertex_property<Normal>("v:normal");
    glGenBuffers(1, &_normal_mvID);
    glBindBuffer(GL_ARRAY_BUFFER, _normal_mvID);
    glBufferData(GL_ARRAY_BUFFER, n_vertices() * sizeof(vec3), vnormals.data(), GL_STATIC_DRAW);

    /// Looping around the mesh to index the vertices
	//std::vector<unsigned int> buf;
	indices = new unsigned int[100000];
	nIndices = 0;
	//copy index of the mesh 
    for(Surface_mesh::Face_iterator fit = faces_begin(); fit != faces_end(); ++fit) {
        unsigned int n = valence(*fit);
		//Loop around one face to get surrounding vertices
        Surface_mesh::Vertex_around_face_circulator vit = vertices(*fit);
        for(unsigned int v = 0; v < n; ++v) {
			//buf.push_back((*vit).idx());
			indices[nIndices] = (*vit).idx();
            ++vit;
			++nIndices;
        }
    }

    glGenBuffers(1, &_elementBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW); 
   
	
	
    // /// Vertex Attribute ID for Normals
    //GLuint normal = glGetAttribLocation(_programID, "normal");
    //glEnableVertexAttribArray(normal);
    //glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    //glVertexAttribPointer(normal, NUM_ELS, GL_FLOAT,GL_FALSE, 0, 0);

}



void VerticesDuck::bind(GLuint vertexAttribID) const {
	// Vertex attribute points to data from the currently binded array buffer.
    /// The binding is part of the binded VAO state.
    glBindVertexArray(_vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glEnableVertexAttribArray(vertexAttribID);
    
    // Bind Vertex Attribute ID for Positions
   /* GLuint position = glGetAttribLocation(_programID, "position");
    glEnableVertexAttribArray(position);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);*/
    glVertexAttribPointer(vertexAttribID, 3, GL_FLOAT, GL_FALSE, 0, 0);

}

void VerticesDuck::draw() const {
	glBindVertexArray(_vertexArrayID);

	//draw the duck
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
}

void VerticesDuck::clean() {
	delete []indices;
	glDeleteBuffers(1, &_vertexBufferID);
	glDeleteBuffers(1, &_elementBufferID);
	glDeleteVertexArrays(1, &_vertexArrayID);
}
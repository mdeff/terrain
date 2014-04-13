// Copyright (C) 2014 - LGG EPFL

#include <iostream>
#include <sstream>
#include "common.h"

#include "rendering_vshader.h"
#include "rendering_fshader.h"
#include "heightmap_vshader.h"
#include "heightmap_fshader.h"


/// Shader program.
GLuint programID = 0;

/// NxN triangle grid.
const int N = 16;
const int nVertices = N*N;
const int nIndices = (N-1)*(N-1)*6;

vec3 vertices[nVertices];
unsigned int indices[nIndices];


GLuint FramebufferName;
GLuint uvbuffer;

	
// The fullscreen quad for binding with texture
static const GLfloat g_quad_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
};



//[Alex] Generating the texture we need
GLuint create_texture(){
	 // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(ONE, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    //for now just create a simple texture image
    glfwLoadTexture2D("C:/Users/Alex/Dropbox/MsSem2/ComputerGraphics/project1/icg-project-group19/build", 0);

    // Nice trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Return the ID of the texture we just created
    return textureID;
}


void update_matrix_stack(const mat4& model) {

    /// Define projection matrix (camera intrinsics)
    static mat4 projection = Eigen::perspective(45.0f, 4.0f/3.0f, 0.1f, 10.0f);
    GLuint projection_id = glGetUniformLocation(programID, "projection");
    glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE, projection.data());

    /// Define the view matrix (camera extrinsics)
    vec3 cam_look(0.0f, 0.0f, 0.0f);
    /// Camera is in the sky, looking down.
    //vec3 cam_pos(0.0f, 0.0f, 2.5f);
    //vec3 cam_up(0.0f, 1.0f, 0.0f);
    /// Camera is in a corner, looking down to the terrain.
    vec3 cam_pos(2.0f, -2.0f, 1.5f);
    vec3 cam_up(0.0f, 0.0f, 1.0f);
    static mat4 view = Eigen::lookAt(cam_pos, cam_look, cam_up);

    /// Assemble the "Model View" matrix
    static mat4 model_view;
    model_view = view * model;
    GLuint model_view_id = glGetUniformLocation(programID, "model_view");
    glUniformMatrix4fv(model_view_id, ONE, DONT_TRANSPOSE, model_view.data());

}


void triangle_grid() {

    /// Generate the vertices (line by line) : 16^2 = 256 vertices.
    //vec3 vertices[nVertices];
    for(int y=0; y<N; y++) {
        for(int x=0; x<N; x++) {
            vertices[y*N+x] = vec3(float(2*x)/(N-1)-1, float(2*y)/(N-1)-1, 0);
            //cout << vertices[y*N+x] << endl;
        }
    }

    /// Copy the vertices in a vertex buffer.
    GLuint vertexbuffer;
    glGenBuffers(ONE, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /// Vertex Attribute ID for positions.
    GLuint position = glGetAttribLocation(programID, "position");
    const int NUM_ELS = 3; /// floats per each vertex "position"
    glEnableVertexAttribArray(position);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(position, NUM_ELS, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

    /// Indices that form the triangles.
    /// Grid of 15x15 squares : 225 squares -> 450 triangles -> 1350 indices.
    
    for(int y=0; y<N-1; y++) {
        for(int x=0; x<N-1; x++) {
            /// Upper left triangle of the square.
            indices[y*(N-1)*6+x*6+0] = y*N + x + 0;
            indices[y*(N-1)*6+x*6+1] = y*N + x + 1;
            indices[y*(N-1)*6+x*6+2] = y*N + x + N;
            /// Lower right triangle of the square.
            indices[y*(N-1)*6+x*6+3] = y*N + x + 1;
            indices[y*(N-1)*6+x*6+4] = y*N + x + N+1;
            indices[y*(N-1)*6+x*6+5] = y*N + x + N;
        }
    }

    /// Copy the indices in an index array.
    GLuint elementbuffer;
    glGenBuffers(ONE, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}




void init() {

    /// Vertex Array
    GLuint VertexArrayID;
    glGenVertexArrays(ONE, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

	

	////[Alex] Generating the frame buffer for containing height map
	//
	//// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
 //   FramebufferName = 0;
 //   glGenFramebuffers(1, &FramebufferName);
 //   glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	////The height map
	//GLuint heightMap = create_texture();
	//// Set "renderedTexture" as our colour attachement #0
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, heightMap, 0);
 //
	//// Set the list of draw buffers.
	//GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	//// Always check that our framebuffer is ok
	//if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	std::cerr << "Framebuffer not complete." <<std::endl;

	//// Render to our framebuffer
	//glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	//glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the upper right

	//// Create and compile our GLSL program from the shaders
	//GLuint quad_programID = compile_shaders(heightmap_vshader, heightmap_fshader);
	//GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");
	//glUseProgram(quad_programID);
	////GLuint timeID = glGetUniformLocation(quad_programID, "time");
 //   // The fullscreen quad's FBO
	//GLuint quad_VertexArrayID;
	//glGenVertexArrays(1, &quad_VertexArrayID);
	//glBindVertexArray(quad_VertexArrayID);
 //
	//static const GLfloat g_quad_vertex_buffer_data[] = {
	//	-1.0f, -1.0f, 0.0f,
	//	1.0f, -1.0f, 0.0f,
	//	-1.0f,  1.0f, 0.0f,
	//	-1.0f,  1.0f, 0.0f,
	//	1.0f, -1.0f, 0.0f,
	//	1.0f,  1.0f, 0.0f,
	//};
 //
	//GLuint quad_vertexbuffer;
	//glGenBuffers(1, &quad_vertexbuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
 //
	//
 //   /// Vertex Attribute ID for positions.
 //   GLuint position = glGetAttribLocation(quad_programID, "position");

	//// Render to the screen
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	

    /// Compile the rendering shaders.
    /// Triangle grid needs the programID to get the "position" attribute.
    programID = compile_shaders(rendering_vshader, rendering_fshader);
    if(!programID)
        exit(EXIT_FAILURE);
    glUseProgram(programID);

    ///// Generate a flat and regular triangle grid.
    triangle_grid();

    /// Initialize the matrix stack
    update_matrix_stack(mat4::Identity());

    /// OpenGL parameters
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    
}


void display() {
	
	//To render only the boundary
	//comment it if you want to render full triangles
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    
    glDrawElements(GL_TRIANGLES,nIndices, GL_UNSIGNED_INT, 0);
	
}


int main(int, char**) {
    glfwInitWindowSize(1024, 768);
    glfwCreateWindow("Project - Group 19");
    glfwDisplayFunc(display);
    init();
    glfwTrackball(update_matrix_stack);
    glfwMainLoop();
    return EXIT_SUCCESS;    
}

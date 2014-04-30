#pragma once
#include "common.h"
#include "skybox_vshader.h"
#include "skybox_fshader.h"




GLuint loadTexture(const char * imagepath, const int slotNum){
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(ONE, &textureID);

	glActiveTexture(GL_TEXTURE0 + slotNum);
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Read the file, call glTexImage2D with the right parameters
    if (glfwLoadTexture2D(imagepath, 0)){
		// Nice trilinear filtering.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D); 
	} else {
		std::cout << "Cannot load texture file!" << endl;
		return -1;
	}

    // Return the ID of the texture we just created
    return textureID;
};


class Skybox {
private:
	GLuint skybox_vao;
	GLuint skybox_program_id;
	GLuint skybox_tex;

public:
	void init(){

		static const unsigned int nVertices = 8;
		static const GLfloat skybox_vertices[] ={
			-1.0f,-1.0f,-1.0f, // triangle 1 : begin
			-1.0f,-1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f, // triangle 1 : end
			1.0f, 1.0f,-1.0f, // triangle 2 : begin
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f, // triangle 2 : end
			1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			1.0f,-1.0f,-1.0f,
			1.0f, 1.0f,-1.0f,
			1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			1.0f,-1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f,-1.0f,-1.0f,
			1.0f, 1.0f,-1.0f,
			1.0f,-1.0f,-1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f,-1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f,-1.0f, 1.0f
		};
		
		glGenVertexArrays(1, &skybox_vao);
		glBindVertexArray(skybox_vao);

		/// Vertex Buffer
		GLuint vertexbuffer;
		glGenBuffers(ONE, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);

		/*char *CubeMapTextureFiles[6] = {"../../skybox/right.jpg", "../../skybox/left.jpg", "../../skybox/top.jpg", 
													"../../skybox/bottom.jpg", "../../skybox/front.jpg", "../../skybox/back.jpg"};
*/
		
		//compile shader
		skybox_program_id = compile_shaders(skybox_vshader, skybox_fshader);
		if (!skybox_program_id) exit(EXIT_FAILURE);
		
		GLuint positionID = glGetAttribLocation(skybox_program_id, "position");
		glEnableVertexAttribArray(positionID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		// vec3: 3 floats per vertex for the position attribute.
		glVertexAttribPointer(positionID, 3, GL_FLOAT, DONT_NORMALIZE, 0, (void*)0);


		// OpenGL parameters.
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_TEXTURE_2D);
	}
	

	void draw(mat4& projection, mat4& modelview){
		
		glUseProgram(skybox_program_id);

		GLuint modelviewID = glGetUniformLocation(skybox_program_id, "modelview");
		glUniformMatrix4fv(modelviewID, ONE, DONT_TRANSPOSE, modelview.data());

		GLuint projectionID = glGetUniformLocation(skybox_program_id, "projection");
		glUniformMatrix4fv(projectionID, ONE, DONT_TRANSPOSE, projection.data());

		glBindVertexArray(skybox_vao);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 12*3);

		glBindVertexArray(0);
	}

};
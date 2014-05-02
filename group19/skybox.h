#pragma once
#include "common.h"
#include "skybox_vshader.h"
#include "skybox_fshader.h"








class Skybox {
public:
	GLuint skybox_vao;
	GLuint skybox_program_id;
	GLuint skybox_tex;
	//mat4 projection, modelview;

public:
	
	void init_skybox(){

		static const unsigned int nVertices = 36;
		static const float size = 5.0f;
		static const GLfloat skybox_vertices[] ={
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
		
		
		glGenVertexArrays(1, &skybox_vao);
		glBindVertexArray(skybox_vao);

	
		/*char *CubeMapTextureFiles[6] = {"../../skybox/right.jpg", "../../skybox/left.jpg", "../../skybox/top.jpg", 
													"../../skybox/bottom.jpg", "../../skybox/front.jpg", "../../skybox/back.jpg"};
*/
		
		//compile shader
		skybox_program_id = compile_shaders(skybox_vshader, skybox_fshader);
		if (!skybox_program_id) exit(EXIT_FAILURE);
		
	    /// Vertex Buffer
		GLuint vertexbuffer;
		glGenBuffers(ONE, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);


		GLuint positionID = glGetAttribLocation(skybox_program_id, "position");
		glEnableVertexAttribArray(positionID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		// vec3: 3 floats per vertex for the position attribute.
		glVertexAttribPointer(positionID, 3, GL_FLOAT, DONT_NORMALIZE, 0, (void*)0);

		//update_matrix_stack(mat4::Identity());
	};

	


	void draw(mat4& projection, mat4& modelview){
		
		glUseProgram(skybox_program_id);

		GLuint modelviewID = glGetUniformLocation(skybox_program_id, "modelview");
		glUniformMatrix4fv(modelviewID, ONE, DONT_TRANSPOSE, modelview.data());

		GLuint projectionID = glGetUniformLocation(skybox_program_id, "projection");
		glUniformMatrix4fv(projectionID, ONE, DONT_TRANSPOSE, projection.data());

		glBindVertexArray(skybox_vao);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		 glDrawArrays(GL_TRIANGLES,0, 36);

		//make sure the VAO is not changed from the outside
		//glBindVertexArray(0);
	}

};

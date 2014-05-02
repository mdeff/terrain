#include "skybox.h"

#include <GL/glew.h>
#include "opengp.h"

#include "skybox_vshader.h"
#include "skybox_fshader.h"

const unsigned int nVertices = 36;

void Skybox::init() {

    const float size = 5.0f;
    const GLfloat skybox_vertices[] ={
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


    // Common initialization.
    RenderingContext::init(skybox_vshader, skybox_fshader);

	//Read texture and bind it
	skybox_tex = 8; //make sure not coincide with previous texture slot
	GLuint skyID = load_skybox_texture(8);
	GLuint skyTexId  = glGetUniformLocation(_programID, "skybox_tex");
	glUniform1i(skyTexId, skybox_tex);

    /// Vertex Buffer
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);


    GLuint positionID = glGetAttribLocation(_programID, "position");
    glEnableVertexAttribArray(positionID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // vec3: 3 floats per vertex for the position attribute.
    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //update_matrix_stack(mat4::Identity());

    /// Set uniform IDs.
    _modelviewID = glGetUniformLocation(_programID, "modelview");
    _projectionID = glGetUniformLocation(_programID, "projection");

}


void Skybox::draw(mat4& projection, mat4& modelview) const {

    // Common drawing.
    RenderingContext::draw();

    //--- Update the content of the uniforms (texture IDs, matrices, ...)
    glUniformMatrix4fv(_modelviewID, 1, GL_FALSE, modelview.data());
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glDrawArrays(GL_TRIANGLES, 0, nVertices);

    //make sure the VAO is not changed from the outside
    //glBindVertexArray(0);
}


int Skybox::loadBMPtex(const char* imagepath, unsigned char* data){

		printf("Reading image %s\n", imagepath);

		// Data read from the header of the BMP file
		unsigned char header[54];
		unsigned int dataPos;
		unsigned int imageSize;
		unsigned int width, height;

		// Open the file
		FILE * file = fopen(imagepath,"rb");
		if (!file) {
			printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); 
			return 0;
		}

		// Read the header, i.e. the 54 first bytes

		// If less than 54 bytes are read, problem
		if ( fread(header, 1, 54, file)!=54 ){ 
			printf("Not a correct BMP file\n");
			return 0;
		}
		// A BMP files always begins with "BM"
		if ( header[0]!='B' || header[1]!='M' ){
			printf("Not a correct BMP file\n");
			return 0;
		}
		// Make sure this is a 24bpp file
		if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    return 0;}
		if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    return 0;}

		// Read the information about the image
		dataPos    = *(int*)&(header[0x0A]);
		imageSize  = *(int*)&(header[0x22]);
		width      = *(int*)&(header[0x12]);
		height     = *(int*)&(header[0x16]);

		//for debugging only
		//printf("size of image is width = %d and height = %d\n", width, height);

		// Some BMP files are misformatted, guess missing information
		if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
		if (dataPos==0)      dataPos=54; // The BMP header is done that way


		// Read the actual data from the file into the buffer
		fread(data,1,imageSize,file);

		//Need to swap the order of channel since the order here is BGR not RGB
		for(int i=0; i<imageSize; i=i+3){
			unsigned char tmp = data[i];
			data[i] = data[i+2];
			data[i+2] = tmp;
		}
		// Everything is in memory now, the file wan be closed
		fclose (file);


		// Return 1 if the file is read successfully
		return 1;
}

GLuint Skybox::load_skybox_texture(int slotNum){
		
		// hardcode the size of image for now 
		int width = 1024, height = 1024, channel = 3;
		int imgSize = width*height*channel;

		/* Allocate data for each buffer */
		back = new unsigned char [imgSize];
		front = new unsigned char [imgSize];
		top = new unsigned char [imgSize];
		bottom = new unsigned char [imgSize];
		left = new unsigned char [imgSize];
		right = new unsigned char [imgSize];


		/* load image of each face to the data */
		if (!loadBMPtex("../../skybox/back.bmp", &back[0]))
			printf("Cannot read texture\n");
		if (!loadBMPtex("../../skybox/front.bmp", &front[0]))
			printf("Cannot read texture\n");
		if (!loadBMPtex("../../skybox/bottom.bmp", &bottom[0]))
			printf("Cannot read texture\n");
		if (!loadBMPtex("../../skybox/top.bmp", &top[0]))
			printf("Cannot read texture\n");
		if (!loadBMPtex("../../skybox/left.bmp", &left[0]))
			printf("Cannot read texture\n");
		if (!loadBMPtex("../../skybox/right.bmp", &right[0]))
			printf("Cannot read texture\n");

		/* Enable texture and bind the cube map */
		GLuint tex_cub;
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glGenTextures(1, &tex_cub);
		glActiveTexture(GL_TEXTURE0 + slotNum);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cub);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, left); 
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, right); 
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, back); 
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, front); 
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, top); 
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bottom); 

		return tex_cub;
}

Skybox::~Skybox(){
	/*delete[] back;
	delete[] front;
	delete[] top;
	delete[] bottom;
	delete[] left;
	delete[] right;*/

}
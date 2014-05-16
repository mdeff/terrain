// Copyright (C) 2014 - LGG EPFL

#include <iostream>
#include <sstream>

#include "common.h"
#include "heightmap.h"
#include "shadowmap.h"
#include "watermap.h"
#include "skybox.h"
#include "terrain.h"
#include "vertices.h"
#include "vertices_quad.h"
#include "vertices_grid.h"
#include "vertices_skybox.h"

/// Screen size.
const int windowWidth(1024);
const int windowHeight(768);

/// Textures (heightmap and shadowmap) sizes.
const int textureWidth(1024);
const int textureHeight(1024);

/// Instanciate the rendering contexts.
Shadowmap shadowmap(textureWidth, textureHeight);
Skybox skybox(windowWidth, windowHeight);
Terrain terrain(windowWidth, windowHeight);
Watermap water(windowWidth, windowHeight);

/// Instanciate the vertices.
Vertices* verticesGrid = new VerticesGrid();
Vertices* verticesSkybox = new VerticesSkybox();

/// Matrices that have to be shared between functions.
static mat4 cameraModelview;
static mat4 lightMVP;
static vec3 lightPositionModel;

/// Projection parameters.
// Horizontal field of view in degrees : amount of "zoom" ("camera lens").
// Usually between 90° (extra wide) and 30° (quite zoomed in).
const float fieldOfView = 45.0f;
// Aspect ratio depends on the window size (for example 4/3 or 1).
const float aspectRatio = float(windowWidth) / float(windowHeight);
// Near clipping plane. Keep as little as possible (precision issues).
const float nearPlane = 0.1f;
// Far clipping plane. Keep as big as possible (usually 10.0f or 100.0f).
const float farPlane  = 100.0f;

/// Camera projection matrix (camera intrinsics).
const mat4 cameraProjection = Eigen::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);

/// Spot light projection matrix.
const mat4 lightProjection = Eigen::perspective(fieldOfView, float(textureWidth)/float(textureHeight), nearPlane, farPlane);


void update_matrix_stack(const mat4& model) {

    /// View matrix (camera extrinsics) (position in world space).

    /// Global view from outside.
    vec3 camPos(0.0f, -3.0f, 4.0f);
    vec3 camLookAt(0.0f, 0.0f, 0.0f);
    vec3 camUp(0.0f, 0.0f, 1.0f);

    /// Internal view from center.
//    vec3 camPos(0.9f, -0.8f, 1.0f);
//    vec3 camLookAt(-0.3f, 0.1f, 0.5f);
//    vec3 camUp(0.0f, 0.0f, 1.0f);

    /// Close texture view, for screenshots.
//    vec3 camPos(0.2f, -0.1f, 0.5f);
//    vec3 camLookAt(-0.3f, 0.1f, 0.2f);
//    vec3 camUp(0.0f, 0.0f, 1.0f);

    /// Assemble the view matrix.
    mat4 view = Eigen::lookAt(camPos, camLookAt, camUp);

    /// Assemble the "Model View" matrix.
    cameraModelview = view * model;

}


/// Key press callback.
void GLFWCALL keyboard_callback(int key, int action) {

    /// Distance from center (0,0,0) to sun.
    const float r = 3.0f;

    if(action == GLFW_PRESS) {

        //std::cout << "Pressed key : " << key << std::endl;

        /// 49 corressponds to 1, 57 to 9 (keyboard top keys).
        if(key >= 49 && key <= 57) {

            /// Angle from 0° (key 1) to 90° (key 9).
            float theta = M_PI / 8.0f * float(key-49);

            /// Position from sunrise (-r,0,0) to noon (0,0,r).
            lightPositionModel = vec3(-std::cos(theta)*r, 0.0, std::sin(theta)*r);

            /// Light source position (model coordinates).
            const vec3 lightLookAt(0.0, 0.0, 0.0);
            const vec3 lightUp(0.0, 1.0, 0.0);
            mat4 lightView = Eigen::lookAt(lightPositionModel, lightLookAt, lightUp);

            /// Assemble the lightMVP matrix for a spotlight source.
            lightMVP = lightProjection * lightView;
        }
    }
}


void init() {
	
    /// OpenGL parameters.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //glEnable(GL_CULL_FACE);

    /// Generate the heightmap texture.
    Vertices* verticesQuad = new VerticesQuad();
    verticesQuad->generate();
    Heightmap heightmap(textureWidth, textureHeight);
    GLuint heightMapTexID = heightmap.init(verticesQuad);
    heightmap.draw();
    heightmap.clean();
    verticesQuad->clean();
    delete verticesQuad;

    /// Generate the vertices.
    verticesGrid->generate();
    verticesSkybox->generate();

    /// Initialize the rendering contexts.
    GLuint shadowMapTexID = shadowmap.init(verticesGrid, heightMapTexID);
    terrain.init(verticesGrid, heightMapTexID, shadowMapTexID);
    skybox.init(verticesSkybox);
	water.init(verticesGrid);

    /// Initialize the matrix stack.  	
	update_matrix_stack(mat4::Identity());

    /// Initialize the light position.
    keyboard_callback(49, GLFW_PRESS);

}


void handleKeyboard(){
	
	static double posX = 1.0f;
	static double posY = 0.0f;
	static double posZ = 0.7f;

	static double lookX = 0.0f;
	static double lookY = 0.0f;
	static double lookZ = 0.7f;

	//double Dist = sqrt((posX-lookX)*(posX-lookX) + (posY-lookY)*(posY-lookY) + (posZ-lookZ)*(posZ-lookZ));
	//std::cout << 100*(posX-lookX)*(posX-lookX)/Dist <<"% "<< 100*(posY-lookY)*(posY-lookY) /Dist<<"% "<<100* (posZ-lookZ)*(posZ-lookZ)/Dist<<"% "<< Dist <<endl;
	

	static bool dirtyBit = false;  

	static double velocityForward = 0;
	static double velocityBackward = 0;
	static double velocityLeft = 0;
	static double velocityRight = 0;
	
	//std::cout<<velocityForward<<endl;

	if (glfwGetKey(87)==1 | velocityForward != 0.0f){//W pressed => go forward 
		if(glfwGetKey(87)==1 & velocityForward<0.05f){
				velocityForward = velocityForward + 0.001f;
		}
		else{
			velocityForward = velocityForward - 0.001f;
		}
		
		double dispX = (posX - lookX)*velocityForward;
		double dispY = (posY - lookY)*velocityForward;
		double dispZ = (posZ - lookZ)*velocityForward;
		//std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	
		posX = posX - dispX; 
		posY = posY - dispY; 
		posZ = posZ - dispZ; 

		lookX = lookX - dispX; 
		lookY = lookY - dispY; 
		lookZ = lookZ - dispZ;
		dirtyBit = true; 
		std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	}
	if  (glfwGetKey(83)==1 | velocityBackward != 0.0f){//S pressed => go backward 
		if(glfwGetKey(83)==1 & velocityBackward<0.05f){
				velocityBackward = velocityBackward + 0.001f;
		}
		else{
			velocityBackward = velocityBackward - 0.001f;
		}
		//std::cout<<"S pressed"<<endl;

		double dispX = (posX - lookX)*velocityBackward;
		double dispY = (posY - lookY)*velocityBackward;
		double dispZ = (posZ - lookZ)*velocityBackward;
		//std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	
		posX = posX + dispX; 
		posY = posY + dispY; 
		posZ = posZ + dispZ; 

		lookX = lookX + dispX; 
		lookY = lookY + dispY; 
		lookZ = lookZ + dispZ; 
		dirtyBit = true; 
		//std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	}
	if (glfwGetKey(65)==1 | velocityLeft != 0.0f){//A pressed => turn left
		if(glfwGetKey(65)==1 & velocityLeft<1.0f){
				velocityLeft = velocityLeft + 0.01f;
		}
		else{
			velocityLeft = velocityLeft - 0.01f;
		}
		//std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	
		double Angle = 0.0174f*velocityLeft; // more or less 1 degree
		double tmpX = (lookX-posX)*cos(Angle) - (lookY-posY)*sin(Angle) + posX;
		double tmpY = (lookX-posX)*sin(Angle) + (lookY-posY)*cos(Angle) + posY;

		lookX = tmpX;
		lookY = tmpY;
		dirtyBit = true; 
		/*
		double tmpXZ = (lookZ-posZ)*cos(Angle) - (lookY-posY)*sin(Angle) + posZ;
		double tmpXY = (lookZ-posZ)*sin(Angle) + (lookY-posY)*cos(Angle) + posY;
		double tmpYX = (lookX-posX)*cos(Angle) - (lookZ-posZ)*sin(Angle) + posX;
		double tmpYZ = (lookX-posX)*sin(Angle) + (lookZ-posZ)*cos(Angle) + posZ;
		double tmpZX = (lookX-posX)*cos(Angle) - (lookY-posY)*sin(Angle) + posX;
		double tmpZY = (lookX-posX)*sin(Angle) + (lookY-posY)*cos(Angle) + posY;

		
		double coefX = (posX-lookX)*(posX-lookX)/Dist;
		double coefY = (posY-lookY)*(posY-lookY)/Dist;
		double coefZ = (posZ-lookZ)*(posZ-lookZ)/Dist;


		lookX = (coefY*tmpYX + coefZ*tmpZX)/2;
		lookY = (coefX*tmpXY + coefZ*tmpZY)/2;
		lookZ = (coefX*tmpXZ + coefY*tmpYZ)/2;
		dirtyBit = true; 
		*/
		//std::cout << 100*(posX-lookX)*(posX-lookX)/Dist <<"% "<< 100*(posY-lookY)*(posY-lookY) /Dist<<"% "<<100* (posZ-lookZ)*(posZ-lookZ)/Dist<<"% "<< Dist <<endl;
	
		//std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	
	}
	if (glfwGetKey(68)==1 | velocityRight != 0.0f){//D pressed =turn right
		if(glfwGetKey(68)==1 & velocityRight<1.0f){
				velocityRight = velocityRight + 0.01f;
		}
		else{
			velocityRight = velocityRight - 0.01f;
		}
		//angleZ = angleZ - 0.001f;
		//std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	
		double Angle = -0.0174f*velocityRight; // more or less 1 degree
		double tmpX = (lookX-posX)*cos(Angle) - (lookY-posY)*sin(Angle) + posX;
		double tmpY = (lookX-posX)*sin(Angle) + (lookY-posY)*cos(Angle) + posY;
		lookX = tmpX ; 
		lookY = tmpY ;
		dirtyBit = true; 
		//std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	}
	if  (glfwGetKey(81)==1){//Q pressed
		std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	
		double Angle = 0.0174f; // more or less 1 degree
		double tmpX = (lookX-posX)*cos(Angle) - (lookZ-posZ)*sin(Angle) + posX;
		double tmpZ = (lookX-posX)*sin(Angle) + (lookZ-posZ)*cos(Angle) + posZ;
		lookX = tmpX ; 
		lookZ = tmpZ ;
		dirtyBit = true; 
		std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	}
	if  (glfwGetKey(69)==1){//E pressed 
		std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	
		double Angle = -0.0174f; // more or less 1 degree
		double tmpX = (lookX-posX)*cos(Angle) - (lookZ-posZ)*sin(Angle) + posX;
		double tmpZ = (lookX-posX)*sin(Angle) + (lookZ-posZ)*cos(Angle) + posZ;
		lookX = tmpX ; 
		lookZ = tmpZ ;
		dirtyBit = true; 
		std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;
	}
	if (dirtyBit == true){ 


		/*
		double A = cos(angleX);
		double B = sin(angleX);
		double C = cos(angleY);
		double D = sin(angleY);
		double E = cos(angleZ);
		double F = sin(angleZ);
		
		double tmpX = (posX-lookX);
		double tmpY = (posY-lookY);
		double tmpZ = (posZ-lookZ);
/*
		lookX = tmpX * C * E  - tmpY * C * F + tmpZ * D;
		lookY = tmpX * (E * D * B + F * A) + tmpY * (E * A - F * D * B) - tmpZ * B * C;
		lookZ = tmpX * (F* B - E * A * D ) + tmpY * (F * A * D + E * B) + tmpZ * A * C;
		*/
		vec3 camPos(posX,posY,posZ);
		vec3 camLookAt(lookX, lookY, lookZ);
		vec3 camUp(0.0f, 0.0f, 1.0f);

		mat4 view = Eigen::lookAt(camPos, camLookAt, camUp);
		cameraModelview = view;
		dirtyBit = false; 
	}
/*	switch (key)
	{
		case 87: //W pressed => go forward 
		{ 
			 
			posX = posX - (posX - lookX)*0.05; 
			posY = posY - (posY - lookY)*0.05; 
			posZ = posZ - (posZ - lookZ)*0.05; 

			lookX = lookX - (posX - lookX)*0.05; 
			lookY = lookY - (posY - lookY)*0.05; 
			lookZ = lookZ - (posZ - lookZ)*0.05;
			std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;

			break;
		}
		case 83: //S pressed => go backward 
		{
			posX = posX + (posX - lookX)*0.05; 
			posY = posY + (posY - lookY)*0.05; 
			posZ = posZ + (posZ - lookZ)*0.05; 

			lookX = lookX + (posX - lookX)*0.05; 
			lookY = lookY + (posY - lookY)*0.05; 
			lookZ = lookZ + (posZ - lookZ)*0.05; 

			std::cout <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;

			break;
		}
		case 65:
		{
			std::cout <<"Distance befor rotation =" <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;

			double Angle = 0.0174f; // more or less 1 degree
			lookX = (lookX-posX)*cos(Angle) - (lookY-posY)*sin(Angle) + posX;
			lookY = (lookX-posX)*sin(Angle) + (lookY-posY)*cos(Angle) + posY;

			std::cout  <<"Distance after rotation ="<<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;

			break;
		}
		case 68:
		{
			std::cout <<"Distance befor rotation =" <<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;

			double Angle = -0.0174f; // more or less 1 degree
			lookX = (lookX-posX)*cos(Angle) - (lookY-posY)*sin(Angle) + posX;
			lookY = (lookX-posX)*sin(Angle) + (lookY-posY)*cos(Angle) + posY;

			std::cout  <<"Distance after rotation ="<<sqrt(((posX-lookX)*(posX-lookX))+((posY-lookY)*(posY-lookY))+((posZ-lookZ)*(posZ-lookZ)))<<endl;

			break;
		}
	}
	/*if(action == GLFW_RELEASE){
		std::cout << "GLFW_RELEASE key : " << key << std::endl;
	}*/


}


void display() {

    /// Measure and print FPS (every second).
	static double lastTime = glfwGetTime();
    static int nbFrames = 0;
    double currentTime = glfwGetTime();
    nbFrames++;
    if(currentTime - lastTime >= 1.0) {
        std::cout << nbFrames << " FPS" << std::endl;
        nbFrames = 0;
        lastTime += 1.0;
    }
	handleKeyboard();
    
    /// Uncomment to render only the boundary (not full triangles).
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /// Render shadowmap, terrain and skybox.
    shadowmap.draw(lightMVP);
    terrain.draw(cameraProjection, cameraModelview, lightMVP, lightPositionModel);
    water.draw(cameraProjection, cameraModelview, lightMVP, lightPositionModel);
    skybox.draw(cameraProjection, cameraModelview);

}


int main(int, char**) {
    glfwInitWindowSize(windowWidth, windowHeight);
    glfwCreateWindow("Project - Group 19");	
    glfwDisplayFunc(display);
    init();
    glfwTrackball(update_matrix_stack);
    glfwSetKeyCallback(keyboard_callback);
    glfwMainLoop();
    return EXIT_SUCCESS;    
}

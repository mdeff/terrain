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

//camera movement according to bezier curve
static vec3 bezierCurve[1000];
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

    //// Global view from outside.
    //vec3 camPos(0.0f, -3.0f, 4.0f);
    //vec3 camLookAt(0.0f, 0.0f, 0.0f);
    //vec3 camUp(0.0f, 0.0f, 1.0f);

	//fps exploration
	vec3 camPos(0.78f, 0.42f, 0.30f);
    vec3 camLookAt(-0.24f, 0.19f, 0.13f);
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
	std::cout << "Pressed key : " << key << std::endl;

    if(action == GLFW_PRESS) {

        std::cout << "Pressed key : " << key << std::endl;

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

GLfloat *heightmapCPU = new GLfloat[1024*1024];
void CopyHeightmapToCPU(GLuint heightMapTexID){
	/*float test;
	glReadPixels( 0, 0, textureWidth, textureHeight, GL_RED, GL_FLOAT, &test);
	std::cout<<test<<endl;
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _width, _height, 0, GL_RED, GL_FLOAT, 0);
	*/
	//GLubyte *pixels = new GLubyte[w*h*4];
    glBindTexture(GL_TEXTURE_2D, heightMapTexID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, heightmapCPU);
 	//glGetTexImage(GL_TEXTURE_2D,0,GL_R32F,GL_FLOAT,&Pixels);
	/*float tmpMax =heightmapCPU[0];
	float tmpMin =heightmapCPU[0];
	for (int i=0; i<1024*1024 ; i++){
		if (tmpMax<=heightmapCPU[i]){
			tmpMax=heightmapCPU[i];
		}
		if (tmpMin>=heightmapCPU[i]){
			tmpMin=heightmapCPU[i];
		}
	}
	std::cout<<tmpMax<<" "<<tmpMin<<endl;*/
}


void deCasteljau4PointsInit(){
	int i = 0;
	for(double t=0 ; t<=1 ; t = t + 0.001){
		//set control points
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

		//new control
		double b0X = -0.51f;
		double b0Y =  1.09f;
		double b0Z =  0.40f;

		double b1X =  0.57f;
		double b1Y =  1.26f;
		double b1Z =  0.40f;

		double b2X = 1.31f;
		double b2Y = 0.92f;
		double b2Z = 0.40f;

		double b3X = 1.25f;
		double b3Y = -0.41f;
		double b3Z = 0.40f;

		double posX = powf((1-t),3) * b0X + 3*t*powf((1-t),2) * b1X + 3*powf(t,2)*(1-t) *b2X+powf(t,3)*b3X;
		double posY = powf((1-t),3) * b0Y + 3*t*powf((1-t),2) * b1Y + 3*powf(t,2)*(1-t) *b2Y+powf(t,3)*b3Y;
		double posZ = powf((1-t),3) * b0Z + 3*t*powf((1-t),2) * b1Z + 3*powf(t,2)*(1-t) *b2Z+powf(t,3)*b3Z;

		bezierCurve[i]=vec3(posX,posY,posZ);
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

	// write heightmap in CPU 
	CopyHeightmapToCPU(heightMapTexID);
    

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

	//create beziercurve
	deCasteljau4PointsInit();
}

void update_camera_modelview(double posX,double posY,double posZ,double lookX,double lookY,double lookZ){
	vec3 camPos(posX,posY,posZ);
	vec3 camLookAt(lookX, lookY, lookZ);
	vec3 camUp(0.0f,0.0f,1.0f);

	mat4 view = Eigen::lookAt(camPos, camLookAt, camUp);
	cameraModelview = view;
	//std::cout<<(powf(posX-lookX,2)+powf(posY-lookY,2)+powf(posZ-lookZ,2))<<endl;

	/*
	std::cout<<endl<<endl<<"update"<<endl<<endl;
	std::cout<<posX-lookX<<" "<<posY-lookY<<" "<<posZ-lookZ<<endl;
	std::cout<<powf(posX-lookX,2)<<" "<<powf(posY-lookY,2)<<" "<<powf(posZ-lookZ,2)<<endl;
	std::cout<<powf(posX-lookX,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))<<" "<<powf(posY-lookY,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))<<endl;
*/
}

void rotate2D(double pos1, double pos2, double& look1, double& look2, double angle){
	double tmp1 = (look1-pos1)*cos(angle) - (look2-pos2)*sin(angle) + pos1;
	double tmp2 = (look1-pos1)*sin(angle) + (look2-pos2)*cos(angle) + pos2;
	look1=tmp1;
	look2=tmp2;
}

void rotate3D(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double& recordRotZ){
	double tmpLookX = lookX ; 
	double tmpLookY = lookY ; 
	double tmpLookZ = lookZ ; 
	//go back to origine
	tmpLookX -=posX;
	tmpLookY -=posY;
	tmpLookZ -=posZ;
	
	//compute angle to return
	double returnRecordedRotY = fmod((6.2831-recordRotY),6.2831);
	double returnRecordedRotZ = fmod((6.2831-recordRotZ),6.2831);

	double newLookX = tmpLookX*cos(returnRecordedRotY)+tmpLookY*sin(returnRecordedRotZ)*sin(returnRecordedRotY)+tmpLookZ*sin(returnRecordedRotY)*cos(returnRecordedRotZ);
	double newLookY = tmpLookY*cos(returnRecordedRotZ)-tmpLookZ*sin(returnRecordedRotZ);
	double newLookZ = -tmpLookX*sin(returnRecordedRotY)+tmpLookY*cos(returnRecordedRotY)*sin(returnRecordedRotZ)+tmpLookZ*cos(returnRecordedRotY)*cos(returnRecordedRotZ);
	
	double newLookX2 = newLookX*cos(recordRotY)+newLookY*sin(recordRotZ)*sin(recordRotY)+newLookZ*sin(recordRotY)*cos(recordRotZ);
	double newLookY2 = newLookY*cos(recordRotZ)-newLookZ*sin(recordRotZ);
	double newLookZ2 = -newLookX*sin(recordRotY)+newLookY*cos(recordRotY)*sin(recordRotZ)+newLookZ*cos(recordRotY)*cos(recordRotZ);
	

	lookX = newLookX2 + posX;
	lookY = newLookY2 + posY;
	lookZ = newLookZ2 + posZ;

}
void rotateLeftRight(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double& recordRotZ,double velocity){
		
		//1 cancel Y rotation
		double tmpX =lookX;
		double tmpZ =lookZ;
		rotate2D(posX,posZ,tmpX,tmpZ, fmod((6.2831-recordRotY),6.2831));
		
		//2 cancel z rotation
		double tmpY = lookY;
		rotate2D(posX,posY,tmpX,tmpY, fmod((6.2831-recordRotZ),6.2831));

		//3 rotate on Z axis
		double Angle = 0.0174f*velocity; // more or less 1 degree
		rotate2D(posX,posY,tmpX,tmpY,Angle);
	
		//4 redo Z rotation
		rotate2D(posX,posY,tmpX,tmpY, recordRotZ);

		//5 redo Y rotation
		rotate2D(posX,posZ,tmpX,tmpZ,recordRotY);
		
		lookX = tmpX;
		lookY = tmpY;
		lookZ = tmpZ;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}
void rotateUpDown(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double& recordRotZ,double velocity){
		
		//1 cancel Y rotation
		double tmpX =lookX;
		double tmpZ =lookZ;
		rotate2D(posX,posZ,tmpX,tmpZ, fmod((6.2831-recordRotY),6.2831));
		
		//2 cancel z rotation
		double tmpY = lookY;
		rotate2D(posX,posY,tmpX,tmpY, fmod((6.2831-recordRotZ),6.2831));

		//3 aply rotation arround Y
		double Angle = 0.0174f*velocity; // more or less 1 degree
		rotate2D(posX,posZ,tmpX,tmpZ, Angle);

		//4 redo Z rotation
		rotate2D(posX,posY,tmpX,tmpY, recordRotZ);

		//5 redo Y rotation
		rotate2D(posX,posZ,tmpX,tmpZ,recordRotY);
		
		lookX = tmpX;
		lookY = tmpY;
		lookZ = tmpZ;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}
/*
void rotateLeftRight(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double velocity){
		
		//1 cancel Y rotation
		double tmpX =lookX;
		double tmpZ =lookZ;
		rotate2D(posX,posZ,tmpX,tmpZ, fmod((6.2831-recordRotY),6.2831));

		//2 rotate on Z axis
		double Angle = 0.0174f*velocity; // more or less 1 degree
		double tmpY  = lookY;
		rotate2D(posX,posY,tmpX,tmpY,Angle);
	
		//3 redo Y rotation
		rotate2D(posX,posZ,tmpX,tmpZ,recordRotY);
		
		lookX = tmpX;
		lookY = tmpY;
		lookZ = tmpZ;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}*/
/*
void rotateUpDown(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotZ,double velocity){

		//1 cancel Z rotation
		double tmpX = lookX;
		double tmpY = lookY;
		rotate2D(posX,posY,tmpX,tmpY, fmod((6.2831-recordRotZ),6.2831));
		//2 rotate on Y axis
		double Angle = 0.0174f*velocity; // more or less 1 degree
		double tmpZ = lookZ;
		rotate2D(posX,posZ,tmpX,tmpZ, Angle);
		//3 redo Z rotation
		rotate2D(posX,posY,tmpX,tmpY, recordRotZ);
		
		lookX = tmpX ;
		lookY = tmpY ;
		lookZ = tmpZ ;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}*/

void moveAlongAxis(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ, double velocity){
		
		double dispX = (posX - lookX)*velocity;
		double dispY = (posY - lookY)*velocity;
		double dispZ = (posZ - lookZ)*velocity;
		
		posX = posX - dispX; 
		posY = posY - dispY; 
		posZ = posZ - dispZ; 

		lookX = lookX - dispX; 
		lookY = lookY - dispY; 
		lookZ = lookZ - dispZ;

		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void fpsExplorationForwardBackward(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double dispX,double dispY){
	posX = posX - dispX; 
	posY = posY - dispY; 
	lookX = lookX - dispX; 
	lookY = lookY - dispY; 
	if(posX>1){posX=1;}
	else if(posX<-1){posX=-1;}
	if(posY>1){posY=1;}
	else if(posY<-1){posY=-1;}

	int tmpX = int((1+posX)*1024/2);
	int tmpY = int((1+posY)*1024/2);
	tmpX = tmpX -2;
	tmpY = tmpY -2;
	double tmpZ=0;
	double AmountOfPoints = 0; 
	for(int i=0;i<5;i++){//take in count 24 z values arround to have a smooth move 
		for(int j=0;j<5;j++){
			tmpX = tmpX + i;
			tmpY = tmpY + j;
			if (tmpX>0 & tmpX<1025 & tmpY>0 &tmpY<1025){
				tmpZ = tmpZ + heightmapCPU[tmpX+1024*tmpY];
				AmountOfPoints++;
			}
		}
	}
	//std::cout<<"AmountOfPoints ="<<AmountOfPoints<<endl;
	tmpZ=tmpZ/AmountOfPoints;
	posZ = tmpZ+0.06; //fps person height
	//lookZ = posZ+0.06;
	update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void deCasteljauTest3Points(){

	static double t=0;

	if(t<=1){
		//set control points
		double b0X = 1.0f;
		double b0Y = 1.0f;
		double b0Z = 0.4f;

		double b1X = 1.0f;
		double b1Y = -1.0f;
		double b1Z = 0.4f;

		double b2X = -1.0f;
		double b2Y = -1.0f;
		double b2Z = 0.4f;

		double posX = powf((1-t),2) * b0X + 2*t*(1-t) * b1X + powf(t,2) *b2X;
		double posY = powf((1-t),2) * b0Y + 2*t*(1-t) * b1Y + powf(t,2) *b2Y;
		double posZ = powf((1-t),2) * b0Z + 2*t*(1-t) * b1Z + powf(t,2) *b2Z;
	
		double lookX = 0.0f;
		double lookY = 0.0f;
		double lookZ = 0.3f;
		t = t+0.001;
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
	}
	else{
		t=0;
	}

}

void deCasteljauTest4Points(){

	static double t=0;

	if(t<=1){
		//set control points
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

		//new control
		double b0X = -0.51f;
		double b0Y =  1.09f;
		double b0Z =  0.40f;

		double b1X =  0.57f;
		double b1Y =  1.26f;
		double b1Z =  0.40f;

		double b2X = 1.31f;
		double b2Y = 0.92f;
		double b2Z = 0.40f;

		double b3X = 1.25f;
		double b3Y = -0.41f;
		double b3Z = 0.40f;

		double posX = powf((1-t),3) * b0X + 3*t*powf((1-t),2) * b1X + 3*powf(t,2)*(1-t) *b2X+powf(t,3)*b3X;
		double posY = powf((1-t),3) * b0Y + 3*t*powf((1-t),2) * b1Y + 3*powf(t,2)*(1-t) *b2Y+powf(t,3)*b3Y;
		double posZ = powf((1-t),3) * b0Z + 3*t*powf((1-t),2) * b1Z + 3*powf(t,2)*(1-t) *b2Z+powf(t,3)*b3Z;
		
	
		double lookX = 0.0f;
		double lookY = 0.0f;
		double lookZ = 0.3f;
		t = t+0.001;
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
	}
	else{
		t=0;
	}

}

void handleKeyboard(){
	
	/*static double posX =  0.0f; 
	static double posY =  0.15f;
	static double posZ =  0.5f;

	static double lookX = 1.0f;
	static double lookY = 0.1f;
	static double lookZ = 0.5f;*/

	//fps exploration
	static double posX =  0.78f;
	static double posY =  0.42f;
	static double posZ =  0.30f;

	static double lookX = -0.24f;
	static double lookY = 0.19f;
	static double lookZ = 0.13f;

	static double velocityForward = 0;
	static double velocityBackward = 0;
	static double velocityLeft = 0;
	static double velocityRight = 0;
	static double velocityUp = 0;
	static double velocityDown = 0;

	static double recordRotZ = 0;
	static double recordRotY = 0;

	static bool jumping = false;
	static double initJumpPosZ = 0;
	static double initJumpLookZ = 0;
	static double jumpLevel =0;
	//deCasteljauTest4Points();
//	std::cout<<fmod((6.2831-recordRotY),6.2831)<<endl;
	//std::cout<<(recordRotY)<<" "<<(recordRotZ)<<endl;
	//std::cout<<posX<<" "<<posY<<" "<<posZ<<" "<<lookX<<" "<<lookY<<" "<<lookZ<<" "<<endl;
	
	if(glfwGetKey(32)==1 & jumping==false){//space => jump
			jumping = true;
			initJumpPosZ = posZ;
			initJumpLookZ = lookZ;
	}
	if (jumping ==true ){

		if(jumpLevel>80 & jumpLevel<100){
			jumpLevel +=1;
		}
		else if ((jumpLevel>60 & jumpLevel<=80)|(jumpLevel>=100 & jumpLevel<120)){
			jumpLevel +=1.5;
		}
		else{
			jumpLevel +=2.5;
		}
		//jumpLevel += 1;
		
		//estimate floor level 8only if mooving while jumping
		int tmpX = int((1+posX)*1024/2);
		int tmpY = int((1+posY)*1024/2);
		tmpX = tmpX -2;
		tmpY = tmpY -2;
		double tmpZ=0;
		double AmountOfPoints = 0; 
		for(int i=0;i<5;i++){//take in count 24 z values arround to have a smooth move 
			for(int j=0;j<5;j++){
				tmpX = tmpX + i;
				tmpY = tmpY + j;
				if (tmpX>0 & tmpX<1025 & tmpY>0 &tmpY<1025){
					tmpZ = tmpZ + heightmapCPU[tmpX+1024*tmpY];
					AmountOfPoints++;
				}
			}
		}
		//std::cout<<"AmountOfPoints ="<<AmountOfPoints<<endl;
		tmpZ=tmpZ/AmountOfPoints;
		double floorLevel = tmpZ+0.06; //fps person height

		if (jumpLevel >= 90 & posZ<=floorLevel){ //reach floor =>jump finished
			jumping = false;
			jumpLevel = 0;
			posZ = floorLevel;// 0.06 person height
			std::cout<<"jump end"<<endl;
		}
		else if(jumpLevel > 180 ){// falling ! 
			posZ -= 0.005f; 
		}
		else{
			posZ = initJumpPosZ + 0.1 * sin(0.0174f*jumpLevel);
			lookZ = initJumpLookZ + 0.1 * sin(0.0174f*jumpLevel);
		}

		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
	}
	if (glfwGetKey(87)==1 | velocityForward > 0.0f){//W pressed => go forward 
		if(glfwGetKey(87)==1 & velocityForward<0.01f){
				velocityForward = velocityForward + 0.0005f;
		}
		else{
			velocityForward = velocityForward - 0.0005f;
		}

		if(glfwGetKey(287) ==1 & glfwGetKey(87)==1 & velocityForward<0.02f){ // press shift => running
			velocityForward +=0.01;
		}
		
		//Flying exploration
		//moveAlongAxis(posX,posY,posZ,lookX,lookY,lookZ,velocityForward);

		//FPS exploration
		if(posX<=1 & posX>=-1 & posY<=1 & posY>=-1 & jumping == false){//stay on the map
			double dispX = 0.2f*velocityForward*powf(posX-lookX,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posX-lookX)/abs(posX-lookX);
			double dispY = 0.2f*velocityForward*powf(posY-lookY,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posY-lookY)/abs(posY-lookY);
			fpsExplorationForwardBackward(posX,posY,posZ,lookX,lookY,lookZ,dispX,dispY);
		}
		else if(posX<=1 & posX>=-1 & posY<=1 & posY>=-1 & jumping == true){//stay on the map jumping forward
			double dispX = 0.2f*velocityForward*powf(posX-lookX,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posX-lookX)/abs(posX-lookX);
			double dispY = 0.2f*velocityForward*powf(posY-lookY,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posY-lookY)/abs(posY-lookY);
			posX = posX - dispX; 
			posY = posY - dispY; 
			lookX = lookX - dispX; 
			lookY = lookY - dispY; 
			if(posX>1){posX=1;}
			else if(posX<-1){posX=-1;}
			if(posY>1){posY=1;}
			else if(posY<-1){posY=-1;}
			update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
		}
		
	}
	if  (glfwGetKey(83)==1 | velocityBackward != 0.0f){//S pressed => go backward 
		if(glfwGetKey(83)==1 & velocityBackward<0.01f){
				velocityBackward = velocityBackward + 0.0005f;
		}
		else{
			velocityBackward = velocityBackward - 0.0005f;
		}
		//flying exploration
		//moveAlongAxis(posX,posY,posZ,lookX,lookY,lookZ,-velocityBackward);
		
		//fps exploration
		if(posX<=1 & posX>=-1 & posY<=1 & posY>=-1){//stay on the map
			double dispX = 0.2f*velocityBackward*powf(posX-lookX,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posX-lookX)/abs(posX-lookX);
			double dispY = 0.2f*velocityBackward*powf(posY-lookY,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posY-lookY)/abs(posY-lookY);
			fpsExplorationForwardBackward(posX,posY,posZ,lookX,lookY,lookZ,-dispX,-dispY);
		}
	}
	if (glfwGetKey(65)==1 | velocityLeft != 0.0f){//A pressed => turn left
		if(glfwGetKey(65)==1 & velocityLeft<1.0f){
				velocityLeft = velocityLeft + 0.01f;
		}
		else{
			velocityLeft = velocityLeft - 0.01f;
		}
		rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,velocityLeft);
	
		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,velocityLeft);
		recordRotZ = fmod((recordRotZ+0.0174f*velocityLeft),6.2831); //save rotation done
	}
	if (glfwGetKey(68)==1 | velocityRight != 0.0f){//D pressed =turn right
		if(glfwGetKey(68)==1 & velocityRight<1.0f){
				velocityRight = velocityRight + 0.01f;
		}
		else{
			velocityRight = velocityRight - 0.01f;
		}
		rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,-velocityRight);
	
		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,-velocityRight);
		recordRotZ = fmod((recordRotZ-0.0174f*velocityRight),6.2831); //save rotation done
	}
	if  (glfwGetKey(81)==1  | velocityUp != 0.0f){//Q pressed turn up
		if(glfwGetKey(81)==1 & velocityUp<1.0f){
				velocityUp = velocityUp + 0.01f;
		}
		else{
			velocityUp = velocityUp - 0.01f;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);
		rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,velocityUp);
	
		recordRotY = fmod((recordRotY+0.0174f*velocityUp),6.2831); //save rotation done	
	}
	if  (glfwGetKey(69)==1  | velocityDown != 0.0f){//E pressed => down
		if(glfwGetKey(69)==1 & velocityDown<1.0f){
				velocityDown = velocityDown + 0.01f;
		}
		else{
			velocityDown = velocityDown - 0.01f;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
		rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,-velocityDown);
	
		recordRotY = fmod((recordRotY-0.0174f*velocityDown),6.2831); //save rotation done
	}
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

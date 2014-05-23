
#include "camera.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "camera_vshader.h"
#include "camera_fshader.h"


//save heightmap to cpu to read for fps exploration
GLfloat *heightmapCPU = new GLfloat[1024*1024];

//key status 
static bool KeyW = false; 
static bool KeyA = false; 
static bool KeyS = false; 
static bool KeyD = false;
static bool KeyE = false; 
static bool KeyQ = false; 
static bool KeySHIFT = false; 
static bool KeySPACE = false; 
static bool ModeFlying = false;
static bool ModeFps = false;
static bool ModeBCurve = false;
static bool ModeFix = true;

//Bezier curve 
static double bezierCurve[1000*3];

Camera::Camera(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


void Camera::init(Vertices* vertices) {

    /// Common initialization.
    RenderingContext::init(vertices, camera_vshader, camera_fshader, "vertexPosition3DModel", 0);

    // FIXME : points generated here and in vertices_bezier.
    InitdeCasteljau4Points();

    /// Set uniform IDs.
    _modelviewID = glGetUniformLocation(_programID, "modelview");
    _projectionID = glGetUniformLocation(_programID, "projection");

}


void Camera::draw(const mat4& projection, const mat4& modelview) const {

    /// Common drawing.
    RenderingContext::draw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_modelviewID, 1, GL_FALSE, modelview.data());
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());

    /// Do not clear the default framebuffer (screen) : done by Terrain.
    /// Otherwise already drawn pixels will be cleared.

    /// Render the Bézier curve from camera point of view to default framebuffer.
    _vertices->draw();

}


void Camera::CopyHeightmapToCPU(GLuint heightMapTexID){
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

void Camera::update_camera_modelview(double posX,double posY,double posZ,double lookX,double lookY,double lookZ){
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

void Camera::rotate2D(double pos1, double pos2, double& look1, double& look2, double angle){
	double tmp1 = (look1-pos1)*cos(angle) - (look2-pos2)*sin(angle) + pos1;
	double tmp2 = (look1-pos1)*sin(angle) + (look2-pos2)*cos(angle) + pos2;
	look1=tmp1;
	look2=tmp2;
}

void Camera::rotate3D(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double& recordRotZ){
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

void Camera::fpsRotateLeftRight(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double velocity){

		double tmpX = lookX-posX;
		double tmpY = lookY-posY;

		//3 rotate on Z axis
		double Angle = 0.0174f*velocity; // more or less 1 degree
		rotate2D(0,0,tmpX,tmpY,Angle);
			
		lookX = tmpX+posX;
		lookY = tmpY+posY;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void Camera::fpsRotateUpDown(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double recordRotZ, double velocity){
		
		double tmpX =lookX-posX;
		double tmpY =lookY-posY;
		double tmpZ =lookZ-posZ;
		
		//undo lateral (Z) rotation
		rotate2D(0,0,tmpX,tmpY,-recordRotZ);
		
		//proceed vertical (Y) rotation
		double Angle = 0.0174f*velocity; // more or less 1 degree

		rotate2D(0,0,tmpX,tmpZ, Angle);

		//redo lateral (Z) rotation
		rotate2D(0,0,tmpX,tmpY,recordRotZ);

		lookX = tmpX + posX;
		lookY = tmpY + posY;
		lookZ = tmpZ + posZ;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void Camera::rotateLeftRight(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double velocity){
		
		//1 move to the origine
		double tmpX = lookX - posX;
		double tmpY = lookY - posY;
		double tmpZ = lookZ - posZ;

		//2 cancel Y rotation
		rotate2D(0,0,tmpX,tmpZ, -recordRotY);

		//3 rotate on Z axis
		double Angle = 0.0174f*velocity; // more or less 1 degree
		rotate2D(0,0,tmpX,tmpY,Angle);
	
		//4 redo Y rotation
		rotate2D(0,0,tmpX,tmpZ,recordRotY);
		
		lookX = tmpX + posX;
		lookY = tmpY + posY;
		lookZ = tmpZ + posZ;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void Camera::rotateUpDown(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotZ,double velocity){

		//1 move to the origine
		double tmpX = lookX - posX;
		double tmpY = lookY - posY;
		double tmpZ = lookZ - posZ;

		//2 undo lateral (Z) rotation
		rotate2D(0,0,tmpX,tmpY,-recordRotZ);

		//3 rotate on Y axis
		double Angle = 0.0174f*velocity; // more or less 1 degree
		rotate2D(0,0,tmpX,tmpZ, Angle);

		//4 redo Z rotation
		rotate2D(0,0,tmpX,tmpY, recordRotZ);
		
		lookX = tmpX + posX ;
		lookY = tmpY + posY ;
		lookZ = tmpZ + posZ ;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void Camera::moveAlongAxis(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ, double velocity){
		
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

void Camera::fpsExplorationForwardBackward(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double dispX,double dispY){
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
			if ((tmpX>0) & (tmpX<1025) & (tmpY>0 &tmpY<1025)){
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

void Camera::deCasteljauTest3Points(){

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

void Camera::deCasteljauTest4Points(){
	static int i =0;

	if(i<1000){

		double posX	= bezierCurve[i*3+0];
		double posY	= bezierCurve[i*3+1];
		double posZ	= bezierCurve[i*3+2];

		double lookX = 0.0f;
		double lookY = 0.0f;
		double lookZ = 0.3f;
		i++;
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
	}
	else{
		i=0;
	}
	/*static double t=0;

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
		double b3Z = 0.40f;

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
	}*/

}

void Camera::InitdeCasteljau4Points(){

	double t=0;
	for (int i=0; i<1000; i++){
		//set control points
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
		
		bezierCurve[i*3+0] = posX;
		bezierCurve[i*3+1] = posY;
		bezierCurve[i*3+2] = posZ;
		t = t+0.001;
	}
}

void Camera::flyingExploration(){
	
	static double posX =  0.78f;
	static double posY =  0.42f;
	static double posZ =  0.30f;

	static double lookX = -0.24f;
	static double lookY = 0.19f;
	static double lookZ = 0.13f;
	//std::cout<<posX<<"  "<<posY<<"  "<<posZ<<std::endl;
	static double velocityForward = 0;
	static double velocityBackward = 0;
	static double velocityLeft = 0;
	static double velocityRight = 0;
	static double velocityUp = 0;
	static double velocityDown = 0;

	static double recordRotZ = 0;
	static double recordRotY = 0;

	static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaT = float(currentTime - lastTime); //deltaT in sc 
    lastTime = currentTime;

	if ((KeyW==true) | (velocityForward > 0.0f)){//W pressed => go forward 
		if((KeyW==1) & (velocityForward<0.5f*deltaT)){
				velocityForward = velocityForward + 0.1f * deltaT;
		}
		else{
			velocityForward = velocityForward - 0.1f*deltaT;
		}
		
		//Flying exploration
		moveAlongAxis(posX,posY,posZ,lookX,lookY,lookZ,velocityForward);

		
	}
	if  ((KeyS==true)| (velocityBackward > 0.0f)){//S pressed => go backward 
		if((KeyS==true) & (velocityBackward<0.5f*deltaT)){
				velocityBackward = velocityBackward + 0.1f*deltaT;
		}
		else{
			velocityBackward = velocityBackward - 0.1f*deltaT;
		}
		//flying exploration
		moveAlongAxis(posX,posY,posZ,lookX,lookY,lookZ,-velocityBackward);
		
	}
	if ((KeyA==true) | (velocityLeft > 0.0f)){//A pressed => turn left
		if((KeyA==true) & (velocityLeft<20.0f*deltaT)){
				velocityLeft = velocityLeft + 5.0f*deltaT;
		}
		else{
			velocityLeft = velocityLeft - 5.0f*deltaT;
		}
	//	rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,velocityLeft);
		rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,velocityLeft);

		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,velocityLeft);
		recordRotZ = fmod((recordRotZ+0.0174f*velocityLeft),6.2831); //save rotation done
	}
	if ((KeyD==true) | (velocityRight > 0.0f)){//D pressed =turn right
		if((KeyD==true) & (velocityRight<20.0f*deltaT)){
				velocityRight = velocityRight + 5.0f*deltaT;
		}
		else{
			velocityRight = velocityRight - 5.0f*deltaT;
		}
	//	rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,-velocityRight);
		rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,-velocityRight);
		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,-velocityRight);
		recordRotZ = fmod((recordRotZ-0.0174f*velocityRight),6.2831); //save rotation done
	}
	if  ((KeyQ==true)  | (velocityUp > 0.0f)){//Q pressed turn up
		if((KeyQ==true) & (velocityUp<20.0f*deltaT)){
				velocityUp = velocityUp + 5.0f*deltaT;
		}
		else{
			velocityUp = velocityUp - 5.0f*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);
	//	rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,velocityUp);
		rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);

		recordRotY = fmod((recordRotY+0.0174f*velocityUp),6.2831); //save rotation done	
	}
	if  ( (KeyE==true) | (velocityDown > 0.0f)){//E pressed => down
		if((KeyE==true) & (velocityDown<20.0f*deltaT)){
				velocityDown = velocityDown + 5.0f*deltaT;
		}
		else{
			velocityDown = velocityDown - 5.0f*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
	//	rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,-velocityDown);
		rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
		recordRotY = fmod((recordRotY-0.0174f*velocityDown),6.2831); //save rotation done
	}
}

void Camera::fpsExploration(){
	
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

	static bool jumping = false;
	static double initJumpPosZ = 0;
	static double initJumpLookZ = 0;
	static double jumpLevel =0;

	static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaT = float(currentTime - lastTime); //deltaT in sc 
    lastTime = currentTime;

	if((KeySPACE==true) & (jumping==false)){//space => jump
			jumping = true;
			initJumpPosZ = posZ;
			initJumpLookZ = lookZ;
	}
	if (jumping ==true ){

		if((jumpLevel>80) & (jumpLevel<100)){
			jumpLevel +=1;
		}
		else if (((jumpLevel>60) & (jumpLevel<=80))|((jumpLevel>=100) & (jumpLevel<120))){
			jumpLevel +=1.5;
		}
		else{
			jumpLevel +=2.5;
		}
		
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
				if ((tmpX>0) & (tmpX<1025) & (tmpY>0) &(tmpY<1025)){
					tmpZ = tmpZ + heightmapCPU[tmpX+1024*tmpY];
					AmountOfPoints++;
				}
			}
		}
		//std::cout<<"AmountOfPoints ="<<AmountOfPoints<<endl;
		tmpZ=tmpZ/AmountOfPoints;
		double floorLevel = tmpZ+0.06; //fps person height

		if ((jumpLevel >= 90) & (posZ<=floorLevel)){ //reach floor =>jump finished
			jumping = false;
			jumpLevel = 0;
			posZ = floorLevel;// 0.06 person height
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
	if ((KeyW==true) | (velocityForward > 0.0f)){//W pressed => go forward 
		if((KeyW==1) & (velocityForward<0.5f*deltaT)){
				velocityForward = velocityForward + 0.1f*deltaT;
		}
		else{
			velocityForward = velocityForward - 0.1f*deltaT;
		}

		if((KeySHIFT==true) & (KeyW==true) & (velocityForward<0.02f)){ // press shift => running
			velocityForward +=0.5*deltaT;
		}
		
		//FPS exploration
		if((posX<=1) & (posX>=-1) & (posY<=1) & (posY>=-1) & (jumping == false)){//stay on the map
			double dispX = 0.2f*velocityForward*powf(posX-lookX,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posX-lookX)/abs(posX-lookX);
			double dispY = 0.2f*velocityForward*powf(posY-lookY,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posY-lookY)/abs(posY-lookY);
			fpsExplorationForwardBackward(posX,posY,posZ,lookX,lookY,lookZ,dispX,dispY);
		}
		else if((posX<=1) & (posX>=-1) & (posY<=1) & (posY>=-1) & (jumping == true)){//stay on the map jumping forward
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
	if  ((KeyS==true)| (velocityBackward > 0.0f)){//S pressed => go backward 
		if((KeyS==true) & (velocityBackward<0.5f*deltaT)){
				velocityBackward = velocityBackward + 0.1f*deltaT;
		}
		else{
			velocityBackward = velocityBackward - 0.1f*deltaT;
		}
		
		//fps exploration
		if((posX<=1) & (posX>=-1) & (posY<=1) & (posY>=-1)){//stay on the map
			double dispX = 0.2f*velocityBackward*powf(posX-lookX,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posX-lookX)/abs(posX-lookX);
			double dispY = 0.2f*velocityBackward*powf(posY-lookY,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posY-lookY)/abs(posY-lookY);
			fpsExplorationForwardBackward(posX,posY,posZ,lookX,lookY,lookZ,-dispX,-dispY);
		}
	}
	if ((KeyA==true) | (velocityLeft > 0.0f)){//A pressed => turn left
		if((KeyA==true) & (velocityLeft<20.0f*deltaT)){
				velocityLeft = velocityLeft + 2.0f*deltaT;
		}
		else{
			velocityLeft = velocityLeft - 2.0f*deltaT;
		}
		fpsRotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,velocityLeft);
		recordRotZ = fmod((recordRotZ+0.0174f*velocityRight),6.2831); //save rotation done
	
		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,velocityLeft);
	}
	if ((KeyD==true) | (velocityRight > 0.0f)){//D pressed =turn right
		if((KeyD==true) & (velocityRight<20.0f*deltaT)){
				velocityRight = velocityRight + 2.0f*deltaT;
		}
		else{
			velocityRight = velocityRight - 2.0f*deltaT;
		}
		fpsRotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,-velocityRight);
		recordRotZ = fmod((recordRotZ-0.0174f*velocityRight),6.2831); //save rotation done
	
		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,-velocityRight);
	}
	if  ((KeyQ==true)  | (velocityUp > 0.0f)){//Q pressed turn up
		if((KeyQ==true) & (velocityUp<20.0f*deltaT)){
				velocityUp = velocityUp + 2.0f*deltaT;
		}
		else{
			velocityUp = velocityUp - 2.0f*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);
		fpsRotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);
	}
	if  ( (KeyE==true) | (velocityDown > 0.0f)){//E pressed => down
		if((KeyE==true) & (velocityDown<20.0f*deltaT)){
				velocityDown = velocityDown + 2.0f*deltaT;
		}
		else{
			velocityDown = velocityDown - 2.0f*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
		fpsRotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
	}
}

void Camera::handleCamera(){
	if (ModeFlying == true){
		flyingExploration();
	}
	else if (ModeFps == true){
		fpsExploration();
	}
	else if (ModeBCurve == true){
		deCasteljauTest4Points();
	}
}

void Camera::handleCameraControls(int key, int action){
	//std::cout<<key<<"  "<<action<<std::endl;
	
	switch(key){
	case 87 :
		KeyW= !KeyW;
		break;
	case 65 :
		KeyA = !KeyA; 
		break;
	case 83 :
		KeyS = !KeyS; 
		break;
	case 68 :
		KeyD = !KeyD; 
		break;
	case 69 :
		KeyE = !KeyE; 
		break;
	case 81 :
		KeyQ = !KeyQ; 
		break;
	case 287 :
		KeySHIFT = !KeySHIFT; 
		break;
	case 32 :
		KeySPACE = !KeySPACE; 
		break;
	case 303: //1  
		std::cout<<"Exploration in free flying mode"<<std::endl;
		ModeFlying = true;
		ModeFps = false;
		ModeBCurve = false;
		ModeFix = false;
		break;
	case 304: //2 
		std::cout<<"Exploration in fps mode"<<std::endl;
		ModeFlying = false;
		ModeFps = true;
		ModeBCurve = false;
		ModeFix = false;
		break;
	case 305: //3
		std::cout<<"Exploration in Bcurve mode"<<std::endl;
		ModeFlying = false;
		ModeFps = false;
		ModeBCurve = true;
		ModeFix = false;
		break;
	case 306: //4
		std::cout<<"Frontal view"<<std::endl;
		ModeFix = true;
		ModeFlying = false;
		ModeFps = false;
		ModeBCurve = false;
		/// Frontal view to observe falling particles.
		update_camera_modelview(0.0f,-4.8f,1.0f,0.0f,0.0f,1.0f);
	}
}

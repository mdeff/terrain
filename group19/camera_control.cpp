
#include "camera_control.h"
#include "vertices_camera_path.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"
#include <Eigen/Geometry> 


//key status 
static bool KeyW = false;
static bool KeyA = false;
static bool KeyS = false;
static bool KeyD = false;
static bool KeyE = false;
static bool KeyQ = false;
static bool KeySHIFT = false;
static bool KeySPACE = false;
static bool Key1 = false;
static bool Key2 = false;
static bool KeyENTER = false;
static bool flagAnimatePictorialCamera=false;
		
static double pictorialCameraPos[3];
static double pictorialCameraLookat[3];

void CameraControl::init(VerticesCameraPath* verticesCameraPath, VerticesCameraPath* verticesCameraPathControls, GLuint heightMapTexID) {

    /// The camera control can change the camera path. It should thus have
    /// access to the path vertices object.
    _verticesCameraPath = verticesCameraPath;
    _verticesCameraPathControls = verticesCameraPathControls;

    /// Copy the heightmap texture to CPU. Used by FPS exploration mode to
    /// follow the terrain.
    _heightmapCPU = new float[1024*1024];
    glBindTexture(GL_TEXTURE_2D, heightMapTexID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, _heightmapCPU);

    /// Generate the camera path.
    //bezier_4_points(0, 0, 0, 0);
	//MultipleBezier();
	MultipleBezier_controlled(0,0, 0, 0);
   // InitdeCasteljauSubdivision();
    //InitSubdivision();

    /// Initialize the camera position.
    _explorationMode = TRACKBALL;
    trackball(mat4::Identity());

    /// HACK to show pictorial camera.
    /// Simple translation along z-axis for now.
    _cameraPictorialModel = mat4::Identity();
    _cameraPictorialModel(2,3) = 0.5f;

    /// Control point curently modified.
    _selectedControlPoint = 0;

}

void CameraControl::trackball(const mat4& model) {

    /// This camera control applies only in TRACKBALL exploration mode.
    /// Other modes have their own camera control.
    if(_explorationMode != TRACKBALL)
        return;

    /// View matrix (camera extrinsics) (position in world space).

    /// Camera is in the sky, looking down.

    vec3 camPos(0.5f, -0.5f, 0.8f);
	//vec3 camPos(2.5f, -2.5f, 2.8f);

//    vec3 camPos(0.0f, -1.5f, 0.8f);

//    vec3 camLookAt(0.0f, 0.0f, 0.0f);
//    vec3 camUp(0.0f, 0.0f, 1.0f);

    /// Camera is right on top (comparison with light position).
//    vec3 camPos = vec3(0.0, 0.0, 5.0);
//    vec3 camLookAt = vec3(0.0, 0.0, 0.0);
//    vec3 camUp = vec3(1.0, 0.0, 0.0);

    /// Global view from outside.
//    vec3 camPos(0.0f, -3.0f, 4.0f);
//    vec3 camLookAt(0.0f, 0.0f, 0.0f);
//    vec3 camUp(0.0f, 0.0f, 1.0f);

    /// Frontal view to observe falling particles.
//    vec3 camPos(0.0f, -4.8f, 1.0f);
//    vec3 camLookAt(0.0f, 0.0f, 0.2f);
//    vec3 camUp(0.0f, 0.0f, 1.0f);

    /// Frontal view to observe water reflection.
    //vec3 camPos(0.0f, -2.8f, 1.7f);
    vec3 camLookAt(0.0f, 0.0f, 0.0f);
    vec3 camUp(0.0f, 0.0f, 1.0f);

    /// FPS exploration.
//	  vec3 camPos(0.78f, 0.42f, 0.30f);
//    vec3 camLookAt(-0.24f, 0.19f, 0.13f);
//    vec3 camUp(0.0f, 0.0f, 1.0f);

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
    _cameraModelview = view * model;

}

void CameraControl::update_camera_modelview(double posX,double posY,double posZ,double lookX,double lookY,double lookZ){
	vec3 camPos(posX,posY,posZ);
	vec3 camLookAt(lookX, lookY, lookZ);
	//std::cout<<posX<<" "<<posY<<" "<<posZ<<" "<<lookX<<" "<<lookY<<" "<<lookZ<<" "<<std::endl;
	//std::cout<<posX-lookX<<" "<<posY-lookY<<" "<<posZ-lookZ<<std::endl;
	//vec3 camUp(posX-lookX,posY-lookY,posZ-lookZ);
	vec3 camUp(0.0f,0.0f,1.0f);
	
    _cameraModelview = Eigen::lookAt(camPos, camLookAt, camUp);

	//std::cout<<(powf(posX-lookX,2)+powf(posY-lookY,2)+powf(posZ-lookZ,2))<<endl;

	/*
	std::cout<<endl<<endl<<"update"<<endl<<endl;
	std::cout<<posX-lookX<<" "<<posY-lookY<<" "<<posZ-lookZ<<endl;
	std::cout<<powf(posX-lookX,2)<<" "<<powf(posY-lookY,2)<<" "<<powf(posZ-lookZ,2)<<endl;
	std::cout<<powf(posX-lookX,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))<<" "<<powf(posY-lookY,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))<<endl;
*/
}

void CameraControl::rotate2D(double pos1, double pos2, double& look1, double& look2, double angle){
	double tmp1 = (look1-pos1)*cos(angle) - (look2-pos2)*sin(angle) + pos1;
	double tmp2 = (look1-pos1)*sin(angle) + (look2-pos2)*cos(angle) + pos2;
	look1=tmp1;
	look2=tmp2;
}

void CameraControl::rotate3D(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double& recordRotZ){
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

void CameraControl::fpsRotateLeftRight(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double velocity){

		double tmpX = lookX-posX;
		double tmpY = lookY-posY;

		//3 rotate on Z axis
		double Angle = 0.0174f*velocity; // more or less 1 degree
		rotate2D(0,0,tmpX,tmpY,Angle);
			
		lookX = tmpX+posX;
		lookY = tmpY+posY;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void CameraControl::fpsRotateUpDown(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double recordRotZ, double velocity){
		
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

void CameraControl::rotateLeftRight(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double velocity){
		
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

void CameraControl::rotateUpDown(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotZ,double velocity){

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

void CameraControl::moveAlongAxis(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ, double velocity){
		
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

void CameraControl::fpsExplorationForwardBackward(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double dispX,double dispY){
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
                tmpZ = tmpZ + _heightmapCPU[tmpX+1024*tmpY];
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

void CameraControl::deCasteljauTest3Points(){

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

void CameraControl::deCasteljauTest4Points(){ // wrong naming => use to follow the curve with the camera... 
	static int i = 0;
	
	static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaT = float(currentTime - lastTime); //deltaT in sc 

	if(deltaT>0.01){
		lastTime = currentTime;
    
        if(i<(_cameraPath.size()/3)-1){
			//std::cout<<i<<std::endl;
            double posX	= _cameraPath[i*3+0];
            double posY	= _cameraPath[i*3+1];
            double posZ	= _cameraPath[i*3+2];

            double lookX = _cameraPath[(i+1)*3+0];
            double lookY = _cameraPath[(i+1)*3+1];
            double lookZ = _cameraPath[(i+1)*3+2];
			i++;
			update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
		}
		else{
			i=0;
		}
	}
}

void CameraControl::animatePictorialCamera(){
	static int i = 0;
	
	static double lastTime = glfwGetTime();

    if(i<(_cameraPath.size()/3)-1){

	double currentTime = glfwGetTime();
    float deltaT = float(currentTime - lastTime); //deltaT in sc 
	if(deltaT>0.05){
		lastTime = currentTime;
			std::cout<<i<<std::endl;
			_cameraPictorialModel(0,3) = _cameraPath[i*3+0];
			_cameraPictorialModel(1,3) = _cameraPath[i*3+1];
			_cameraPictorialModel(2,3) = _cameraPath[i*3+2];
			
			float dirX= _cameraPath[(i+1)*3+0]-_cameraPath[i*3+0];
			float dirY= _cameraPath[(i+1)*3+1]-_cameraPath[i*3+1];
			float dirZ= _cameraPath[(i+1)*3+2]-_cameraPath[i*3+2];

			float length = sqrt(dirX*dirX+dirY*dirY+dirZ*dirZ);
			
			float dirXnorm = dirX/length;
			float dirYnorm = dirY/length;
			float dirZnorm = dirZ/length;

			float angleZ = atan2(dirYnorm,dirXnorm);
			float angleY = atan2(sqrt(dirYnorm*dirYnorm+dirXnorm*dirXnorm),dirZnorm);

			//vec3 vecDir = vec3(dirXnorm,dirYnorm,dirZnorm);
			//vec3 camDir = vec3(0,0,1);
			// 
			//vec3 crossT =(vecDir).cross((camDir));
				
			//float camera_angle = 0.1f*i;
			
			mat3 rot = Eigen::AngleAxisf(angleZ, Eigen::Vector3f::UnitZ()).toRotationMatrix() 
				* Eigen::AngleAxisf(angleY, Eigen::Vector3f::UnitY()).toRotationMatrix();
			
			for (int j=0; j<3;j++){
				for(int k=0; k<3;k++){
					_cameraPictorialModel(k,j)=rot(k,j);
				}
			}


		i++;
		}
		
    }
	else{
		i=0;
	}

}

void CameraControl::InitdeCasteljauSubdivision(){


	
	//control points
	double b0X = -0.51f, b0Y =  1.09f, b0Z = 0.40f;
	double b1X =  0.57f, b1Y =  1.26f, b1Z = 0.40f;
	double b2X =  1.31f, b2Y =  0.92f, b2Z = 0.40f;
	double b3X =  1.25f, b3Y = -0.41f, b3Z = 0.40f;

	double l0X = b0X, l0Y = b0Y, l0Z = b0Z;
	
	double l1X = 0.5 * (b0X + b1X);
	double l1Y = 0.5 * (b0Y + b1Y);
	double l1Z = 0.5 * (b0Z + b1Z);

	double l2X = 0.25 * (b0X + 2*b1X + b2X); 
	double l2Y = 0.25 * (b0Y + 2*b1Y + b2Y); 
	double l2Z = 0.25 * (b0Z + 2*b1Z + b2Z); 
	
	double l3X = 0.125 * (b0X + 3*b1X + 3*b2X + b3X);
	double l3Y = 0.125 * (b0Y + 3*b1Y + 3*b2Y + b3Y);
	double l3Z = 0.125 * (b0Z + 3*b1Z + 3*b2Z + b3Z);
	
	double r0X = l3X, r0Y = l3Y, r0Z = l3Z;

	double r1X = 0.25 * (b1X + 2*b2X + b3X); 
	double r1Y = 0.25 * (b1Y + 2*b2Y + b3Y); 
	double r1Z = 0.25 * (b1Z + 2*b2Z + b3Z); 

	double r2X = 0.5 * (b2X + b3X);
	double r2Y = 0.5 * (b2Y + b3Y);
	double r2Z = 0.5 * (b2Z + b3Z);
	
	double r3X = b3X, r3Y = b3Y, r3Z = b3Z;

	double tmp0X = l0X, tmp0Y=l0Y, tmp0Z=l0Z;
	double tmp1X = l1X, tmp1Y=l1Y, tmp1Z=l1Z;
	double tmp2X = l2X, tmp2Y=l2Y, tmp2Z=l2Z;
	double tmp3X = l3X, tmp3Y=l3Y, tmp3Z=l3Z;

	double t=0;
	for (int i=0; i<=500; i++){
	
		double posX = powf((1-t),3) * tmp0X + 3*t*powf((1-t),2) * tmp1X + 3*powf(t,2)*(1-t) *tmp2X + powf(t,3)*tmp3X;
		double posY = powf((1-t),3) * tmp0Y + 3*t*powf((1-t),2) * tmp1Y + 3*powf(t,2)*(1-t) *tmp2Y + powf(t,3)*tmp3Y;
		double posZ = powf((1-t),3) * tmp0Z + 3*t*powf((1-t),2) * tmp1Z + 3*powf(t,2)*(1-t) *tmp2Z + powf(t,3)*tmp3Z;
		
        _cameraPath[i*3+0] = posX;
        _cameraPath[i*3+1] = posY;
        _cameraPath[i*3+2] = posZ;
		//std::cout<<"1 "<<t<<" "<<posX<<" "<<posY<<std::endl;
	
		t = t+0.001;
	}

	tmp0X = r0X, tmp0Y=r0Y, tmp0Z=r0Z;
	tmp1X = r1X, tmp1Y=r1Y, tmp1Z=r1Z;
	tmp2X = r2X, tmp2Y=r2Y, tmp2Z=r2Z;
	tmp3X = r3X, tmp3Y=r3Y, tmp3Z=r3Z;
	for (int i=500; i<=1000; i++){

		double posX = powf((1-t),3) * tmp0X + 3*t*powf((1-t),2) * tmp1X + 3*powf(t,2)*(1-t) *tmp2X + powf(t,3)*tmp3X;
		double posY = powf((1-t),3) * tmp0Y + 3*t*powf((1-t),2) * tmp1Y + 3*powf(t,2)*(1-t) *tmp2Y + powf(t,3)*tmp3Y;
		double posZ = powf((1-t),3) * tmp0Z + 3*t*powf((1-t),2) * tmp1Z + 3*powf(t,2)*(1-t) *tmp2Z + powf(t,3)*tmp3Z;
		
        _cameraPath[i*3+0] = posX;
        _cameraPath[i*3+1] = posY;
        _cameraPath[i*3+2] = posZ;

		//std::cout<<"2 "<<t<<" "<<posX<<" "<<posY<<std::endl;
		
		t = t+0.001;
	}
}

void CameraControl::InitSubdivision(){

	//test time to compute
	static double lastTime = glfwGetTime();

	//control points
	//b0[iteration][XYZ]
	const int iteration = 8;
	const int nPoints = 256; //2^iteration
	double res0[nPoints][3]; //[3][2] = {{1,1},{2,2},{3,3}};
	double res1[nPoints][3];
	double res2[nPoints][3];
	double res3[nPoints][3];
	
	res0[0][0]=-0.51f; //b0X
	res1[0][0]=-0.43f; //b1X
	res2[0][0]= 0.31f; //b2X
	res3[0][0]= 0.55f; //b3X

	res0[0][1]=-0.91f; //b0Y
	res1[0][1]= 2.66f; //b1Y
	res2[0][1]=-2.08f; //b2Y
	res3[0][1]= 0.39f; //b3Y

	res0[0][2]= 0.20f; //b0Z
	res1[0][2]= 0.00f; //b1Z
	res2[0][2]= 0.50f; //b2Z
	res3[0][2]= 0.30f; //b3Z

	double b0,b1,b2,b3;
	double l0,l1,l2,l3,r1,r2,r3;
	for(int i=0; i<iteration;i++){ // iterate to get more set of points
		for(int j=powf(2,i)-1; j>=0;j--){
			for(int k=0;k<3;k++){//iterate for X Y Z
				b0=res0[j][k];
				b1=res1[j][k];
				b2=res2[j][k];
				b3=res3[j][k];

				Subdivision(b0,b1,b2,b3,l0,l1,l2,l3,r1,r2,r3);
				res0[j*2][k] =l0;
				res1[j*2][k] =l1;
				res2[j*2][k] =l2;
				res3[j*2][k] =l3;
				res0[j*2+1][k]=l3;
				res1[j*2+1][k]=r1;
				res2[j*2+1][k]=r2;
				res3[j*2+1][k]=r3;
			}
		}
	}

    _cameraPath.clear();
    /// To avoid vector resizing on every loop.
    _cameraPath.reserve(3*nPoints*3);

    /// Generate coordinates.
    for(int k=0; k<nPoints; ++k) {
        _cameraPath.push_back(res0[k][0]);
        _cameraPath.push_back(res0[k][1]);
        _cameraPath.push_back(res0[k][2]);

        _cameraPath.push_back(res1[k][0]);
        _cameraPath.push_back(res1[k][1]);
        _cameraPath.push_back(res1[k][2]);
		
        _cameraPath.push_back(res2[k][0]);
        _cameraPath.push_back(res2[k][1]);
        _cameraPath.push_back(res2[k][2]);
    }

    
	//end test time to compute
	double currentTime = glfwGetTime();
    float deltaT = float(currentTime - lastTime); //deltaT in sc 
    std::cout<<"subdivision computed in "<<deltaT<<" sc with N = "<< _cameraPath.size()<<" points"<<std::endl;
	lastTime = currentTime;

	/// Copy the vertices to GPU.
    _verticesCameraPath ->copy(_cameraPath.data(), _cameraPath.size());

}

void CameraControl::Subdivision(double b0,double b1, double b2,double b3, double& l0, double& l1, double& l2, double& l3 ,double& r1,double& r2, double& r3 ){
	l0 = b0;
	l1 = 0.5 * (b0 + b1);
	l2 = 0.25 * (b0 + 2*b1 + b2); 
	l3 = 0.125 * (b0 + 3*b1 + 3*b2 + b3);
	r1 = 0.25 * (b1 + 2*b2 + b3); 
	r2 = 0.5 * (b2 + b3);
	r3 = b3;
}

void CameraControl::bezier_4_points(int PointToChange, float deltaX, float deltaY, float deltaZ) {

    /// Bézier curve control points.
    static float b0X = -0.51f, b0Y = -0.91f, b0Z = 0.20f;
    static float b1X = -0.43f, b1Y =  2.66f, b1Z = 0.001f;
    static float b2X =  0.31f, b2Y = -2.08f, b2Z = 0.50f;
    static float b3X =  0.55f, b3Y =  0.39f, b3Z = 0.30f;

	switch(PointToChange){
	case 0:
        b0X += deltaX;
        b0Y += deltaY;
        b0Z += deltaZ;
		break;
	case 1:
        b1X += deltaX;
        b1Y += deltaY;
        b1Z += deltaZ;
		break;
	case 2:
        b2X += deltaX;
        b2Y += deltaY;
        b2Z += deltaZ;
		break;
	case 3:
        b3X += deltaX;
        b3Y += deltaY;
        b3Z += deltaZ;
		break;
    }

    /// Fill the control points vector.
    _cameraPathControls.clear();
    _cameraPathControls.reserve(12);
    _cameraPathControls.push_back(b0X);
    _cameraPathControls.push_back(b0Y);
    _cameraPathControls.push_back(b0Z);
    _cameraPathControls.push_back(b1X);
    _cameraPathControls.push_back(b1Y);
    _cameraPathControls.push_back(b1Z);
    _cameraPathControls.push_back(b2X);
    _cameraPathControls.push_back(b2Y);
    _cameraPathControls.push_back(b2Z);
    _cameraPathControls.push_back(b3X);
    _cameraPathControls.push_back(b3Y);
    _cameraPathControls.push_back(b3Z);

	/// Choose the resolution.
    const unsigned int nPoints = 200;

    /// To avoid vector resizing on every loop.
    _cameraPath.clear();
    _cameraPath.reserve(3*nPoints);

    /// Generate coordinates.
    for(int k=0; k<nPoints; ++k) {
        float t = float(k) / float(nPoints);
        _cameraPath.push_back(std::pow((1-t),3)*b0X + 3*t*std::pow((1-t),2)*b1X + 3*std::pow(t,2)*(1-t)*b2X + std::pow(t,3)*b3X);
        _cameraPath.push_back(std::pow((1-t),3)*b0Y + 3*t*std::pow((1-t),2)*b1Y + 3*std::pow(t,2)*(1-t)*b2Y + std::pow(t,3)*b3Y);
        _cameraPath.push_back(std::pow((1-t),3)*b0Z + 3*t*std::pow((1-t),2)*b1Z + 3*std::pow(t,2)*(1-t)*b2Z + std::pow(t,3)*b3Z);
    }

    /// Copy the vertices to GPU.
    _verticesCameraPath->copy(_cameraPath.data(), _cameraPath.size());
    _verticesCameraPathControls->copy(_cameraPathControls.data(), _cameraPathControls.size());

}

void CameraControl::createBCurve(){
    int AmountBcurve = int(_cameraPathControls.size()/12);
	
	/// Choose the resolution.
	const unsigned int nPoints = 300*AmountBcurve;

    _cameraPath.clear();
	/// To avoid vector resizing on every loop.
    _cameraPath.reserve(3*nPoints);

	for(int i=1;i<=AmountBcurve;i++){

		//other setting flying through
        float b0X = _cameraPathControls.at(i*12-12);
        float b0Y = _cameraPathControls.at(i*12-11);
        float b0Z = _cameraPathControls.at(i*12-10);
        float b1X = _cameraPathControls.at(i*12-9);
        float b1Y = _cameraPathControls.at(i*12-8);
        float b1Z = _cameraPathControls.at(i*12-7);
        float b2X = _cameraPathControls.at(i*12-6);
        float b2Y = _cameraPathControls.at(i*12-5);
        float b2Z = _cameraPathControls.at(i*12-4);
        float b3X = _cameraPathControls.at(i*12-3);
        float b3Y = _cameraPathControls.at(i*12-2);
        float b3Z = _cameraPathControls.at(i*12-1);

		/// Generate coordinates.
		for(int k=0; k<nPoints; ++k) {
			float t = float(k) / float(nPoints);
            _cameraPath.push_back(std::pow((1-t),3)*b0X + 3*t*std::pow((1-t),2)*b1X + 3*std::pow(t,2)*(1-t)*b2X + std::pow(t,3)*b3X);
            _cameraPath.push_back(std::pow((1-t),3)*b0Y + 3*t*std::pow((1-t),2)*b1Y + 3*std::pow(t,2)*(1-t)*b2Y + std::pow(t,3)*b3Y);
            _cameraPath.push_back(std::pow((1-t),3)*b0Z + 3*t*std::pow((1-t),2)*b1Z + 3*std::pow(t,2)*(1-t)*b2Z + std::pow(t,3)*b3Z);
		}
	}
    /// Copy the vertices to GPU.
    _verticesCameraPath ->copy(_cameraPath.data(), _cameraPath.size());

}

void CameraControl::MultipleBezier() {
	
	//// setting first bezier curve
	const float bA0X = -1.00f,bA0Y =  0.00f,bA0Z =  0.40f;
    const float bA1X = -0.50f,bA1Y = -1.00f,bA1Z =  0.40f;
    const float bA2X = -0.50f,bA2Y =  1.00f,bA2Z =  0.40f;
    const float bA3X =  0.00f,bA3Y =  0.00f,bA3Z =  0.40f;

	//// setting second bezier curve
	const float bB0X =  bA3X ,bB0Y =  bA3Y ,bB0Z =  bA3Z ; // PB0 = PA3
    const float bB1X = 2*bB0X - bA2X;//PB1 = 2PB0 - PA2
	const float bB1Y = 2*bB0Y - bA2Y;//PB1 is the PA2 mirrored on PB0
	const float bB1Z = 2*bB0Z - bA2Z;
    const float bB2X =  0.50f,bB2Y =  1.00f,bB2Z =  0.40f;
    const float bB3X =  1.00f,bB3Y =  0.00f,bB3Z =  0.40f;

    /// Choose the resolution.
    const unsigned int nPoints = 350;

    _cameraPath.clear();
    /// To avoid vector resizing on every loop.
    _cameraPath.reserve(2*3*nPoints);

    /// Generate coordinates of first bezier curve.
    for(int k=0; k<nPoints; ++k) {
        float t = float(k) / float(nPoints);
        _cameraPath.push_back(std::pow((1-t),3)*bA0X + 3*t*std::pow((1-t),2)*bA1X + 3*std::pow(t,2)*(1-t)*bA2X + std::pow(t,3)*bA3X);
        _cameraPath.push_back(std::pow((1-t),3)*bA0Y + 3*t*std::pow((1-t),2)*bA1Y + 3*std::pow(t,2)*(1-t)*bA2Y + std::pow(t,3)*bA3Y);
        _cameraPath.push_back(std::pow((1-t),3)*bA0Z + 3*t*std::pow((1-t),2)*bA1Z + 3*std::pow(t,2)*(1-t)*bA2Z + std::pow(t,3)*bA3Z);
    }
	/// Generate coordinates of second bezier curve.
	for(int k=1; k<nPoints; ++k) {
        float t = float(k) / float(nPoints);
        _cameraPath.push_back(std::pow((1-t),3)*bB0X + 3*t*std::pow((1-t),2)*bB1X + 3*std::pow(t,2)*(1-t)*bB2X + std::pow(t,3)*bB3X);
        _cameraPath.push_back(std::pow((1-t),3)*bB0Y + 3*t*std::pow((1-t),2)*bB1Y + 3*std::pow(t,2)*(1-t)*bB2Y + std::pow(t,3)*bB3Y);
        _cameraPath.push_back(std::pow((1-t),3)*bB0Z + 3*t*std::pow((1-t),2)*bB1Z + 3*std::pow(t,2)*(1-t)*bB2Z + std::pow(t,3)*bB3Z);
    }
	 /// Copy the vertices to GPU.
    _verticesCameraPath ->copy(_cameraPath.data(), _cameraPath.size());

}

void CameraControl::MultipleBezier_controlled(int PointToChange, float deltaX, float deltaY, float deltaZ) {

	//// setting first bezier curve
	static float bA0X = -1.00f,bA0Y =  0.00f,bA0Z =  0.40f;
    static float bA1X = -0.50f,bA1Y = -1.00f,bA1Z =  0.40f;
    static float bA2X = -0.50f,bA2Y =  1.00f,bA2Z =  0.40f;
    static float bA3X =  0.00f,bA3Y =  0.00f,bA3Z =  0.40f;

	//// setting second bezier curve
	static float bB0X =  bA3X ,bB0Y =  bA3Y ,bB0Z =  bA3Z ; // PB0 = PA3
    static float bB1X = 2*bB0X - bA2X;//PB1 = 2PB0 - PA2
	static float bB1Y = 2*bB0Y - bA2Y;//PB1 is the PA2 mirrored on PB0
	static float bB1Z = 2*bB0Z - bA2Z;
    static float bB2X =  0.50f,bB2Y =  1.00f,bB2Z =  0.40f;
    static float bB3X =  1.00f,bB3Y =  0.00f,bB3Z =  0.40f;


	switch(PointToChange){
	case 0:
        bA0X += deltaX;
        bA0Y += deltaY;
        bA0Z += deltaZ;
		break;
	case 1:
        bA1X += deltaX;
        bA1Y += deltaY;
        bA1Z += deltaZ;
		break;
	case 2:
        bA2X += deltaX;
        bA2Y += deltaY;
        bA2Z += deltaZ;
		
		//refresh other control points 
		bB1X = 2*bB0X - bA2X;
		bB1Y = 2*bB0Y - bA2Y;
		bB1Z = 2*bB0Z - bA2Z;
		break;
	case 3:
        bA3X += deltaX;
        bA3Y += deltaY;
        bA3Z += deltaZ;

		//refresh other control points 
		bB0X =  bA3X ,bB0Y =  bA3Y ,bB0Z =  bA3Z ;

		bB1X = 2*bB0X - bA2X;
		bB1Y = 2*bB0Y - bA2Y;
		bB1Z = 2*bB0Z - bA2Z;
		break;
	case 4:
		bB1X += deltaX;
		bB1Y += deltaY;
		bB1Z += deltaZ;

		//refresh other control points 
		bA2X = - bB1X + 2*bB0X;
		bA2Y = - bB1Y + 2*bB0Y;
		bA2Z = - bB1Z + 2*bB0Z;
		break;
	case 5:
        bB2X += deltaX;
        bB2Y += deltaY;
        bB2Z += deltaZ;
		break;
	case 6:
        bB3X += deltaX;
        bB3Y += deltaY;
        bB3Z += deltaZ;
		break;
    }

    /// Fill the control points vector.
    _cameraPathControls.clear();
    _cameraPathControls.reserve(21);
    _cameraPathControls.push_back(bA0X);
    _cameraPathControls.push_back(bA0Y);
    _cameraPathControls.push_back(bA0Z);
    _cameraPathControls.push_back(bA1X);
    _cameraPathControls.push_back(bA1Y);
    _cameraPathControls.push_back(bA1Z);
    _cameraPathControls.push_back(bA2X);
    _cameraPathControls.push_back(bA2Y);
    _cameraPathControls.push_back(bA2Z);
    _cameraPathControls.push_back(bA3X);
    _cameraPathControls.push_back(bA3Y);
    _cameraPathControls.push_back(bA3Z);

	_cameraPathControls.push_back(bB1X);
    _cameraPathControls.push_back(bB1Y);
    _cameraPathControls.push_back(bB1Z);
    _cameraPathControls.push_back(bB2X);
    _cameraPathControls.push_back(bB2Y);
    _cameraPathControls.push_back(bB2Z);
    _cameraPathControls.push_back(bB3X);
    _cameraPathControls.push_back(bB3Y);
    _cameraPathControls.push_back(bB3Z);
	
    /// Choose the resolution.
    const unsigned int nPoints = 200;

    _cameraPath.clear();
    /// To avoid vector resizing on every loop.
    _cameraPath.reserve(2*3*nPoints);

    /// Generate coordinates of first bezier curve.
    for(int k=0; k<nPoints; ++k) {
        float t = float(k) / float(nPoints);
        _cameraPath.push_back(std::pow((1-t),3)*bA0X + 3*t*std::pow((1-t),2)*bA1X + 3*std::pow(t,2)*(1-t)*bA2X + std::pow(t,3)*bA3X);
        _cameraPath.push_back(std::pow((1-t),3)*bA0Y + 3*t*std::pow((1-t),2)*bA1Y + 3*std::pow(t,2)*(1-t)*bA2Y + std::pow(t,3)*bA3Y);
        _cameraPath.push_back(std::pow((1-t),3)*bA0Z + 3*t*std::pow((1-t),2)*bA1Z + 3*std::pow(t,2)*(1-t)*bA2Z + std::pow(t,3)*bA3Z);
    }
	/// Generate coordinates of second bezier curve.
	for(int k=1; k<nPoints; ++k) {
        float t = float(k) / float(nPoints);
        _cameraPath.push_back(std::pow((1-t),3)*bB0X + 3*t*std::pow((1-t),2)*bB1X + 3*std::pow(t,2)*(1-t)*bB2X + std::pow(t,3)*bB3X);
        _cameraPath.push_back(std::pow((1-t),3)*bB0Y + 3*t*std::pow((1-t),2)*bB1Y + 3*std::pow(t,2)*(1-t)*bB2Y + std::pow(t,3)*bB3Y);
        _cameraPath.push_back(std::pow((1-t),3)*bB0Z + 3*t*std::pow((1-t),2)*bB1Z + 3*std::pow(t,2)*(1-t)*bB2Z + std::pow(t,3)*bB3Z);
    }
	/// Copy the vertices to GPU.
    _verticesCameraPath->copy(_cameraPath.data(), _cameraPath.size());
    _verticesCameraPathControls->copy(_cameraPathControls.data(), _cameraPathControls.size());
}


void CameraControl::flyingExploration(){
	static double straightMaxSpeed = 0.5f; 	
	static double rotationMaxSpeed = 20.0f; 
	static double straightAcceleration = 0.05f; 	
	static double rotationAcceleration = 4.0f; 

	//To set user defined bcurve 
	static bool ModeSettingControlPoint=false;
	static int controlPointsCount = 0;
	//To tmp save the control point
	static double tmpCtrlPointX;
	static double tmpCtrlPointY;
	static double tmpCtrlPointZ;

	//starting position
	static double posX =  0.78f;
	static double posY =  0.42f;
	static double posZ =  0.30f;

	static double lookX = -0.24f;
	static double lookY = 0.19f;
	static double lookZ = 0.13f;

	//velocity of actions, nothing moves instanteanously
	static double velocityForward = 0;
	static double velocityBackward = 0;
	static double velocityLeft = 0;
	static double velocityRight = 0;
	static double velocityUp = 0;
	static double velocityDown = 0;

	//tmp save rotation done 
	static double recordRotZ = 0;
	static double recordRotY = 0;

	//use time to set movement speeds
	static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaT = float(currentTime - lastTime); //deltaT in sc 
    lastTime = currentTime;

	if ((KeyW==true) | (velocityForward > 0.0f)){//W pressed => go forward 
		if((KeyW==1) & (velocityForward<straightMaxSpeed*deltaT)){
				velocityForward = velocityForward + straightAcceleration * deltaT ;
		}
		else{
			velocityForward = velocityForward - straightAcceleration*deltaT;
		}
		
		//Flying exploration
		moveAlongAxis(posX,posY,posZ,lookX,lookY,lookZ,velocityForward);

	}
	if  ((KeyS==true)| (velocityBackward > 0.0f)){//S pressed => go backward 
		if((KeyS==true) & (velocityBackward<straightMaxSpeed*deltaT)){
				velocityBackward = velocityBackward + straightAcceleration*deltaT;
		}
		else{
			velocityBackward = velocityBackward - straightAcceleration*deltaT;
		}
		//flying exploration
		moveAlongAxis(posX,posY,posZ,lookX,lookY,lookZ,-velocityBackward);
		
	}
	if ((KeyA==true) | (velocityLeft > 0.0f)){//A pressed => turn left
		if((KeyA==true) & (velocityLeft<rotationMaxSpeed*deltaT)){
				velocityLeft = velocityLeft + rotationAcceleration*deltaT;
		}
		else{
			velocityLeft = velocityLeft - rotationAcceleration*deltaT;
		}
	//	rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,velocityLeft);
		rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,velocityLeft);

		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,velocityLeft);
		recordRotZ = fmod((recordRotZ+0.0174f*velocityLeft),6.2831); //save rotation done
	}
	if ((KeyD==true) | (velocityRight > 0.0f)){//D pressed =turn right
		if((KeyD==true) & (velocityRight<rotationMaxSpeed*deltaT)){
				velocityRight = velocityRight + rotationAcceleration*deltaT;
		}
		else{
			velocityRight = velocityRight - rotationAcceleration*deltaT;
		}
	//	rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,-velocityRight);
		rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,-velocityRight);
		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,-velocityRight);
		recordRotZ = fmod((recordRotZ-0.0174f*velocityRight),6.2831); //save rotation done
	}
	if  ((KeyQ==true)  | (velocityUp > 0.0f)){//Q pressed turn up
		if((KeyQ==true) & (velocityUp<rotationMaxSpeed*deltaT)){
				velocityUp = velocityUp + rotationAcceleration*deltaT;
		}
		else{
			velocityUp = velocityUp - rotationAcceleration*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);
	//	rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,velocityUp);
		rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);

		recordRotY = fmod((recordRotY+0.0174f*velocityUp),6.2831); //save rotation done	
	}
	if  ( (KeyE==true) | (velocityDown > 0.0f)){//E pressed => down
		if((KeyE==true) & (velocityDown<rotationMaxSpeed*deltaT)){
				velocityDown = velocityDown + rotationAcceleration*deltaT;
		}
		else{
			velocityDown = velocityDown - rotationAcceleration*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
	//	rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,-velocityDown);
		rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
		recordRotY = fmod((recordRotY-0.0174f*velocityDown),6.2831); //save rotation done
	}
	if(Key1==true){
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
	}
	if(KeyENTER==true & ModeSettingControlPoint==false){
		KeyENTER=false;
		ModeSettingControlPoint=true;
		/*if(controlPointsCount<=4){//restart creating path
			controlPointsCount=0;
            _cameraPathControls.clear();
		}*/
		if (controlPointsCount%4 == 0){
			if(controlPointsCount>2){// User continue to creat bcurve => push the last controlP of previous Bcurve P0 and the computed control P1
                _cameraPathControls.push_back(_cameraPathControls.at(_cameraPathControls.size()-3));
                _cameraPathControls.push_back(_cameraPathControls.at(_cameraPathControls.size()-3));
                _cameraPathControls.push_back(_cameraPathControls.at(_cameraPathControls.size()-3));
                _cameraPathControls.push_back(tmpCtrlPointX);
                _cameraPathControls.push_back(tmpCtrlPointY);
                _cameraPathControls.push_back(tmpCtrlPointZ);
				controlPointsCount=controlPointsCount+2;
			}
			else{ // User define first control point 
				std::cout<<"Control point 1 of bezier curve settled"<<std::endl;
                _cameraPathControls.push_back(posX-0.02f);
                _cameraPathControls.push_back(posY-0.02f);
                _cameraPathControls.push_back(posZ-0.02f);
				controlPointsCount++;
			}
		}
	}	
	if (ModeSettingControlPoint==true){ // set a control point if necessary
		//use look at to define the 2nd control point (P1) only for first Bcurve
        if (controlPointsCount%4==1 & _cameraPathControls.size()%12==3 & controlPointsCount<3 ){
            _cameraPathControls.push_back(lookX);
            _cameraPathControls.push_back(lookY);
            _cameraPathControls.push_back(lookZ);
		}//modify and refresh the look at to define the 2nd control point (P1) only for first Bcurve
        else if(controlPointsCount%4==1 & _cameraPathControls.size()%12==6 & controlPointsCount<3){
            _cameraPathControls.pop_back();
            _cameraPathControls.pop_back();
            _cameraPathControls.pop_back();
            _cameraPathControls.push_back(lookX);
            _cameraPathControls.push_back(lookY);
            _cameraPathControls.push_back(lookZ);
		}//set P2 and P3 using pos for every Bcurve created
        else if(controlPointsCount%4 ==2  & _cameraPathControls.size()%12==6){
            _cameraPathControls.push_back(lookX);
            _cameraPathControls.push_back(lookY);
            _cameraPathControls.push_back(lookZ);
            _cameraPathControls.push_back(posX-0.02f);
            _cameraPathControls.push_back(posY-0.02f);
            _cameraPathControls.push_back(posZ-0.02f);
		}//refresh P2 and P3 using pos for every Bcurve created
        else if(controlPointsCount%4==2 & _cameraPathControls.size()%12==0){
            _cameraPathControls.pop_back();
            _cameraPathControls.pop_back();
            _cameraPathControls.pop_back();
            _cameraPathControls.pop_back();
            _cameraPathControls.pop_back();
            _cameraPathControls.pop_back();
            _cameraPathControls.push_back(lookX);
            _cameraPathControls.push_back(lookY);
            _cameraPathControls.push_back(lookZ);
            _cameraPathControls.push_back(posX-0.02f);
            _cameraPathControls.push_back(posY-0.02f);
            _cameraPathControls.push_back(posZ-0.02f);
		}
        if(_cameraPathControls.size()>6){//compute and render bezier
			createBCurve();
		}
		if(KeyENTER==true){//finish setting ctrl points
			if(controlPointsCount%4==1){
				controlPointsCount++;
			}
			else if(controlPointsCount%4==2){ // a bcurve is completly settled
				controlPointsCount = controlPointsCount+2;
				ModeSettingControlPoint=false;
                std::cout<<int(_cameraPathControls.size()/12)<<" Bezier curves has been created!"<<std::endl;
			}
			if(controlPointsCount%4==0 & controlPointsCount>2){ // compute control P1 for next Bcurve, respecting linearity
                tmpCtrlPointX = 2*_cameraPathControls.at(_cameraPathControls.size()-3) - _cameraPathControls.at(_cameraPathControls.size()-6);
                tmpCtrlPointY = 2*_cameraPathControls.at(_cameraPathControls.size()-2) - _cameraPathControls.at(_cameraPathControls.size()-5);
                tmpCtrlPointZ = 2*_cameraPathControls.at(_cameraPathControls.size()-1) - _cameraPathControls.at(_cameraPathControls.size()-4);
			}										
			KeyENTER=false;							
		}
	}
}

void CameraControl::fpsExploration(){
	
	static double straightMaxSpeed = 0.5f; 	
	static double rotationMaxSpeed = 40.0f; 
	static double straightAcceleration = 0.05f; 	
	static double rotationAcceleration = 4.0f; 

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
					tmpZ = tmpZ + _heightmapCPU[tmpX+1024*tmpY];
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
		if((KeyW==1) & (velocityForward<straightMaxSpeed*deltaT)){
				velocityForward = velocityForward + straightAcceleration*deltaT;
		}
		else{
			velocityForward = velocityForward - straightAcceleration*deltaT;
		}

		if((KeySHIFT==true) & (KeyW==true) & (velocityForward<0.03f)){ // press shift => running
			velocityForward +=straightMaxSpeed*deltaT;
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
		if((KeyS==true) & (velocityBackward<straightMaxSpeed*deltaT)){
				velocityBackward = velocityBackward + straightAcceleration*deltaT;
		}
		else{
			velocityBackward = velocityBackward - straightAcceleration*deltaT;
		}
	
		//fps exploration
		if((posX<=1) & (posX>=-1) & (posY<=1) & (posY>=-1)){//stay on the map
			double dispX = 0.2f*velocityBackward*powf(posX-lookX,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posX-lookX)/abs(posX-lookX);
			double dispY = 0.2f*velocityBackward*powf(posY-lookY,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posY-lookY)/abs(posY-lookY);
			fpsExplorationForwardBackward(posX,posY,posZ,lookX,lookY,lookZ,-dispX,-dispY);
		}
	}
	if ((KeyA==true) | (velocityLeft > 0.0f)){//A pressed => turn left
		if((KeyA==true) & (velocityLeft<rotationMaxSpeed*deltaT)){
				velocityLeft = velocityLeft + rotationAcceleration*deltaT;
		}
		else{
			velocityLeft = velocityLeft - rotationAcceleration*deltaT;
		}
		fpsRotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,velocityLeft);
		recordRotZ = fmod((recordRotZ+0.0174f*velocityRight),6.2831); //save rotation done
	
		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,velocityLeft);
	}
	if ((KeyD==true) | (velocityRight > 0.0f)){//D pressed =turn right
		if((KeyD==true) & (velocityRight<rotationMaxSpeed*deltaT)){
				velocityRight = velocityRight + rotationAcceleration*deltaT;
		}
		else{
			velocityRight = velocityRight - rotationAcceleration*deltaT;
		}
		fpsRotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,-velocityRight);
		recordRotZ = fmod((recordRotZ-0.0174f*velocityRight),6.2831); //save rotation done
	
		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,-velocityRight);
	}
	if  ((KeyQ==true)  | (velocityUp > 0.0f)){//Q pressed turn up
		if((KeyQ==true) & (velocityUp<rotationMaxSpeed*deltaT)){
				velocityUp = velocityUp + rotationAcceleration*deltaT;
		}
		else{
			velocityUp = velocityUp - rotationAcceleration*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);
		fpsRotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);
	}
	if  ( (KeyE==true) | (velocityDown > 0.0f)){//E pressed => down
		if((KeyE==true) & (velocityDown<rotationMaxSpeed*deltaT)){
				velocityDown = velocityDown + rotationAcceleration*deltaT;
		}
		else{
			velocityDown = velocityDown - rotationAcceleration*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
		fpsRotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
	}
	if(Key2==true){
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
	}
}

void CameraControl::updateCameraPosition(mat4& cameraModelview, mat4& cameraPictorialModel, int& selectedControlPoint) {

    /// Modify camera position according to the exploration mode.
    switch(_explorationMode) {
    case FLYING:
        flyingExploration();
        break;
    case FPS:
        fpsExploration();
        break;
    case PATH:
        deCasteljauTest4Points();
        break;
    default:
        /// Nothing to do : trackball() will update the transformation matrix.
        break;
    }

	if(flagAnimatePictorialCamera==true)
        animatePictorialCamera();

    /// Update the view transformation matrix.
    cameraModelview = _cameraModelview;

    /// Update the camera pictorial model transformation matrix.
    cameraPictorialModel = _cameraPictorialModel;

    /// Update the selected control point.
    selectedControlPoint = _selectedControlPoint;

}

void CameraControl::handleCameraControls(int key, int action){
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
        std::cout << "Exploration mode : FLYING" << std::endl;
		Key1 =!Key1;
        _explorationMode = FLYING;
		break;
    case 304: //2
        std::cout << "Exploration mode : FPS" << std::endl;
        _explorationMode = FPS;
		Key2 =!Key2;
		break;
    case 305: //3
        std::cout << "Exploration mode : PATH" << std::endl;
        _explorationMode = PATH;
		break;
    case 306: //4
        std::cout << "Exploration mode : TRACKBALL" << std::endl;
        _explorationMode = TRACKBALL;
        trackball(mat4::Identity());
    }
	if(action==1){
		switch(key){
			case 90: //Z=> change control point under modification
                _selectedControlPoint++;
                if(_selectedControlPoint>6)
                    _selectedControlPoint=0;
                std::cout<<"Changing control point nB"<<_selectedControlPoint<<std::endl;
				std::cout<<"Control list :"<<std::endl<<"Key Z : change point to set"<<std::endl;
				std::cout<<"Key U : position X + 0.1"<<std::endl;
				std::cout<<"Key J : position X - 0.1"<<std::endl;
				std::cout<<"Key I : position Y + 0.1"<<std::endl;
				std::cout<<"Key K : position Y - 0.1"<<std::endl;
				std::cout<<"Key O : position Z + 0.1"<<std::endl;
				std::cout<<"Key L : position Z - 0.1"<<std::endl;

				break;
			case 85: //U=> X+
                //bezier_4_points(_selectedControlPoint,0.1f,0.0f,0.0f);
				MultipleBezier_controlled(_selectedControlPoint,0.1f,0.0f,0.0f);
				break;
			case 74: //J=> X-
                //bezier_4_points(_selectedControlPoint,-0.1f,0.0f,0.0f);
				MultipleBezier_controlled(_selectedControlPoint,-0.1f,0.0f,0.0f);
				break;
			case 73: //I=> Y+
                //bezier_4_points(_selectedControlPoint,0.0f,0.1f,0.0f);
				MultipleBezier_controlled(_selectedControlPoint,0.0f,0.1f,0.0f);
				break;
			case 75: //K=> Y-
                //bezier_4_points(_selectedControlPoint,0.0f,-0.1f,0.0f);
				MultipleBezier_controlled(_selectedControlPoint,0.0f,-0.1f,0.0f);
				break;
			case 79: //O=> Z+
                //bezier_4_points(_selectedControlPoint,0.0f,0.0f,0.1f);
				MultipleBezier_controlled(_selectedControlPoint,0.0f,0.0f,0.1f);
				break;
			case 76: //L=> Z-
                //bezier_4_points(_selectedControlPoint,0.0f,0.0f,-0.1f);
				MultipleBezier_controlled(_selectedControlPoint,0.0f,0.0f,-0.1f);
				break;
			case 307: //5
				InitSubdivision();
				break;
			case 308: //6
				MultipleBezier();
				break;
			case 294://ENTER => drop control point
				if(_explorationMode == FLYING){
					KeyENTER = true;
				}
				break;
			case 309://7
				flagAnimatePictorialCamera=!flagAnimatePictorialCamera;
		}
	}
}

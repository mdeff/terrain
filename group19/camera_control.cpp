
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

const float OneRad = 0.0174f;

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
	MultipleBezier();
	
	//MultipleBezier_controlled(0,0, 0, 0);
   // InitdeCasteljauSubdivision();
    //InitSubdivision();

    /// Initialize the camera position.
    _explorationMode = FIX;
    trackball(mat4::Identity());

	
    update_camera_modelview(0.5f, -0.5f, 0.8f, 0.0f, 0.0f, 0.0f);

    /// HACK to show pictorial camera.
    /// Simple translation along z-axis for now.
    _cameraPictorialModel = mat4::Identity();
    _cameraPictorialModel(2,3) = 0.5f;

    /// Control point curently modified.
    _selectedControlPoint = 0;

}


/// This controls the controller view camera.
void CameraControl::trackball(const mat4& model) {

    /// View matrix (camera extrinsics) (position in world space).

    /// Camera is in the sky, looking down.

//    vec3 camPos(0.5f, -0.5f, 0.8f);
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

  /*  vec3 camPos(0.0f, -3.0f, 1.3f);
    vec3 camLookAt(0.0f, 0.0f, 0.2f);
    vec3 camUp(0.0f, 0.0f, 1.0f);
*/
    /// Frontal view to observe water reflection.

    vec3 camPos(0.0f, -2.8f, 1.7f);
    vec3 camLookAt(0.0f, 0.0f, 0.0f);
    vec3 camUp(0.0f, 0.0f, 1.0f);


    /// Frontal view to observe water reflection.
    //vec3 camPos(-3.0f, 0.0f, 1.0f);
//    vec3 camLookAt(0.0f, 0.0f, 0.0f);
//    vec3 camUp(0.0f, 0.0f, 1.0f);


    //vec3 camPos(-3.0f, 0.0f, 1.0f);
//    vec3 camLookAt(0.0f, 0.0f, 0.0f);
//    vec3 camUp(0.0f, 0.0f, 1.0f);


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
    _controllerModelview = view * model;

}


/// This controls the camera view camera.
void CameraControl::update_camera_modelview(float posX,float posY,float posZ,float lookX,float lookY,float lookZ){
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

void CameraControl::rotate2D(float pos1, float pos2, float& look1, float& look2, float angle){
	float tmp1 = (look1-pos1)*cos(angle) - (look2-pos2)*sin(angle) + pos1;
	float tmp2 = (look1-pos1)*sin(angle) + (look2-pos2)*cos(angle) + pos2;
	look1=tmp1;
	look2=tmp2;
}

void CameraControl::fpsRotateLeftRight(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ,float velocity){

		float tmpX = lookX-posX;
		float tmpY = lookY-posY;

		//3 rotate on Z axis
		float Angle = OneRad*velocity; // more or less 1 degree
		rotate2D(0,0,tmpX,tmpY,Angle);
			
		lookX = tmpX+posX;
		lookY = tmpY+posY;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void CameraControl::fpsRotateUpDown(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ,float recordRotZ, float velocity){
		
		float tmpX =lookX-posX;
		float tmpY =lookY-posY;
		float tmpZ =lookZ-posZ;
		
		//undo lateral (Z) rotation
		rotate2D(0,0,tmpX,tmpY,-recordRotZ);
		
		//proceed vertical (Y) rotation
		float Angle = OneRad*velocity; // more or less 1 degree

		rotate2D(0,0,tmpX,tmpZ, Angle);

		//redo lateral (Z) rotation
		rotate2D(0,0,tmpX,tmpY,recordRotZ);

		lookX = tmpX + posX;
		lookY = tmpY + posY;
		lookZ = tmpZ + posZ;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void CameraControl::rotateLeftRight(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ,float& recordRotY,float velocity){
		
		//1 move to the origine
		float tmpX = lookX - posX;
		float tmpY = lookY - posY;
		float tmpZ = lookZ - posZ;

		//2 cancel Y rotation
		rotate2D(0,0,tmpX,tmpZ, -recordRotY);

		//3 rotate on Z axis
		float Angle = OneRad*velocity; // more or less 1 degree
		rotate2D(0,0,tmpX,tmpY,Angle);
	
		//4 redo Y rotation
		rotate2D(0,0,tmpX,tmpZ,recordRotY);
		
		lookX = tmpX + posX;
		lookY = tmpY + posY;
		lookZ = tmpZ + posZ;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void CameraControl::rotateUpDown(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ,float& recordRotZ,float velocity){

		//1 move to the origine
		float tmpX = lookX - posX;
		float tmpY = lookY - posY;
		float tmpZ = lookZ - posZ;

		//2 undo lateral (Z) rotation
		rotate2D(0,0,tmpX,tmpY,-recordRotZ);

		//3 rotate on Y axis
		float Angle = OneRad*velocity; // more or less 1 degree
		rotate2D(0,0,tmpX,tmpZ, Angle);

		//4 redo Z rotation
		rotate2D(0,0,tmpX,tmpY, recordRotZ);
		
		lookX = tmpX + posX ;
		lookY = tmpY + posY ;
		lookZ = tmpZ + posZ ;
		
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void CameraControl::moveAlongAxis(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ, float velocity){
		
		float dispX = (posX - lookX)*velocity;
		float dispY = (posY - lookY)*velocity;
		float dispZ = (posZ - lookZ)*velocity;
		
		posX = posX - dispX; 
		posY = posY - dispY; 
		posZ = posZ - dispZ; 

		lookX = lookX - dispX; 
		lookY = lookY - dispY; 
		lookZ = lookZ - dispZ;

		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void CameraControl::fpsExplorationForwardBackward(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ,float dispX,float dispY){
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
	float tmpZ=0;
	float AmountOfPoints = 0; 
	for(int i=0;i<5;i++){//take in count 24 z values arround to have a smooth move 
		for(int j=0;j<5;j++){
			tmpX = tmpX + i;
			tmpY = tmpY + j;
			if ((tmpX>0) & (tmpX<1025) & (tmpY>0) & (tmpY<1025)){
                tmpZ = tmpZ + _heightmapCPU[tmpX+1024*tmpY];
				AmountOfPoints++;
			}
		}
	}
	//std::cout<<"AmountOfPoints ="<<AmountOfPoints<<endl;
	tmpZ=tmpZ/AmountOfPoints;
	posZ = float(tmpZ+0.06); //fps person height
	//lookZ = posZ+0.06;
	update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
}

void CameraControl::deCasteljauTest3Points(){

	static float t=0;

	if(t<=1){
		//set control points
		float b0X = 1.0f;
		float b0Y = 1.0f;
		float b0Z = 0.4f;

		float b1X = 1.0f;
		float b1Y = -1.0f;
		float b1Z = 0.4f;

		float b2X = -1.0f;
		float b2Y = -1.0f;
		float b2Z = 0.4f;

		float posX = powf((1-t),2) * b0X + 2*t*(1-t) * b1X + powf(t,2) *b2X;
		float posY = powf((1-t),2) * b0Y + 2*t*(1-t) * b1Y + powf(t,2) *b2Y;
		float posZ = powf((1-t),2) * b0Z + 2*t*(1-t) * b1Z + powf(t,2) *b2Z;
	
		float lookX = 0.0f;
		float lookY = 0.0f;
		float lookZ = 0.3f;
		t = float(t+0.001);
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
	}
	else{
		t=0;
	}

}

void CameraControl::deCasteljauTest4Points(){ // wrong naming => use to follow the curve with the camera... 
	static unsigned int i = 0;
	
	static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaT = float(currentTime - lastTime); //deltaT in sc 

	if(deltaT>0.01){
		lastTime = currentTime;
    
        if(i<(_cameraPath.size()/3)-1){
			//std::cout<<i<<std::endl;
            float posX	= _cameraPath[i*3+0];
            float posY	= _cameraPath[i*3+1];
            float posZ	= _cameraPath[i*3+2];

            float lookX = _cameraPath[(i+1)*3+0];
            float lookY = _cameraPath[(i+1)*3+1];
            float lookZ = _cameraPath[(i+1)*3+2];
			i++;
			update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
		}
		else{
			i=0;
		}
	}
}

void CameraControl::animatePictorialCamera(unsigned int back){
	static unsigned int i = 0;
	i-=back;
	static double lastTime = glfwGetTime();

    if(i<(_cameraPath.size()/3)-1){

		double currentTime = glfwGetTime();
		float deltaT = float(currentTime - lastTime); //deltaT in sc 
		if(deltaT>0.02){
			lastTime = currentTime;

			float posX	= _cameraPath[i*3+0];
            float posY	= _cameraPath[i*3+1];
            float posZ	= _cameraPath[i*3+2];
			
            float lookX = _cameraPath[(i+1)*3+0];
            float lookY = _cameraPath[(i+1)*3+1];
            float lookZ = _cameraPath[(i+1)*3+2];

			//update camera position and look at
			update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);

			//update pictorial camera poisition and look at
			_cameraPictorialModel(0,3) = posX;
			_cameraPictorialModel(1,3) = posY;
			_cameraPictorialModel(2,3) = posZ;
			
			float dirX= lookX-posX;
			float dirY= lookY-posY;
			float dirZ= lookZ-posZ;

			float length = sqrt(dirX*dirX+dirY*dirY+dirZ*dirZ);
			
			float dirXnorm = dirX/length;
			float dirYnorm = dirY/length;
			float dirZnorm = dirZ/length;

			float angleZ = atan2(dirYnorm,dirXnorm);
			float angleY = atan2(sqrt(dirYnorm*dirYnorm+dirXnorm*dirXnorm),dirZnorm);
			
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
	//here we use subdivision to create more control point in order to generate bezier curve
	//control points
	float b0X = -0.51f, b0Y =  1.09f, b0Z = 0.40f;
	float b1X =  0.57f, b1Y =  1.26f, b1Z = 0.40f;
	float b2X =  1.31f, b2Y =  0.92f, b2Z = 0.40f;
	float b3X =  1.25f, b3Y = -0.41f, b3Z = 0.40f;

	float l0X = b0X, l0Y = b0Y, l0Z = b0Z;
	
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

	float t=0;
	for (int i=0; i<=500; i++){
	
		double posX = powf((1-t),3) * tmp0X + 3*t*powf((1-t),2) * tmp1X + 3*powf(t,2)*(1-t) *tmp2X + powf(t,3)*tmp3X;
		double posY = powf((1-t),3) * tmp0Y + 3*t*powf((1-t),2) * tmp1Y + 3*powf(t,2)*(1-t) *tmp2Y + powf(t,3)*tmp3Y;
		double posZ = powf((1-t),3) * tmp0Z + 3*t*powf((1-t),2) * tmp1Z + 3*powf(t,2)*(1-t) *tmp2Z + powf(t,3)*tmp3Z;
		
        _cameraPath[i*3+0] = float(posX);
        _cameraPath[i*3+1] = float(posY);
        _cameraPath[i*3+2] = float(posZ);
		//std::cout<<"1 "<<t<<" "<<posX<<" "<<posY<<std::endl;
	
		t = float(t+0.001);
	}

	tmp0X = r0X, tmp0Y=r0Y, tmp0Z=r0Z;
	tmp1X = r1X, tmp1Y=r1Y, tmp1Z=r1Z;
	tmp2X = r2X, tmp2Y=r2Y, tmp2Z=r2Z;
	tmp3X = r3X, tmp3Y=r3Y, tmp3Z=r3Z;
	for (int i=500; i<=1000; i++){

		double posX = powf((1-t),3) * tmp0X + 3*t*powf((1-t),2) * tmp1X + 3*powf(t,2)*(1-t) *tmp2X + powf(t,3)*tmp3X;
		double posY = powf((1-t),3) * tmp0Y + 3*t*powf((1-t),2) * tmp1Y + 3*powf(t,2)*(1-t) *tmp2Y + powf(t,3)*tmp3Y;
		double posZ = powf((1-t),3) * tmp0Z + 3*t*powf((1-t),2) * tmp1Z + 3*powf(t,2)*(1-t) *tmp2Z + powf(t,3)*tmp3Z;
		
        _cameraPath[i*3+0] = float(posX);
        _cameraPath[i*3+1] = float(posY);
        _cameraPath[i*3+2] = float(posZ);

		//std::cout<<"2 "<<t<<" "<<posX<<" "<<posY<<std::endl;
		
		t = float(t+0.001);
	}
}

void CameraControl::InitSubdivision(){

	//test time to compute
	static double lastTime = glfwGetTime();

	//control points
	//b0[iteration][XYZ]
	const int iteration = 8;
	const int nPoints = 256; //2^iteration
	float res0[nPoints][3]; //[3][2] = {{1,1},{2,2},{3,3}};
	float res1[nPoints][3];
	float res2[nPoints][3];
	float res3[nPoints][3];
	
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

	float b0,b1,b2,b3;
	float l0,l1,l2,l3,r1,r2,r3;
	for(int i=0; i<iteration;i++){ // iterate to get more set of points
		for(int j=int(pow(2,i)-1); j>=0;j--){
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

void CameraControl::Subdivision(float b0,float b1, float b2,float b3, float& l0, float& l1, float& l2, float& l3 ,float& r1,float& r2, float& r3 ){
	l0 = b0;
	l1 = float(0.5 * (b0 + b1));
	l2 = float(0.25 * (b0 + 2*b1 + b2)); 
	l3 = float(0.125 * (b0 + 3*b1 + 3*b2 + b3));
	r1 = float(0.25 * (b1 + 2*b2 + b3)); 
	r2 = float(0.5 * (b2 + b3));
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
    const unsigned int nPoints = 500;

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
		for(unsigned int k=0; k<nPoints; ++k) {
			float t = float(k) / float(nPoints);
            _cameraPath.push_back(std::pow((1-t),3)*b0X + 3*t*std::pow((1-t),2)*b1X + 3*std::pow(t,2)*(1-t)*b2X + std::pow(t,3)*b3X);
            _cameraPath.push_back(std::pow((1-t),3)*b0Y + 3*t*std::pow((1-t),2)*b1Y + 3*std::pow(t,2)*(1-t)*b2Y + std::pow(t,3)*b3Y);
            _cameraPath.push_back(std::pow((1-t),3)*b0Z + 3*t*std::pow((1-t),2)*b1Z + 3*std::pow(t,2)*(1-t)*b2Z + std::pow(t,3)*b3Z);
		}
	}
    /// Copy the vertices to GPU.
    _verticesCameraPath ->copy(_cameraPath.data(), _cameraPath.size());

}

void CameraControl::MultipleBezier() {//init
	
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
	
	 /// Copy the vertices to GPU.
    _verticesCameraPathControls->copy(_cameraPathControls.data(), _cameraPathControls.size());

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

void CameraControl::N_MultipleBezier_controlled(unsigned int PointToChange, float deltaX, float deltaY, float deltaZ) {
	
	if(PointToChange==0){
		_cameraPathControls.at(PointToChange+0)+= deltaX;
		_cameraPathControls.at(PointToChange+1)+= deltaY;
		_cameraPathControls.at(PointToChange+2)+= deltaZ;

	}
	else if (PointToChange==1){
		_cameraPathControls.at(PointToChange*3+0) += deltaX;
        _cameraPathControls.at(PointToChange*3+1) += deltaY;
        _cameraPathControls.at(PointToChange*3+2) += deltaZ;
	}
	else if (PointToChange==2){
		_cameraPathControls.at(PointToChange*3+0) += deltaX;					
        _cameraPathControls.at(PointToChange*3+1) += deltaY;							
        _cameraPathControls.at(PointToChange*3+2) += deltaZ;							
		if(_cameraPathControls.size()>12){							
			//refresh other control points to respect contstraint								
			_cameraPathControls.at(PointToChange*3+0+6) = 2*_cameraPathControls.at(PointToChange*3+0+3) - _cameraPathControls.at(PointToChange*3+0);											
			_cameraPathControls.at(PointToChange*3+1+6) = 2*_cameraPathControls.at(PointToChange*3+1+3) - _cameraPathControls.at(PointToChange*3+1);											
			_cameraPathControls.at(PointToChange*3+2+6) = 2*_cameraPathControls.at(PointToChange*3+2+3) - _cameraPathControls.at(PointToChange*3+2);	
		}
	}																	
	else if (PointToChange==3){											
		_cameraPathControls.at(PointToChange*3+0) += deltaX;													
        _cameraPathControls.at(PointToChange*3+1) += deltaY;													
        _cameraPathControls.at(PointToChange*3+2) += deltaZ;													
		if(_cameraPathControls.size()>12){						
			//refresh other control points to respect contstraint								
			_cameraPathControls.at(PointToChange*3+0+3) = 2*_cameraPathControls.at(PointToChange*3+0+0) - _cameraPathControls.at(PointToChange*3+0-3);											
			_cameraPathControls.at(PointToChange*3+1+3) = 2*_cameraPathControls.at(PointToChange*3+1+0) - _cameraPathControls.at(PointToChange*3+1-3);											
			_cameraPathControls.at(PointToChange*3+2+3) = 2*_cameraPathControls.at(PointToChange*3+2+0) - _cameraPathControls.at(PointToChange*3+2-3);	
		}
	}																	
	else if((PointToChange>1) & (((PointToChange-1)%3) ==0)){
		_cameraPathControls.at(PointToChange*3+0) += deltaX;													
        _cameraPathControls.at(PointToChange*3+1) += deltaY;													
        _cameraPathControls.at(PointToChange*3+2) += deltaZ;

		//refresh other control points to respect contstraint
		_cameraPathControls.at(PointToChange*3+0-6) = 2*_cameraPathControls.at(PointToChange*3+0-3) - _cameraPathControls.at(PointToChange*3+0);											
		_cameraPathControls.at(PointToChange*3+1-6) = 2*_cameraPathControls.at(PointToChange*3+1-3) - _cameraPathControls.at(PointToChange*3+1);											
		_cameraPathControls.at(PointToChange*3+2-6) = 2*_cameraPathControls.at(PointToChange*3+2-3) - _cameraPathControls.at(PointToChange*3+2);											

		//_cameraPathControls.at(6) = - _cameraPathControls.at(12) + 2*_cameraPathControls.at(9) ;
		//_cameraPathControls.at(7) = - _cameraPathControls.at(13) + 2*_cameraPathControls.at(10);
		//_cameraPathControls.at(8) = - _cameraPathControls.at(14) + 2*_cameraPathControls.at(11);
	}
	else if((PointToChange>1) & (((PointToChange-1)%3) ==1)){
		_cameraPathControls.at(PointToChange*3+0) += deltaX;													
        _cameraPathControls.at(PointToChange*3+1) += deltaY;													
        _cameraPathControls.at(PointToChange*3+2) += deltaZ;
		if((_cameraPathControls.size()/3)>PointToChange+3){//only if necessary
			//refresh other control points to respect contstraint		
			_cameraPathControls.at(PointToChange*3+0+6) = 2*_cameraPathControls.at(PointToChange*3+0+3) - _cameraPathControls.at(PointToChange*3+0);											
			_cameraPathControls.at(PointToChange*3+1+6) = 2*_cameraPathControls.at(PointToChange*3+1+3) - _cameraPathControls.at(PointToChange*3+1);											
			_cameraPathControls.at(PointToChange*3+2+6) = 2*_cameraPathControls.at(PointToChange*3+2+3) - _cameraPathControls.at(PointToChange*3+2);											
		}
	}
	else if((PointToChange>1) & (((PointToChange-1)%3) ==2)){
		_cameraPathControls.at(PointToChange*3+0) += deltaX;													
        _cameraPathControls.at(PointToChange*3+1) += deltaY;													
        _cameraPathControls.at(PointToChange*3+2) += deltaZ;
		if((_cameraPathControls.size()/3)>PointToChange+2){//only if necessary
			//refresh other control points to respect contstraint								
			_cameraPathControls.at(PointToChange*3+0+3) = 2*_cameraPathControls.at(PointToChange*3+0+0) - _cameraPathControls.at(PointToChange*3+0-3);											
			_cameraPathControls.at(PointToChange*3+1+3) = 2*_cameraPathControls.at(PointToChange*3+1+0) - _cameraPathControls.at(PointToChange*3+1-3);											
			_cameraPathControls.at(PointToChange*3+2+3) = 2*_cameraPathControls.at(PointToChange*3+2+0) - _cameraPathControls.at(PointToChange*3+2-3);											
		}
	}

    /// Choose the resolution.
    const unsigned int nPoints = 200;

    _cameraPath.clear();
    /// To avoid vector resizing on every loop.
    _cameraPath.reserve(2*3*nPoints);

    /// Generate coordinates of first bezier curve.
    for(int k=0; k<nPoints; ++k) {
        float t = float(k) / float(nPoints);
		for(int l=0;l<3;l++){
			_cameraPath.push_back(std::pow((1-t),3)*_cameraPathControls.at(0+l) + 3*t*std::pow((1-t),2)*_cameraPathControls.at(3+l) + 3*std::pow(t,2)*(1-t)*_cameraPathControls.at(6+l) + std::pow(t,3)*_cameraPathControls.at(9+l));
		}
	}
	/// Generate coordinates of nexts bezier curve.
	for(int m=1;m<=int((_cameraPathControls.size()-3)/9)-1;m++){
		for(int k=1; k<nPoints; ++k) {
			float t = float(k) / float(nPoints);
			for(int l=0;l<3;l++){
			_cameraPath.push_back(std::pow((1-t),3)*_cameraPathControls.at(9+l+(m-1)*9) + 3*t*std::pow((1-t),2)*_cameraPathControls.at(12+l+(m-1)*9) + 3*std::pow(t,2)*(1-t)*_cameraPathControls.at(15+l+(m-1)*9) + std::pow(t,3)*_cameraPathControls.at(18+l+(m-1)*9));
			}
			//_cameraPath.push_back(std::pow((1-t),3)*bB0Y + 3*t*std::pow((1-t),2)*bB1Y + 3*std::pow(t,2)*(1-t)*bB2Y + std::pow(t,3)*bB3Y);
			//_cameraPath.push_back(std::pow((1-t),3)*bB0Z + 3*t*std::pow((1-t),2)*bB1Z + 3*std::pow(t,2)*(1-t)*bB2Z + std::pow(t,3)*bB3Z);
		}
	}
	/// Copy the vertices to GPU.
    _verticesCameraPath->copy(_cameraPath.data(), _cameraPath.size());
    _verticesCameraPathControls->copy(_cameraPathControls.data(), _cameraPathControls.size());
	animatePictorialCamera(1);
}

void CameraControl::Add_Bcurve(){

	//add control point 1 under constraint of the two previous ctrl point to keep continuity
	_cameraPathControls.push_back( 2*_cameraPathControls.at(_cameraPathControls.size()-3) - _cameraPathControls.at(_cameraPathControls.size()-6));											
	_cameraPathControls.push_back( 2*_cameraPathControls.at(_cameraPathControls.size()-3) - _cameraPathControls.at(_cameraPathControls.size()-6));											
	_cameraPathControls.push_back( 2*_cameraPathControls.at(_cameraPathControls.size()-3) - _cameraPathControls.at(_cameraPathControls.size()-6));											
	
	//add control point 2 near control point 1
	_cameraPathControls.push_back(_cameraPathControls.at(_cameraPathControls.size()-3)+float(0.1));											
	_cameraPathControls.push_back(_cameraPathControls.at(_cameraPathControls.size()-3));											
	_cameraPathControls.push_back(_cameraPathControls.at(_cameraPathControls.size()-3));											
	
	//add control point 3 near control point 2
	_cameraPathControls.push_back(_cameraPathControls.at(_cameraPathControls.size()-9)+float(0.4));											
	_cameraPathControls.push_back(_cameraPathControls.at(_cameraPathControls.size()-9));											
	_cameraPathControls.push_back(_cameraPathControls.at(_cameraPathControls.size()-9));	

	//display new ctrl point and Bcurve
	N_MultipleBezier_controlled(0,0,0,0);
	//_verticesCameraPathControls->copy(_cameraPathControls.data(), _cameraPathControls.size());
}

void CameraControl::Remove_Bcurve(){

	//add control point 1 under constraint of the two previous ctrl point to keep continuity
	_cameraPathControls.pop_back();
	_cameraPathControls.pop_back();	
	_cameraPathControls.pop_back();	

	_cameraPathControls.pop_back();	
	_cameraPathControls.pop_back();	
	_cameraPathControls.pop_back();	
	
	_cameraPathControls.pop_back();	
	_cameraPathControls.pop_back();	
	_cameraPathControls.pop_back();		
	
	//display Bcurve
	N_MultipleBezier_controlled(0,0,0,0);
}

void CameraControl::MultipleBezier_controlled(unsigned int PointToChange, float deltaX, float deltaY, float deltaZ) {

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

	
	std::cout<<_cameraPathControls.size()<<" "<<int((_cameraPathControls.size()-3)/9)<<std::endl;

 

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
	static float straightMaxSpeed = 0.5f; 	
	static float rotationMaxSpeed = 20.0f; 
	static float straightAcceleration = 0.05f; 	
	static float rotationAcceleration = 4.0f; 

	//To set user defined bcurve 
	static bool ModeSettingControlPoint=false;
	static int controlPointsCount = 0;
	//To tmp save the control point
	static float tmpCtrlPointX;
	static float tmpCtrlPointY;
	static float tmpCtrlPointZ;

	//starting position
	static float posX =  -0.92f;
	static float posY =  -0.72f;
	static float posZ =  0.20f;

	static float lookX = 0.05f;
	static float lookY =-0.32f;
	static float lookZ = 0.15f;
	//std::cout << std::endl  << std::endl << posX << " " << lookX << std::endl;
	//std::cout<< posY << " " << lookY << std::endl;
	//std::cout<< posZ << " " << lookZ << std::endl;

	//velocity of actions, nothing moves instanteanously
	static float velocityForward = 0;
	static float velocityBackward = 0;
	static float velocityLeft = 0;
	static float velocityRight = 0;
	static float velocityUp = 0;
	static float velocityDown = 0;

	//tmp save rotation done 
	static float recordRotZ = 0;
	static float recordRotY = 0;

	//use time to set movement speeds
	static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaT = float(currentTime - lastTime); //deltaT in sc 
    lastTime = currentTime;

    if ((KeyW) | (velocityForward > 0.0f)){//W pressed => go forward
		if((KeyW==1) & (velocityForward<straightMaxSpeed*deltaT)){
				velocityForward = velocityForward + straightAcceleration * deltaT ;
		}
		else{
			velocityForward = velocityForward - straightAcceleration*deltaT;
		}
		
		//Flying exploration
		moveAlongAxis(posX,posY,posZ,lookX,lookY,lookZ,velocityForward);

	}
    if  ((KeyS)| (velocityBackward > 0.0f)){//S pressed => go backward
        if((KeyS) & (velocityBackward<straightMaxSpeed*deltaT)){
				velocityBackward = velocityBackward + straightAcceleration*deltaT;
		}
		else{
			velocityBackward = velocityBackward - straightAcceleration*deltaT;
		}
		//flying exploration
		moveAlongAxis(posX,posY,posZ,lookX,lookY,lookZ,-velocityBackward);
		
	}
    if ((KeyA) | (velocityLeft > 0.0f)){//A pressed => turn left
        if((KeyA) & (velocityLeft<rotationMaxSpeed*deltaT)){
				velocityLeft = velocityLeft + rotationAcceleration*deltaT;
		}
		else{
			velocityLeft = velocityLeft - rotationAcceleration*deltaT;
		}
	//	rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,velocityLeft);
		rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,velocityLeft);

		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,velocityLeft);
		recordRotZ = fmod((recordRotZ+OneRad*velocityLeft),float(2*M_PI)); //save rotation done
	}
    if ((KeyD) | (velocityRight > 0.0f)){//D pressed =turn right
        if((KeyD) & (velocityRight<rotationMaxSpeed*deltaT)){
				velocityRight = velocityRight + rotationAcceleration*deltaT;
		}
		else{
			velocityRight = velocityRight - rotationAcceleration*deltaT;
		}
	//	rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,-velocityRight);
		rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,-velocityRight);
		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,-velocityRight);
		recordRotZ = fmod((recordRotZ-OneRad*velocityRight),float(2*M_PI)); //save rotation done
	}
    if  ((KeyQ)  | (velocityUp > 0.0f)){//Q pressed turn up
        if((KeyQ) & (velocityUp<rotationMaxSpeed*deltaT)){
				velocityUp = velocityUp + rotationAcceleration*deltaT;
		}
		else{
			velocityUp = velocityUp - rotationAcceleration*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);
	//	rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,velocityUp);
		rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);

		recordRotY = fmod((recordRotY+OneRad*velocityUp),float(2*M_PI)); //save rotation done	
	}
    if  ( (KeyE) | (velocityDown > 0.0f)){//E pressed => down
        if((KeyE) & (velocityDown<rotationMaxSpeed*deltaT)){
				velocityDown = velocityDown + rotationAcceleration*deltaT;
		}
		else{
			velocityDown = velocityDown - rotationAcceleration*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
	//	rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,recordRotZ,-velocityDown);
		rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
		recordRotY = fmod((recordRotY-OneRad*velocityDown),float(2*M_PI)); //save rotation done
	}
    if(Key1){
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
	}
    if((KeyENTER) & (ModeSettingControlPoint==false)){
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
    if (ModeSettingControlPoint){ // set a control point if necessary
		//use look at to define the 2nd control point (P1) only for first Bcurve
        if ((controlPointsCount%4==1) & (_cameraPathControls.size()%12==3) & (controlPointsCount<3) ){
            _cameraPathControls.push_back(lookX);
            _cameraPathControls.push_back(lookY);
            _cameraPathControls.push_back(lookZ);
		}//modify and refresh the look at to define the 2nd control point (P1) only for first Bcurve
        else if((controlPointsCount%4==1) & (_cameraPathControls.size()%12==6) & (controlPointsCount<3)){
            _cameraPathControls.pop_back();
            _cameraPathControls.pop_back();
            _cameraPathControls.pop_back();
            _cameraPathControls.push_back(lookX);
            _cameraPathControls.push_back(lookY);
            _cameraPathControls.push_back(lookZ);
		}//set P2 and P3 using pos for every Bcurve created
        else if((controlPointsCount%4 ==2)  & (_cameraPathControls.size()%12==6)){
            _cameraPathControls.push_back(lookX);
            _cameraPathControls.push_back(lookY);
            _cameraPathControls.push_back(lookZ);
            _cameraPathControls.push_back(posX-0.02f);
            _cameraPathControls.push_back(posY-0.02f);
            _cameraPathControls.push_back(posZ-0.02f);
		}//refresh P2 and P3 using pos for every Bcurve created
        else if((controlPointsCount%4==2) & (_cameraPathControls.size()%12==0)){
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
        if(KeyENTER){//finish setting ctrl points
			if(controlPointsCount%4==1){
				controlPointsCount++;
			}
			else if(controlPointsCount%4==2){ // a bcurve is completly settled
				controlPointsCount = controlPointsCount+2;
				ModeSettingControlPoint=false;
                std::cout<<int(_cameraPathControls.size()/12)<<" Bezier curves has been created!"<<std::endl;
			}
			if((controlPointsCount%4==0) & (controlPointsCount>2)){ // compute control P1 for next Bcurve, respecting linearity
                tmpCtrlPointX = 2*_cameraPathControls.at(_cameraPathControls.size()-3) - _cameraPathControls.at(_cameraPathControls.size()-6);
                tmpCtrlPointY = 2*_cameraPathControls.at(_cameraPathControls.size()-2) - _cameraPathControls.at(_cameraPathControls.size()-5);
                tmpCtrlPointZ = 2*_cameraPathControls.at(_cameraPathControls.size()-1) - _cameraPathControls.at(_cameraPathControls.size()-4);
			}										
			KeyENTER=false;							
		}
	}
}

void CameraControl::fpsExploration(){

	static float straightMaxSpeed = 0.5f; 	
	static float rotationMaxSpeed = 60.0f; 
	static float straightAcceleration = 0.05f; 	
	static float rotationAcceleration = 4.0f; 

	static float posX = -0.73f;
	static float posY =  0.51f;
	static float posZ =  0.08f;

	static float lookX = 0.19f;
	static float lookY = 0.04f;
	static float lookZ = 0.01f;
	
	//std::cout << std::endl  << std::endl << posX << " " << lookX << std::endl;
	//std::cout<< posY << " " << lookY << std::endl;
	//std::cout<< posZ << " " << lookZ << std::endl;

	static float velocityForward = 0;
	static float velocityBackward = 0;
	static float velocityLeft = 0;
	static float velocityRight = 0;
	static float velocityUp = 0;
	static float velocityDown = 0;
	
	static float recordRotZ = 0;

	static bool jumping = false;
	static float initJumpPosZ = 0;
	static float initJumpLookZ = 0;
	static float jumpLevel =0;

	static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaT = float(currentTime - lastTime); //deltaT in sc 
    lastTime = currentTime;

    if((KeySPACE) & (jumping==false)){//space => jump
			jumping = true;
			initJumpPosZ = posZ;
			initJumpLookZ = lookZ;
	}
    if (jumping  ){

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
		float tmpZ=0;
		float AmountOfPoints = 0; 
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
		float floorLevel = tmpZ+float(0.06); //fps person height

		if ((jumpLevel >= 90) & (posZ<=floorLevel)){ //reach floor =>jump finished
			jumping = false;
			jumpLevel = 0;
			posZ = floorLevel;// 0.06 person height
		}
		else if(jumpLevel > 180 ){// falling ! 
			posZ -= 0.005f; 
		}
		else{
			posZ = initJumpPosZ + float(0.1) * sin(OneRad*jumpLevel);
			lookZ = initJumpLookZ + float(0.1) * sin(OneRad*jumpLevel);
		}

		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
	}
    if ((KeyW) | (velocityForward > 0.0f)){//W pressed => go forward
		if((KeyW==1) & (velocityForward<straightMaxSpeed*deltaT)){
				velocityForward += straightAcceleration*deltaT;
		}
		else if (jumping ==false){
			velocityForward -= straightAcceleration*deltaT;
		}

        if((KeySHIFT) & (KeyW) & (velocityForward<2*straightMaxSpeed*deltaT)){ // press shift => running
			velocityForward +=3*straightAcceleration*deltaT;
		}
	
		//FPS exploration
		if((posX<=1) & (posX>=-1) & (posY<=1) & (posY>=-1) & (jumping == false)){//stay on the map
			float dispX = 0.2f*velocityForward*powf(posX-lookX,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posX-lookX)/abs(posX-lookX);
			float dispY = 0.2f*velocityForward*powf(posY-lookY,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posY-lookY)/abs(posY-lookY);
			fpsExplorationForwardBackward(posX,posY,posZ,lookX,lookY,lookZ,dispX,dispY);
		}
		else if((posX<=1) & (posX>=-1) & (posY<=1) & (posY>=-1) & (jumping == true)){//stay on the map jumping forward
			float dispX = 0.2f*velocityForward*powf(posX-lookX,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posX-lookX)/abs(posX-lookX);
			float dispY = 0.2f*velocityForward*powf(posY-lookY,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posY-lookY)/abs(posY-lookY);
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
    if  ((KeyS)| (velocityBackward > 0.0f)){//S pressed => go backward
        if((KeyS) & (velocityBackward<straightMaxSpeed*deltaT)){
				velocityBackward = velocityBackward + straightAcceleration*deltaT;
		}
		else if (jumping ==false){
			velocityBackward = velocityBackward - straightAcceleration*deltaT;
		}
	
		//fps exploration
		if((posX<=1) & (posX>=-1) & (posY<=1) & (posY>=-1)){//stay on the map
			float dispX = 0.2f*velocityBackward*powf(posX-lookX,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posX-lookX)/abs(posX-lookX);
			float dispY = 0.2f*velocityBackward*powf(posY-lookY,2)/(powf(posX-lookX,2)+powf(posY-lookY,2))*(posY-lookY)/abs(posY-lookY);
			fpsExplorationForwardBackward(posX,posY,posZ,lookX,lookY,lookZ,-dispX,-dispY);
		}
	}
    if ((KeyA) | (velocityLeft > 0.0f)){//A pressed => turn left
        if((KeyA) & (velocityLeft<rotationMaxSpeed*deltaT)){
				velocityLeft = velocityLeft + rotationAcceleration*deltaT;
		}
		else if (jumping ==false){
			velocityLeft = velocityLeft - rotationAcceleration*deltaT;
		}
		fpsRotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,velocityLeft);
		recordRotZ = fmod((recordRotZ+OneRad*velocityRight),float(2*M_PI)); //save rotation done
	
		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,velocityLeft);
	}
    if ((KeyD) | (velocityRight > 0.0f)){//D pressed =turn right
        if((KeyD) & (velocityRight<rotationMaxSpeed*deltaT)){
				velocityRight = velocityRight + rotationAcceleration*deltaT;
		}
		else if (jumping ==false){
			velocityRight = velocityRight - rotationAcceleration*deltaT;
		}
		fpsRotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,-velocityRight);
		recordRotZ = fmod((recordRotZ-OneRad*velocityRight),float(2*M_PI)); //save rotation done
	
		//rotateLeftRight(posX,posY,posZ,lookX,lookY,lookZ,recordRotY,-velocityRight);
	}
    if  ((KeyQ)  | (velocityUp > 0.0f)){//Q pressed turn up
        if((KeyQ) & (velocityUp<rotationMaxSpeed*deltaT)){
				velocityUp = velocityUp + rotationAcceleration*deltaT;
		}
		else if (jumping ==false){
			velocityUp = velocityUp - rotationAcceleration*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);
		fpsRotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,velocityUp);
	}
    if  ( (KeyE) | (velocityDown > 0.0f)){//E pressed => down
        if((KeyE) & (velocityDown<rotationMaxSpeed*deltaT)){
				velocityDown = velocityDown + rotationAcceleration*deltaT;
		}
		else if (jumping ==false){
			velocityDown = velocityDown - rotationAcceleration*deltaT;
		}
		//rotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
		fpsRotateUpDown(posX,posY,posZ,lookX,lookY,lookZ,recordRotZ,-velocityDown);
	}
    if(Key2){
		update_camera_modelview(posX,posY,posZ,lookX,lookY,lookZ);
	}
}

void CameraControl::updateCameraPosition(mat4 views[], mat4& cameraPictorialModel, int& selectedControlPoint) {

    /// Modify camera position according to the exploration mode.
    switch(_explorationMode) {
    case FLYING:
        flyingExploration();
        break;
    case FPS:
        fpsExploration();
        break;
    //case PATH:
    //    deCasteljauTest4Points();
    //    break;
    default:
        /// Nothing to do : trackball() will update the transformation matrix.
        break;
    }

    if(flagAnimatePictorialCamera)
        animatePictorialCamera(0);

    /// Update the view transformation matrix.
    views[0] = _controllerModelview;
    views[1] = _cameraModelview;

    /// Update the camera pictorial model transformation matrix.
    cameraPictorialModel = _cameraPictorialModel;

    /// Update the selected control point.
    selectedControlPoint = _selectedControlPoint;

}

void CameraControl::handleKeyboard(int key, int action){
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
        flagAnimatePictorialCamera = false;
		break;
    case 304: //2
        std::cout << "Exploration mode : FPS" << std::endl;
        _explorationMode = FPS;
        flagAnimatePictorialCamera = false;
		Key2 =!Key2;
		break;
    case 306: //4
        std::cout << "Exploration mode : FIX" << std::endl;
        _explorationMode = FIX;
        flagAnimatePictorialCamera = false;
    }
	if(action==1){
		switch(key){
			 case 305: //3
				std::cout << "Exploration mode : PATH" << std::endl;
				//_explorationMode = PATH;
				flagAnimatePictorialCamera = !flagAnimatePictorialCamera;
				break;
			case 90: //Z=> change control point under modification
                _selectedControlPoint++;
                if(_selectedControlPoint> 3*int((_cameraPathControls.size()-3)/9))
                    _selectedControlPoint=0;
                std::cout<<"Changing control point nB"<<_selectedControlPoint<<std::endl;
				std::cout<<"Control list :"<<std::endl<<"Key Z : change point to set"<<std::endl;
				std::cout<<"Key U : position X + 0.1"<<std::endl;
				std::cout<<"Key J : position X - 0.1"<<std::endl;
				std::cout<<"Key I : position Y + 0.1"<<std::endl;
				std::cout<<"Key K : position Y - 0.1"<<std::endl;
				std::cout<<"Key O : position Z + 0.1"<<std::endl;
				std::cout<<"Key L : position Z - 0.1"<<std::endl;
				std::cout<<"Key + : add bezier curve"<<std::endl;

				break;
			case 85: //U=> X+
                //bezier_4_points(_selectedControlPoint,0.1f,0.0f,0.0f);
				N_MultipleBezier_controlled(_selectedControlPoint,0.1f,0.0f,0.0f);
				break;
			case 74: //J=> X-
                //bezier_4_points(_selectedControlPoint,-0.1f,0.0f,0.0f);
				N_MultipleBezier_controlled(_selectedControlPoint,-0.1f,0.0f,0.0f);
				break;
			case 73: //I=> Y+
                //bezier_4_points(_selectedControlPoint,0.0f,0.1f,0.0f);
				N_MultipleBezier_controlled(_selectedControlPoint,0.0f,0.1f,0.0f);
				break;
			case 75: //K=> Y-
                //bezier_4_points(_selectedControlPoint,0.0f,-0.1f,0.0f);
				N_MultipleBezier_controlled(_selectedControlPoint,0.0f,-0.1f,0.0f);
				break;
			case 79: //O=> Z+
                //bezier_4_points(_selectedControlPoint,0.0f,0.0f,0.1f);
				N_MultipleBezier_controlled(_selectedControlPoint,0.0f,0.0f,0.1f);
				break;
			case 76: //L=> Z-
                //bezier_4_points(_selectedControlPoint,0.0f,0.0f,-0.1f);
				N_MultipleBezier_controlled(_selectedControlPoint,0.0f,0.0f,-0.1f);
				break;
			case 307: //5
				InitSubdivision();
				break;
			case 308: //6
				bezier_4_points(0,0,0,0);
				break;
//			case 294://ENTER => drop control point
//				if(_explorationMode == FLYING){
//					KeyENTER = true;
//				}
//				break;
            case 294://+
				Add_Bcurve();
				break;
			case 314://-
				if(_cameraPathControls.size()>12){
					if(((_cameraPathControls.size()/3)-4)<_selectedControlPoint){
						_selectedControlPoint=0;
					}
					Remove_Bcurve();

				}
				break;
		}
	}
}

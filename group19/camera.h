
#ifndef __camera_h__
#define __camera_h__

#include "rendering_context.h"
#include <GL/glew.h>
#include "opengp.h"

extern mat4 cameraModelview;

class Camera : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    Camera(unsigned int width, unsigned int height);
    void init(Vertices* vertices);
    void draw(const mat4& projection, const mat4& modelview) const;

    /// Common methods of all RenderingContext.
	void handleCamera();
	void handleCameraControls(int key, int action);
	void update_camera_modelview(double posX,double posY,double posZ,double lookX,double lookY,double lookZ);
	void handleKeyboard();
	void CopyHeightmapToCPU(GLuint heightMapTexID);
	void rotate2D(double pos1, double pos2, double& look1, double& look2, double angle);
	void rotate3D(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double& recordRotZ);
	void rotateLeftRight(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double& recordRotZ,double velocity);
	void rotateUpDown(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double& recordRotZ,double velocity);
	void moveAlongAxis(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ, double velocity);
	void fpsExplorationForwardBackward(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double dispX,double dispY);
	void deCasteljauTest3Points();
	void deCasteljauTest4Points();

private:

    /// Uniform IDs.
    unsigned int _modelviewID;
    unsigned int _projectionID;

};

#endif /* __camera_h__ */

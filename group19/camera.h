
#ifndef __camera_h__
#define __camera_h__

#include "rendering_context.h"
#include <vector>
#include <GL/glew.h>
#include "opengp.h"

/// Forward declarations (no header includes) (namespace pollution, build time).
class VerticesBezier;

extern mat4 cameraModelview;

class Camera : public RenderingContext {

public:

    /// Common methods of all RenderingContext.
    Camera(unsigned int width, unsigned int height);
    void init(VerticesBezier* vertices);
    void draw(const mat4& projection, const mat4& modelview) const;

    /// Specific interface for camera control.
    void handleCamera();
    void handleCameraControls(int key, int action);
	void update_camera_modelview(double posX,double posY,double posZ,double lookX,double lookY,double lookZ);
	void CopyHeightmapToCPU(GLuint heightMapTexID);


private:

    /// Uniform IDs.
    unsigned int _modelviewID;
    unsigned int _projectionID;

    /// Camera path (Bézier curve) coordinates.
    std::vector<float> _bezierCurve;

    /// HACK.
    VerticesBezier* _verticesBezier;

    /// Helper functions.
    void handleKeyboard();
    void rotate2D(double pos1, double pos2, double& look1, double& look2, double angle);
    void rotate3D(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double& recordRotZ);
    void moveAlongAxis(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ, double velocity);
    void fpsExplorationForwardBackward(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double dispX,double dispY);
    void deCasteljauTest3Points();
    void deCasteljauTest4Points();
    void InitdeCasteljau4Points();
    void flyingExploration();
    void fpsExploration();
    void fpsRotateLeftRight(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double velocity);
    void fpsRotateUpDown(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double recordRotZ, double velocity);
    void rotateUpDown(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotZ,double velocity);
    void rotateLeftRight(double& posX, double& posY, double& posZ, double& lookX, double& lookY, double& lookZ,double& recordRotY,double velocity);
    void InitdeCasteljauSubdivision();

};

#endif /* __camera_h__ */

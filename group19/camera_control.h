
#ifndef __camera_control_h__
#define __camera_control_h__

#include <vector>
#include <GL/glew.h>
#include "opengp.h"

/// Forward declarations (no header includes) (namespace pollution, build time).
class VerticesCameraPath;

class CameraControl {

public:

    /// Camera control interface.
    void init(VerticesCameraPath* verticesCameraPath, VerticesCameraPath* verticesCameraPathControls, unsigned int heightMapTexID);
    void trackball(const mat4& model);
    void updateCameraPosition(mat4 views[], mat4& cameraPictorialModel, int& selectedControlPoint);
    void handleKeyboard(int key, int action);

private:

    /// Camera view transformation matrix.
    mat4 _cameraModelview;
    mat4 _controllerModelview;

    /// Camera pictorial transformation matrix.
    mat4 _cameraPictorialModel;

    /// Camera path (Bézier curve) coordinates.
    std::vector<float> _cameraPath;

    /// Camera path (Bézier curve) control points coordinates.
    std::vector<float> _cameraPathControls;

    /// Handle to camera path vertices.
    VerticesCameraPath* _verticesCameraPath;
    VerticesCameraPath* _verticesCameraPathControls;

    /// Control point curently modified.
    int _selectedControlPoint;

    /// Heightmap texture (copied back to CPU).
    float* _heightmapCPU;

    /// World exploration mode.
    enum exploration_mode {FPS, FLYING, PATH, FIX};
    enum exploration_mode _explorationMode;

    /// Helper functions.
    void update_camera_modelview(float posX,float posY,float posZ,float lookX,float lookY,float lookZ);
    void handleKeyboard();
    void rotate2D(float pos1, float pos2, float& look1, float& look2, float angle);
    void moveAlongAxis(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ, float velocity);
    void fpsExplorationForwardBackward(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ,float dispX,float dispY);
    void deCasteljauTest3Points();
    void deCasteljauTest4Points();
    void flyingExploration();
    void fpsExploration();
    void fpsRotateLeftRight(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ,float velocity);
    void fpsRotateUpDown(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ,float recordRotZ, float velocity);
    void rotateUpDown(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ,float& recordRotZ,float velocity);
    void rotateLeftRight(float& posX, float& posY, float& posZ, float& lookX, float& lookY, float& lookZ,float& recordRotY,float velocity);
    void InitdeCasteljauSubdivision();
	void InitSubdivision();
	void Subdivision(float b0,float b1, float b2,float b3, float& l0, float& l1, float& l2, float& l3 ,float& r1,float& r2, float& r3 );
    void bezier_4_points(int PointToChange, float deltaX, float deltaY, float deltaZ);
	void animatePictorialCamera(unsigned int back);
	void MultipleBezier();
	void createBCurve();
	void MultipleBezier_controlled(unsigned int PointToChange, float deltaX, float deltaY, float deltaZ); 
	void N_MultipleBezier_controlled(unsigned int PointToChange, float deltaX, float deltaY, float deltaZ); 
	void Add_Bcurve();
	void Remove_Bcurve();
};

#endif /* __camera_control_h__ */

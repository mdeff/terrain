
#include <GL/glew.h>
#include "opengp.h"

/// @brief OpenGL wrapper class template
class Terrain {

public:
    void init(GLuint heightMapTexID);  ///< Or in the constructor
    void draw(mat4& projection, mat4& model_view);
    void clean();  ///< Or in the destructor

private:
    // Those are common, can be inherited.
    GLuint _vertexArrayID;
    GLuint _programID;
    GLuint _modelviewID;
    GLuint _projectionID;

    // Specialized to this object.
    GLuint _timerID;

    void gen_triangle_grid();
    GLuint loadTexture(const char * imagepath, const int slotNum);

};

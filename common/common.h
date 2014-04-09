// Copyright (C) 2014 - LGG EPFL
// Copyright (C) 2014 - Andrea Tagliasacchi

///--- LOAD THE OPENGL LIBRARIES HERE (IN CROSS PLATFORM)
#include <GL/glew.h> ///< must be before glfw
#include <GL/glfw.h>

/// We use a modified (to support OpenGL3) version of the Eigen OpenGL module 
/// @see http://eigen.tuxfamily.org/dox/unsupported/group__OpenGLSUpport__Module.html
#include <OpenGP/GL/EigenOpenGLSupport3.h>
// #include <Eigen/OpenGL3Support>

/// We use Eigen for linear algebra
typedef Eigen::Vector2f vec2;
typedef Eigen::Vector3f vec3;
typedef Eigen::Vector4f vec4;
typedef Eigen::Matrix4f mat4;
typedef Eigen::Matrix3f mat3;

/// On some OSs the exit flags are not defined
#ifndef EXIT_SUCCESS
    #define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
    #define EXIT_FAILURE 1
#endif

/// Utilities to parse shader and compile shader files
#include <OpenGP/GL/glfw_helpers.h>
#include <OpenGP/GL/glfw_trackball.h>

/// For mesh I/O we use OpenGP
#include <OpenGP/Surface_mesh.h>

/// These namespaces assumed by default
using namespace std;
using namespace opengp;

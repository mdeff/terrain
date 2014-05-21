#version 330 core

// Transformation matrices from model space to camera clip space.
uniform mat4 projection;
uniform mat4 modelview;

// Transformation matrix from model space to light clip space.
uniform mat4 lightMVP;

// Transformation matrix from model space to light clip space.
uniform mat4 lightOffsetMVP;

// Light source position in model space.
uniform vec3 lightPositionModel;


// Vertices 2D position in model space.
// First input buffer. Defined here, retrieved in C++ by glGetAttribLocation.
layout(location = 0) in vec2 vertexPosition2DModel;

// Vertices 3D position (after heightmap displacement) in model space.
out vec3 vertexPosition3DModel;

// Vertex position in light source clip space.
out vec3 reflectionCoord;

// Light and view directions.
out vec3 lightDir, viewDir;


void main() {
	
	//fixed height of water surface
	float waterSurfaceHeight = 0.018f;  
    float height = waterSurfaceHeight; 
  
    // 3D vertex position : X and Y from vertex array, Z is fixed water surface height
    vertexPosition3DModel = vec3(vertexPosition2DModel.xy, height);

    // Model matrix transforms from model space to world space.
    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
    vec4 vertexPositionCamera = modelview * vec4(vertexPosition3DModel, 1.0);
    gl_Position = projection * vertexPositionCamera;

    // Vertex position in light source clip space.
    vec4 vertexPositionReflection = lightMVP * vec4(vertexPosition3DModel, 1.0);
    // Reflection map texture coordinates with w division for perspective.
    reflectionCoord = vertexPositionReflection.xyz / vertexPositionReflection.w * 0.5 + 0.5;


    // Light and view directions : subtraction of 2 points gives vector.
    // Camera space --> camera position at origin --> subtraction by [0,0,0].
    // No need to normalize as interpolation will not preserve vector lengths.
    lightDir = lightPositionModel - vertexPosition3DModel;
    viewDir = vec3(vertexPositionCamera);

}

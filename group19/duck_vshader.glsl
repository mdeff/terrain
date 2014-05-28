#version 330 core

// Transformation matrices from model space to camera clip space.
uniform mat4 view;
uniform mat4 projection;
uniform mat4 translation;
uniform mat4 animation;

// Light source position in world space.
uniform vec3 lightPositionWorld;

// Vertices 3D position in world space.
layout(location = 0) in vec3 vertexPosition3DWorld;
layout(location = 1) in vec3 normal_mv;

// Vertex position (mandatory output).
out gl_PerVertex {
    vec4 gl_Position;
};

// 3D texture coordinates.
// for now. Dont want to use pass through shader
out vec3 vertexPosition3DWorld_tmp;
out vec3 normal_mv_tmp;

// Light and view directions.
out vec3 lightDirWorld, viewDirCamera;
void main() {
	
	float scale = 35.0f;
	//hack: no need to rotate the duck. Just swap the element's position
	vertexPosition3DWorld_tmp = vertexPosition3DWorld.xzy;
    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
	
	vec4 vertexPositionCamera = view * vec4(vertexPosition3DWorld_tmp, 1.0);
    gl_Position = projection * view * animation * translation *  vec4(vertexPosition3DWorld_tmp/scale, 1.0);	

	lightDirWorld = lightPositionWorld - vertexPosition3DWorld_tmp;
    viewDirCamera = vec3(vertexPositionCamera);

	normal_mv_tmp = normal_mv;
}
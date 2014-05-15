#version 330 core



// Vertices 3D position (after heightmap displacement) in model space.
in vec3 vertexPosition3DModel;


// Light and view directions.
in vec3 lightDir, viewDir;

// First output buffer is pixel color.
// gl_FragColor
layout(location = 0) out vec3 color;


void main()
{
	color = vec3(0.0,0.0,1.0);
}
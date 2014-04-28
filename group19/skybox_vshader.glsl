#version 330 core

in vec3 position;

uniform mat4 modelview;
uniform mat4 projection;


void main()
{
	// Vertex in camera space then projection/clip space.
    vec4 position_mv = modelview * vec4(position, 1.0);
    gl_Position = projection * position_mv;

}
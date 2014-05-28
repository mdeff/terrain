#version 330 core

uniform mat4 projection;
uniform mat4 model_view;
uniform vec3 light_pos;

in vec3 position;
in vec3 normal;

// TODO: These variables need to be set approriatly.
out vec3 normal_mv;
out vec3 light_dir, view_dir;


void main() {
	vec3 tmp_position = position/10.0;
    vec4 position_mv = model_view * vec4(tmp_position, 1.0);
    gl_Position = vec4(tmp_position,1.0f); //projection * position_mv;
    
    /// TODO: Phong shading.
    /// 1) compute normal_mv using the model_view matrix.
	normal_mv = vec3(inverse(transpose(model_view))*vec4(normal,1.0));
    /// 2) compute the light direction light_dir
	light_dir = light_pos - vec3(position_mv);
    /// 3) compute the view direction view_dir.    
	view_dir = vec3(position_mv);
}

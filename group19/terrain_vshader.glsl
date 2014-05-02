#version 330 core


uniform mat4 projection;
uniform mat4 modelview;
uniform vec3 light_dir_tmp;

// First texture. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D heightMapTex;

// First input buffer. Defined here, retrieved in C++ by glGetAttribLocation.
layout(location = 0) in vec3 position;


// Position (world coordinates) after heightmap displacement.
// Add suffix for coordinate system.
out vec3 displaced;
//light direction
out vec3 light_dir;

//view direction
out vec3 view_dir;

//
out vec3 displaced_mv;


void main() {

    // World (triangle grid) coordinates are (-1,-1) x (1,1).
    // Texture (height map) coordinates are (0,0) x (1,1).
    vec2 UV = vec2((position.xy+1.0)/2.0);
    float height = texture(heightMapTex, UV).r;

    displaced = vec3(position.xy, height);

    // Vertex in camera space then projection/clip space.
    vec4 position_mv = modelview * vec4(displaced.xyz,  1.0);
    gl_Position = projection * position_mv;

    //vertex position in camera coordinate
    displaced_mv = vec3(gl_Position);

    //compute the light direction
    light_dir = light_dir_tmp;
    view_dir = vec3(position_mv);

}

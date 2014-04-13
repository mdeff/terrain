#version 330 core

uniform int N;
uniform mat4 projection;
uniform mat4 modelview;
uniform vec3 light_pos;

// First texture. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D heightMapTex;

// First input buffer. Defined here, retrieved in C++ by glGetAttribLocation.
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

// Position (world coordinates) after heightmap displacement.
out vec3 displaced;
//light direction
out vec3 light_dir;
//normal vector
out vec3 normal_mv;


void main() {

    // World (triangle grid) coordinates are (-1,-1) x (1,1).
    // Texture (height map) coordinates are (0,0) x (1,1).
    vec2 UV = vec2((position.xy+1)/2);
    float height = texture2D(heightMapTex, UV).r;

    displaced = vec3(position.xy, height);

    // Vertex in camera space then projection/clip space.
    vec4 position_mv = modelview * vec4(displaced, 1.0);
    gl_Position = projection * position_mv;


    //compute the light direction
    light_dir = light_pos - vec3(position_mv);

    //compute the normal vector using finite difference
    float grid_step = 1/N;
    //first find neighbors' positions in x,y direction
    vec3 x_prev = vec3(position.x, position.y, 0.0);
    vec3 x_next = vec3(position.x, position.y, 0.0);
    vec3 y_prev = vec3(position.x, position.y, 0.0);
    vec3 y_next = vec3(position.x, position.y, 0.0);

    //Update location and truncate it
    x_prev.x = min(-1.0,x_prev.x - grid_step); x_next.x = max(1.0, x_next.x+grid_step);
    y_prev.y = min(-1.0,y_prev.y - grid_step); y_next.y = max(1.0, y_next.y+grid_step);

    //find the evalation at surrounding positions
    x_prev.z = texture2D(heightMapTex, x_prev.xy).r;
    x_next.z = texture2D(heightMapTex, x_next.xy).r;
    y_prev.z = texture2D(heightMapTex, y_prev.xy).r;
    y_next.z = texture2D(heightMapTex, y_next.xy).r;

    //Approximate the tangent vectors along x,y direction
    vec3 x_gradient = (x_next - x_prev)/(2*grid_step);
    vec3 y_gradient = (y_next - y_prev)/(2*grid_step);

    //finally, compute the normal vector
    vec3 normal = cross(x_gradient, y_gradient);

    //normal_mv in model_view
    normal_mv = vec3(inverse(transpose(modelview))*vec4(normal,1.0));

}

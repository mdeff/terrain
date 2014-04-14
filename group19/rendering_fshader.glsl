#version 330 core

uniform vec3 Id;
uniform vec3 kd;

// Position (world coordinates) after heightmap displacement.
//in vec3 displaced;

//light direction
in vec3 light_dir;
//normal vector
in vec3 normal_mv;

// First output buffer is pixel color.
layout(location = 0) out vec3 color;


void main() {

    //re-normalize the input vector
    vec3 L = normalize(light_dir);
    vec3 N = normalize(normal_mv);

    color = Id * kd * max(dot(N,L),0.0);

//    float green = displaced.z;
//    color = vec3(1.0, green, 0.0);

}

#version 330 core

uniform vec3 Id;
uniform vec3 kd;

// First texture. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D heightMapTex;

// Position (world coordinates) after heightmap displacement.
in vec3 displaced;

//light direction
in vec3 light_dir;
//normal vector
in vec3 normal_mv;

// First output buffer is pixel color.
layout(location = 0) out vec3 color;


void main() {

    // Color dependent on the elevation (similar to texture mapping).
    float grey = displaced.z * 5.0f + 0.3f;
    vec3 mapped = vec3(grey);

    // Normalize the vectors.
    vec3 L = normalize(light_dir);
    vec3 N = normalize(normal_mv);

    // Compute the diffuse color component.
    vec3 diffuse = Id * kd * max(dot(N,L),0.0);

    // Assemble the color.
    color = mapped + diffuse;

}

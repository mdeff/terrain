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
    vec3 mapped;

    // Blue for see level (at and below ground)
    const vec3 seeColor = vec3(0.0, 0.0, 0.5);
    // Yellow at the ground level.
    const float ground = 0.0f;
    const vec3 groundColor = vec3(0.6f, 0.6f, 0.0f);
    // Green at vegetation start.
    const float vegstart = 0.05f;
    const vec3 vegstartColor = vec3(0.0f, 0.4f, 0.0f);
    // Black at the vegetation limit.
    const float veglim = 0.15f;
    const vec3 veglimColor = vec3(0.0f, 0.0f, 0.0f);
    // White at the mountain top.
    const float top = 0.3f;
    const vec3 topColor = vec3(1.0f, 1.0f, 1.0f);

    // Set min and max values for interpolation.
    float minHeight, maxHeight;
    vec3 minColor, maxColor;
    if(displaced.z > veglim) {
        maxHeight = top;
        minHeight = veglim;
        maxColor  = topColor;
        minColor  = veglimColor;
    }
    if(displaced.z > vegstart && displaced.z <= veglim) {
        maxHeight = veglim;
        minHeight = vegstart;
        maxColor  = veglimColor;
        minColor  = vegstartColor;
    }
    if(displaced.z > ground && displaced.z <= vegstart) {
        maxHeight = vegstart;
        minHeight = ground;
        maxColor  = vegstartColor;
        minColor  = groundColor;
    }
    if(displaced.z <= ground) {
        maxHeight = vegstart;
        minHeight = ground;
        maxColor  = seeColor;
        minColor  = seeColor;
    }

    // Linear interpolation of the color.
    mapped = (displaced.z-minHeight) / (maxHeight-minHeight) * (maxColor-minColor) + minColor;

    // Normalize the vectors.
    vec3 L = normalize(light_dir);
    vec3 N = normalize(normal_mv);

    // Compute the diffuse color component.
    vec3 diffuse = Id * kd * max(dot(N,L),0.0);

    // Assemble the colors.
    color = mapped + diffuse;

}

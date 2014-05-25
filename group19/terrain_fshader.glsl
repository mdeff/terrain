#version 330 core

// Light properties.
uniform vec3 Ia, Id, Is;

// Texture 0. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D heightMapTex;

// Texture 1. Defined by glActiveTexture and passed by glUniform1i.
// Shadow sampler for percentage closer filtering (PCF).
//uniform sampler2D shadowMapTex;
uniform sampler2DShadow shadowMapTex;

// Environmental textures 2-7. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D sandTex, iceMoutainTex, treeTex, stoneTex, underWaterTex, snowTex;

// Vertices 3D position (after heightmap displacement) in world space.
in vec3 vertexPosition3DWorld;

// Vertex position in light source clip space.
in vec3 ShadowCoord;

// Light and view directions.
in vec3 lightDir, viewDir;

// First output buffer is pixel color (mandatory output, gl_FragColor).
layout(location = 0) out vec3 color;


// Different levels of height for texture mapping.
const float ground = 0.008f;  //water height level
const float sand = 0.015f;
const float forest = 0.11f;
const float snow = 0.1f;


vec3 compute_normal(vec3 position) {

    const ivec3 off = ivec3(-1, 0, 1);
    // Retrieve the size of the generated height map texture.
    int width = textureSize(heightMapTex, 0).x;
    vec2 size = vec2(2.0/width, 0.0);

    //current UV coordinate
    vec2 UV = vec2((position.xy +1.0)/2.0);  
  
    //first calculate the normal vector using finite difference
    float s11 = texture(heightMapTex, UV).r;
    float s01 = textureOffset(heightMapTex, UV, off.xy).r;
    float s21 = textureOffset(heightMapTex, UV, off.zy).r;
    float s10 = textureOffset(heightMapTex, UV, off.yx).r;
    float s12 = textureOffset(heightMapTex, UV, off.yz).r;

    vec3 va = normalize(vec3(size.xy, s21 - s01));
    vec3 vb = normalize(vec3(0.0, size.x,  s12 - s10));
    vec3 tmp = cross(va,vb);
    vec3 normal = vec3(tmp.xy, 2*tmp.z);
    
    return normalize(normal);

}


vec3 texture_mapping(vec3 position, vec3 normal) {

    // Color dependent on the elevation (similar to texture mapping).
    vec3 mapped;

    float slope = smoothstep(0.45, 0.55 , normal.z);

    if(position.z < ground) {
        mapped = texture2D(sandTex, 60*position.xy).rgb;
    } else if (position.z < sand) {
        mapped = texture2D(sandTex, position.xy).rgb;
    } else if (position.z  < forest) {  //mix between forest and rock
        vec3 stone = texture2D(stoneTex, 10*position.xy).rgb;
        vec3 forest = texture2D(treeTex, 10*position.xy).rgb;
        mapped = mix(stone, forest, slope);
    } else {
        mapped = texture2D(snowTex, position.xy).rgb;
    }

    return mapped;

}


float shadowmap(vec3 coord) {

    // Small epsilon to avoid Z-fighting : from 0.001 to 0.00001.
    const float bias = 0.001;

    // The texture only stores one component : r (red).
    // Z is the distance to the camera in camera space.

    // Simple binary shadow test.
//    float visibility = 1.0;
//    if(texture(shadowMapTex, coord.xy).r  <  (coord.z-bias)) {
//        visibility = 0.0;
//    }

    // Percentage closer filtering (PCF).
    // Need sampler2DShadow and compare function in texture parameters.
    vec3 UVC = vec3(coord.xy, coord.z-bias);
    float visibility = texture(shadowMapTex, UVC);

    return visibility;

}


void main() {

    // Normalize the vectors.
    vec3 L = normalize(lightDir);
    vec3 V = normalize(viewDir);

    // Compute the normal.
    vec3 normal = compute_normal(vertexPosition3DWorld);

    // Retrieve material properties.
    vec3 material = texture_mapping(vertexPosition3DWorld, normal);

    // Specular lightning only relevant for water surfaces.
    float ka, kd, ks;   
    ka = 0.4f;
    kd = 0.7f;

    //Diffuse component
    vec3 ambient, diffuse;

    // Compute diffuse : "color" of the object.
     diffuse = Id * kd * material * max(dot(normal,L),0.0);

    // Compute ambient : simulates indirect lighting.

    /* different setting for snow */
    if (vertexPosition3DWorld.z >= forest){
            ambient = vec3(0.9f,0.9f,0.9f)*0.8*material;
    } else {
            ambient = Ia * ka * material;
    }

    // Query the visibility.
    float visibility = shadowmap(ShadowCoord);

    // Assemble the colors. No specular term
    color = ambient + visibility * diffuse;

}

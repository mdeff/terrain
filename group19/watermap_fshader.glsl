#version 330 core

// Transformation matrices from world space to camera clip space.
uniform mat4 projection;
uniform mat4 view;

// Light properties.
uniform vec3 Ia, Id, Is;

// Time for water animation.
uniform float time;

// Textures.
uniform sampler2D flippedTerrainTex;
uniform sampler2D waterNormalMap;
uniform sampler2D riverSurfaceMap;

// Vertices 3D position (after heightmap displacement) in world space.
in vec3 vertexPosition3DWorld;

// Coordinates for flipped camera texture look-up.
// Vertex position in flipped camera clip space.
//in vec2 flippedCameraCoord;
//in vec4 vertexPositionFlippedCamera;

// Light and view directions.
in vec3 lightDir, viewDir;

// First output buffer is pixel color (mandatory output, gl_FragColor).
layout(location = 0) out vec4 color;



layout(pixel_center_integer) in vec4 gl_FragCoord;


void main() {

    // Normalize the vectors.
//    vec3 L = normalize(lightDir);
//    vec3 V = normalize(viewDir);

    //current UV coordinate
//    vec2 UV = vec2((vertexPosition3DWorld.xy +1.0)/2.0);

    //reflection UV
//    vec2 ref_UV = reflectionCoord.xy; //gl_FragCoord.xy * 0.5 + 0.5;

    //Get the normal vector from normal map
//    vec3 normal = normalize(texture(waterNormalMap,ref_UV).rgb);

    //Define other properties of light for water surface
//    float power = 60.0f;
//    float ka, kd, ks;
//    ka = 0.6f;
//    kd = 0.4f;
//    ks = 0.6f;

    //Get the texture value from texture map
//    float time_tmp = time * 0.0002;
//    vec3 material = texture2D(riverSurfaceMap,vec2(cos(UV.x + time_tmp), sin(UV.y-time_tmp))).rgb;
//    vec3 material = texture2D(reflectionTex, ref_UV).rgb;
     // Compute the ambient color component based on texture mapping.
//    vec3 ambient = Ia * ka * material;

    // Compute the diffuse color component.
//    vec3 diffuse = Id * kd * material * max(dot(normal,L),0.0);

    // Hack : water is normal mapped for diffuse lightning and flat for specular.
//    normal = vec3(0.0, 0.0, 1.0);

    // Compute the specular color component.
//    vec3 specular = Is * ks * material * pow(max(dot(V,reflect(L,normal)),0.0),power);

//    color = vec4(material,0.6);; //vec4(ambient + diffuse + specular, 1.0);


    // Transform vertex position from world space to flipped camera clip space.
    // Account for perspective by dividing by w. Map from camera coordinates in
    // (-1,-1)x(1,1) to texture coordinates in (0,0)x(1,1).
    vec4 vertexPositionFlippedCamera = projection * view * vec4(vertexPosition3DWorld, 1.0);
    vec2 flippedCameraCoord = vertexPositionFlippedCamera.xy / vertexPositionFlippedCamera.w * 0.5 + 0.5;


//    vec2 flippedCameraCoord = vertexPositionFlippedCamera.xy / vertexPositionFlippedCamera.w * 0.5 + 0.5;
    color = vec4(texture(flippedTerrainTex, flippedCameraCoord).rgb, 1.0);


//    color = vec4(texture(flippedTerrainTex, vertexPosition3DWorld.xy*0.5+0.5).rgb, 1.0);

//    color = vec4(0,0,0, 1.0);


    // Texture test (need to pass a fullscreen quad to be called on every pixel).
//    color = vec4(texelFetch(flippedTerrainTex, ivec2(gl_FragCoord.xy), 0).rgb, 0.5);

}

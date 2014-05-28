#version 330 core

// Transformation matrices from world space to camera clip space.
uniform mat4 projection;
uniform mat4 view;

// Light properties.
uniform vec3 Ia, Id, Is;

// Time for water animation.
uniform float deltaT;

// Textures.
uniform sampler2D reflectionTex;
uniform sampler2D waterNormalMap;
uniform sampler2D riverSurfaceMap;


// Light source position in world space.
uniform vec3 lightPositionWorld;


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
    vec3 L = normalize(lightDir);
    vec3 V = normalize(viewDir);

    // Normal vector.
    vec3 N = vec3(0.0, 0.0, 1.0);

    //current UV coordinate
//    vec2 UV = vec2((vertexPosition3DWorld.xy +1.0)/2.0);

    //reflection UV
//    vec2 ref_UV = reflectionCoord.xy; //gl_FragCoord.xy * 0.5 + 0.5;

    //Get the normal vector from normal map
//    vec3 normal = normalize(texture(waterNormalMap,ref_UV).rgb);

    //Define other properties of light for water surface
    float power = 60.0f;
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
    vec4 vertexPositionClip = projection * view * vec4(vertexPosition3DWorld, 1.0);
    vec2 flippedCameraCoord = vertexPositionClip.xy / vertexPositionClip.w * 0.5 + 0.5;



    // Normal vector transformed in camera space.
    vec3 normalCamera = normalize(vec3(transpose(inverse(view)) * vec4(0,0,1,1)));

    // Balance between reflected and refrative light. Transparent when N
    // and V are parallel, purely reflective when they are perpendicular.
    float alpha = 1.0 - 1.0 * dot(-V, normalCamera);


//    vec2 flippedCameraCoord = vertexPositionFlippedCamera.xy / vertexPositionFlippedCamera.w * 0.5 + 0.5;
    color = vec4(texture(reflectionTex, flippedCameraCoord).rgb, alpha);




//    vec3 viewDirr = normalize(vec3(view * vec4(vertexPosition3DWorld,1.0)));
//    color = vec4(-viewDirr, 1.0); // * vec4(vertexPosition3DWorld, 1.0);
//    color = vec4(-V.y,-V.y,-V.y, 1.0);
//    color = vec4(texture(reflectionTex, vertexPosition3DWorld.xy*0.5+0.5).rgb, 1.0);

//    color = vec4(0,0,0, 1.0);
//    color = vec4(vec3(alpha), 1.0);
//    color = vec4(vec3(dot(N,-viewDirr)), 1.0);

//    vec4 normalView1 = inverse(transpose(view)) * vec4(0,0,1,1);
//    vec3 normalView2 = normalize(vec3(normalView1));


//    vec3 bluewish = vec3(0.32, 0.56, 0.64);
//    color = vec4(bluewish * pow(max(dot(V,reflect(L,N2)),0.0),power), 1.0);

//    color = vec4(vec3(dot(V, normalView2)), 1.0);


//    color = vec4(vec3(dot(vec3(0,0,1),L)), 1.0);
    // Texture test (need to pass a fullscreen quad to be called on every pixel).
//    color = vec4(texelFetch(reflectionTex, ivec2(gl_FragCoord.xy), 0).rgb, 1.0);

}

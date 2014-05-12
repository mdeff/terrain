#version 330 core

uniform vec3 Ia;
uniform vec3 Id;
uniform vec3 Is;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float p;
uniform float time;

uniform int N; //size of the grid

// First texture. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D heightMapTex;

// Environmental textures.
uniform sampler2D sandTex;
uniform sampler2D iceMoutainTex;
uniform sampler2D treeTex;
uniform sampler2D stoneTex;
uniform sampler2D waterTex;
uniform sampler2D snowTex;
uniform sampler2D waterNormalMap;


uniform sampler2D shadowMapTex;
//uniform sampler2DShadow shadowMapTex;
in vec4 ShadowCoord;


// Position (world coordinates) after heightmap displacement.
in vec3 displaced;
// Position (camera coordinates) after heightmap displacement.
in vec3 displaced_mv;


//light direction
in vec3 light_dir;
//view direction
in vec3 view_dir;

// First output buffer is pixel color.
// gl_FragColor
layout(location = 0) out vec3 color;


// Different levels of height for texture mapping
const float ground = 0.01f;
const float sandMax = 0.015f;
const float forestMin = 0.025f;
const float forestMax = 0.25f;
const float snowMin= 0.315f;
const float snowMax = 0.425;


vec3 compute_normal() {

    const ivec3 off = ivec3(-1, 0, 1);
    const vec2 size = vec2(2.0/1024.0, 0.0);   //1024 is the size of the generated height map

    //current UV coordinate
    vec2 UV = vec2((displaced.xy +1.0)/2.0);

    // if it is water region, use normal from normal map
    // otherwise need to calculate it
    vec3 normal;
    if (displaced.z < ground) {
        normal = normalize(texture(waterNormalMap,UV).rgb);
    } else {
        //first calculate the normal vector using finite difference
        float s11 = texture(heightMapTex, UV).r;
        float s01 = textureOffset(heightMapTex, UV, off.xy).r;
        float s21 = textureOffset(heightMapTex, UV, off.zy).r;
        float s10 = textureOffset(heightMapTex, UV, off.yx).r;
        float s12 = textureOffset(heightMapTex, UV, off.yz).r;

        vec3 va = normalize(vec3(size.xy, s21 - s01));
        vec3 vb = normalize(vec3(0.0, size.x,  s12 - s10));
        vec3 tmp = cross(va,vb);
        normal = normalize(vec3(tmp.xy, 2*tmp.z));
    }
    return normal;
}


vec3 texture_mapping(vec3 normal) {

    // Color dependent on the elevation (similar to texture mapping).
    vec3 mapped;

    float slope = smoothstep(0.35, 0.65 , normal.z);

    if(displaced.z < ground) {
        mapped = texture2D(waterTex, 5*vec2(displaced.x+ cos(time/5000.0),displaced.y+sin(time/5000.0))).rgb;
    } else if (displaced.z < sandMax) {
        mapped = texture2D(sandTex, displaced.xy).rgb;
    } else if (displaced.z < forestMin) {  //mix between sand, rock
        vec3 stone = texture2D(stoneTex, 10*displaced.xy).rgb;
        vec3 sand = texture2D(sandTex, 30*displaced.xy).rgb;
        mapped = mix(stone, sand, slope);
    } else if (displaced.z  < forestMax) {  //mix between forest and rock
        vec3 stone = texture2D(stoneTex, 10*displaced.xy).rgb;
        vec3 forest = texture2D(treeTex, 10*displaced.xy).rgb;
        mapped = mix(stone, forest, slope);
    } else if (displaced.z < snowMin) { //mix between forest, rock and snow
        vec3 stone = texture2D(stoneTex, 10*displaced.xy).rgb;
        vec3 ice = texture2D(iceMoutainTex, 10*displaced.xy).rgb;
        vec3 forest = texture2D(treeTex, 20*displaced.xy).rgb;
        if (slope > 0.5)
            mapped = mix(stone, forest, slope);
        else
            mapped = mix(forest, ice, 2.0*(displaced.z-forestMax)/(snowMin-forestMax));
    } else if (displaced.z < snowMax) {
        vec3 snow = texture2D(snowTex, 60*displaced.xy).rgb;
        vec3 iceMoutain = texture2D(iceMoutainTex, 20*displaced.xy).rgb;
        mapped = mix(iceMoutain, snow, (displaced.z - snowMin)/(snowMax-snowMin));
    } else {
        mapped = texture2D(snowTex, 60*displaced.xy).rgb;
    }

    return mapped;
}


void main() {

    // Normalize the vectors.
    vec3 L = normalize(light_dir);
    vec3 V = normalize(view_dir);

    // Compute the normal.
    vec3 normal = compute_normal();

    // Compute the diffuse color component.
    vec3 diffuse = Id * kd * max(dot(normal,L),0.0);

    // Compute the specular color component.
    vec3 specular = Is * ks * pow(max(dot(V,reflect(L,normal)),0.0),p);

    // Compute the ambient color component based on texture mapping.
    vec3 ambient = Ia * ka * texture_mapping(normal);

    // Assemble the colors.
    color = ambient + diffuse + specular;




    vec3 light = vec3(0.8);

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO: Practical 6.
    /// 1) Assign the texture color in tex at position UV to diffuse instead of the interpolated vertexcolor
    ///<<<<<<<<<< TODO <<<<<<<<<<<
//    vec3 diffuse = vcolor;

    //Shadow / visibility
    float bias = 0.005;  // 0.001
    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO: Practical 6.
    /// 2) query the visibility of ShadowCoord in shadowMap, bias the query by subtracting bias. What happens without bias?
    /// Hint: Divide the ShadowCoord by its w-component before using it as a 3d point.
    /// Ressources: https://www.opengl.org/wiki/Sampler_(GLSL)#Shadow_samplers
    ///<<<<<<<<<< TODO <<<<<<<<<<<
    // The texture only stores one component : r (red).
    // Z is the distance to the camera in camera space.
    // A perspective transformation is not affine, and as such, can't be represented entirely by a matrix. After beeing multiplied by the ProjectionMatrix, homogeneous coordinates are divided by their own W component. This W component happens to be -Z (because the projection matrix has been crafted this way). This way, points that are far away from the origin are divided by a big Z; their X and Y coordinates become smaller; points become more close to each other, objects seem smaller; and this is what gives the perspective.
    float visibility = 1.0;
    //if(texture(shadowMapTex, ShadowCoord.xy).r  <  ShadowCoord.z) {
    //if(texture(shadowMapTex, ShadowCoord.xy).r < ShadowCoord.z - bias) {
    //if(textureProj(shadowMapTex, ShadowCoord.xy).r < ShadowCoord.z - bias) {
    if(texture(shadowMapTex, ShadowCoord.xy/ShadowCoord.w).r  <  (ShadowCoord.z-bias)/ShadowCoord.w) {
    //if(textureProj(shadowMapTex, ShadowCoord.xyw).r  <  (ShadowCoord.z-bias)/ShadowCoord.w) {
        visibility = 0.5;
    }

//    color =
//     // Ambient : simulates indirect lighting
//     MaterialAmbientColor +
//     // Diffuse : "color" of the object
//     visibility * MaterialDiffuseColor * LightColor * LightPower * cosTheta+
//     // Specular : reflective highlight, like a mirror
//     visibility * MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5);

//    color = ambient + visibility * diffuse + visibility * specular;
//    color = visibility * diffuse + visibility * specular;
    color = visibility * ambient;

    // Observe the shadow map.
    //color = vec3(texture(shadowMapTex, ShadowCoord.xy).r);
    //color = vec3(texture2D(shadowMapTex, ShadowCoord.xy).r);
//    color = vec3(textureProj(shadowMapTex, ShadowCoord.xy).r);
    //color = vec3(texture(shadowMapTex, ShadowCoord.xy/ShadowCoord.w).r);

    // Observe distance from light.
    //color = vec3(ShadowCoord.x);
}

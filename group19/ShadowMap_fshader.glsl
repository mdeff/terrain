#version 330 core
in vec2 UV;
in vec4 ShadowCoord;
in vec3 vcolor;

layout(location = 0) out vec3 color;

uniform sampler2D tex;
uniform sampler2DShadow shadowMap;

void main(){
    vec3 light = vec3(0.8);
    
    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO: Practical 6.
    /// 1) Assign the texture color in tex at position UV to diffuse instead of the interpolated vertexcolor
    ///<<<<<<<<<< TODO <<<<<<<<<<<
    vec3 diffuse = vcolor;
    
    //Shadow / visibility
    float bias = 0.001;
    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO: Practical 6.
    /// 2) query the visibility of ShadowCoord in shadowMap, bias the query by subtracting bias. What happens without bias?
    /// Hint: Divide the ShadowCoord by its w-component before using it as a 3d point.
    /// Ressources: https://www.opengl.org/wiki/Sampler_(GLSL)#Shadow_samplers
    ///<<<<<<<<<< TODO <<<<<<<<<<<
    float visibility = 1.0;
    
    color = light * diffuse * visibility;
}

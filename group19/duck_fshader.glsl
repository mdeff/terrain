#version 330 core

uniform vec3 Ia, Id, Is;
uniform vec3 ka, kd, ks;
uniform float p;


// TODO: These variables need to be used to compute the phong shading.
in vec3 normal_mv;
in vec3 light_dir, view_dir;

out vec3 color;

void main() {
    color = vec3(0.0,0.0,0.0);

	//Since length is not preserved, we need to
	//re-normalize the input vectors first
	vec3 N = normalize(normal_mv);
	vec3 L = normalize(light_dir);
	vec3 V = normalize(view_dir);

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO: Phong shading.
    /// 1) compute ambient term.
	vec3 ambient_color = Ia * ka;
    /// 2) compute diffuse term.
	vec3 diffuse_color = Id * kd * max(dot(N,L),0.0);
    /// 3) compute specular term.
	vec3 specular_color = Is * ks * pow(max(dot(V,reflect(L,N)),0.0),p);
    ///<<<<<<<<<< TODO <<<<<<<<<<<
	color = ambient_color + diffuse_color + specular_color;
	color = vec3(1.0f,1.0f,0.0f);
}

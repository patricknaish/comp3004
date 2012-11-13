#version 150
in vec3 position;
in vec3 in_normal;
out vec4 ex_color;
uniform vec4 LightV;
uniform vec4 LightC;
uniform vec4 Material;
uniform mat4 MVP;
 
void main(){
    vec4 v = vec4(position,1);
    gl_Position = MVP * v;
	vec4 n = vec4(in_normal,0);
	ex_color = Material * LightC * dot(LightV, n);
}
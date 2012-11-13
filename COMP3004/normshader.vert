#version 150
in vec3 position;
in vec3 in_normal;
out vec4 ex_normal;
uniform mat4 MVP;
 
void main(){
    vec4 v = vec4(position,1);
    gl_Position = MVP * v;
	ex_normal = vec4(in_normal,0);
}
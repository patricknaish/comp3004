#version 150
precision highp float;

in vec4 ex_color;
out vec4 gl_FragColor;

void main(void) {
	gl_FragColor = ex_color;
}

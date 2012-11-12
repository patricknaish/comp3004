#version 150
precision highp float;

in vec3 ex_color;
out vec4 gl_FragColor;

void main(void) {
	//gl_FragColor = vec4(1.0,0.0,0.0,1.0) * (1.0,1.0,1.0,1.0) * dot(vec4(1.0), ex_Normal);
	gl_FragColor = vec4(ex_color,1);
}

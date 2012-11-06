#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

int generateSphere(float radius, int ) {

	glm::vec3 pos;

	return 0;

}

int render(void) {
	return 0;
}

int main(void) {
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	if (!glfwOpenWindow(600,600,0,0,0,0,0,0,GLFW_WINDOW)) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	int running = GL_TRUE;
	double start_time = glfwGetTime();
	int frame_count = 0;
	char title_str[255];
	while( running ) { 
		double current_time = glfwGetTime();
		if (current_time - start_time > 1) {
			sprintf_s(title_str, "%2.1f FPS", frame_count/(current_time-start_time));
			glfwSetWindowTitle(title_str);
			frame_count = 0;
			start_time = current_time;
		}
		frame_count++;

		render();
        glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
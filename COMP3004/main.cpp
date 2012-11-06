#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

int generateSphere(float radius, int ) {

	glm::vec3 pos;

}

int render(void) {

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
	int start_time = glfwGetTime();
	while( running ) { 
		
		int current_time = glfwGetTime();
		//Do something with the time
		start_time = current_time;

		render();
        glfwSwapBuffers();
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
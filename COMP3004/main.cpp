#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

using namespace std; 
using namespace glm;

typedef struct {
	double x, y, z;
} Vertex;

GLuint vao, vbo;
vector<Vertex> sphereVerts;
vector<GLushort> sphereIndices;

void generateSphere(double rad, int slices, int sectors) {

	double phi, theta;
	double x, y, z;

	for (int i = 0; i <= slices; i++) {
		phi = 2*M_PI*((double)i/(double)slices);
		for (int j = 0; j <= sectors; j++) {
			theta = M_PI * ((double)j/(double)sectors);
			x = (double)(rad * sin(theta) * cos(phi));
			y = (double)(rad * sin(theta) * sin(phi));
			z = (double)(rad * cos(theta));
			Vertex vals = {x, y, z};
			sphereVerts.push_back(vals);
			sphereIndices.push_back((i+1) * sectors + j+1);
			sphereIndices.push_back(i * sectors + j);
			sphereIndices.push_back((i+1) * sectors + j+1);
			sphereIndices.push_back(i * sectors + j);
		}
	}
}

void render() {
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(vao);
	//glDrawArrays(GL_LINES, 0, sphereVerts.size());
	glDrawElements(GL_QUAD_STRIP, sphereIndices.size(), GL_UNSIGNED_SHORT, &sphereIndices[0]);
	glBindVertexArray(0);

	glFlush();
}

void GLFWCALL keyHandler(int key, int action) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESC || key == 'Q' || key == 'q') {
			glfwTerminate();
			exit(EXIT_SUCCESS);
		}
		switch(key) {
			case 'A': ;
			case 'a': /*do something*/ break;
			case 'B': ;
			case 'b': /*do something*/ break;
			case 'C': ;
			case 'c': /*do something*/ break;
			case 'D': ;
			case 'd': /*do something*/ break;
			case 'E': ;
			case 'e': /*do something*/ break;
		}
	}
}

int main(void) {
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	if (!glfwOpenWindow(600,600,0,0,0,0,0,0,GLFW_WINDOW)) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glewInit();
	glfwSetKeyCallback(keyHandler);

	//Sphere stuff
	generateSphere(0.8,40,40);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sphereVerts.size() * sizeof(Vertex), &sphereVerts[0], GL_STATIC_DRAW); 

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao); 
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);

	//Running stuff
	int running = GL_TRUE;
	double start_time = glfwGetTime();
	int frame_count = 0;
	char title_str[255];
	while( running ) { 
		double current_time = glfwGetTime();
		if (current_time - start_time >= 1) {
			sprintf_s(title_str, "%2.1f FPS", frame_count/(current_time-start_time));
			glfwSetWindowTitle(title_str);
			frame_count = 0;
			start_time = current_time;
		}
		frame_count++;

		render();
        glfwSwapBuffers();
		running = glfwGetWindowParam(GLFW_OPENED);
	}
}
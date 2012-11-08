#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace std; 
using namespace glm;

typedef struct {
	double x, y, z;
} Vertex;

GLuint vao, vbo;
GLchar *vertexsource, *fragmentsource;
GLuint vertexshader, fragmentshader;
GLuint shaderProgram;

vector<Vertex> sphereVerts;
vector<GLushort> sphereIndices;
double start_time = 0;

//Modified from Tutorial 2
char* filetobuf(char *file) { /* A simple function that will read a file into an allocated char pointer buffer */
    FILE *fptr;
    long length;
    char *buf;
    fptr = fopen(file, "rb"); /* Open file for reading */
    if (!fptr) { /* Return NULL on failure */
        fprintf(stderr, "failed to open %s\n", file);
        return NULL;
        }
    fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
    length = ftell(fptr); /* Find out how many bytes into the file we are */
    buf = (char*)malloc(length+1); /* Allocate a buffer for the entire length of the file and a null terminator */
    fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
    fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
    fclose(fptr); /* Close the file */
    buf[length] = 0; /* Null terminator */
    return buf; /* Return the buffer */
}

//Modified from Tutorial 2
void setupShaders() {
	char text[1000];
    int length;
    fprintf(stderr, "Set up shaders\n"); /* Allocate and assign two Vertex Buffer Objects to our handle */
    vertexsource = filetobuf("shader.vert"); /* Read our shaders into the appropriate buffers */
    fragmentsource = filetobuf("shader.frag");
    vertexshader = glCreateShader(GL_VERTEX_SHADER); /* Assign our handles a "name" to new shader objects */
    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0); /* Associate the source code buffers with each handle */
    glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
    glCompileShader(fragmentshader);/* Compile our shader objects */
    glCompileShader(vertexshader);
    shaderProgram = glCreateProgram();/* Assign our program handle a "name" */
    glAttachShader(shaderProgram, vertexshader); /* Attach our shaders to our program */
    glAttachShader(shaderProgram, fragmentshader);
    glLinkProgram(shaderProgram); /* Link our program */
    glGetProgramInfoLog(shaderProgram, 1000, &length, text); // Check for errors
    if(length>0)
        fprintf(stderr, "Validate Shader Program\n%s\n",text );
    glUseProgram(shaderProgram); /* Set it as being actively used */
}

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
			case GLFW_KEY_UP: ; break;
			case GLFW_KEY_DOWN: ; break;
			case GLFW_KEY_RIGHT: ; break;
			case GLFW_KEY_LEFT: ; break;
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
	glEnable(GL_DEPTH_TEST);

	setupShaders();

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
	int frame_count = 0;
	char title_str[255];
	float rotation = 0.f;
	while( running ) { 
		double current_time = glfwGetTime();
		if (current_time - start_time >= 1) {
			sprintf_s(title_str, "%2.1f FPS", frame_count/(current_time-start_time));
			glfwSetWindowTitle(title_str);
			frame_count = 0;
			start_time = current_time;
		}
		frame_count++;

		mat4 Projection = perspective(45.0f, 1.0f, 0.1f, 100.0f);
			mat4 View = lookAt(vec3(0,2,1), vec3(0,0,0), vec3(0,1,0));
			View = rotate(View, rotation, vec3(1, 1, 1));
			mat4 Model = mat4(1.0f);
			mat4 MVP = Projection * View * Model;
			GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			rotation++;
			if (rotation >= 360) {
				rotation = 0;
			}
		
		render();
        glfwSwapBuffers();
		running = glfwGetWindowParam(GLFW_OPENED);
		
	}
}
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
#include <SOIL.h>

using namespace std; 
using namespace glm;

typedef struct {
	double x, y, z;
	double nx, ny, nz;
} Vertex;

GLuint vao[4], vbo[4];
GLchar *vertexsource, *fragmentsource;
GLuint vertexshader, fragmentshader;
GLuint wireframeShaderProgram, normalShaderProgram;

const float speed = 60;

//Modified from Tutorial 2
char* filetobuf(char *file) { /* A simple function that will read a file into an allocated char pointer buffer */
    FILE *fptr;
    long length;
    char *buf;
	errno_t err;
    err = fopen_s(&fptr, file, "rb"); /* Open file for reading */
    if (err != 0) { /* Return NULL on failure */
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
	printf("%s read successfully\n", file);
    return buf; /* Return the buffer */
}

//From Tutorial 2
GLuint setupShaders(char *vert, char *frag) {
	GLuint programID;
	char text[1000];
    int length;
    fprintf(stderr, "Setting up shaders...\n"); /* Allocate and assign two Vertex Buffer Objects to our handle */
    vertexsource = filetobuf(vert); /* Read our shaders into the appropriate buffers */
    fragmentsource = filetobuf(frag);
    vertexshader = glCreateShader(GL_VERTEX_SHADER); /* Assign our handles a "name" to new shader objects */
    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0); /* Associate the source code buffers with each handle */
    glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
    glCompileShader(fragmentshader);/* Compile our shader objects */
    glCompileShader(vertexshader);
    programID = glCreateProgram();/* Assign our program handle a "name" */
    glAttachShader(programID, vertexshader); /* Attach our shaders to our program */
    glAttachShader(programID, fragmentshader);
	glBindAttribLocation(programID, 0, "position");
	glBindAttribLocation(programID, 1, "in_normal");
    glLinkProgram(programID); /* Link our program */
    glGetProgramInfoLog(programID, 1000, &length, text); // Check for errors
    if(length>0)
        fprintf(stderr, "Validating shader programs...\n%s\n",text );
	return programID;
}

class IModel {
	public:
		virtual ~IModel() {}
		virtual void render() = 0;
};

/*
 * Sphere generation code adapted from http://stackoverflow.com/questions/8959338/draw-a-sphere-with-opengl
*/
class Sphere: public IModel {
	protected:
		vector<Vertex> sphereVerts, sphereDisplayNormals;
		vector<GLushort> sphereIndices;
		int vboIndex;
	public:
		Sphere(double rad, Vertex centre, int slices, int sectors, int vboIndex) {
			this->vboIndex = vboIndex;

			double phi, theta;
			double x, y, z;
			double nx, ny, nz; //Normals

			for (int i = 0; i <= slices; i++) {
				phi = 2*M_PI*((double)i/(double)slices);
				for (int j = 0; j <= sectors; j++) {
					theta = M_PI * ((double)j/(double)sectors);
					x = (double)(rad * sin(theta) * cos(phi)) + centre.x;
					y = (double)(rad * sin(theta) * sin(phi)) + centre.y;
					z = (double)(rad * cos(theta)) + centre.z;
					nx = (x - centre.x)/rad;
					ny = (y - centre.y)/rad;
					nz = (z - centre.z)/rad;
					Vertex vals = {x, y, z, nx, ny, nz};
					sphereVerts.push_back(vals);
					sphereDisplayNormals.push_back(vals);
					vals.x -= centre.x;
					vals.y -= centre.y;
					vals.z -= centre.z;
					vals.x *= 1.1*rad;
					vals.y *= 1.1*rad;
					vals.z *= 1.1*rad;
					vals.x += centre.x;
					vals.y += centre.y;
					vals.z += centre.z;
					sphereDisplayNormals.push_back(vals);
					sphereIndices.push_back(i * sectors + j);
					if ((i+1) * sectors + j+1 < (slices+1) * (sectors+1)) {
						sphereIndices.push_back((i+1) * sectors + j+1);
					}
				}
			}

			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex]);
			glBufferData(GL_ARRAY_BUFFER, sphereVerts.size() * sizeof(Vertex), &sphereVerts[0], GL_STATIC_DRAW); 

			glBindVertexArray(vao[vboIndex]); 
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex]);
			glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, x));
		}
		void render() {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glBindVertexArray(vao[vboIndex]);
			glDrawElements(GL_QUAD_STRIP, sphereIndices.size(), GL_UNSIGNED_SHORT, &sphereIndices[0]);
			glBindVertexArray(0);
		}
};

class SphereDisplayNormals: public Sphere {
	public:
		SphereDisplayNormals(double rad, Vertex centre, int slices, int sectors, int vboIndex): Sphere(rad, centre, slices, sectors, vboIndex) {
			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex+1]);
			glBufferData(GL_ARRAY_BUFFER, sphereDisplayNormals.size() * sizeof(Vertex), &sphereDisplayNormals[0], GL_STATIC_DRAW); 

			glBindVertexArray(vao[vboIndex+1]);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex+1]);
			glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, x));
		}
		void render() {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex]);
			glBindVertexArray(vao[vboIndex]);
			glDrawElements(GL_QUAD_STRIP, sphereIndices.size(), GL_UNSIGNED_SHORT, &sphereIndices[0]);
			glBindVertexArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex+1]);
			glBindVertexArray(vao[vboIndex+1]);
			glDrawArrays(GL_LINES, 0, sphereDisplayNormals.size());
			glBindVertexArray(0);
		}
};

class SphereShaded: public Sphere {
	public:
		SphereShaded(double rad, Vertex centre, int slices, int sectors, int vboIndex): Sphere(rad, centre, slices, sectors, vboIndex) {
			glBindVertexArray(vao[vboIndex]);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex]);
			glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, nx));
		}
		void render () {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex]);
			glBindVertexArray(vao[vboIndex]);
			glDrawElements(GL_QUAD_STRIP, sphereIndices.size(), GL_UNSIGNED_SHORT, &sphereIndices[0]);
			glBindVertexArray(0);
		}
};

class Cone: public IModel {
	protected:
		vector<Vertex> coneVerts;
		vector<GLushort> coneIndices;
		int vboIndex;
	public:
		Cone(double rad, double height, Vertex centre, int sectors, int vboIndex) {
			this->vboIndex = vboIndex;

			double theta;
			double x, y, z;

			Vertex origin = centre;
			Vertex peak = {centre.x,centre.y,centre.z+height};
			coneVerts.push_back(origin);
			coneVerts.push_back(peak);
			coneIndices.push_back(0);

			for (int i = 0; i <= sectors; i++) {
				theta = 2*M_PI*((double)i/(double)sectors);
				x = (double)(rad * cos(theta)) + centre.x;
				y = (double)(rad * sin(theta)) + centre.y;
				z = centre.z;
				Vertex vals = {x, y, z};
				coneVerts.push_back(vals);
				coneIndices.push_back(i+2);
				if (i+3 <= sectors) {
					coneIndices.push_back(i+3);
				}
				if (i != sectors) {
					coneIndices.push_back(0);
				}
			}

			coneIndices.push_back(1);

			for (int i = 0; i < sectors; i++) {
				coneIndices.push_back(i+2);
				coneIndices.push_back(i+3);
				coneIndices.push_back(1);
			}

			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex]);
			glBufferData(GL_ARRAY_BUFFER, coneVerts.size() * sizeof(Vertex), &coneVerts[0], GL_STATIC_DRAW); 

			glBindVertexArray(vao[vboIndex]); 
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex]);
			glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, x));
		}
		void render() {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glBindVertexArray(vao[vboIndex]);
			glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_SHORT, &coneIndices[0]);
			glBindVertexArray(0);
		}
};

class IScene {
	public:
		virtual ~IScene() {}
		virtual void run() = 0;
		virtual void interrupt() = 0;
};

/*
 * Camera manipulations adapted from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
 */
class SceneA: public IScene {
	private:
		int running;	
	public:
		SceneA() {}
		void run() {
			glUseProgram(wireframeShaderProgram);

			Vertex centre = {0,0,0};

			Sphere sphere = Sphere(1, centre, 30, 30, 0);

			//Running stuff
			running = GL_TRUE;
			double old_time = 0, fps_time = 0;
			int frame_count = 0;
			char title_str[255];
			float rotation = 0.f;
			while( running ) { 
				double current_time = glfwGetTime();
				rotation += (float)((current_time - old_time) * speed);
				if (rotation >= 360.f) {
					rotation = 0.f;
				}
				old_time = current_time;
				if (current_time - fps_time >= 1) {
					sprintf_s(title_str, "%2.1f FPS", frame_count/(current_time-fps_time));
					glfwSetWindowTitle(title_str);
					frame_count = 0;
					fps_time = current_time;
				}
				frame_count++;

				mat4 Projection = perspective(45.0f, 1.0f, 0.1f, 100.0f);
				mat4 View = lookAt(vec3(0,2,1), vec3(0,0,0), vec3(0,1,0));
				View = rotate(View, rotation, vec3(1, 1, 1));
				View = scale(View, vec3(0.7f));
				mat4 Model = mat4(1.0f);
				mat4 MVP = Projection * View * Model;
				GLuint MatrixID = glGetUniformLocation(wireframeShaderProgram, "MVP");
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		
				glClearColor(0,0,0,0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				sphere.render();
				glFlush();
				glfwSwapBuffers();
				if (!glfwGetWindowParam(GLFW_OPENED)) {
					running = GL_FALSE;
				}
			}
		}
		void interrupt() {
			running = GL_FALSE;
		}
};

class SceneB: public IScene {
	private:
		int running;
	public:
		SceneB() {}
		void run() {
			glUseProgram(wireframeShaderProgram);

			Vertex centre = {0,0,0};

			Cone cone = Cone(1, 1, centre, 30, 0);

			//Running stuff
			running = GL_TRUE;
			double old_time = 0, fps_time = 0;
			int frame_count = 0;
			char title_str[255];
			float rotation = 0.f;
			while( running ) { 
				double current_time = glfwGetTime();
				rotation += (float)((current_time - old_time) * speed);
				if (rotation >= 360.f) {
					rotation = 0.f;
				}
				old_time = current_time;
				if (current_time - fps_time >= 1) {
					sprintf_s(title_str, "%2.1f FPS", frame_count/(current_time-fps_time));
					glfwSetWindowTitle(title_str);
					frame_count = 0;
					fps_time = current_time;
				}
				frame_count++;

				mat4 Projection = perspective(45.0f, 1.0f, 0.1f, 100.0f);
				mat4 View = lookAt(vec3(0,2,1), vec3(0,0,0), vec3(0,1,0));
				View = rotate(View, rotation, vec3(1, 1, 1));
				View = scale(View, vec3(0.7f));
				mat4 Model = mat4(1.0f);
				mat4 MVP = Projection * View * Model;
				GLuint MatrixID = glGetUniformLocation(wireframeShaderProgram, "MVP");
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		
				glClearColor(0,0,0,0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				cone.render();
				glFlush();
				glfwSwapBuffers();	
				if (!glfwGetWindowParam(GLFW_OPENED)) {
					running = GL_FALSE;
				}
			}
		}
		void interrupt() {
			running = false;
		}
};

class SceneC: public IScene {
	private:
		int running;
	public:
		SceneC() {}
		void run() {
			glUseProgram(wireframeShaderProgram);

			Vertex centre = {0,0,0};

			SphereDisplayNormals sphere = SphereDisplayNormals(1, centre, 30, 30, 0);

			//Running stuff
			running = GL_TRUE;
			double old_time = 0, fps_time = 0;
			int frame_count = 0;
			char title_str[255];
			float rotation = 0.f;
			while( running ) { 
				double current_time = glfwGetTime();
				rotation += (float)((current_time - old_time) * speed);
				if (rotation >= 360.f) {
					rotation = 0.f;
				}
				old_time = current_time;
				if (current_time - fps_time >= 1) {
					sprintf_s(title_str, "%2.1f FPS", frame_count/(current_time-fps_time));
					glfwSetWindowTitle(title_str);
					frame_count = 0;
					fps_time = current_time;
				}
				frame_count++;

				mat4 Projection = perspective(45.0f, 1.0f, 0.1f, 100.0f);
				mat4 View = lookAt(vec3(0,2,1), vec3(0,0,0), vec3(0,1,0));
				View = rotate(View, rotation, vec3(1, 1, 1));
				View = scale(View, vec3(0.7f));
				mat4 Model = mat4(1.0f);
				mat4 MVP = Projection * View * Model;
				GLuint MatrixID = glGetUniformLocation(wireframeShaderProgram, "MVP");
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		
				glClearColor(0,0,0,0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				sphere.render();
				glFlush();
				glfwSwapBuffers();	
				if (!glfwGetWindowParam(GLFW_OPENED)) {
					running = GL_FALSE;
				}
			}
		}
		void interrupt() {
			running = false;
		}
};

class SceneD: public IScene {
	private:
		int running;
	public:
		SceneD() {}
		void run() {
			glUseProgram(normalShaderProgram);

			Vertex centre = {0,0,0};

			SphereShaded sphere = SphereShaded(1, centre, 50, 50, 0);

			//Running stuff
			running = GL_TRUE;
			double old_time = 0, fps_time = 0;
			int frame_count = 0;
			char title_str[255];
			float rotation = 0.f;
			while( running ) { 
				double current_time = glfwGetTime();
				rotation += (float)((current_time - old_time) * speed);
				if (rotation >= 360.f) {
					rotation = 0.f;
				}
				old_time = current_time;
				if (current_time - fps_time >= 1) {
					sprintf_s(title_str, "%2.1f FPS", frame_count/(current_time-fps_time));
					glfwSetWindowTitle(title_str);
					frame_count = 0;
					fps_time = current_time;
				}
				frame_count++;

				mat4 Projection = perspective(45.0f, 1.0f, 0.1f, 100.0f);
				mat4 View = lookAt(vec3(0,2,1), vec3(0,0,0), vec3(0,1,0));
				View = rotate(View, rotation, vec3(1, 1, 1));
				View = scale(View, vec3(0.7f));
				mat4 Model = mat4(1.0f);
				mat4 MVP = Projection * View * Model;
				GLuint MatrixID = glGetUniformLocation(normalShaderProgram, "MVP");
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

				vec4 LightV = vec4(1.f, 1.f, 1.f, 1.f);
				GLuint LightVID = glGetUniformLocation(normalShaderProgram, "LightV");
				glUniform4fv(LightVID, 1, &LightV[0]);
				
				vec4 LightC = vec4(0.7f, 0.7f, 0.7f, 1.f);
				GLuint LightCID = glGetUniformLocation(normalShaderProgram, "LightC");
				glUniform4fv(LightCID, 1, &LightC[0]);
				
				vec4 Material = vec4(1.f, 0.f, 0.f, 1.f);
				GLuint MaterialID = glGetUniformLocation(normalShaderProgram, "Material");
				glUniform4fv(MaterialID, 1, &Material[0]);
		
				glClearColor(0,0,0,0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				sphere.render();
				glFlush();
				glfwSwapBuffers();	
				if (!glfwGetWindowParam(GLFW_OPENED)) {
					running = GL_FALSE;
				}
			}
		}
		void interrupt() {
			running = false;
		}
};

class SceneE: public IScene {
	private:
		int running;
	public:
		SceneE() {}
		void run() {
			glUseProgram(wireframeShaderProgram);

			Vertex sphere1Centre = {0,1,0};
			Sphere sphere1 = Sphere(0.3, sphere1Centre, 30, 30, 0);

			Vertex sphere2Centre = {0,-1,0};
			Sphere sphere2 = Sphere(0.4, sphere2Centre, 30, 30, 1);

			Vertex cone1Centre = {1,0,0};
			Cone cone1 = Cone(0.3, 0.3, cone1Centre, 30, 2);

			Vertex cone2Centre = {-1,0,0};
			Cone cone2 = Cone(0.2, 0.5, cone2Centre, 30, 3);

			//Running stuff
			running = GL_TRUE;
			double old_time = 0, fps_time = 0;
			int frame_count = 0;
			char title_str[255];
			float rotation = 0.f;
			while( running ) { 
				double current_time = glfwGetTime();
				rotation += (float)((current_time - old_time) * speed);
				if (rotation >= 360.f) {
					rotation = 0.f;
				}
				old_time = current_time;
				if (current_time - fps_time >= 1) {
					sprintf_s(title_str, "%2.1f FPS", frame_count/(current_time-fps_time));
					glfwSetWindowTitle(title_str);
					frame_count = 0;
					fps_time = current_time;
				}
				frame_count++;

				mat4 Projection = perspective(45.0f, 1.0f, 0.1f, 100.0f);
				mat4 View = lookAt(vec3(0,3,1), vec3(0,0,0), vec3(0,1,0));
				View = rotate(View, rotation, vec3(1, 1, 1));
				View = scale(View, vec3(0.7f));
				mat4 Model = mat4(1.0f);
				mat4 MVP = Projection * View * Model;
				GLuint MatrixID = glGetUniformLocation(wireframeShaderProgram, "MVP");
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		
				glClearColor(0,0,0,0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				sphere1.render();
				sphere2.render();
				cone1.render();
				cone2.render();
				glFlush();
				glfwSwapBuffers();	
				if (!glfwGetWindowParam(GLFW_OPENED)) {
					running = GL_FALSE;
				}
			}
		}
		void interrupt() {
			running = false;
		}
};

IScene *currScene;
IScene * scenes[5];

void GLFWCALL keyHandler(int key, int action) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESC || key == 'Q' || key == 'q') {
			glfwTerminate();
			exit(EXIT_SUCCESS);
		}
		switch(key) {
			case 'A': ;
			case 'a': currScene->interrupt(); currScene = scenes[0]; currScene->run(); break;
			case 'B': ;
			case 'b': currScene->interrupt(); currScene = scenes[1]; currScene->run(); break;
			case 'C': ;
			case 'c': currScene->interrupt(); currScene = scenes[2]; currScene->run(); break;
			case 'D': ;
			case 'd': currScene->interrupt(); currScene = scenes[3]; currScene->run(); break;
			case 'E': ;
			case 'e': currScene->interrupt(); currScene = scenes[4]; currScene->run(); break;
			case GLFW_KEY_UP: ; break;
			case GLFW_KEY_DOWN: ; break;
			case GLFW_KEY_RIGHT: ; break;
			case GLFW_KEY_LEFT: ; break;
		}
	}
}

int main(void) {
	printf("Initialising... \n");
	if (!glfwInit()) {
		printf("Could not initialise GLFW\n");
		exit(EXIT_FAILURE);
	}
	else {
		printf("Initialised GLFW\n");
	}
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	if (!glfwOpenWindow(600,600,0,0,0,0,0,0,GLFW_WINDOW)) {
		printf("Could not open window");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	if (glewInit() != GLEW_OK) {
		printf("Could not initialise GLEW\n");
		exit(EXIT_FAILURE);
	}
	else {
		printf("Initialised GLEW\n");
	}
	if (glewIsSupported("GL_VERSION_4_2"))
		printf("Ready for OpenGL 4.2\n\n");
	else {
		printf("OpenGL 4.2 not supported\n\n");
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(keyHandler);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);

	glGenBuffers(4, vbo);
	glGenVertexArrays(4, vao);
	if (glGetError() != 0) {
		printf("Could not create vbos or vaos\n");
		exit(EXIT_FAILURE);
	}
	else {
		printf("Generated vbos and vaos\n\n");
	}

	wireframeShaderProgram = setupShaders("wireshader.vert", "wireshader.frag");
	normalShaderProgram = setupShaders("normshader.vert", "normshader.frag");

	printf("Creating scenes...\n");
	SceneA sceneA = SceneA();
	scenes[0] = &sceneA;
	SceneB sceneB = SceneB();
	scenes[1] = &sceneB;
	SceneC sceneC = SceneC();
	scenes[2] = &sceneC;
	SceneD sceneD = SceneD();
	scenes[3] = &sceneD;
	SceneE sceneE = SceneE();
	scenes[4] = &sceneE;
	printf("Created scenes\nRunning scene A\n\n");

	currScene = scenes[0];
	currScene->run();
}
#include "stdafx.h"

// standard
#include <assert.h>
#include <math.h>
#include <stdlib.h> 
#include <time.h>
#include <iostream>

// glut
#include <glm/glm.hpp>
#include <GL/glut.h>


//================================
// global variables
//================================
// screen size
int g_screenWidth = 0;
int g_screenHeight = 0;

// frame index
int g_frameIndex = 0;

// Total number of balls
static int totalballs = 10;

// Original Ball Position
glm::vec3 balls_first_position[20] = { glm::vec3(-3.0f, 8.0f, 0.0f),
										glm::vec3(9.0f, 8.5f, 2.0f),
										glm::vec3(4.0f, 7.2f, 4.0f),
										glm::vec3(-4.5f, 6.8f, 6.8f),
										glm::vec3(3.0f, 8.6f, -3.0f),
										glm::vec3(5.0f, 9.8f, -7.1f),
										glm::vec3(-8.0f, 9.0f, -3.1f),
										glm::vec3(4.0f, 12.0f, 0.5f),
										glm::vec3(0.0f, 8.2f, 0.0f),
										glm::vec3(1.0f, 7.6f, 0.5f)};

// current balls' Velocity in X, Y, Z
glm::vec3 ballVelocity[10];

// balls' volecity after time increased
glm::vec3 velocity_Next[10];

// current balls' position
glm::vec3 position[10];

// balls' position after time increased 
glm::vec3 position_Next[10];

// delta time
GLfloat dt = 0.03f;

// gravity acceleration 
glm::vec3 acceleration = glm::vec3(0, -9.8, 0);

// all balls' matrix
GLfloat ballMatrix[10][16];

// coefficient of collision: velocity after collision would reduce to 0.8 of original volecity 
GLfloat coefficient = 0.8f;

//================================
// init
//================================
void init(void) {

	// init balls
	for (int i = 0; i < totalballs; i++) {
		ballMatrix[i][0] = 1.0f;
		ballMatrix[i][5] = 1.0f;
		ballMatrix[i][10] = 1.0f;
		ballMatrix[i][15] = 1.0f;
		position[i] = balls_first_position[i];

		ballVelocity[i] = glm::normalize(glm::vec3((rand() % 10) - 5, (rand() % 10) - 5, (rand() % 10) - 5));
	}
}

//================================
// ball movement
//================================
void TeapotMovement(int index) {

	// collision with ground and walls
	if (position[index].y < -4.5) 
		ballVelocity[index].y = -coefficient * ballVelocity[index][1];

	if (position[index].x < -9.5 || position[index].x > 9.5) 
		ballVelocity[index].x = -coefficient * ballVelocity[index].x;

	if (position[index].z < -9.5 || position[index].z > 9.5) 
		ballVelocity[index].z = -coefficient * ballVelocity[index].z;

	// Colllision between balls
	for (int i = index + 1; i < totalballs; i++)
	{
		if (glm::distance(position[index], position[i]) < 1.0)
		{
			glm::vec3 normal = glm::normalize(position[index] - position[i]);
			glm::vec3 relativeVelocity = ballVelocity[index] - ballVelocity[i];
			GLfloat dot = glm::dot(relativeVelocity, normal);

			ballVelocity[index] = (ballVelocity[index] - normal * dot) * coefficient;
			ballVelocity[i] = (ballVelocity[i] + normal * dot) * coefficient;
		}
	}

	//caculate velocity of next delta time.
	velocity_Next[index] = ballVelocity[index] + acceleration * dt;
	ballVelocity[index] = velocity_Next[index];
	
	if (position[index].y < -4.5 && velocity_Next[index].y < 0) {
		velocity_Next[index].y = 0;
	}
	position_Next[index] = position[index] + velocity_Next[index] * dt;
	position[index] = position_Next[index];
	ballMatrix[index][12] = position_Next[index][0];
	ballMatrix[index][13] = position_Next[index][1];
	ballMatrix[index][14] = position_Next[index][2];
}

// Redndering teapot
void TeapotRender()
{
	for (int i = 0; i < totalballs; i++)
	{
		glPushMatrix();
		TeapotMovement(i);
		glMultMatrixf(ballMatrix[i]);
		glutSolidTeapot(0.5);
		glPopMatrix();
	}
}


void GenerateWalls() {
	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3f(-10.0f, -5.0f, 10.0f);
	glVertex3f(-10.0f, -5.0f, -10.0f);
	glVertex3f(10.0f, -5.0f, -10.0f);
	glVertex3f(10.0f, -5.0f, 10.0f);

	glEnd();

}

//================================
// render
//================================
void render(void) {
	// clear buffer
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render state
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// light source attributes
	GLfloat LightAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightDiffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat LightSpecular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightPosition[] = { 5.0f, 5.0f, 5.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	// surface material attributes
	GLfloat material_Ka[] = { 0.11f, 0.06f, 0.11f, 1.0f };
	GLfloat material_Kd[] = { 0.43f, 0.47f, 0.54f, 1.0f };
	GLfloat material_Ks[] = { 0.33f, 0.33f, 0.52f, 1.0f };
	GLfloat material_Ke[] = { 0.1f , 0.0f , 0.1f , 1.0f };
	GLfloat material_Se = 10;

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_Ks);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_Ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_Se);

	// modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 25, 25, 0, 5, 8, 0, 1, 0);

	// render objects
	GenerateWalls();
	TeapotRender();

	// disable lighting
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	// swap back and front buffers
	glutSwapBuffers();
}

//================================
// keyboard input
//================================
void keyboard(unsigned char key, int x, int y) {
}

//================================
// reshape : update viewport and projection matrix when the window is resized
//================================
void reshape(int w, int h) {
	// screen size
	g_screenWidth = w;
	g_screenHeight = h;

	// viewport
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 2000.0);
}


//================================
// timer : triggered every 16ms ( about 60 frames per second )
//================================
void timer(int value) {
	// increase frame index
	g_frameIndex++;

	// render
	glutPostRedisplay();

	// reset timer
	// 16 ms per frame ( about 60 frames per second )
	glutTimerFunc(16, timer, 0);
}

//================================
// main
//================================
int main(int argc, char** argv) {
	// create opengL window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	// init
	init();

	// set callback functions
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(16, timer, 0);

	// main loop
	glutMainLoop();

	return 0;
}
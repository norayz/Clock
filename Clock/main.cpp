#include <iostream>
#include <math.h>
#include <ctime>
#include "GL\glew.h"
#include "GL\freeglut.h"

const GLdouble PI = 3.14159265359;
const GLfloat RADIUS = 0.8f;

const GLfloat exitX = -0.8f;
const GLfloat exitY = 0.8f;

typedef struct tm Time;

struct Point
{
	GLdouble x;
	GLdouble y;
};

Time* getCurrentTime()
{
	time_t t = time(0);
	Time* currentTime = new Time;
	localtime_s(currentTime, &t);
	return currentTime;
}

Point getCircleCordsAtAngle(GLdouble angle)
{
	GLdouble x = RADIUS * cos(angle);
	GLdouble y = RADIUS * sin(angle);

	Point result = { x, y };
	return result;
}

void drawCircle()
{
	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINE_LOOP);
	for (GLdouble angle = 0; angle <= 2 * PI; angle += 1 / (2 * PI))
	{
		Point pointOnCircle = getCircleCordsAtAngle(angle);
		glVertex2d(pointOnCircle.x, pointOnCircle.y);
	}
	glEnd();
}

GLdouble getSecondsAngle(Time* time)
{
	// In 60 seconds we circle the clock once.
	// Hence, 60 seconds = 2PI => 1 sec = PI/30
	const GLdouble anglePerSec = PI / 30;

	GLint totalSeconds = time->tm_sec;
	GLdouble angle = totalSeconds * anglePerSec;

	return angle;
}

GLdouble getMinutesAngle(Time* time)
{
	// In 1 hour we circle the clock once.
	// Hence, 1 hour = 2PI => 3600 sec = 2PI => 1sec = PI/1800
	const GLdouble anglePerSec = PI / 1800;

	GLint totalSeconds = (60 * time->tm_min) + time->tm_sec;
	GLdouble angle = totalSeconds * anglePerSec;

	return angle;
}

GLdouble getHoursAngle(Time* time)
{
	// In 12 hour we circle the clock once.
	// Hence, 12 hour = 2PI => 43200 sec = 2PI => 1sec = PI/21600
	const GLdouble anglePerSec = PI / 21600;

	GLint totalSeconds = (3600 * time->tm_hour) + (60 * time->tm_min) + time->tm_sec;
	GLdouble angle = totalSeconds * anglePerSec;

	return angle;
}

void drawHoursHand(Time* time)
{
	GLdouble handAngle = getHoursAngle(time);

	Point cordsOnCircle = getCircleCordsAtAngle(handAngle);
	Point endpoint = { 0.5*cordsOnCircle.x, 0.5*cordsOnCircle.y };

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex2d(0.0, 0.0);
	glVertex2d(endpoint.x, endpoint.y);
	glEnd();
}

void drawMinutesHand(Time* time)
{
	GLdouble handAngle = getMinutesAngle(time);

	Point cordsOnCircle = getCircleCordsAtAngle(handAngle);
	Point endpoint = { 0.75*cordsOnCircle.x, 0.75*cordsOnCircle.y };

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex2d(0.0, 0.0);
	glVertex2d(endpoint.x, endpoint.y);
	glEnd();
}

void drawSecondsHand(Time* time)
{
	GLdouble handAngle = getSecondsAngle(time);

	Point cordsOnCircle = getCircleCordsAtAngle(handAngle);
	Point endpoint = { 0.9*cordsOnCircle.x, 0.9*cordsOnCircle.y };

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2d(0.0, 0.0);
	glVertex2d(endpoint.x, endpoint.y);
	glEnd();
}

void drawHands(Time* time)
{
	drawHoursHand(time);
	drawMinutesHand(time);
	drawSecondsHand(time);
}

void renderClock(Time* time)
{
	drawCircle();
	drawHands(time);
}

void clearScreen()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void drawText() {
	glColor3f(0.0f, 0.0f, 1.0f);
	glRasterPos2d(0.8f, 0.5f);
	GLubyte name[11] = { 'Y','a','r','o','n',' ','L','e','i','b','y' };
	for (GLint curChar = 0; curChar < 11; curChar++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, name[curChar]);
	}
}

void drawExit() {
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2d(exitX, exitY);
	GLubyte exit[4] = { 'E','X','I','T' };
	for (GLint curChar = 0; curChar < 4; curChar++) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, exit[curChar]);
	}
}

void display()
{
	clearScreen();

	Time* currentTime = getCurrentTime();
	renderClock(currentTime);

	drawExit();
	drawText();

	glutSwapBuffers();
}

void timer(int intervalMs)
{
	glutPostRedisplay();

	// call timer function again
	glutTimerFunc(intervalMs, timer, intervalMs);
}

void reshape(int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
}

void mouse(int button, int state, int x, int y)
{
	double width = glutGet(GLUT_WINDOW_WIDTH);
	double height = glutGet(GLUT_WINDOW_HEIGHT);

	if (button == GLUT_LEFT_BUTTON)
	{
		double transformedX = x / width;
		double transformedY = y / height;

		if (abs(transformedX - abs(exitX)) < 0.12 && abs(transformedY - abs(exitY)) < 0.12)
			glutLeaveMainLoop();
	}
}

void init()
{
	// rotate axis to match clock axis
	glLoadIdentity();
	glRotatef(180, 1.0, 0.0, 0.0); // flip y axis
	glRotatef(270, 0.0, 0.0, 1.0); // 90 degrees left on z axis

	gluOrtho2D(-1, 1, -1, 1);

	glShadeModel(GL_FLAT);
}

// run program as window instead of console
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Analog Clock");

	init();

	glutMouseFunc(mouse);
	glutDisplayFunc(display);
	timer(1000);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}
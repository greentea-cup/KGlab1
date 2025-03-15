// vim: set fdm=indent :
#include "Render.h"

#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

typedef double vec3[3];
void pyro(void);

void task1(void) {
	// Задание 1
	glBegin(GL_LINES);
	glVertex3d(1.0, 2.0, 3.0);
	glVertex3d(4.0, 2.0, 3.0);
	glEnd();
}

void task2(void) {
	// Задание 2
	glBegin(GL_LINES);
	glVertex3d(1.0, 2.0, 3.0);
	glVertex3d(4.0, 2.0, 3.0);
	glVertex3d(5.0, 5.0, 5.0);
	glVertex3d(6.0, 4.0, 10.0);
	glEnd();
}

void task3(void) {
	// Задание 3
	glBegin(GL_LINE_LOOP);
	glVertex3d(3.0, 1.0, 2.0);
	glVertex3d(3.0, 5.0, 2.0);
	glVertex3d(3.0, 2.0, 6.0);
	glEnd();
}

void task4(void) {
	// Задание 4
	double vertices[3 * 3] = {3.0, 1.0, 2.0, 3.0, 5.0, 2.0, 3.0, 2.0, 6.0};
	glBegin(GL_TRIANGLES);
	glVertex3dv(vertices + 0 * 3);
	glVertex3dv(vertices + 1 * 3);
	glVertex3dv(vertices + 2 * 3);
	glEnd();
}

void task5a(void) {
	// Задание 5 а
	glColor3d(255 / 255.0, 192 / 255.0, 203 / 255.0);
	task4();
}

void task5b(void) {
	// Задание 5 б
	double vertices[3 * 3] = { 3.0, 1.0, 2.0, 3.0, 5.0, 2.0, 3.0, 2.0, 6.0 };
	double colors[3 * 3] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
	glBegin(GL_TRIANGLES);
	glColor3dv(colors + 0 * 3);
	glVertex3dv(vertices + 0 * 3);
	glColor3dv(colors + 1 * 3);
	glVertex3dv(vertices + 1 * 3);
	glColor3dv(colors + 2 * 3);
	glVertex3dv(vertices + 2 * 3);
	glEnd();
}

void task6a(void) {
	// Задание 6 а
	double vertices[4 * 3] = {
		0., 0., 0., 1., 0., 0.,
		1., 1., 0., 0., 1., 0.,
	};
	double colors[4 * 3] = {
		1., 1., 1., 1., 0., 0.,
		1., 0., 0., 1., 1., 1.,
	};
	glBegin(GL_QUADS);
	glColor3dv(colors + 0 * 3);
	glVertex3dv(vertices + 0 * 3);
	glColor3dv(colors + 1 * 3);
	glVertex3dv(vertices + 1 * 3);
	glColor3dv(colors + 2 * 3);
	glVertex3dv(vertices + 2 * 3);
	glColor3dv(colors + 3 * 3);
	glVertex3dv(vertices + 3 * 3);
	glEnd();
}

void task6b(void) {
	// Задание 6 б
	double vertices[4 * 3] = {
		0., 0., 0., 1., 0., 0.,
		1., 1., 0., 0., 1., 0.,
	};
	double colors[4 * 3] = {
		1., 1., 1., 1., 0., 0.,
		1., 0., 0., 1., 1., 1.,
	};
	glBegin(GL_QUADS);
	glColor3dv(colors + 0 * 3);
	glVertex3dv(vertices + 0 * 3);
	glColor3dv(colors + 1 * 3);
	glVertex3dv(vertices + 1 * 3);
	glColor3dv(colors + 3 * 3);
	glVertex3dv(vertices + 3 * 3);
	glColor3dv(colors + 2 * 3);
	glVertex3dv(vertices + 2 * 3);
	glEnd();
}

void task7(void) { pyro(); }

void task8(void) { pyro(); }

void task9a(void) {
	glRotated(45., 0, 0, 1);
	pyro();
}

void task9b(void) {
	glRotated(30., 0, 1, 0);
	pyro();
}

void task9c(void) {
	glScaled(1, 1, .5);
	pyro();
}

void task9d(void) {
	glTranslated(1, 2, 3);
	pyro();
}

void task9e(void) {
	glPushMatrix();
	glTranslated(3, 0, 0);
	glRotated(75, 0, 0, 1);
	pyro();
	glPopMatrix();
	glPushMatrix();
	glRotated(75, 0, 0, 1);
	glTranslated(3, 0, 0);
	pyro();
	glPopMatrix();
}

void task10_1(void) {
	for (int i = 0; i < 360; i += 25) {
		glRotated(i, 0, 0, 1);
		glTranslated(3, 0, 0);
		pyro();
	}
}

void task10_2(void) {
	for (int i = 0; i < 360; i += 25) {
		glPushMatrix();
		glRotated(i, 0, 0, 1);
		glTranslated(3, 0, 0);
		pyro();
		glPopMatrix();
	}
}

/* vim: set fdm=indent : */

#define HEIGHT 3.0
#define NEXTRA1 16
#define NEXTRA2 16
#define SUBDIVISIONS 128

#include "Render.h"

#include <iostream>
#include <cmath>
#define M_PI 3.14159265358979323846
#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

struct vec3 {
	double x, y, z;
	operator double *(void) { return &x; }
	operator double const *(void) const { return &x; }
	vec3 operator+(vec3 const &o) { return vec3{x+o.x, y+o.y, z+o.z}; }
	vec3 operator-(vec3 const &o) { return vec3{x-o.x, y-o.y, z-o.z}; }
};
enum winding_order {
	WND_CCW = 0, /* Против часовой */
	WND_CW = 1, /* По часовой */
};
/* построение против часовой
	A1<--B1
	|   /^
	v /  |
	A--->B
	GL_QUADS         : A B B1 A1
	GL_TRIANGLES     : A B B1 + A B1 A1
	GL_TRIANGLE_FAN  : A B B1 + (A B1) A1
	GL_TRIANGLE_STRIP: B B1 A + (B1 A) A1
*/

vec3 midpoint(vec3 a, vec3 b) {
	vec3 out;
	out.x = a.x + (b.x - a.x) / 2;
	out.y = a.y + (b.y - a.y) / 2;
	out.z = a.z + (b.z - a.z) / 2;
	return out;
}
vec3 circle_center_xy(vec3 p1, vec3 p2, vec3 p3) {
	vec3 out;
	// https://www.desmos.com/calculator/6fgvz0xpmb
	double a = p1.x, b = p1.y,
		c = p2.x, d = p2.y,
		f = p3.x, g = p3.y;
	double m1 = (b-d)/(a-c),
		m2 = (b-g)/(a-f),
		m3 = (d-g)/(c-f),
		m4 = (c-a)/(b-d),
		m5 = (f-a)/(b-g),
		m6 = (f-c)/(d-g);
	double c1 = -a*m1+b,
		c2 = -a*m2+b,
		c3 = -c*m3+d,
		c4 = -m4*(a+c)*0.5+(b+d)*0.5,
		c5 = -m5*(a+f)*0.5+(b+g)*0.5,
		c6 = -m6*(c+f)*0.5+(d+g)*0.5;
	double cx = (c6-c5)/(m5-m6),
		cy = (m6*c5-m5*c6)/(m6-m5);
	out.x = cx;
	out.y = cy;
	out.z = p1.z;
	return out;
}
static void fill_arc_points_xy(vec3 *data, size_t npoints, winding_order cw, vec3 a, vec3 b, double distance_xy, vec3 center) {
	double ax = a.x-center.x, ay = a.y-center.y, bx = b.x-center.x, by = b.y-center.y;
	double radius2 = ax*ax + ay*ay;
	double radius = sqrt(radius2);
	double phase = atan2(ay, ax);
	double full_angle = acos((ax*bx + ay*by) / radius2);
	double step = full_angle / (npoints-1);
	for (size_t i = 0; i < npoints; i++) {
		double angle = (cw == WND_CCW ? npoints-1-i : i) * step + phase;
		data[i].x = center.x + radius * cos(angle);
		data[i].y = center.y + radius * sin(angle);
		data[i].z = a.z;
	}
}

static vec3 /* основные цвета */
	c1 = { 1./14., 1./14., 1./14. },
	c2 = { 2./14., 2./14., 2./14. },
	c3 = { 3./14., 3./14., 3./14. },
	c4 = { 4./14., 4./14., 4./14. },
	c5 = { 5./14., 5./14., 5./14. },
	c6 = { 6./14., 6./14., 6./14. },
	c7 = { 7./14., 7./14., 7./14. },
	c8 = { 8./14., 8./14., 8./14. },
	c9 = { 9./14., 9./14., 9./14. },
	c10 = { 10./14., 10./14., 10./14. },
	c11 = { 11./14., 11./14., 11./14. },
	c12 = { 12./14., 12./14., 12./14. },
	c13 = { 13./14., 13./14., 13./14. },
	c14 = { 14./14., 14./14., 14./14. };
static vec3 /* базовые точки */
	A = { 2., -1., 0. }, B = { -1., -6., 0. },
	C = { -7., -3., 0. }, D = { -5., 4., 0. },
	E = { 1., 2., 0. }, F = { 4., 6., 0. },
	G = { 8., 5., 0. }, H = { 7., -2., 0. },
	A1 = { 2., -1., HEIGHT }, B1 = { -1., -6., HEIGHT },
	C1 = { -7., -3., HEIGHT }, D1 = { -5., 4., HEIGHT },
	E1 = { 1., 2., HEIGHT }, F1 = { 4., 6., HEIGHT },
	G1 = { 8., 5., HEIGHT }, H1 = { 7., -2., HEIGHT };
static vec3 /* дополнительные точки */
	/* M - точка, через которую проходит вырез */
	M = {-5., -2., 0.}, M1 = {-5., -2., HEIGHT},
	/* X - центр выпуклости */
	X = midpoint(H, G), X1 = midpoint(H1, G1),
	/* Y - центр вогнутости */
	Y = circle_center_xy(C, D, M), Y1 = circle_center_xy(C1, D1, M1),
	/* K = mid(B, C) */
	K = midpoint(B, C), K1 = midpoint(B1, C1),
	/* L = mid(D, E) */
	L = midpoint(D, E), L1 = midpoint(D1, E1);

static vec3 const prism30data[] = {
	/* Цвета и вершины призмы (30) */
	/* низ */
	c1, A, B, C, c1, A, C, D,
	c2, A, D, E, c2, A, E, H,
	c3, E, F, G, c3, E, G, H,
	/* стороны */
	c4, A, H, H1, c4, A, H1, A1, 
	c5, H, G, G1, c5, H, G1, H1, 
	c6, G, F, F1, c6, G, F1, G1, 
	c7, F, E, E1, c7, F, E1, F1, 
	c8, E, D, D1, c8, E, D1, E1, 
	c9, D, C, C1, c9, D, C1, D1, 
	c10, C, B, B1, c10, C, B1, C1, 
	c11, B, A, A1, c11, B, A1, B1, 
	/* верх */
	c12, H1, G1, F1, c12, H1, F1, E1, 
	c13, H1, E1, D1, c13, H1, D1, A1, 
	c14, D1, C1, B1, c14, D1, B1, A1, 
};
static vec3 const prism40data[] = {
	/* Цвета и вершины призмы (40) */
	/* низ */
	c1, A, B, C, c1, A, C, D,
	c2, A, D, E, c2, A, E, H,
	c3, E, F, G, c3, E, G, H,
	/* стороны */
	c4, A, H, H1, c4, A, H1, A1, 
	/* c5, H, G, G1, c5, H, G1, H1, */
	c6, G, F, F1, c6, G, F1, G1, 
	c7, F, E, E1, c7, F, E1, F1, 
	c8, E, D, D1, c8, E, D1, E1, 
	c9, D, C, C1, c9, D, C1, D1, 
	c10, C, B, B1, c10, C, B1, C1, 
	c11, B, A, A1, c11, B, A1, B1, 
	/* верх */
	c12, H1, G1, F1, c12, H1, F1, E1, 
	c13, H1, E1, D1, c13, H1, D1, A1, 
	c14, D1, C1, B1, c14, D1, B1, A1, 
};
static vec3 const prism50data[] = {
	/* Цвета и вершины призмы (50) */
	/* низ */
	/* c1, A, B, C, c1, A, C, D, */
	c1, A, B, K, c1, A, K, L,
	/* c2, A, D, E, */
	c2, A, L, E, c2, A, E, H,
	c3, E, F, G, c3, E, G, H,
	/* стороны */
	c4, A, H, H1, c4, A, H1, A1, 
	/* c5, H, G, G1, c5, H, G1, H1, */
	c6, G, F, F1, c6, G, F1, G1, 
	c7, F, E, E1, c7, F, E1, F1, 
	c8, E, D, D1, c8, E, D1, E1, 
	/* c9, D, C, C1, c9, D, C1, D1, */
	c10, C, B, B1, c10, C, B1, C1, 
	c11, B, A, A1, c11, B, A1, B1, 
	/* верх */
	c12, H1, G1, F1, c12, H1, F1, E1, 
	/* c13, H1, E1, D1, c13, H1, D1, A1, */
	c13, A1, E1, L1, c13, A1, H1, E1,
	/* c14, D1, C1, B1, c14, D1, B1, A1, */
	c14, A1, K1, B1, c14, A1, L1, K1, c14, A1, E1, L1,
};
static vec3 extra1[2*NEXTRA1], extra2[2*NEXTRA2];

static void init(void) {
	fill_arc_points_xy(extra1, NEXTRA1, WND_CCW, H, G, 0., X);
	fill_arc_points_xy(extra1+NEXTRA1, NEXTRA1, WND_CW, H1, G1, 0., X1);
	fill_arc_points_xy(extra2, NEXTRA2, WND_CCW, C, D, 0., Y);
	fill_arc_points_xy(extra2+NEXTRA2, NEXTRA2, WND_CW, C1, D1, 0., Y1);
}

void draw_ticks(void) {
	glBegin(GL_LINES);
	// red
	for (int i = 1; i <= 10; i++) {
		glVertex3d(i, 0., -.25);
		glVertex3d(i, 0., +.25);
	}
	// green
	for (int i = 1; i <= 10; i++) {
		glVertex3d(0., i, -.25);
		glVertex3d(0., i, +.25);
	}
	// blue
	for (int i = 1; i <= 10; i++) {
		glVertex3d(-.125, +.125, i);
		glVertex3d(+.125, -.125, i);
	}
	glEnd();
}

void pyro(void) {
	// Задание 7
	vec3 A = { +1., +1., 0. },
		 B = { +1., -1., 0. },
		 C = { -1., -1., 0. },
		 D = { -1., +1., 0. },
		 A1 = { +1., +1., 1. },
		 B1 = { +1., -1., 1. },
		 C1 = { -1., -1., 1. },
		 D1 = { -1., +1., 1. },
		 E = { +0., +0., 3. };

	vec3 col1 = { 10 / 255., 10 / 255., 10 / 255. },
		 col2 = { 30 / 255., 30 / 255., 30 / 255. },
		 col3 = { 60 / 255., 60 / 255., 60 / 255. },
		 col4 = { 90 / 255., 90 / 255., 90 / 255. },
		 col5 = { 120 / 255., 120 / 255., 120 / 255. },
		 col6 = { 150 / 255., 150 / 255., 150 / 255. },
		 col7 = { 180 / 255., 180 / 255., 180 / 255. },
		 col8 = { 210 / 255., 210 / 255., 210 / 255. },
		 col9 = { 240 / 255., 240 / 255., 240 / 255. };

	glBegin(GL_QUADS);
	glColor3dv(col1); glVertex3dv(A); glVertex3dv(B); glVertex3dv(C); glVertex3dv(D); // низ
	glColor3dv(col2); glVertex3dv(A); glVertex3dv(A1); glVertex3dv(B1); glVertex3dv(B); // c 1
	glColor3dv(col3); glVertex3dv(B); glVertex3dv(B1); glVertex3dv(C1); glVertex3dv(C); // с 2
	glColor3dv(col4); glVertex3dv(C); glVertex3dv(C1); glVertex3dv(D1); glVertex3dv(D); // с 3
	glColor3dv(col5); glVertex3dv(D); glVertex3dv(D1); glVertex3dv(A1); glVertex3dv(A); // с 4
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3dv(col6); glVertex3dv(A1); glVertex3dv(E); glVertex3dv(B1); // в 1
	glColor3dv(col7); glVertex3dv(B1); glVertex3dv(E); glVertex3dv(C1); // в 2
	glColor3dv(col8); glVertex3dv(C1); glVertex3dv(E); glVertex3dv(D1); // в 3
	glColor3dv(col9); glVertex3dv(D1); glVertex3dv(E); glVertex3dv(A1); // в 4
	glEnd();
}

void prism30(void) {
	glBegin(GL_TRIANGLES);
		for (size_t i = 0; i < sizeof(prism30data) / sizeof(*prism30data); i += 4) {
			glColor3dv(prism30data[i+0]);
			glVertex3dv(prism30data[i+1]);
			glVertex3dv(prism30data[i+2]);
			glVertex3dv(prism30data[i+3]);
		}
	glEnd();
}

void prism40(void) {
	glBegin(GL_TRIANGLES);
		for (size_t i = 0; i < sizeof(prism40data) / sizeof(*prism40data); i += 4) {
			glColor3dv(prism40data[i+0]);
			glVertex3dv(prism40data[i+1]);
			glVertex3dv(prism40data[i+2]);
			glVertex3dv(prism40data[i+3]);
		}
		// сторона HG
		for (size_t i = 1; i < NEXTRA1; i++) {
			double c = 0.5 + 0.5 * i / NEXTRA1;
			glColor3d(c, c, c);
			// низ
			glVertex3dv(X);
			glVertex3dv(extra1[i-1]);
			glVertex3dv(extra1[i]);
			// верх
			glVertex3dv(X1);
			glVertex3dv(extra1[2*NEXTRA1-i-1]);
			glVertex3dv(extra1[2*NEXTRA1-i]);
			// стороны
			glVertex3dv(extra1[i]);
			glVertex3dv(extra1[i-1]);
			glVertex3dv(extra1[2*NEXTRA1-i]);
			glVertex3dv(extra1[i]);
			glVertex3dv(extra1[2*NEXTRA1-i]);
			glVertex3dv(extra1[2*NEXTRA1-i-1]);
		}
	glEnd();
}

void prism50(void) {
	glBegin(GL_TRIANGLES);
		// Статичная часть призмы
		for (size_t i = 0; i < sizeof(prism50data) / sizeof(*prism50data); i += 4) {
			glColor3dv(prism50data[i+0]);
			glVertex3dv(prism50data[i+1]);
			glVertex3dv(prism50data[i+2]);
			glVertex3dv(prism50data[i+3]);
		}
		// Выпуклость
		for (size_t i = 1; i < NEXTRA1; i++) {
			double c = 0.5 + 0.5 * i / NEXTRA1;
			glColor3d(c, c, c);
			// низ
			glVertex3dv(X);
			glVertex3dv(extra1[i-1]);
			glVertex3dv(extra1[i]);
			// верх
			glVertex3dv(X1);
			glVertex3dv(extra1[2*NEXTRA1-i-1]);
			glVertex3dv(extra1[2*NEXTRA1-i]);
			// стороны
			glVertex3dv(extra1[i]);
			glVertex3dv(extra1[i-1]);
			glVertex3dv(extra1[2*NEXTRA1-i]);
			glVertex3dv(extra1[i]);
			glVertex3dv(extra1[2*NEXTRA1-i]);
			glVertex3dv(extra1[2*NEXTRA1-i-1]);
		}
		// Низ и верх вогнутости (1 - L)
		for (size_t i = 1; i < (NEXTRA2+1)/2; i++) {
			double c = (double)i/NEXTRA2;
			glColor3d(c, c, c);
			glVertex3dv(L);
			glVertex3dv(extra2[i]);
			glVertex3dv(extra2[i-1]);
			glVertex3dv(L1);
			glVertex3dv(extra2[2*NEXTRA2-i]);
			glVertex3dv(extra2[2*NEXTRA2-i-1]);
		}
		// Низ и верх вогнутости (2 - середина)
		{
			double c = 0.5;
			glColor3d(c, c, c);
			glVertex3dv(L);
			glVertex3dv(K);
			glVertex3dv(extra2[NEXTRA2/2]);
			glVertex3dv(K1);
			glVertex3dv(L1);
			glVertex3dv(extra2[NEXTRA2+NEXTRA2/2]);
			if (NEXTRA2 % 2 == 0) {
				glVertex3dv(L);
				glVertex3dv(extra2[NEXTRA2/2]);
				glVertex3dv(extra2[NEXTRA2/2-1]);
				glVertex3dv(K1);
				glVertex3dv(extra2[NEXTRA2+NEXTRA2/2]);
				glVertex3dv(extra2[NEXTRA2+NEXTRA2/2-1]);
			}
		}
		// Низ и верх вогнутости (3 - K)
		for (size_t i = NEXTRA2/2+1; i < NEXTRA2; i++) {
			double c = (double)i/NEXTRA2;
			glColor3d(c, c, c);
			glVertex3dv(K);
			glVertex3dv(extra2[i]);
			glVertex3dv(extra2[i-1]);
			glVertex3dv(K1);
			glVertex3dv(extra2[2*NEXTRA2-i]);
			glVertex3dv(extra2[2*NEXTRA2-i-1]);
		}
		// Внешняя обшивка вогнутости (4)
		for (size_t i = 1; i < NEXTRA2; i++) {
			double c = (double)i/NEXTRA2;
			glColor3d(c, c, c);
			glVertex3dv(extra2[i-1]);
			glVertex3dv(extra2[i]);
			glVertex3dv(extra2[2*NEXTRA2-i]);
			glVertex3dv(extra2[i]);
			glVertex3dv(extra2[2*NEXTRA2-i-1]);
			glVertex3dv(extra2[2*NEXTRA2-i]);
		}
	glEnd();
}

void prism60(GLdouble height, GLdouble angle_deg) {
	GLdouble const k = 1. / (HEIGHT * SUBDIVISIONS);
	GLdouble const divrate = 1. / SUBDIVISIONS;
	GLdouble da = angle_deg * divrate;
	GLdouble dh = height * divrate;
	glPushMatrix();
	glScaled(1., 1., height * k);
	for (size_t i = 0; i < SUBDIVISIONS; i++) {
		prism50();
		glRotated(da, 0., 0., 1.);
		glTranslated(0., 0., HEIGHT);
	}
	glPopMatrix();
}

void Render(double delta_time) {
	{ static int _ = (init(), 0); }
	glClearColor(50.f/255.f, 50.f/255.f, 50.f/255.f, 0.f);
	// glClearColor(150.f/255.f, 150.f/255.f, 150.f/255.f, 0.f);
	// glClearColor(15.f/255.f, 15.f/255.f, 30.f/255.f, 0.f);
	glPointSize(12.);
	glLineWidth(3.);
	// glColor3d(1., 1., 1.);
	glColor3d(0.75, 0.25, 0.25);
	draw_ticks();
	glEnable(GL_CULL_FACE);
	// glDepthFunc(GL_ALWAYS);
	glPushMatrix();
	glScaled(0.25, 0.25, 0.25);
	glTranslated(-10., -10., 0.);
	prism30();
	glTranslated(0., 20., 0.);
	prism40();
	glTranslated(20., -20., 0.);
	prism50();
	glTranslated(0., 20., 0.);
	prism60(6.0, 60.);
	glPopMatrix();
}

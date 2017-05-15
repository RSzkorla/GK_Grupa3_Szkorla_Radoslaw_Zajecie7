// opengl_zbufor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/*
(c) Janusz Ganczarski
http://www.januszg.hg.pl
JanuszG@enter.net.pl
*/

#include <GL/glut.h>
#include <stdlib.h>
#include "colors.h"


typedef GLfloat GLTVector2[2];
typedef GLfloat GLTVector3[3];
typedef GLfloat GLTVector4[4];
typedef GLfloat GLTMatrix[16];
// A column major 4x4 matrix of type GLfloat
#include <math.h>

// Adds two vectors together
void gltAddVectors(const GLTVector3 vFirst, const GLTVector3 vSecond, GLTVector3 vResult) {
	vResult[0] = vFirst[0] + vSecond[0];
	vResult[1] = vFirst[1] + vSecond[1];
	vResult[2] = vFirst[2] + vSecond[2];
}

// Subtract one vector from another
void gltSubtractVectors(const GLTVector3 vFirst, const GLTVector3 vSecond, GLTVector3 vResult)
{
	vResult[0] = vFirst[0] - vSecond[0];
	vResult[1] = vFirst[1] - vSecond[1];
	vResult[2] = vFirst[2] - vSecond[2];
}

// Scales a vector by a scalar
void gltScaleVector(GLTVector3 vVector, const GLfloat fScale)
{
	vVector[0] *= fScale; vVector[1] *= fScale; vVector[2] *= fScale;
}

// Gets the length of a vector squared
GLfloat gltGetVectorLengthSqrd(const GLTVector3 vVector)
{
	return (vVector[0] * vVector[0]) + (vVector[1] * vVector[1]) + (vVector[2] * vVector[2]);
}

// Gets the length of a vector
GLfloat gltGetVectorLength(const GLTVector3 vVector)
{
	return (GLfloat)sqrt(gltGetVectorLengthSqrd(vVector));
}

// Scales a vector by it's length - creates a unit vector
void gltNormalizeVector(GLTVector3 vNormal)
{
	GLfloat fLength = 1.0f / gltGetVectorLength(vNormal);
	gltScaleVector(vNormal, fLength);
}

// Copies a vector
void gltCopyVector(const GLTVector3 vSource, GLTVector3 vDest)
{
	memcpy(vDest, vSource, sizeof(GLTVector3));
}

// Get the dot product between two vectors
GLfloat gltVectorDotProduct(const GLTVector3 vU, const GLTVector3 vV)
{
	return vU[0] * vV[0] + vU[1] * vV[1] + vU[2] * vV[2];
}

// Calculate the cross product of two vectors
void gltVectorCrossProduct(const GLTVector3 vU, const GLTVector3 vV, GLTVector3 vResult)
{
	vResult[0] = vU[1] * vV[2] - vV[1] * vU[2];
	vResult[1] = -vU[0] * vV[2] + vV[0] * vU[2];
	vResult[2] = vU[0] * vV[1] - vV[0] * vU[1];
}



// Given three points on a plane in counter clockwise order, calculate the unit normal
void gltGetNormalVector(const GLTVector3 vP1, const GLTVector3 vP2, const GLTVector3 vP3, GLTVector3 vNormal)
{
	GLTVector3 vV1, vV2;

	gltSubtractVectors(vP2, vP1, vV1);
	gltSubtractVectors(vP3, vP1, vV2);

	gltVectorCrossProduct(vV1, vV2, vNormal);
	gltNormalizeVector(vNormal);
}



// Transform a point by a 4x4 matrix
void gltTransformPoint(const GLTVector3 vSrcVector, const GLTMatrix mMatrix, GLTVector3 vOut)
{
	vOut[0] = mMatrix[0] * vSrcVector[0] + mMatrix[4] * vSrcVector[1] + mMatrix[8] * vSrcVector[2] + mMatrix[12];
	vOut[1] = mMatrix[1] * vSrcVector[0] + mMatrix[5] * vSrcVector[1] + mMatrix[9] * vSrcVector[2] + mMatrix[13];
	vOut[2] = mMatrix[2] * vSrcVector[0] + mMatrix[6] * vSrcVector[1] + mMatrix[10] * vSrcVector[2] + mMatrix[14];
}

// Rotates a vector using a 4x4 matrix. Translation column is ignored
void gltRotateVector(const GLTVector3 vSrcVector, const GLTMatrix mMatrix, GLTVector3 vOut)
{
	vOut[0] = mMatrix[0] * vSrcVector[0] + mMatrix[4] * vSrcVector[1] + mMatrix[8] * vSrcVector[2];
	vOut[1] = mMatrix[1] * vSrcVector[0] + mMatrix[5] * vSrcVector[1] + mMatrix[9] * vSrcVector[2];
	vOut[2] = mMatrix[2] * vSrcVector[0] + mMatrix[6] * vSrcVector[1] + mMatrix[10] * vSrcVector[2];
}


// Gets the three coefficients of a plane equation given three points on the plane.
void gltGetPlaneEquation(GLTVector3 vPoint1, GLTVector3 vPoint2, GLTVector3 vPoint3, GLTVector3 vPlane)
{
	// Get normal vector from three points. The normal vector is the first three coefficients
	// to the plane equation...
	gltGetNormalVector(vPoint1, vPoint2, vPoint3, vPlane);

	// Final coefficient found by back substitution
	vPlane[3] = -(vPlane[0] * vPoint3[0] + vPlane[1] * vPoint3[1] + vPlane[2] * vPoint3[2]);
}

// Determine the distance of a point from a plane, given the point and the
// equation of the plane.
GLfloat gltDistanceToPlane(GLTVector3 vPoint, GLTVector4 vPlane)
{
	return vPoint[0] * vPlane[0] + vPoint[1] * vPlane[1] + vPoint[2] * vPlane[2] + vPlane[3];
}









// rozmiary bry�y obcinania

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near_ = 3.0;
const GLdouble far_ = 7.0;

// sta�e do obs�ugi menu podr�cznego

enum
{
	CUTTING_PLANE, // p�aszczyzna przekroju
	POLYGON_OFFSET, // przesuni�cie warto�ci g��bi
	EXIT // wyj�cie
};

// k�t obrotu kuli

GLfloat angle = 0.0;

// k�ty obrotu sze�cianu

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wska�nik rysowania p�aszczyzna przekroju

bool cutting_plane = true;

// wska�nik przesuni�cia warto�ci g��bi

bool polygon_offset = true;

// wska�nik naci�ni�cia lewego przycisku myszki

int button_state = GLUT_UP;

// po�o�enie kursora myszki

int button_x, button_y;

// funkcja generuj�ca scen� 3D

#define GL_PI 3.1415f

void Display()
{
	GLint n = 5;
	GLTVector3 vNormal;
	GLTVector3 vCorners[12] =
	{
		{ 0.0f, 1.0f, 0.0f }, // G�ra 0
	};

	for (GLint i = 1; i <= n; i++)
	{

		vCorners[i][0] = vCorners[i - 1][0] + ((0.5*cos(i * 2 * GL_PI / n)) * 1.0f);
		vCorners[i][2] = vCorners[i - 1][2] + ((0.5*sin(i * 2 * GL_PI / n)) * 1.0f);

	}


	//wyznaczanie �rodka
	GLfloat sx = 0, sz = 0;

	for (GLint i = 1; i <= n; i++)
	{
		sx = sx + vCorners[i][0];
		sz = sz + vCorners[i][2];
	}

	vCorners[0][0] = sx / n;
	vCorners[0][1] = 1.f;
	vCorners[0][2] = sz / n;

	vCorners[11][0] = sx / n;
	vCorners[11][1] = 0.f;
	vCorners[11][2] = sz / n;

	//xRot = vCorners[11][0];

	// Czyszczenie okna aktualnym kolorem czyszcz�cym
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Zapisanie stanu macierzy i wykonanie obrot�w
	glPushMatrix();
	// Cofni�cie obiekt�w
	glTranslatef(0.0f, -0.25f, -4.0f);
	glRotatef(rotatex, 1.0f, 0.0f, 0.0f);
	glRotatef(rotatey, 0.0f, 1.0f, 0.0f);
	// Rysowanie piramidy
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, -1.0f, 0.0f);

	/*for (GLint i = 1; i < n; i++)
	{
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[i]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[i+1]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[11]);
	}

	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[n]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[1]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[11]);
	*/

	for (GLint i = 1; i < n; i++)
	{
		gltGetNormalVector(vCorners[i], vCorners[i + 1], vCorners[0], vNormal);
		glNormal3fv(vNormal);
		//glTexCoord2f(0.5f, 1.0f);
		glVertex3fv(vCorners[0]);
		//glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[i + 1]);
		//glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(vCorners[i]);
	}

	gltGetNormalVector(vCorners[n], vCorners[1], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	//glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	//glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[1]);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[n]);

	for (GLint i = 1; i < n; i++)
	{
		gltGetNormalVector(vCorners[i], vCorners[i + 1], vCorners[0], vNormal);
		glNormal3fv(vNormal);
		//glTexCoord2f(0.5f, 1.0f);
		glVertex3fv(vCorners[0]);
		//glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[i]);
		//glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(vCorners[i + 1]);
	}

	gltGetNormalVector(vCorners[n], vCorners[1], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	//glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	//glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[n]);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[1]);


	glEnd();;
	// Odtworzenie stanu macierzy
	glPopMatrix();
	// Zamiana bufor�w
	//glutSwapBuffers();




	// kolor t�a - zawarto�� bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora g��boko�ci
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wyb�r macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesuni�cie uk�adu wsp�rz�dnych sze�cianu do �rodka bry�y odcinania
	glTranslatef(0, 0, -(near_ + far_) / 2);

	// obroty sze�cianu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// niewielkie powi�kszenie sze�cianu
	glScalef(1.15, 1.15, 1.15);

	// w��czenie testu bufora g��boko�ci
	glEnable(GL_DEPTH_TEST);

	// najpierw rysujemy kul� obracaj�c� si� wewn�trz sze�cianu;
	// z uwagi na celowy brak efekt�w o�wietlenia, obr�t kuli
	// podkre�la druga kula w wersji "szkieletowej"
	glPushMatrix();
	angle += 0.2;
	glRotatef(angle, 1.0, 1.0, 0.0);
	glColor3fv(Yellow);
	if (polygon_offset)
		glEnable(GL_POLYGON_OFFSET_FILL);

	glPolygonOffset(1.0, 1.0);
	glutSolidSphere(0.5, 10, 10);
	glColor3fv(Black);
	glutWireSphere(0.5, 10, 10);
	if (polygon_offset)
		glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();

	glPushMatrix();
	// Cofni�cie obiekt�w
	glTranslatef(0.0f, -0.25f, -4.0f);
	glRotatef(rotatex, 1.0f, 0.0f, 0.0f);
	glRotatef(rotatey, 0.0f, 1.0f, 0.0f);
	// Rysowanie piramidy
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);

	glNormal3f(0.0f, -1.0f, 0.0f);

	/*for (GLint i = 1; i < n; i++)
	{
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[i]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[i+1]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[11]);
	}

	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(vCorners[n]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(vCorners[1]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[11]);
	*/

	for (GLint i = 1; i < n; i++)
	{
		gltGetNormalVector(vCorners[i], vCorners[i + 1], vCorners[0], vNormal);
		glNormal3fv(vNormal);
		//glTexCoord2f(0.5f, 1.0f);
		glVertex3fv(vCorners[0]);
		//glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[i + 1]);
		//glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(vCorners[i]);
	}

	gltGetNormalVector(vCorners[n], vCorners[1], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	//glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	//glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[1]);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[n]);

	for (GLint i = 1; i < n; i++)
	{
		gltGetNormalVector(vCorners[i], vCorners[i + 1], vCorners[0], vNormal);
		glNormal3fv(vNormal);
		//glTexCoord2f(0.5f, 1.0f);
		glVertex3fv(vCorners[0]);
		//glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[i]);
		//glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(vCorners[i + 1]);
	}

	gltGetNormalVector(vCorners[n], vCorners[1], vCorners[0], vNormal);
	glNormal3fv(vNormal);
	//glTexCoord2f(0.5f, 1.0f);
	glVertex3fv(vCorners[0]);
	//glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(vCorners[n]);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(vCorners[1]);


	glEnd();;
	// Odtworzeni


	
	// w drugiej kolejno�ci rysujemy wn�trze sze�cianu;
	// rysowane s� tylko przednie strony wewn�trznych �cian
	// no i nie jest rysowana �ciana przednia, w kt�rej b�dzie otw�r
	glEnable(GL_CULL_FACE);
	glBegin(GL_QUADS);
	glColor3fv(Blue);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(1.0, 1.0, -1.0);
	glVertex3f(-1.0, 1.0, -1.0);

	glColor3fv(Lime);
	glVertex3f(-1.0, 1.0, 1.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(-1.0, 1.0, -1.0);

	glColor3fv(Cyan);
	glVertex3f(1.0, 1.0, 1.0);
	glVertex3f(-1.0, 1.0, 1.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glVertex3f(1.0, 1.0, -1.0);

	glColor3fv(Green);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(1.0, 1.0, 1.0);
	glVertex3f(1.0, 1.0, -1.0);
	glVertex3f(1.0, -1.0, -1.0);

	glColor3fv(Cyan);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glEnd();
	glDisable(GL_CULL_FACE);

	// rysowanie p�aszczyzny otworu w sze�cianie
	if (cutting_plane)
	{
		// wy��czenie rysowania w buforze kolor�w
		glDrawBuffer(GL_NONE);

		// rysowanie kwadratu cz�ciowo ods�aniaj�cego wn�trze sze�cianu
		// (kwadrat jest po�o�ony o 0,001 jednostki nad bokiem sze�cianu)
		glBegin(GL_QUADS);
		glVertex3f(-0.6, -0.6, 1.001);
		glVertex3f(0.6, -0.6, 1.001);
		glVertex3f(0.6, 0.6, 1.001);
		glVertex3f(-0.6, 0.6, 1.001);
		glEnd();

		// w��czenie rysowania w buforze kolor�w
		glDrawBuffer(GL_BACK);
	}

	// w�a�ciwy sze�cian z obramowaniem, kt�rego rysowanie wymusza brak o�wietlenia
	glColor3fv(Red);
	if (polygon_offset)
		glEnable(GL_POLYGON_OFFSET_FILL);

	glPolygonOffset(1.0, 1.0);
	glutSolidCube(2.0);
	glColor3fv(Black);
	glutWireCube(2.0);
	if (polygon_offset)
		glDisable(GL_POLYGON_OFFSET_FILL);

	// skierowanie polece� do wykonania
	glFlush();

	// zamiana bufor�w koloru
	glutSwapBuffers();
}

// zmiana wielko�ci okna

void Reshape(int width, int height)
{
	// obszar renderingu - ca�e okno
	glViewport(0, 0, width, height);

	// wyb�r macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bry�y obcinania
	glFrustum(left, right, bottom, top, near_, far_);

	// generowanie sceny 3D
	Display();
}

// obs�uga klawiszy funkcyjnych i klawiszy kursora

void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
		// kursor w lewo
	case GLUT_KEY_LEFT:
		rotatey -= 1;
		break;

		// kursor w g�r�
	case GLUT_KEY_UP:
		rotatex -= 1;
		break;

		// kursor w prawo
	case GLUT_KEY_RIGHT:
		rotatey += 1;
		break;

		// kursor w d�
	case GLUT_KEY_DOWN:
		rotatex += 1;
		break;
	}

	// odrysowanie okna
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

// obs�uga przycisk�w myszki

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapami�tanie stanu lewego przycisku myszki
		button_state = state;

		// zapami�tanie po�o�enia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

// obs�uga ruchu kursora myszki

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) *(x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) *(button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

// obs�uga menu podr�cznego

void Menu(int value)
{
	switch (value)
	{
		// p�aszczyzna przekroju
	case CUTTING_PLANE:
		cutting_plane = !cutting_plane;
		Display();
		break;

		// przesuni�cie warto�ci g��bi
	case POLYGON_OFFSET:
		polygon_offset = !polygon_offset;
		Display();
		break;

		// wyj�cie
	case EXIT:
		exit(0);
	}
}

int main(int argc, char * argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// rozmiary g��wnego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie g��wnego okna programu
	glutCreateWindow("Z-bufor");

	// do��czenie funkcji generuj�cej scen� 3D
	glutDisplayFunc(Display);

	// do��czenie funkcji wywo�ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// do��czenie funkcji obs�ugi klawiszy funkcyjnych i klawiszy kursora
	glutSpecialFunc(SpecialKeys);

	// obs�uga przycisk�w myszki
	glutMouseFunc(MouseButton);

	// obs�uga ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie menu podr�cznego
	glutCreateMenu(Menu);

	// menu g��wne
	glutCreateMenu(Menu);
#ifdef WIN32

	glutAddMenuEntry("P�aszczyzna przekroju: rysowana/nierysowana", CUTTING_PLANE);
	glutAddMenuEntry("Przesuni�cie warto�ci g��bi: w��cz/wy��cz", POLYGON_OFFSET);
	glutAddMenuEntry("Wyj�cie", EXIT);
#else

	glutAddMenuEntry("Plaszczyzna przekroju: rysowana/nierysowana", CUTTING_PLANE);
	glutAddMenuEntry("Przesuniecie wartosci glebi: wlacz/wylacz", POLYGON_OFFSET);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// okre�lenie przycisku myszki obs�uguj�cej menu podr�czne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// do��czenie funkcji wywo�ywanej podczas "bezczynno�ci" systemu
	glutIdleFunc(Display);

	// wprowadzenie programu do obs�ugi p�tli komunikat�w
	glutMainLoop();
	return 0;
}
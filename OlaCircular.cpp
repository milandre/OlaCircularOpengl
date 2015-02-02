// Cubica

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#define DEF_floorGridScale	1.0
#define DEF_floorGridXSteps	10.0
#define DEF_floorGridZSteps	10.0
#define PI 3.14159265358979323846

int SoloUno = 0;

GLUnurbsObj *theNurb;

float knots[25] = {0.0,0.0,0.0,0.0,1.0,2.0,
	               3.0,4.0,5.0,6.0,7.0,
				   8.0,9.0,10.0,11.0,12.0,
				   13.0,14.0,15.0,16.0,17.0,
				   18.0,18.0,18.0,18.0};

float decaimiento[21] = {0.05,0.1,0.2,0.325,0.45,
						 0.575,0.7,0.8,0.875,0.95,
						 1.0,
						 0.95,0.875,0.8,0.7,0.575,
						 0.45,0.325,0.2,0.1,0.05};

float ctlpoints[21][21][3]; //puntos de control.
float D[2]; //puntos de la direccion.

// Para las animaciones.
GLfloat AmplitudOla = 0.5f;
GLfloat LongitudOla = 3.5f;
GLfloat VelocidadOla = 2.0f;
GLfloat DecaimientoOla = 0.0f;		// Tengo que revisarlo, no funciona correctamente
GLfloat AmplitudRuido = 30.0f;		// NO ENTIENDO QUE DEBE HACER (AL PARECER GENERA MAS O MENOS SUAVIDAD EN EL RUIDO) Y NO SE SI LO ESTA HACIENDO
GLfloat OffsetRuido = 10.4f;		// NO ENTIENDO QUE DEBE HACER Y NO SE SI LO ESTA HACIENDO
GLfloat AlturaRuido = 1.0f;
GLfloat SizeTurbulencia = 16.0f;	
GLfloat AmplitudDeformador;
GLfloat TranslacionDeformador;
GLfloat centroX = 0.0f;
GLfloat centroZ = 0.0f;
GLint pausaAnimacion = 0;
GLint desactivaRuido = 0;
GLint desactivaOla = 0;
GLfloat time = 0.1;

//Calcular frecuencia de la ola.
GLfloat w; 

//Calcular velocidad de la ola.
GLfloat s;

// Para el ruido y la turbulencia.
#define N 0x1000
#define B 0x100
#define BM 0xff

static int start = 1;
static int p[B + B + 2];
static float g2[B + B + 2][2];

static void init_ruido(void);

#define s_curve(t) ( t * t * (3. - 2. * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;

float noise2(float vec[2])
{
	int bx0, bx1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, rz0, rz1, *q, sx, sz, a, b, t, u, v;
	int i, j;

	if (start) {
		start = 0;
		init_ruido();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + bz0 ];
	b10 = p[ j + bz0 ];
	b01 = p[ i + bz1 ];
	b11 = p[ j + bz1 ];

	sx = s_curve(rx0);
	sz = s_curve(rz0);

#define at2(rx,rz) (rx * q[0] + rz * q[1]) 

	q = g2[ b00 ] ; u = at2(rx0,rz0);
	q = g2[ b10 ] ; v = at2(rx1,rz0);
	a = lerp(sx, u, v);

	q = g2[ b01 ] ; u = at2(rx0,rz1);
	q = g2[ b11 ] ; v = at2(rx1,rz1);
	b = lerp(sx, u, v);

	return lerp(sz, a, b);
}

float turbulence(float x, float z) {

	float n_noise[2];
    float value = 0.0;
	float inicialSize = SizeTurbulencia;
	float size = SizeTurbulencia;
	n_noise[0] = x / size;
	n_noise[1] = z / size;
	while(size >= 1) {
		value += noise2(n_noise) * size;
		size /= 2.0;
	}
	return(128.0 * value / inicialSize);

}

static void normalize2(float v[2])
{
	float s;
	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

static void init_ruido(void) {
	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
	}
}

float ruido(float ptoX, float ptoZ) {

	float n_noise[2];
	float turbulencia;

	n_noise[0] = ptoX * AmplitudRuido + OffsetRuido;
	n_noise[1] = ptoZ * AmplitudRuido + OffsetRuido;
			
	//turbulencia
	turbulencia = turbulence(n_noise[0],n_noise[1]);
	return  (AlturaRuido * 0.005 * turbulencia);

}

// Para las olas.
void circular(float x, float z) {

	float dist = sqrt (pow(x-centroX, 2) + pow(z-centroZ, 2));
	if(dist == 0){
		D[0] = 0.0;
		D[1] = 0.0;
	} else {
		D[0] = (x - centroX) / dist;
		D[1] = (z - centroZ) / dist;
	}

}

void olas(){

	float productoEscalar;
	float decaimientoPto;
	float diferenciaDecaimiento;
	float decaimientoTotal;

	w = ((2 * PI) / LongitudOla );
	s = ((VelocidadOla * 2 * PI) / LongitudOla );

	for(int i = 0; i < 21; i++){
		for(int j = 0; j < 21; j++){

			circular(ctlpoints[i][j][0],ctlpoints[i][j][2]);
			productoEscalar = (D[0] * ctlpoints[i][j][0]) + (D[1] * ctlpoints[i][j][2]); 
			/*
			decaimientoPto = decaimiento[i] * decaimiento[j];
			if (DecaimientoOla <= 0.0) {
				diferenciaDecaimiento = (1 - decaimientoPto) / 100;
				decaimientoTotal = decaimientoPto - 100 * DecaimientoOla * diferenciaDecaimiento;
			} else {
				diferenciaDecaimiento = decaimientoPto / 100;
				decaimientoTotal = decaimientoPto - 100 * DecaimientoOla * diferenciaDecaimiento;
			}
			*/
			if (!desactivaOla && !desactivaRuido) {
				//ctlpoints[i][j][1] = decaimientoTotal * (AmplitudOla * sinf( -1.0 * (productoEscalar * w) + time * s  + ruido(ctlpoints[i][j][0],ctlpoints[i][j][2])));
				ctlpoints[i][j][1] = (AmplitudOla * sinf( -1.0 * (productoEscalar * w) + time * s  + ruido(ctlpoints[i][j][0],ctlpoints[i][j][2])));
			} else if (!desactivaOla && desactivaRuido) {
				//ctlpoints[i][j][1] = decaimientoTotal * (AmplitudOla * sinf( -1.0 * (productoEscalar * w) + time * s ));
				ctlpoints[i][j][1] = (AmplitudOla * sinf( -1.0 * (productoEscalar * w) + time * s ));
			} else if (desactivaOla && !desactivaRuido) {
				// ASI MUESTRA ALGO MUY PARECIDO AL DEL PROFESOR, AUNQUE NO ME CONVENCE ESE SENO. 
				ctlpoints[i][j][1] = sinf(time + ruido(ctlpoints[i][j][0],ctlpoints[i][j][2]));
			} else {
				ctlpoints[i][j][1] = 0.0; 
			}

		}
	}

}

void changeViewport(int w, int h) {
	
	float aspectratio;
	if (h==0)
		h=1;
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(30, (GLfloat) w/(GLfloat) h, 1.0, 200.0);
   glMatrixMode (GL_MODELVIEW);

}

void init_surface() {

   int u, v;
   for (u = 0; u < 21; u++) {
      for (v = 0; v < 21; v++) {
         ctlpoints[v][u][0] = ((GLfloat)u - 10.0);
		 ctlpoints[v][u][2] = ((GLfloat)v - 10.0);
      }
   }

}

void init(){

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_AUTO_NORMAL);
   glEnable(GL_NORMALIZE);
   init_surface();
   theNurb = gluNewNurbsRenderer();
   gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 15.0);
   gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);

}

void Keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
	case 27:

		exit (0);
		break;

	case 65: case 97:   //tecla a

		AmplitudOla += 0.1f;
		break;

	case 90: case 122: //tecla z
		
		if (AmplitudOla > 0.1) {
			AmplitudOla -= 0.1f;
		}
		break;

	case 83: case 115: //tecla s

		LongitudOla += 0.1f;
		break;

	case 88: case 120: //tecla x

		if (LongitudOla > 0.1) {
			LongitudOla -= 0.1;
		}
		break;

	case 100: case 68: //tecla d

		VelocidadOla += 0.1;
		break;

	case 67: case 99: //tecla c 

		if (VelocidadOla > 0.1) {
			VelocidadOla -= 0.1;
		}
		break;

	case 70: case 102: //tecla f

		if (DecaimientoOla < 1.00) {
			DecaimientoOla += 0.01;
		}
		break;

	case 86: case 118: //tecla v

		if (DecaimientoOla > -1.00) {
			DecaimientoOla -= 0.01;
		}
		break;

	case 71: case 103: //tecla g

		AmplitudRuido += 0.01;
		break;

	case 66: case 98: //tecla b

		if (AmplitudRuido > 0.01) {
			AmplitudRuido -= 0.01;
		}
		break;

	case 72: case 104: //tecla h

		OffsetRuido += 0.01;
		break; 

	case 78: case 110: //tecla n

		OffsetRuido -= 0.01;
		break;

	case 74: case 106: //tecla j

		AlturaRuido += 0.01;
		break;

	case 77: case 109: //tecla m

		if (AlturaRuido > 0.01) {
			AlturaRuido -= 0.01;
		}
		break;

	case 84: case 116: //tecla t

		SizeTurbulencia += 1.0;
		break;

	case 89: case 121: //tecla y 

		if (SizeTurbulencia > 1.0) {
			SizeTurbulencia -= 1.0;
		}
		break;

	case 85: case 117: //tecla u

		AmplitudDeformador += 0.001;
		break;

	case 73: case 105: //tecla i

		if (AmplitudDeformador > 0.001) {
			AmplitudDeformador -= 0.001;
		}
		break;
	
	case 79: case 111: //tecla o

		TranslacionDeformador += 0.1;
		break;

	case 76: case 108: //tecla l

		TranslacionDeformador -= 0.1;
		break;

	case 81: case 113: //tecla q

		centroX += 0.1;
		break;

	case 87: case 119: //tecla w

		centroX -= 0.1;
		break;

	case 69: case 101: //tecla e

		centroZ += 0.1;
		break;

	case 82: case 114: //tecla r

		centroZ -= 0.1;
		break;

	case 49: //tecla numero 1

		if(!pausaAnimacion){
			pausaAnimacion = 1;
		} else {
			pausaAnimacion = 0;
		}
		break;

	case 50: //tecla numero 2

		if(!desactivaRuido){
			desactivaRuido = 1;
		} else {
			desactivaRuido = 0;
		}
		break;

	case 51: //tecla numero 3

		if(!desactivaOla){
			desactivaOla = 1;
		} else {
			desactivaOla = 0;
		}
		break;
  }

}

void render(){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat zExtent, xExtent, xLocal, zLocal;
    int loopX, loopZ;

	glLoadIdentity ();                       
	gluLookAt (25.0, 12.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	// Luz y material
	GLfloat mat_diffuse[] = { 0.6, 0.6, 0.9, 1.0 };
	GLfloat mat_specular[] = { 0.8, 0.8, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 60.0 };
	
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	
    GLfloat light_ambient[] = { 0.0, 0.0, 0.2, 1.0 };
	GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light_specular[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat light_position[] = { -10.0, 5.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);   
	
	//Suaviza las lineas
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_LINE_SMOOTH );	

	gluBeginSurface(theNurb);

	gluNurbsSurface(theNurb, 
						25, knots, 25, knots,
                        21 * 3, 3, &ctlpoints[0][0][0], 
                        4, 4, GL_MAP2_VERTEX_3);

	gluEndSurface(theNurb);
	
	//crearPuntosControl();
	/* Muestra los puntos de control 
	
		int i,j;
		glPointSize(5.0);
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 1.0, 0.0);
		glBegin(GL_POINTS);
		for (i = 0; i <21; i++) {
			for (j = 0; j < 21; j++) {
	            glVertex3f(ctlpoints[i][j][0], 	ctlpoints[i][j][1], ctlpoints[i][j][2]);
			}
		}
		glEnd();
		glEnable(GL_LIGHTING);*/
			
		glPointSize(9.0);
		glColor3f(1.0, 1.0, 0.0);
		glBegin(GL_POINTS);
	            glVertex3f(ctlpoints[20][20][0], ctlpoints[20][20][1], ctlpoints[20][20][2]);
		glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glutSwapBuffers();

}

void animacion(int value) {

	if(!pausaAnimacion){
		//olas	
		time += 0.1;
		olas();
	}

	glutTimerFunc(2.0,animacion,1);
    glutPostRedisplay();
	
}



int main (int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(960,540);

	glutCreateWindow("Test Opengl");

	init();

	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);
	glutTimerFunc(10,animacion,1);
	glutKeyboardFunc (Keyboard);
		
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}
	
	glutMainLoop();
	return 0;

}

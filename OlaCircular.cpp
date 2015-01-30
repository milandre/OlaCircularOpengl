// Cubica

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <math.h>

using namespace std;

#define DEF_floorGridScale	1.0
#define DEF_floorGridXSteps	10.0
#define DEF_floorGridZSteps	10.0
#define PI 3.14159265358979323846

GLUnurbsObj *theNurb;

float knots[25] = {0.0,0.0,0.0,0.0,1.0,2.0,
	               3.0,4.0,5.0,6.0,7.0,
				   8.0,9.0,10.0,11.0,12.0,
				   13.0,14.0,15.0,16.0,17.0,
				   18.0,18.0,18.0,18.0};

float ctlpoints[21][21][3]; //puntos de control.
float D[2]; //puntos de la direccion.

GLfloat AmplitudOla = 0.5f; 
GLfloat LongitudOla = 5.0f;
GLfloat VelocidadOla = 2.0f;
GLfloat DecaimientoOla;
GLfloat AmplitudRuido;
GLfloat OffsetRuido;
GLfloat AlturaRuido;
GLfloat SizeTurbulencia;
GLfloat AmplitudDeformador;
GLfloat TranslacionDeformador;
GLfloat centroX = 0.0f;
GLfloat centroZ = 0.0f;
GLint pausaAnimacion = 0;
GLint desactivaRuido;
GLint desactivaOla;
GLfloat time = 0.1;

//calcular frecuencia
GLfloat w = ((2 * PI) / LongitudOla ); 

//calcular speed
GLfloat s = ((VelocidadOla * 2 * PI) / LongitudOla );

/*float ctlpoints[21][21][3] = {


	//Puntos positivos

	{{-10.0, 0.0, 10.0},{-9.0, 0.0, 10.0},{-8.0, 0.0, 10.0},{-7.0, 0.0, 10.0},{-6.0, 0.0, 10.0},{-5.0, 0.0, 10.0},{-4.0, 0.0, 10.0},{-3.0, 0.0, 10.0},{-2.0, 0.0, 10.0},{-1.0, 0.0, 10.0},{0.0, 0.0, 10.0},{1.0, 0.0, 10.0},{2.0, 0.0, 10.0},{3.0, 0.0, 10.0},{4.0, 0.0, 10.0},{5.0, 0.0, 10.0},{6.0, 0.0, 10.0},{7.0, 0.0, 10.0},{8.0, 0.0, 10.0},{9.0, 0.0, 10.0},{10.0, 0.0, 10.0}},

	{{-10.0, 0.0, 9.0},{-9.0, 0.0, 9.0},{-8.0, 0.0, 9.0},{-7.0, 0.0, 9.0},{-6.0, 0.0, 9.0},{-5.0, 0.0, 9.0},{-4.0, 0.0, 9.0},{-3.0, 0.0, 9.0},{-2.0, 0.0, 9.0},{-1.0, 0.0, 9.0},{0.0, 0.0, 9.0},{1.0, 0.0, 9.0},{2.0, 0.0, 9.0},{3.0, 0.0, 9.0},{4.0, 0.0, 9.0},{5.0, 0.0, 9.0},{6.0, 0.0, 9.0},{7.0, 0.0, 9.0},{8.0, 0.0, 9.0},{9.0, 0.0, 9.0},{10.0, 0.0, 9.0}},

	{{-10.0, 0.0, 8.0},{-9.0, 0.0, 8.0},{-8.0, 0.0, 8.0},{-7.0, 0.0, 8.0},{-6.0, 0.0, 8.0},{-5.0, 0.0, 8.0},{-4.0, 0.0, 8.0},{-3.0, 0.0, 8.0},{-2.0, 0.0, 8.0},{-1.0, 0.0, 8.0},{0.0, 0.0, 8.0},{1.0, 0.0, 8.0},{2.0, 0.0, 8.0},{3.0, 0.0, 8.0},{4.0, 0.0, 8.0},{5.0, 0.0, 8.0},{6.0, 0.0, 8.0},{7.0, 0.0, 8.0},{8.0, 0.0, 8.0},{9.0, 0.0, 8.0},{10.0, 0.0, 8.0}},

	{{-10.0, 0.0, 7.0},{-9.0, 0.0, 7.0},{-8.0, 0.0, 7.0},{-7.0, 0.0, 7.0},{-6.0, 0.0, 7.0},{-5.0, 0.0, 7.0},{-4.0, 0.0, 7.0},{-3.0, 0.0, 7.0},{-2.0, 0.0, 7.0},{-1.0, 0.0, 7.0},{0.0, 0.0, 7.0},{1.0, 0.0, 7.0},{2.0, 0.0, 7.0},{3.0, 0.0, 7.0},{4.0, 0.0, 7.0},{5.0, 0.0, 7.0},{6.0, 0.0, 7.0},{7.0, 0.0, 7.0},{8.0, 0.0, 7.0},{9.0, 0.0, 7.0},{10.0, 0.0, 7.0}},

	{{-10.0, 0.0, 6.0},{-9.0, 0.0, 6.0},{-8.0, 0.0, 6.0},{-7.0, 0.0, 6.0},{-6.0, 0.0, 6.0},{-5.0, 0.0, 6.0},{-4.0, 0.0, 6.0},{-3.0, 0.0, 6.0},{-2.0, 0.0, 6.0},{-1.0, 0.0, 6.0},{0.0, 0.0, 6.0},{1.0, 0.0, 6.0},{2.0, 0.0, 6.0},{3.0, 0.0, 6.0},{4.0, 0.0, 6.0},{5.0, 0.0, 6.0},{6.0, 0.0, 6.0},{7.0, 0.0, 6.0},{8.0, 0.0, 6.0},{9.0, 0.0, 6.0},{10.0, 0.0, 6.0}},

	{{-10.0, 0.0, 5.0},{-9.0, 0.0, 5.0},{-8.0, 0.0, 5.0},{-7.0, 0.0, 5.0},{-6.0, 0.0, 5.0},{-5.0, 0.0, 5.0},{-4.0, 0.0, 5.0},{-3.0, 0.0, 5.0},{-2.0, 0.0, 5.0},{-1.0, 0.0, 5.0},{0.0, 0.0, 5.0},{1.0, 0.0, 5.0},{2.0, 0.0, 5.0},{3.0, 0.0, 5.0},{4.0, 0.0, 5.0},{5.0, 0.0, 5.0},{6.0, 0.0, 5.0},{7.0, 0.0, 5.0},{8.0, 0.0, 5.0},{9.0, 0.0, 5.0},{10.0, 0.0, 5.0}},

	{{-10.0, 0.0, 4.0},{-9.0, 0.0, 4.0},{-8.0, 0.0, 4.0},{-7.0, 0.0, 4.0},{-6.0, 0.0, 4.0},{-5.0, 0.0, 4.0},{-4.0, 0.0, 4.0},{-3.0, 0.0, 4.0},{-2.0, 0.0, 4.0},{-1.0, 0.0, 4.0},{0.0, 0.0, 4.0},{1.0, 0.0, 4.0},{2.0, 0.0, 4.0},{3.0, 0.0, 4.0},{4.0, 0.0, 4.0},{5.0, 0.0, 4.0},{6.0, 0.0, 4.0},{7.0, 0.0, 4.0},{8.0, 0.0, 4.0},{9.0, 0.0, 4.0},{10.0, 0.0, 4.0}},

	{{-10.0, 0.0, 3.0},{-9.0, 0.0, 3.0},{-8.0, 0.0, 3.0},{-7.0, 0.0, 3.0},{-6.0, 0.0, 3.0},{-5.0, 0.0, 3.0},{-4.0, 0.0, 3.0},{-3.0, 0.0, 3.0},{-2.0, 0.0, 3.0},{-1.0, 0.0, 3.0},{0.0, 0.0, 3.0},{1.0, 0.0, 3.0},{2.0, 0.0, 3.0},{3.0, 0.0, 3.0},{4.0, 0.0, 3.0},{5.0, 0.0, 3.0},{6.0, 0.0, 3.0},{7.0, 0.0, 3.0},{8.0, 0.0, 3.0},{9.0, 0.0, 3.0},{10.0, 0.0, 3.0}},

	{{-10.0, 0.0, 2.0},{-9.0, 0.0, 2.0},{-8.0, 0.0, 2.0},{-7.0, 0.0, 2.0},{-6.0, 0.0, 2.0},{-5.0, 0.0, 2.0},{-4.0, 0.0, 2.0},{-3.0, 0.0, 2.0},{-2.0, 0.0, 2.0},{-1.0, 0.0, 2.0},{0.0, 0.0, 2.0},{1.0, 0.0, 2.0},{2.0, 0.0, 2.0},{3.0, 0.0, 2.0},{4.0, 0.0, 2.0},{5.0, 0.0, 2.0},{6.0, 0.0, 2.0},{7.0, 0.0, 2.0},{8.0, 0.0, 2.0},{9.0, 0.0, 2.0},{10.0, 0.0, 2.0}},

	{{-10.0, 0.0, 1.0},{-9.0, 0.0, 1.0},{-8.0, 0.0, 1.0},{-7.0, 0.0, 1.0},{-6.0, 0.0, 1.0},{-5.0, 0.0, 1.0},{-4.0, 0.0, 1.0},{-3.0, 0.0, 1.0},{-2.0, 0.0, 1.0},{-1.0, 0.0, 1.0},{0.0, 0.0, 1.0},{1.0, 0.0, 1.0},{2.0, 0.0, 1.0},{3.0, 0.0, 1.0},{4.0, 0.0, 1.0},{5.0, 0.0, 1.0},{6.0, 0.0, 1.0},{7.0, 0.0, 1.0},{8.0, 0.0, 1.0},{9.0, 0.0, 1.0},{10.0, 0.0, 1.0}},
	
	//Puntos que pasan por el origen
	{{-10.0, 0.0, 0.0},{-9.0, 0.0, 0.0},{-8.0, 0.0, 0.0},{-7.0, 0.0, 0.0},{-6.0, 0.0, 0.0},{-5.0, 0.0, 0.0},{-4.0, 0.0, 0.0},{-3.0, 0.0, 0.0},{-2.0, 0.0, 0.0},{-1.0, 0.0, 0.0},{0.0, 0.0, 0.0},{1.0, 0.0, 0.0},{2.0, 0.0, 0.0},{3.0, 0.0, 0.0},{4.0, 0.0, 0.0},{5.0, 0.0, 0.0},{6.0, 0.0, 0.0},{7.0, 0.0, 0.0},{8.0, 0.0, 0.0},{9.0, 0.0, 0.0},{10.0, 0.0, 0.0}},
 
	//Puntos negativos
	{{-10.0, 0.0, -1.0},{-9.0, 0.0, -1.0},{-8.0, 0.0, -1.0},{-7.0, 0.0, -1.0},{-6.0, 0.0, -1.0},{-5.0, 0.0, -1.0},{-4.0, 0.0, -1.0},{-3.0, 0.0, -1.0},{-2.0, 0.0, -1.0},{-1.0, 0.0, -1.0},{0.0, 0.0, -1.0},{1.0, 0.0, -1.0},{2.0, 0.0, -1.0},{3.0, 0.0, -1.0},{4.0, 0.0, -1.0},{5.0, 0.0, -1.0},{6.0, 0.0, -1.0},{7.0, 0.0, -1.0},{8.0, 0.0, -1.0},{9.0, 0.0, -1.0},{10.0, 0.0, -1.0}},

	{{-10.0, 0.0, -2.0},{-9.0, 0.0, -2.0},{-8.0, 0.0, -2.0},{-7.0, 0.0, -2.0},{-6.0, 0.0, -2.0},{-5.0, 0.0, -2.0},{-4.0, 0.0, -2.0},{-3.0, 0.0, -2.0},{-2.0, 0.0, -2.0},{-1.0, 0.0, -2.0},{0.0, 0.0, -2.0},{1.0, 0.0, -2.0},{2.0, 0.0, -2.0},{3.0, 0.0, -2.0},{4.0, 0.0, -2.0},{5.0, 0.0, -2.0},{6.0, 0.0, -2.0},{7.0, 0.0, -2.0},{8.0, 0.0, -2.0},{9.0, 0.0, -2.0},{10.0, 0.0, -2.0}},

	{{-10.0, 0.0, -3.0},{-9.0, 0.0, -3.0},{-8.0, 0.0, -3.0},{-7.0, 0.0, -3.0},{-6.0, 0.0, -3.0},{-5.0, 0.0, -3.0},{-4.0, 0.0, -3.0},{-3.0, 0.0, -3.0},{-2.0, 0.0, -3.0},{-1.0, 0.0, -3.0},{0.0, 0.0, -3.0},{1.0, 0.0, -3.0},{2.0, 0.0, -3.0},{3.0, 0.0, -3.0},{4.0, 0.0, -3.0},{5.0, 0.0, -3.0},{6.0, 0.0, -3.0},{7.0, 0.0, -3.0},{8.0, 0.0, -3.0},{9.0, 0.0, -3.0},{10.0, 0.0, -3.0}},

	{{-10.0, 0.0, -4.0},{-9.0, 0.0, -4.0},{-8.0, 0.0, -4.0},{-7.0, 0.0, -4.0},{-6.0, 0.0, -4.0},{-5.0, 0.0, -4.0},{-4.0, 0.0, -4.0},{-3.0, 0.0, -4.0},{-2.0, 0.0, -4.0},{-1.0, 0.0, -4.0},{0.0, 0.0, -4.0},{1.0, 0.0, -4.0},{2.0, 0.0, -4.0},{3.0, 0.0, -4.0},{4.0, 0.0, -4.0},{5.0, 0.0, -4.0},{6.0, 0.0, -4.0},{7.0, 0.0, -4.0},{8.0, 0.0, -4.0},{9.0, 0.0, -4.0},{10.0, 0.0, -4.0}},

	{{-10.0, 0.0, -5.0},{-9.0, 0.0, -5.0},{-8.0, 0.0, -5.0},{-7.0, 0.0, -5.0},{-6.0, 0.0, -5.0},{-5.0, 0.0, -5.0},{-4.0, 0.0, -5.0},{-3.0, 0.0, -5.0},{-2.0, 0.0, -5.0},{-1.0, 0.0, -5.0},{0.0, 0.0, -5.0},{1.0, 0.0, -5.0},{2.0, 0.0, -5.0},{3.0, 0.0, -5.0},{4.0, 0.0, -5.0},{5.0, 0.0, -5.0},{6.0, 0.0, -5.0},{7.0, 0.0, -5.0},{8.0, 0.0, -5.0},{9.0, 0.0, -5.0},{10.0, 0.0, -5.0}},

	{{-10.0, 0.0, -6.0},{-9.0, 0.0, -6.0},{-8.0, 0.0, -6.0},{-7.0, 0.0, -6.0},{-6.0, 0.0, -6.0},{-5.0, 0.0, -6.0},{-4.0, 0.0, -6.0},{-3.0, 0.0, -6.0},{-2.0, 0.0, -6.0},{-1.0, 0.0, -6.0},{0.0, 0.0, -6.0},{1.0, 0.0, -6.0},{2.0, 0.0, -6.0},{3.0, 0.0, -6.0},{4.0, 0.0, -6.0},{5.0, 0.0, -6.0},{6.0, 0.0, -6.0},{7.0, 0.0, -6.0},{8.0, 0.0, -6.0},{9.0, 0.0, -6.0},{10.0, 0.0, -6.0}},

	{{-10.0, 0.0, -7.0},{-9.0, 0.0, -7.0},{-8.0, 0.0, -7.0},{-7.0, 0.0, -7.0},{-6.0, 0.0, -7.0},{-5.0, 0.0, -7.0},{-4.0, 0.0, -7.0},{-3.0, 0.0, -7.0},{-2.0, 0.0, -7.0},{-1.0, 0.0, -7.0},{0.0, 0.0, -7.0},{1.0, 0.0, -7.0},{2.0, 0.0, -7.0},{3.0, 0.0, -7.0},{4.0, 0.0, -7.0},{5.0, 0.0, -7.0},{6.0, 0.0, -7.0},{7.0, 0.0, -7.0},{8.0, 0.0, -7.0},{9.0, 0.0, -7.0},{10.0, 0.0, -7.0}},

	{{-10.0, 0.0, -8.0},{-9.0, 0.0, -8.0},{-8.0, 0.0, -8.0},{-7.0, 0.0, -8.0},{-6.0, 0.0, -8.0},{-5.0, 0.0, -8.0},{-4.0, 0.0, -8.0},{-3.0, 0.0, -8.0},{-2.0, 0.0, -8.0},{-1.0, 0.0, -8.0},{0.0, 0.0, -8.0},{1.0, 0.0, -8.0},{2.0, 0.0, -8.0},{3.0, 0.0, -8.0},{4.0, 0.0, -8.0},{5.0, 0.0, -8.0},{6.0, 0.0, -8.0},{7.0, 0.0, -8.0},{8.0, 0.0, -8.0},{9.0, 0.0, -8.0},{10.0, 0.0, -8.0}},

	{{-10.0, 0.0, -9.0},{-9.0, 0.0, -9.0},{-8.0, 0.0, -9.0},{-7.0, 0.0, -9.0},{-6.0, 0.0, -9.0},{-5.0, 0.0, -9.0},{-4.0, 0.0, -9.0},{-3.0, 0.0, -9.0},{-2.0, 0.0, -9.0},{-1.0, 0.0, -9.0},{0.0, 0.0, -9.0},{1.0, 0.0, -9.0},{2.0, 0.0, -9.0},{3.0, 0.0, -9.0},{4.0, 0.0, -9.0},{5.0, 0.0, -9.0},{6.0, 0.0, -9.0},{7.0, 0.0, -9.0},{8.0, 0.0, -9.0},{9.0, 0.0, -9.0},{10.0, 0.0, -9.0}},

	{{-10.0, 0.0, -10.0},{-9.0, 0.0, -10.0},{-8.0, 0.0, -10.0},{-7.0, 0.0, -10.0},{-6.0, 0.0, -10.0},{-5.0, 0.0, -10.0},{-4.0, 0.0, -10.0},{-3.0, 0.0, -10.0},{-2.0, 0.0, -10.0},{-1.0, 0.0, -10.0},{0.0, 0.0, -10.0},{1.0, 0.0, -10.0},{2.0, 0.0, -10.0},{3.0, 0.0, -10.0},{4.0, 0.0, -10.0},{5.0, 0.0, -10.0},{6.0, 0.0, -10.0},{7.0, 0.0, -10.0},{8.0, 0.0, -10.0},{9.0, 0.0, -10.0},{10.0, 0.0, -10.0}},

};*/

void directionalOrCircular(float x, float z) {

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

	for(int i = 0; i < 21; i++){

		for(int j = 0; j < 21; j++){
		
			directionalOrCircular(ctlpoints[i][j][0],ctlpoints[i][j][2]);

			productoEscalar = (D[0] * ctlpoints[i][j][0]) + (D[1] * ctlpoints[i][j][2]); 
			
			ctlpoints[i][j][1] = AmplitudOla * sinf( -1.0 * (productoEscalar * w)+ time * s);

		}
	
	}

}

void ejesCoordenada() {
	
	glLineWidth(2.5);
	glBegin(GL_LINES);
		glColor3f(1.0,0.0,0.0);
		glVertex2f(0,10);
		glVertex2f(0,-10);
		glColor3f(0.0,0.0,1.0);
		glVertex2f(10,0);
		glVertex2f(-10,0);
	glEnd();

	glLineWidth(1.5);
	int i;
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINES);
		for(i = -10; i <=10; i++){
			if (i!=0) {		
				if ((i%2)==0){	
					glVertex2f(i,0.4);
					glVertex2f(i,-0.4);

					glVertex2f(0.4,i);
					glVertex2f(-0.4,i);
				}else{
					glVertex2f(i,0.2);
					glVertex2f(i,-0.2);

					glVertex2f(0.2,i);
					glVertex2f(-0.2,i);

				}
			}
		}
		
	glEnd();

	glLineWidth(1.0);
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

         ctlpoints[u][v][0] = ((GLfloat)u - 10.0);
		 //ctlpoints[u][v][1] = 0.0;
         //ctlpoints[u][v][1] = 2.0*((GLfloat)v - 1.5);
		 ctlpoints[u][v][2] = ((GLfloat)v - 10.0);


         /*if ( (u == 10 || u == 11 || u == 12) && (v == 10 || v == 11 || v == 12))
            ctlpoints[u][v][1] = 1.0;
         else
            ctlpoints[u][v][1] = -1.0;*/
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

		AmplitudOla -= 0.1f;
		break;

	case 83: case 115: //tecla s

		LongitudOla += 0.1f;
		if(LongitudOla != 0) {
			w = ((2 * PI) / LongitudOla );
			s = ((VelocidadOla * 2 * PI) / LongitudOla );
		}
		break;

	case 88: case 120: //tecla x

		LongitudOla -= 0.1f;
		if(LongitudOla != 0) {
			w = ((2 * PI) / LongitudOla );
			s = ((VelocidadOla * 2 * PI) / LongitudOla );
		}
		break;

	case 100: case 68: //tecla d

		VelocidadOla += 0.1;
		break;

	case 67: case 99: //tecla c 

		VelocidadOla -= 0.1;
		break;

	case 70: case 102: //tecla f

		DecaimientoOla += 0.01;
		break;

	case 86: case 118: //tecla v

		DecaimientoOla -= 0.01;
		break;

	case 71: case 103: //tecla g

		AmplitudRuido += 0.01;
		break;

	case 66: case 98: //tecla b

		AmplitudRuido -= 0.01;
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

		AlturaRuido -= 0.01;
		break;

	case 84: case 116: //tecla t

		SizeTurbulencia += 1.0;
		break;

	case 89: case 121: //tecla y 

		SizeTurbulencia -= 1.0;
		break;

	case 85: case 117: //tecla u

		AmplitudDeformador += 0.001;
		break;

	case 73: case 105: //tecla i

		AmplitudDeformador -= 0.001;
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

		if(pausaAnimacion == 0){
			pausaAnimacion = 1;
		} else {
			pausaAnimacion = 0;
		}
		break;

	case 50: //tecla numero 2

		if(desactivaRuido == 0){
			desactivaRuido = 1;
		} else {
			desactivaRuido = 0;
		}
		break;

	case 51: //tecla numero 2

		if(desactivaOla == 0){
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


	
	/* Render Grid 
	glDisable(GL_LIGHTING);
	glLineWidth(1.0);
	glPushMatrix();
	glRotatef(90,1.0,0.0,0.0);
    glColor3f( 0.0, 0.7, 0.7 );
    glBegin( GL_LINES );
    zExtent = DEF_floorGridScale * DEF_floorGridZSteps;
    for(loopX = -DEF_floorGridXSteps; loopX <= DEF_floorGridXSteps; loopX++ )
	{
	xLocal = DEF_floorGridScale * loopX;
	glVertex3f( xLocal, -zExtent, 0.0 );
	glVertex3f( xLocal, zExtent,  0.0 );
	}
    xExtent = DEF_floorGridScale * DEF_floorGridXSteps;
    for(loopZ = -DEF_floorGridZSteps; loopZ <= DEF_floorGridZSteps; loopZ++ )
	{
	zLocal = DEF_floorGridScale * loopZ;
	glVertex3f( -xExtent, zLocal, 0.0 );
	glVertex3f(  xExtent, zLocal, 0.0 );
	}
    glEnd();
	ejesCoordenada();
    glPopMatrix();
	glEnable(GL_LIGHTING);
	// Fin Grid*/
	


	//Suaviza las lineas
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_LINE_SMOOTH );	

	gluBeginSurface(theNurb);

	glRotatef(180.0f,0.0f,0.0f,0.0f);

		gluNurbsSurface(theNurb, 
						25, knots, 25, knots,
                        21 * 3, 3, &ctlpoints[0][0][0], 
                        4, 4, GL_MAP2_VERTEX_3);
    
	/*gluNurbsSurface(theNurb, 
                   25, variableKnots, 25, variableKnots,
                   21 * 3, 3, variablePuntosControl, 
                   4, 4, GL_MAP2_VERTEX_3);*/
	/*

		No cambien los numeros de la funcion, solo deben de poner los nombres de las variables correspondiente.
		
	*/


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

	glutTimerFunc(10,animacion,1);
    glutPostRedisplay();
	
}

int main (int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(960,540);

	glutCreateWindow("Test Opengl");

	init ();

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

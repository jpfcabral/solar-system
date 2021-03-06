#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

static int year = 0, day = 0;
static float x_planet = 0.0, y_planet = 0.0, z_planet = 0.0;
static float t = 0.0;

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);

   glPushMatrix();

   glColor3f (0.901, 0.654, 0.039); /* laranja amarelado */
   glutSolidSphere(1.0, 20, 16);   /* solid sun */

   glColor3f (1.0, 1.0, 1.0);	   /* branco */
   glutWireSphere(1.0, 20, 16);

   glRotatef ((GLfloat) year, 0.0, 1.0, 0.0);
   glTranslatef ((GLfloat) x_planet, (GLfloat) y_planet, 0.0);
   glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);

   glColor3f (0.2, 0.2, 1.0); /* azul claro */
   glutSolidSphere(0.2, 10, 8);    /* draw smaller planet */
   glColor3f (1.0, 1.0, 1.0);  	   /* branco */
   glutWireSphere(0.2, 10, 8);

   glPopMatrix();
   glutSwapBuffers();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
      case 'd':
         day = (day + 10) % 360;
         glutPostRedisplay();
         break;
      case 'D':
         day = (day - 10) % 360;
         glutPostRedisplay();
         break;
      case 'y':
         t += 5;
         x_planet = 2 * cos(t * M_PI / 360);
         y_planet = 2 * sin(t * M_PI / 360);
         printf("x_planet = %f\n", x_planet);
         glutPostRedisplay();
         break;
      case 'Y':
         year = (year - 5) % 360;
         glutPostRedisplay();
         break;
      case 'a':
         t += 5;
         x_planet = 2 * cos(t * M_PI / 360);
         y_planet = 2 * sin(t * M_PI / 360);
         day = (day + 10) % 360;
         printf("x_planet = %f\n", x_planet);
         glutPostRedisplay();
         break;
      case 'A':
         day = (day - 10) % 360;
         year = (year - 5) % 360;
         glutPostRedisplay();
         break;
      default:
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}

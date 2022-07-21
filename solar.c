#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

// Biblioteca para leitura de imagens
// https://github.com/nothings/stb/blob/master/stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "codes/stb_image.h"

typedef struct {
   int faces;
   float raio;
   float posX;
   float posY;
   float posZ;
} Corpo;


static int year = 0, day = 0;
static float x_planet = 0.0, y_planet = 0.0, z_planet = 0.0;
static float t = 0.0;

static const int TEXT_SOL_INDEX = 0;
static const int TEXT_TERRA_INDEX = 1;

Corpo sol;
Corpo terra; 

// Identificador das texturas
GLuint texturaId[10];

void inicializaCorpos(){
  sol.faces = 50;
  sol.raio = 5.0;
  sol.posX = 0.0;
  sol.posY = 0.0;
  sol.posZ = 0.0;

	terra.raio = 1.1;
	terra.faces = 50;
  terra.posX = 0.0;
  terra.posY = 0.0;
  terra.posZ = 0.0;
}

void carregaTextura(GLuint texId, char* filePath){
    // Variável para guardar os dados da imagem
    unsigned char* imgData;

    // Variáveis para guardar os parâmetros da imagem
    int largura, altura, canais;
    
    // Define um flip vertical de leitura da imagem
	/**
	 * x - onde começa a leitura
	 * 
	 *  ________________				 ________________
	 * |x				     |			|		           |
	 * |		           |			|			        |
	 * |		           |			|				     |
	 * |	imagem	     | 			|	  imagee      |
	 * |				     |		->	|			        |
	 * |		           | 			|		           |
	 * |_______________ |			|x_______________|
	 *    antes do flip						apos o flip
	 * 
	 * A mudança foi necessária devido ao opengl que tem como padrão de leitura
	 * o canto inferior esquerdo
	 */
	stbi_set_flip_vertically_on_load(true);

    // Leitura da imagem -> caminho/largura/altura/quantidade de canais no arquivo/quantidade de canais
    imgData = stbi_load(filePath, &largura, &altura, &canais, 4);

    if(imgData){
        // Liga um espaço de memória no opengl ao texId passado para criar a textura
        glBindTexture(GL_TEXTURE_2D, texId);
    
    // Criação da textura a partir de uma imagem 2D
		// Parâmetros:
		// 1 - O objeto: GL_TEXTURE_2D
		// 2 - O nivel de detalhamento: para imagens é "0" 
		// 3 - Especifica o formato das cores na textura: GL_RGBA8 (Red - Gree - Blue - Alpha de 8bits)
		// 4 - lagura da textura: no momento da leitura da imagem foi atribuída a variável "largura"
		// 5 - altura da textura: no momento da leitura da imagem foi atribuída a variável "altura"
		// 6 - tamanho da borda: "0" (sem borda)
		// 7 - Formato de cores dos dados da imagem: GL_RGBA
		// 8 - Especifica o tipo de dado da imagem: GL_UNSIGNED_BYTE (8 bits sem sinal)
		// 9 - Dados da imagem: Armazenado em "imgData"
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, largura, altura, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);

		// Resolução de problema de miniaturização (Textura com alta resolução quando distante da câmera)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Quando há necessidade de ampliação da textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Repete a imagem em x (S no opengl) sempre que a largura da imagem for menor que a lagura da textura
		// Para esse projeto a "lagura da Imagem" = "lagura da textura"
		// devido ao parâmetro "largura" da textura ser o mesmo extraído da imagem
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

		// Repete a imagem em y (T no opengl) sempre que a altura da imagem for menor que a altura da textura
		// Para esse projeto a "altura da Imagem" = "altura da textura"
		// devido ao parâmetro "altura" da textura ser o mesmo extraído da imagem
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Liberação da memória ocupada pela imagem
        stbi_image_free(imgData);
    }
    else {
        printf("ERRO - Não foi possível carregar a Imagem! - ");
    }
}

void drawSpheres(int raio, GLuint _sphereTextureId){
    glColor3f(0.0,1.0,0.0);
    GLUquadricObj* pQuadric = gluNewQuadric();
    gluQuadricDrawStyle(pQuadric, GLU_FILL);
    glBindTexture(GL_TEXTURE_2D, _sphereTextureId);
    gluQuadricTexture(pQuadric, 'y');
    gluQuadricNormals(pQuadric, GLU_SMOOTH);
    glTranslatef(-200.0,200,-1000.0);
    gluSphere(pQuadric, raio, 20, 20);
}

void desenhaPlaneta(float raio, int qtd_meridianos, int qtd_paralelos, GLint texId){
    //glColor3f(0.0,1.0,0.0);
   
   glEnable(GL_TEXTURE_2D);
   //glPushMatrix();
   
   GLUquadricObj* q = gluNewQuadric ( );

   // Define o estilo de desenho  - GLU_FILL -> renderização das quádricas como polígonos primitivos
   gluQuadricDrawStyle ( q, GLU_FILL );

   glBindTexture(GL_TEXTURE_2D, texId);

   // Habilita textura na Quadric
   gluQuadricTexture ( q, GL_TRUE );

   // Define os vetores normas da Quadric - GLU_SMOOTH -> Uma normal para cada vértice
   gluQuadricNormals ( q, GLU_SMOOTH );


   // Desenha esfera com a Quadric - (lHori e lVert - corresponde a quantidade de meridianos e paralelos)
   gluSphere ( q, raio, qtd_meridianos, qtd_paralelos);

   // Exclui a quadric
   //gluDeleteQuadric ( q );
   //glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

void renderizaSistema(){
   glClear (GL_COLOR_BUFFER_BIT);
   glPushMatrix();
  // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   // Desenha o Sol
   desenhaPlaneta(sol.raio, sol.faces, sol.faces, texturaId[TEXT_SOL_INDEX]);
   //drawSpheres(sol.raio, texturaId[TEXT_SOL_INDEX]);

   glTranslatef ((GLfloat) terra.posX, (GLfloat) terra.posY, (GLfloat) terra.posZ);
	
   desenhaPlaneta(terra.raio, terra.faces, terra.faces, texturaId[TEXT_TERRA_INDEX]);
   //drawSpheres(terra.raio, texturaId[TEXT_TERRA_INDEX]);
   glPopMatrix();

}

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);

   //Gera o vetor de texturas
   glGenTextures(2, texturaId);

   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   // Carrega as texturas
  carregaTextura(texturaId[TEXT_SOL_INDEX], "texturas/Sol.jpg");
  carregaTextura(texturaId[TEXT_TERRA_INDEX], "texturas/Terra.jpg");
	// carregaTextura(texturaId[2], "../texturas/Venus.jpg");
	// carregaTextura(texturaId[3], "../texturas/Mercurio.jpg");
	// carregaTextura(texturaId[4], "../texturas/Marte.jpg");
	// carregaTextura(texturaId[5], "../texturas/Jupiter.jpg");
	// carregaTextura(texturaId[6], "../texturas/Saturno.jpg");
	// carregaTextura(texturaId[7], "../texturas/AnelSaturno.png");
	// carregaTextura(texturaId[8], "../texturas/Urano.jpg");
	// carregaTextura(texturaId[9], "../texturas/Netuno.jpg");
}

void display(void)
{

   // glColor3f (0.901, 0.654, 0.039); /* laranja amarelado */
   // glutSolidSphere(1.0, 20, 16);   /* solid sun */

   // glColor3f (1.0, 1.0, 1.0);	   /* branco */
   // glutWireSphere(1.0, 20, 16);

   // glRotatef ((GLfloat) year, 0.0, 1.0, 0.0);
   // glTranslatef ((GLfloat) x_planet, (GLfloat) y_planet, 0.0);
   // glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);

   // glColor3f (0.2, 0.2, 1.0); /* azul claro */
   // glutSolidSphere(0.2, 10, 8);    /* draw smaller planet */
   // glColor3f (1.0, 1.0, 1.0);  	   /* branco */
   // glutWireSphere(0.2, 10, 8);

   // glutSwapBuffers();
   //glLoadIdentity();

   renderizaSistema();
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
   gluLookAt (0.0, 0.0, 20, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
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
         terra.posX = 10 * cos(t * M_PI / 360);
         terra.posY = 10 * sin(t * M_PI / 360);
         //printf("x_planet = %f\n", x_planet);
         glutPostRedisplay();
         break;
      case 'Y':
         year = (year - 5) % 360;
         glutPostRedisplay();
         break;
      case 'a':
         t += 5;
         terra.posX = 10 * cos(t * M_PI / 360);
         terra.posY = 10 * sin(t * M_PI / 360);
         day = (day + 10) % 360;
         //printf("x_planet = %f\n", x_planet);
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
   inicializaCorpos();
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize(1000, 1000); 
   glutInitWindowPosition (0, 0);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}

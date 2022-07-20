/*Planetas(rotação e translação)------------------------------------------------------------------------------------------------------------------
Mercúrio: 59 dias(day) de rotação e 87 dias(year) de translação
Vênus: 243 dias(day) de rotação e 225 dias(year) de translação
Terra: 1 dia(day) de rotação e 365 dias(year) de translação
Marte: 1 dia(day) de rotação e 687 dias(year) de translação
Júpiter: 0,39 dias(day) de rotação e 12*365 dia(year) de translação
Saturno: 0,42 dias(day) de rotação e 30*365 dia(year) de translação
Urano: 0,71 dias(day) de rotação e 84*365 dia(year) de translação
Netuno: 0,67 dias(day) de rotação e 164*365 dia(year) de translação
------------------------------------------------------------------------------------------------------------------------------------------------*/

/*Planetas(raio)----------------------------------------------------------------------------------------------------------------------------------
Mercúrio: 2.439,7 km (0,11)
Vênus: 6.051,8 km (0,28)
Terra: 6.371 km (0.4)
Marte: 3.389,5 km (0,15)
Júpiter: 69.911 km (3,29)
Saturno: 58.232 km (2,74)
Urano: 25.632 km (1,21)
Netuno: 24.622 km (1,16)
------------------------------------------------------------------------------------------------------------------------------------------------*/

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "codes/stb_image.h"

typedef struct{
	float X;
	float Y;
	float Z;
}EixoFLOAT;

typedef struct{
	int X;
	int Y;
	int Z;
}EixoINT;

typedef struct{
	float Largura;
	float Altura;
}Janela;

static int translacao_t = 0, translacao_me = 0, translacao_v = 0, translacao_ma = 0, translacao_j = 0, translacao_s = 0, translacao_u = 0, translacao_n = 0;

static int rotacao_t = 0, rotacao_me = 0, rotacao_v = 0, rotacao_ma = 0, rotacao_j = 0, rotacao_s = 0, rotacao_u = 0, rotacao_n = 0;

EixoFLOAT camera;

int modoCamera = 0;
EixoFLOAT camera;
EixoINT cursor;
EixoINT mouse;

int xInicial;
int yInicial;
int xMove = 0;
int yMove = 0;
float anguloAtualX = M_PI_2;
float anguloAtualY = M_PI;
int distCamera = 10;
float focoCamX = 0;
float focoCamY = 0;
float focoCamZ = 0;
float offsetX = 0;
float offsetZ = 0;

GLuint texturaId[10];

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
	 * |x				        |				|				         |
	 * |				        |				|				         |
	 * |				        |				|				         |
	 * |	imagem		    | 			|	  imagem	     |
	 * |				        |		->	|				         |
	 * |				        |				|				         |
	 * |________________|				|x_______________|
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

void posicao_cam(){
	/**
	 * Equação paramétrica da esfera no espaço (definição para esse projeto com up da câmera em y)
	 * x = (raio)*cos(theta)*cos(phi)
	 * y = (raio)*sin(phi)
	 * z = (raio)*sin(theta)*cos(phi)
	 * 
	 * (raio) - distância entre o foco da câmera e a câmera
	 * theta - corresponde ao ângulo rotacionado com eixo rotacional vertical
	 * phi - corresponde ao ângulo rotacionado com eixo rotacional horizontal
	 * 
	 * A cada 500 pixel corresponde a 180 graus de rotação
	 */
	float theta = (xMove/500.0)*M_PI + anguloAtualX;
	float phi = (yMove/500.0)*M_PI + anguloAtualY;

	// Define a posição de câmera (x,y,z) apartir dos parâmetros theta e phi
	// OffsetX e OffsetZ deslocamento de posição de câmera quando o foco é atribuído aos planetas
	camera.X = distCamera*cos(theta)*cos(phi) + offsetX;
	camera.Y = distCamera*sin(phi);
	camera.Z = distCamera*sin(theta)*cos(phi) + offsetZ;
	
	// Define qual o valor de phi na faixa 0 até 2pi
    float faixaAngulo = ((phi/(2.0*M_PI)) - (int)(phi/(2.0*M_PI)))*2.0*M_PI;
	
    if((faixaAngulo >= M_PI_2 && faixaAngulo <= 3*M_PI_2 )||(faixaAngulo <= -M_PI_2 && faixaAngulo >= -3*M_PI_2 ))
	  // Orientação do up da câmera em -y quando -90° < phi < 90°
		gluLookAt(camera.X, camera.Y, camera.Z, focoCamX, focoCamY, focoCamZ, 0, -1, 0);
    else 
		    // Orientação do up da câmera em -y quando phi < -90° e phi > 90°
        gluLookAt(camera.X, camera.Y, camera.Z, focoCamX, focoCamY, focoCamZ, 0, 1, 0);
}

void moveMouse(int x, int y){
	// Modo rotação
	if(modoCamera == 1){
		// Calcula o quanto o mouse se moveu em "x" e "y" a partir de "x" e "y" em que ocorreu o pressionamento 
		// do botão esquerdo do mouse
		xMove = x - xInicial;
		yMove = y - yInicial;

		// Faz um redesenho na tela
		glutPostRedisplay();
	} 
	// Modo zoom
	else if(modoCamera == 2){
		// Muda a distância de câmera
		distCamera += (y - yInicial);
		yInicial = y;

		// Limita a aproximação
		if(distCamera < 2){
			distCamera = 2;
		}

		// Limita o afastamento 
		if(distCamera > 100){
			distCamera = 100;
		}

		glutPostRedisplay();
	}
	
}

void mousePress(int button, int state, int x, int y){
	switch (button)
    {
    // Avalia o pressionamento do botão esquerdo do mouse
		case GLUT_LEFT_BUTTON:
			// Quando pressionado
			if(state == GLUT_DOWN){
				// Define a posição (x,y) na tela em que foi pressinado
				xInicial = x;
				yInicial = y;

				// Define o modo de câmera 1 (rotação de visualização)
				modoCamera = 1;
			}
			
			// Quando liberado
			if(state == GLUT_UP){
				// Define os ângulos em que parou após a rotação (theta e phi)
				anguloAtualX += (xMove/500.0)*M_PI;
				anguloAtualY += (yMove/500.0)*M_PI;

				// Seta os valores para "0"
				xMove = 0;
				yMove = 0;

				// Define o modo de câmera para "0" (Nada ocorre com a câmera)
				modoCamera = 0;
			}
            break;
		    // Avalia o pressionamento do botão direito do mouse
        case GLUT_RIGHT_BUTTON: 
			// Quando pressionado
			if (state == GLUT_DOWN){
				// Define y Inicial para o valor de y no momento em que foi pressionado
				yInicial = y;

				// Muda o modo de câmera 2 (Zoom de câmera)
				modoCamera = 2;
			}
			
			// Quando e liberado
			if(state == GLUT_UP){
				// Define o modo de câmera para "0" (Nada ocorre com a câmera)
				modoCamera = 0;
			}
            break;
    }
    
}

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   
	  // Habilita a definição da cor do material a partir da cor corrente
   glEnable(GL_COLOR_MATERIAL);

    //Habilita o uso de iluminação
   glEnable(GL_LIGHTING);  

	  // Habilita a luz de número 0
   glEnable(GL_LIGHT0);

	  // Habilita o depth-buffering
   glEnable(GL_DEPTH_TEST);
   
      glGenTextures(10, texturaId);

    // Parâmetros da textura no objeto -> GL_REPLACE - Substitui a cor pela textura
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   
   	carregaTextura(texturaId[0], "texturas/Sol.jpg");
	carregaTextura(texturaId[1], "texturas/Mercurio.jpg");
	carregaTextura(texturaId[2], "texturas/Venus.jpg");
	carregaTextura(texturaId[3], "texturas/Terra.jpg");
	carregaTextura(texturaId[4], "texturas/Marte.jpg");
	carregaTextura(texturaId[5], "texturas/Jupiter.jpg");
	carregaTextura(texturaId[6], "texturas/Saturno.jpg");
	carregaTextura(texturaId[7], "texturas/AnelSaturno.png");
	carregaTextura(texturaId[8], "texturas/Urano.jpg");
	carregaTextura(texturaId[9], "texturas/Netuno.jpg");
	
	glShadeModel (GL_SMOOTH);
}


void DefineIluminacao (void)
{
		    // Parâmetro de iluminação ambiente
        GLfloat luzAmbiente[4]={0.03,0.03,0.03,1.0};

		    // Cor - variação (0.0 a 1.0) - define como cor da luz "branca"
        GLfloat luzDifusa[4]={1.0,1.0,1.0,1.0};

		    // Brilho
        GLfloat luzEspecular[4]={0.2, 0.2, 0.2, 1.0};

		    // Posição da luz no espaço
        GLfloat posicaoLuz[4]={0.0, 0.0, 0.0, 1.0};
 
        // Capacidade de brilho do material
        GLfloat especularidade[4]={1.0,1.0,1.0,1.0}; 
        GLint especMaterial = 60;
 
        // Define a reflectância do material 
        glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);

        // Define a concentração do brilho
        glMateriali(GL_FRONT,GL_SHININESS,especMaterial);
 
        // Ativa o uso da luz ambiente 
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);
 
        // Define os parâmetros da luz de número 0
        glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente); 
        glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
        glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
        glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );   
}


void display(void)
{

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   glColor3f (1.0, 1.0, 1.0);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

   posicao_cam();
   
   glEnable(GL_TEXTURE_2D);
      glPushMatrix();
   glBindTexture(GL_TEXTURE_2D, texturaId[0]);
   GLUquadricObj* sol = gluNewQuadric ( );
			      // Define o estilo de desenho  - GLU_FILL -> renderização das quádricas como polígonos primitivos
   gluQuadricDrawStyle ( sol, GLU_FILL );

			      // Define os vetores normas da Quadric - GLU_SMOOTH -> Uma normal para cada vértice
   gluQuadricNormals ( sol, GLU_SMOOTH );
			      // Habilita textura na Quadric
   gluQuadricTexture ( sol, GL_TRUE );

   gluQuadricOrientation (sol, GLU_OUTSIDE);
			      // Desenha esfera com a Quadric - (lHori e lVert - corresponde a quantidade de meridianos e paralelos)
   gluSphere ( sol, 1, 20, 16);

			      // Exclui a quadric
   gluDeleteQuadric ( sol );

   //terra
      
   glRotatef ((GLfloat) translacao_t, 0.0, 1.0, 0.0);
   glTranslatef (2.0, 0.0, 0.0);
   glRotatef ((GLfloat) rotacao_t, 0.0, 1.0, 0.0);
   
   glPushMatrix();
   glBindTexture(GL_TEXTURE_2D, texturaId[3]);
   GLUquadricObj* terra = gluNewQuadric ( );
			      // Define o estilo de desenho  - GLU_FILL -> renderização das quádricas como polígonos primitivos
   gluQuadricDrawStyle ( terra, GLU_FILL );

			      // Define os vetores normas da Quadric - GLU_SMOOTH -> Uma normal para cada vértice
   gluQuadricNormals ( terra, GLU_SMOOTH );
			      // Habilita textura na Quadric
   gluQuadricTexture ( terra, GL_TRUE );

			      // Desenha esfera com a Quadric - (lHori e lVert - corresponde a quantidade de meridianos e paralelos)
   gluQuadricOrientation (terra, GLU_OUTSIDE);
   gluSphere ( terra, 0.020, 10, 8);

			      // Exclui a quadric
   gluDeleteQuadric ( terra );

   glPopMatrix();
  
   //Mercurio
   glPushMatrix();
   glRotatef ((GLfloat) translacao_me, 0.0, 1.0, 0.0);
   glTranslatef(3.0, 0.0, 0.0);
   glRotatef ((GLfloat) rotacao_me, 0.0, 1.0, 0.0);

   glBindTexture(GL_TEXTURE_2D, texturaId[1]);
   GLUquadricObj* me = gluNewQuadric ( );
			      // Define o estilo de desenho  - GLU_FILL -> renderização das quádricas como polígonos primitivos
   gluQuadricDrawStyle ( me, GLU_FILL );

			      // Define os vetores normas da Quadric - GLU_SMOOTH -> Uma normal para cada vértice
   gluQuadricNormals ( me, GLU_SMOOTH );
			      // Habilita textura na Quadric
   gluQuadricTexture ( me, GL_TRUE );

			      // Desenha esfera com a Quadric - (lHori e lVert - corresponde a quantidade de meridianos e paralelos)
   gluQuadricOrientation (me, GLU_OUTSIDE);
   gluSphere ( me, 0.031, 10, 8);

			      // Exclui a quadric
   gluDeleteQuadric ( me );

   glPopMatrix();

   //venus
   glPushMatrix();
   
   glRotatef ((GLfloat) translacao_v, 0.0, 1.0, 0.0);
   glTranslatef(4.0, 0.0, 0.0);
   glRotatef ((GLfloat) rotacao_v, 0.0, 1.0, 0.0);

   glBindTexture(GL_TEXTURE_2D, texturaId[2]);
   GLUquadricObj* v = gluNewQuadric ( );
			      // Define o estilo de desenho  - GLU_FILL -> renderização das quádricas como polígonos primitivos
   gluQuadricDrawStyle ( v, GLU_FILL );

			      // Define os vetores normas da Quadric - GLU_SMOOTH -> Uma normal para cada vértice
   gluQuadricNormals ( v, GLU_SMOOTH );
			      // Habilita textura na Quadric
   gluQuadricTexture ( v, GL_TRUE );

			      // Desenha esfera com a Quadric - (lHori e lVert - corresponde a quantidade de meridianos e paralelos)
      gluQuadricOrientation (v, GLU_OUTSIDE);
   gluSphere ( v, 0.05, 10, 8);

			      // Exclui a quadric
   gluDeleteQuadric ( v ); 
     
   glPopMatrix();
   
   //marte
   glPushMatrix();
   
   glRotatef ((GLfloat) translacao_ma, 0.0, 1.0, 0.0);
   glTranslatef(5.0, 0.0, 0.0);
   glRotatef ((GLfloat) rotacao_ma, 0.0, 1.0, 0.0);
   
   glBindTexture(GL_TEXTURE_2D, texturaId[4]);
   GLUquadricObj* ma = gluNewQuadric ( );
			      // Define o estilo de desenho  - GLU_FILL -> renderização das quádricas como polígonos primitivos
   gluQuadricDrawStyle ( ma, GLU_FILL );

			      // Define os vetores normas da Quadric - GLU_SMOOTH -> Uma normal para cada vértice
   gluQuadricNormals ( ma, GLU_SMOOTH );
			      // Habilita textura na Quadric
   gluQuadricTexture ( ma, GL_TRUE );

			      // Desenha esfera com a Quadric - (lHori e lVert - corresponde a quantidade de meridianos e paralelos)
      gluQuadricOrientation (ma, GLU_OUTSIDE);
   gluSphere ( ma, 0.08, 10, 8);

			      // Exclui a quadric
   gluDeleteQuadric ( ma );
   glPopMatrix();
   
   //jupiter
   glPushMatrix();
   
   glRotatef ((GLfloat) translacao_j, 0.0, 1.0, 0.0);
   glTranslatef(6.0, 0.0, 0.0);
   glRotatef ((GLfloat) rotacao_j, 0.0, 1.0, 0.0);
  
   glBindTexture(GL_TEXTURE_2D, texturaId[5]);
   GLUquadricObj* j = gluNewQuadric ( );
			      // Define o estilo de desenho  - GLU_FILL -> renderização das quádricas como polígonos primitivos
   gluQuadricDrawStyle ( j, GLU_FILL );

			      // Define os vetores normas da Quadric - GLU_SMOOTH -> Uma normal para cada vértice
   gluQuadricNormals ( j, GLU_SMOOTH );
			      // Habilita textura na Quadric
   gluQuadricTexture ( j, GL_TRUE );

			      // Desenha esfera com a Quadric - (lHori e lVert - corresponde a quantidade de meridianos e paralelos)
      gluQuadricOrientation (j, GLU_OUTSIDE);
   gluSphere ( j, 0.329, 10, 8);

			      // Exclui a quadric
   gluDeleteQuadric ( j );
   
   glPopMatrix();

  //saturno
   glPushMatrix();
   
   glRotatef ((GLfloat) translacao_s, 0.0, 1.0, 0.0);
   glTranslatef(7.00, 0.0, 0.0);
   glRotatef ((GLfloat) rotacao_s, 0.0, 1.0, 0.0);
   glBindTexture(GL_TEXTURE_2D, texturaId[6]);
   GLUquadricObj* s = gluNewQuadric ( );
			      // Define o estilo de desenho  - GLU_FILL -> renderização das quádricas como polígonos primitivos
   gluQuadricDrawStyle ( s, GLU_FILL );

			      // Define os vetores normas da Quadric - GLU_SMOOTH -> Uma normal para cada vértice
   gluQuadricNormals ( s, GLU_SMOOTH );
			      // Habilita textura na Quadric
   gluQuadricTexture ( s, GL_TRUE );

			      // Desenha esfera com a Quadric - (lHori e lVert - corresponde a quantidade de meridianos e paralelos)
      gluQuadricOrientation (s, GLU_OUTSIDE);
   gluSphere ( s, 0.274, 10, 8);

			      // Exclui a quadric
   gluDeleteQuadric ( s );
   glPopMatrix();

   //urano
   glPushMatrix();
   
   glRotatef ((GLfloat) translacao_u, 0.0, 1.0, 0.0);
   glTranslatef(7.5, 0.0, 0.0);
   glRotatef ((GLfloat) rotacao_u, 0.0, 1.0, 0.0);
   glBindTexture(GL_TEXTURE_2D, texturaId[8]);
   GLUquadricObj* u = gluNewQuadric ( );
			      // Define o estilo de desenho  - GLU_FILL -> renderização das quádricas como polígonos primitivos
   gluQuadricDrawStyle ( u, GLU_FILL );

			      // Define os vetores normas da Quadric - GLU_SMOOTH -> Uma normal para cada vértice
   gluQuadricNormals ( u, GLU_SMOOTH );
			      // Habilita textura na Quadric
   gluQuadricTexture ( u, GL_TRUE );

			      // Desenha esfera com a Quadric - (lHori e lVert - corresponde a quantidade de meridianos e paralelos)
      gluQuadricOrientation (u, GLU_OUTSIDE);
   gluSphere ( u, 0.121, 10, 8);

			      // Exclui a quadric
   gluDeleteQuadric ( u );
   glPopMatrix();

   //netuno
   glPushMatrix();
   
   glRotatef ((GLfloat) translacao_n, 0.0, 1.0, 0.0);
   glTranslatef(8.0, 0.0, 0.0);
   glRotatef ((GLfloat) rotacao_n, 0.0, 1.0, 0.0);
   glBindTexture(GL_TEXTURE_2D, texturaId[9]);
   GLUquadricObj* n = gluNewQuadric ( );
			      // Define o estilo de desenho  - GLU_FILL -> renderização das quádricas como polígonos primitivos
   gluQuadricDrawStyle ( n, GLU_FILL );

			      // Define os vetores normas da Quadric - GLU_SMOOTH -> Uma normal para cada vértice
   gluQuadricNormals ( n, GLU_SMOOTH );
			      // Habilita textura na Quadric
   gluQuadricTexture ( n, GL_TRUE );

			      // Desenha esfera com a Quadric - (lHori e lVert - corresponde a quantidade de meridianos e paralelos)
      gluQuadricOrientation (n, GLU_OUTSIDE);
   gluSphere ( n, 0.116, 10, 8);

			      // Exclui a quadric
   gluDeleteQuadric ( n );
   glPopMatrix();

  
   DefineIluminacao();

   glDisable(GL_TEXTURE_2D);
      
   glutSwapBuffers();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.2, 4000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(0, 0, 1080, 0, 0, 0, 0, 1, 0);
}

void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
      case 'd':
         distCamera = 10;
	 focoCamX = 0.0;
	 focoCamY = 0.0;
	 focoCamZ = 0.0;
	 offsetX = 0.0;
	 offsetZ = 0.0;
         rotacao_t = (rotacao_t + 1) % 360;
         rotacao_me = (rotacao_me + 59) % 360;
         rotacao_v = (rotacao_v + 243) % 360;
         rotacao_ma = (rotacao_ma + 1) % 360;
         rotacao_j = (rotacao_j + 0,39) % 360;
         rotacao_s = (rotacao_s + 0,42) % 360;
         rotacao_u = (rotacao_u + 0,71) % 360;
         rotacao_n = (rotacao_n + 0,67) % 360;
         glutPostRedisplay();
         break;
         
      case 'D':
         distCamera = 50;
	 focoCamX = 0.0;
	 focoCamY = 0.0;
	 focoCamZ = 0.0;
	 offsetX = 0.0;
	 offsetZ = 0.0;
         rotacao_t = (rotacao_t - 1) % 360;
         rotacao_me = (rotacao_me - 59) % 360;
         rotacao_v = (rotacao_v - 243) % 360;
         rotacao_ma = (rotacao_ma - 1) % 360;
         rotacao_j = (rotacao_j - 0,39) % 360;
         rotacao_s = (rotacao_s - 0,42) % 360;
         rotacao_u = (rotacao_u - 0,71) % 360;
         rotacao_n = (rotacao_n - 0,67) % 360; 
         glutPostRedisplay();
         break;
         
      case 'y':
         distCamera = 10;

         translacao_t = (translacao_t + 365) % 360;
         translacao_me = (translacao_me + 87) % 360;
         translacao_v = (translacao_v + 225) % 360;
         translacao_ma = (translacao_ma + 687) % 360;
         translacao_j = (translacao_j + 4380) % 360;
         translacao_s = (translacao_s + 10950) % 360;
         translacao_u = (translacao_u + 30660) % 360;
         translacao_n = (translacao_n + 59860) % 360;
         glutPostRedisplay();
         break;
         
      case 'Y':
         distCamera = 50;
	 focoCamX = 0.0;
	 focoCamY = 0.0;
	 focoCamZ = 0.0;
	 offsetX = 0.0;
	 offsetZ = 0.0;

         translacao_t = (translacao_t - 365) % 360;
         translacao_me = (translacao_me - 87) % 360;
         translacao_v = (translacao_v - 225) % 360;
         translacao_ma = (translacao_ma - 687) % 360;
         translacao_j = (translacao_j - 4380) % 360;
         translacao_s = (translacao_s - 10950) % 360;
         translacao_u = (translacao_u - 30660) % 360;
         translacao_n = (translacao_n - 59860) % 360;
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
   glutInitWindowSize (1000, 1000); 
   glutInitWindowPosition (0, 0);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMouseFunc(mousePress);
   glutMotionFunc(moveMouse);
   glutMainLoop();
   return 0;
}



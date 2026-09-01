/*
   Exemplo 3D - Movimentação simples do observador
   em câmera perspectiva - OpenGL e IUP

   Professor: Rafael Rieder
   UPF - Universidade de Passo Fundo
*/
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <iup.h>
#include <iupgl.h>
#include <iupkey.h>

using namespace std;

// Variáveis globais para controle do observador
float obsX, obsY, obsZ;

// Declaração do Protótipo
void EspecificaParametrosVisualizacao(float AspectRatio);

////// Desenho do cubo //////
static void polygon(int a, int b, int c, int d)
{
  double vertices[][3]={{-1,-1, 1}, {-1, 1, 1}, { 1, 1, 1}, { 1,-1, 1},
                        {-1,-1,-1}, {-1, 1,-1}, { 1, 1,-1}, { 1,-1,-1}};
  glBegin(GL_POLYGON);
    glVertex3dv(vertices[a]);
    glVertex3dv(vertices[b]);
    glVertex3dv(vertices[c]);
    glVertex3dv(vertices[d]);
  glEnd();
}

static void desenhaCuboColorido()
{
  glColor3f(1,0,0);
  glNormal3f(1,0,0);
  polygon(2,3,7,6);

  glColor3f(0,1,0);
  glNormal3f(0,1,0);
  polygon(1,2,6,5);

  glColor3f(0,0,1);
  glNormal3f(0,0,1);
  polygon(0,3,2,1);

  glColor3f(1,0,1);
  glNormal3f(0,-1,0);
  polygon(3,0,4,7);

  glColor3f(1,1,0);
  glNormal3f(0,0,-1);
  polygon(4,5,6,7);

  glColor3f(0,1,1);
  glNormal3f(-1,0,0);
  polygon(5,4,0,1);
}
/////////////////////////////

static int teclado(Ihandle *ih, int key, int pressed)
{
  int w, h;
  char *size = IupGetAttribute(ih, "RASTERSIZE");
  sscanf(size, "%dx%d", &w, &h);

  if(pressed) {
    switch(key) {
      case K_UP:
        obsZ -= 0.1;
        break;
      case K_DOWN:
        obsZ += 0.1;
        break;
      case K_LEFT:
        obsX -= 0.1;
        break;
      case K_RIGHT:
        obsX += 0.1;
        break;
      case K_PGDN:
        obsY -= 0.1;
        break;
      case K_PGUP:
        obsY += 0.1;
        break;
      // Reset initial position
      case K_R:
      case K_r:
        obsX = IupGetFloat(NULL, "obsX_INI");
        obsY = IupGetFloat(NULL, "obsY_INI");
        obsZ = IupGetFloat(NULL, "obsZ_INI");
        break;
      case K_ESC:
        IupDestroy(ih);
        return IUP_CLOSE;
    }
  }
  EspecificaParametrosVisualizacao((GLfloat)w/(GLfloat)h);
  IupRedraw(ih, 0);
  return IUP_DEFAULT;
}

static int desenha(Ihandle *ih, float posx, float posy)
{
  // Ativa o canvas para o contexto OpenGL
  IupGLMakeCurrent(ih);

  glClear(GL_COLOR_BUFFER_BIT);
  // glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // Desenha Cubo Wireframe
  desenhaCuboColorido();

  glFlush();
  // IupGLSwapBuffers(ih);  // Atualiza todos os buffers

  (void)posx;
  (void)posy;
  return IUP_DEFAULT;
}

// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao(float AspectRatio)
{
	// Especifica sistema de coordenadas de projeção
	glMatrixMode(GL_PROJECTION);
	// Inicializa sistema de coordenadas de projeção
	glLoadIdentity();

  // Define o modo de projeção perspectiva
  gluPerspective(60, AspectRatio, 0.01, 200);

	// Especifica sistema de coordenadas do modelo
	glMatrixMode(GL_MODELVIEW);
	// Inicializa sistema de coordenadas do modelo
	glLoadIdentity();

	// Especifica posição do observador, do alvo e da câmera
	gluLookAt(obsX, obsY, obsZ,  // Posição do observador
	          0, 0, 0,  // Posição do alvo
			      0, 1, 0); // Lado de cima da câmera

  cout << "Posicao do observador (x, y, z) = (" << obsX << ", " <<
       obsY << ", " << obsZ << ")" << endl;
}

static int redimensiona(Ihandle *ih, int w, int h)
{
  float fAspect;

  // Evita a divisao por zero
  if(h == 0) h = 1;

  // Especifica as dimensões da Viewport
  glViewport(0, 0, w, h);

	// Calcula a correção de aspecto
	fAspect = (GLfloat)w/(GLfloat)h;

	EspecificaParametrosVisualizacao(fAspect);

  return IUP_DEFAULT;
}

static void inicializa(Ihandle *ih)
{
  int w, h;
  char *size = IupGetAttribute(ih, "RASTERSIZE");
  sscanf(size, "%dx%d", &w, &h);

  // Inicializa posição do observador para (0, 4, 10)
  obsX = 0.0f;
  obsY = 4.0f;
  obsZ = 10.0f;
  IupSetFloat(NULL, "obsX_INI", obsX);
  IupSetFloat(NULL, "obsY_INI", obsY);
  IupSetFloat(NULL, "obsZ_INI", obsZ);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // glEnable(GL_DEPTH_TEST);  // Habilita o buffer de profundidade

  redimensiona(ih, w, h);
}

int main(int argc, char* argv[])
{
  Ihandle *dlg, *canvas, *box;

  IupOpen(&argc, &argv);
  IupGLCanvasOpen();

  box = IupVbox(NULL);
  IupSetAttribute(box, "MARGIN", "5x5");

  canvas = IupGLCanvas(NULL);
  IupSetHandle("MeuCanvas", canvas);

  // Define o modo de operação (cores RGBA e single)
  IupSetAttribute(canvas, "COLOR", "RGBA");
  IupSetAttribute(canvas, "BUFFER", "SINGLE");
  // IupSetAttribute(canvas, "BUFFER", "DOUBLE");

  IupSetAttribute(canvas, "BORDER", "NO");
  IupSetAttribute(canvas, "RASTERSIZE", "800x600");

  // Registra a callback para redesenho da janela de visualização
  IupSetCallback(canvas, "ACTION",      (Icallback)desenha);
  // Registra a callback para tratamento das teclas
  IupSetCallback(canvas, "KEYPRESS_CB", (Icallback)teclado);
  // Registra a callback para tratamento dos botões do mouse
  IupSetCallback(canvas, "RESIZE_CB", (Icallback)redimensiona);

  IupAppend(box, canvas);

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupGL Animação de um cubo 3D");
  // Define o uso de caracteres especiais/específicos para elementos IUP
  IupSetGlobal("UTF8MODE", "YES");

  IupMap(dlg);

  IupGLMakeCurrent(canvas);

  inicializa(canvas);  // define a janela de visualização 2D

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  // Ativa o foco de teclado para o canvas
  IupSetFocus(canvas);

  IupMainLoop();

  IupClose();

  return 0;
}

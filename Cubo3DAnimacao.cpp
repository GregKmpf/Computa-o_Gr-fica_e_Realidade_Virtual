/*
   Exemplo - Animação 3D usando Timer - OpenGL e IUP
   Desenha um cubo casa que se move até colidir em uma
   das extremidades da janela de diálogo, voltando
   a se mover em outra direção.

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

// Variável global para rotação do cubo em Y
float angulo = 0.0f;
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

static void desenhaCuboWireframe()
{
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glRotatef(angulo, 0, 1, 0);
  // Linhas vermelhas
  glColor3f(1,0,0);
  // Face 1
  glNormal3f(1,0,0);
  polygon(2,3,7,6);
  // Face 2
  glNormal3f(0,1,0);
  polygon(1,2,6,5);
  // Face 3
  glNormal3f(0,0,1);
  polygon(0,3,2,1);
  // Face 4
  glNormal3f(0,-1,0);
  polygon(3,0,4,7);
  // Face 5
  glNormal3f(0,0,-1);
  polygon(4,5,6,7);
  // Face 6
  glNormal3f(-1,0,0);
  polygon(5,4,0,1);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
/////////////////////////////

static int anima(Ihandle *ih)
{
	// Muda o angulo de rotação, e se chegar a 360, passa para zero
	if( ++angulo > 360.0f )
		angulo = 0.0f;

  // Redesenha a casinha em outra posição
  IupRedraw(IupGetHandle("MeuCanvas"), 0);  // força o redesenho
  return IUP_DEFAULT;
}

static int teclado(Ihandle *ih, int key, int pressed)
{
  switch(key) {
    case K_ESC:
      IupDestroy(ih);
      return IUP_CLOSE;
  }
  return IUP_DEFAULT;
}

static int desenha(Ihandle *ih, float posx, float posy)
{
  // Ativa o canvas para o contexto OpenGL
  IupGLMakeCurrent(ih);

  glClear(GL_COLOR_BUFFER_BIT);

  // Desenha Cubo Wireframe
  desenhaCuboWireframe();

  glFlush();
  // Habilita o buffer de fundo visível
  // Necessária quando DOUBLE buffer é usado
  // No Linux também precisa ser habilitado.
  // IupGLSwapBuffers(ih);

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
	gluLookAt(0, 5, 10,  // Posição do observador
	          0, 0, 0,  // Posição do alvo
			      0, 1, 0); // Lado de cima da câmera
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

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  redimensiona(ih, w, h);
}

int main(int argc, char* argv[])
{
  Ihandle *dlg, *canvas, *box, *MeuTimer;

  IupOpen(&argc, &argv);
  IupGLCanvasOpen();

  MeuTimer = IupTimer();
  IupSetAttribute(MeuTimer, "TIME", "60");
  IupSetAttribute(MeuTimer, "RUN", "YES");
  IupSetCallback(MeuTimer, "ACTION_CB", (Icallback)anima);

  box = IupVbox(NULL);
  IupSetAttribute(box, "MARGIN", "5x5");

  canvas = IupGLCanvas(NULL);
  IupSetHandle("MeuCanvas", canvas);

  // Define o modo de operação (cores RGBA e single)
  IupSetAttribute(canvas, "COLOR", "RGBA");
  IupSetAttribute(canvas, "BUFFER", "SINGLE");

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

  // Timers devem ser destruídos manualmente
  IupDestroy(MeuTimer);

  IupClose();

  return 0;
}

/*
   Exemplo de uma primeira aplicação 2D usando OpenGL e IUP
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

void desenhaCirculo(Ihandle *ih)
{
  GLfloat ang;
  // Define a cor de desenho: Preto (RGB)
  glColor3f(0.0f, 0.0f, 0.0f);

  // Desenha um círculo de linhas conectadas
  glBegin(GL_LINE_LOOP);
    for(ang = 0; ang < 2 * 3.14f; ang += 3.14f / 7.0f)
      glVertex2f(0.9f * cos(ang), 0.6f * sin(ang));
  glEnd();

  // Executa os comandos OpenGL
  glFlush();

  (void)ih;
}

static int mouse(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  if(button == IUP_BUTTON1 && pressed)
  {
    double w, h;
    double xGL, yGL;
    char *str;

    /* Subdividindo uma string */
    str = strtok(IupGetAttribute(ih, "RASTERSIZE"), "x");
    w = atof(str);
    str = strtok(NULL, "\n");
    h = atof(str);

    /*  Convertendo para o espaço definido em gluOrtho2D!!!

    GL Window (in units)   GL Size (in pixels)
     2 -+-                 --- w
        |                   |
     0 -+-                 -+- w/2
        |                   |
    -2 -+-                 --- 0

    Razao => w / (2 - (-2)) => w / 4
    */
    xGL = (x - (w / 2)) / (w / 4);
    yGL = (y - (h / 2)) / (h / 4) * -1;  // Y invertido

    glPushMatrix();
      glPointSize(5);
      glColor3f(1.0, 1.0, 0.0);
      glBegin(GL_POINTS);
        glVertex2d(xGL, yGL);
      glEnd();
    glPopMatrix();

    glFlush();
  }

  (void)status;
  return IUP_DEFAULT;
}

static int teclado(Ihandle *ih, int key, int pressed)
{
  if (pressed)
  {
    switch(key)
    {
      case K_c:
      case K_C:
        /* Desenha Circulo sobre o desenho atual */
        desenhaCirculo(ih);
        break;
      case K_a:
      case K_A:
        /* Apaga Circulo - redesenha a chamada principal */
        IupRedraw(ih, 0);  // chamará a callback "desenha" para atualizar os modelos
        break;
      case K_ESC:
        IupDestroy(ih);
        return IUP_CLOSE;
    }
  }

  return IUP_DEFAULT;
}

static int desenha(Ihandle *ih, float posx, float posy)
{
  // Ativa o canvas para o contexto OpenGL
  IupGLMakeCurrent(ih);

  // Limpa a janela de visualização com a cor branca
  glClear(GL_COLOR_BUFFER_BIT);

  // Define a cor de desenho: Vermelho (RGB)
  glColor3f(1.0f, 0.0f, 0.0f);

  // Desenha um quadrado preenchido com a cor corrente
  glBegin(GL_QUADS);
    glVertex3f(0.5f, -0.5f, 0);
    glVertex3f(1.0f, -0.5f, 0);
    // Especifica que a cor corrente agora é azul
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0);
    glVertex3f(0.5f, -1.0f, 0);
  glEnd();

  // Define a cor de desenho: Verde (RGB)
  glColor3f(0.0f, 1.0f, 0.0f);

  // Desenha um triângulo no centro da janela
  glBegin(GL_TRIANGLES);
    glVertex3f(-0.5f, -0.5f, 0);
    glVertex3f( 0.0f,  0.5f, 0);
    glVertex3f( 0.5f, -0.5f, 0);
  glEnd();

  // Define a cor de desenho: Lilás
  glColor3f(0.7f, 0.0f, 1.0f);

  // Desenha um pequeno retângulo
  glBegin(GL_QUADS);
    glVertex3f(-1.7f, 1.7f, 0);
    glVertex3f(-1.7f, 1.9f, 0);
    glVertex3f(-1.4f, 1.9f, 0);
    glVertex3f(-1.4f, 1.7f, 0);
  glEnd();

  // Executa os comandos OpenGL em modo Single Buffer
  glFlush();

  // Habilita o buffer de fundo visível
  // Necessária quando DOUBLE buffer é usado
  // No Linux também precisa ser habilitado.
  // IupGLSwapBuffers(ih);

  (void)posx;
  (void)posy;

  return IUP_DEFAULT;
}

static void inicializa(void)
{
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(-2.0, 2.0, -2.0, 2.0);
  glMatrixMode(GL_MODELVIEW);
  glClearColor(1, 1, 1, 0);  // Cor de fundo: Branca
}

int main(int argc, char* argv[])
{
  Ihandle *dlg, *canvas, *box;

  IupOpen(&argc, &argv);
  IupGLCanvasOpen();

  box = IupVbox(NULL);
  IupSetAttribute(box, "MARGIN", "5x5");

  canvas = IupGLCanvas(NULL);

  // Define o modo de operação (cores RGBA e single)
  IupSetAttribute(canvas, "COLOR", "RGBA");
  IupSetAttribute(canvas, "BUFFER", "SINGLE");

  IupSetAttribute(canvas, "BORDER", "NO");
  IupSetAttribute(canvas, "RASTERSIZE", "300x200");

  // Registra a callback para redesenho da janela de visualização
  IupSetCallback(canvas, "ACTION",      (Icallback)desenha);
  // Registra a callback para tratamento das teclas
  IupSetCallback(canvas, "KEYPRESS_CB", (Icallback)teclado);
  // Registra a callback para tratamento dos botões do mouse
  IupSetCallback(canvas, "BUTTON_CB", (Icallback)mouse);

  IupAppend(box, canvas);

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupGL Exemplo 1");
  // IupSetAttribute(dlg, "PLACEMENT", "MAXIMIZED");

  IupMap(dlg);

  IupGLMakeCurrent(canvas);

  inicializa();  // define a janela de visualização 2D

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();
  IupClose();

  return 0;
}

/*
   Exemplo de uma primeira aplicação 2D usando OpenGL e IUP
   Versão simplificada (só desenha um retângulo na viewport)
   Professor: Rafael Rieder
   UPF - Universidade de Passo Fundo
*/
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupgl.h"
#include "iupkey.h"

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

  // Limpa a janela de visualização com a cor branca
  glClear(GL_COLOR_BUFFER_BIT);

  // Define a cor de desenho: Azul (RGB)
  glColor3f(0.0f, 0.0f, 1.0f);

  // Desenha um retângulo preenchido com a cor corrente
  glBegin(GL_QUADS);
    glVertex3f(0.5f, -0.5f, 0);
    glVertex3f(2.0f, -0.5f, 0);
    glVertex3f(2.0f, -2.0f, 0);
    glVertex3f(0.5f, -2.0f, 0);
  glEnd();

  // Executa os comandos OpenGL em modo Single Buffer
  glFlush();

  (void)posx;
  (void)posy;

  return IUP_DEFAULT;
}

static void inicializa(void)
{
  // Inicia uma projeção ortogonal
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(-2.0, 2.0, -2.0, 2.0);
  glMatrixMode(GL_MODELVIEW);
  glClearColor(1, 1, 1, 0);  // Cor de fundo: Branca
}

int main(int argc, char* argv[])
{
  Ihandle *dlg, *canvas, *box;

  IupOpen(&argc, &argv);  // Inicia a IUP
  IupGLCanvasOpen();      // Habilita a OpenGL para desenhar no canvas

  box = IupVbox(NULL);    // Cria um container (organização vertical)
  IupSetAttribute(box, "MARGIN", "5x5");

  canvas = IupGLCanvas(NULL);  // Cria uma área de desenho (canvas)

  // Define o modo de operação (cores RGBA e single)
  IupSetAttribute(canvas, "COLOR", "RGBA");
  IupSetAttribute(canvas, "BUFFER", "SINGLE");

  IupSetAttribute(canvas, "BORDER", "NO");
  IupSetAttribute(canvas, "RASTERSIZE", "300x200");

  // Registra a callback para redesenho da janela de visualização
  IupSetCallback(canvas, "ACTION",      (Icallback)desenha);
  // Registra a callback para tratamento das teclas
  IupSetCallback(canvas, "KEYPRESS_CB", (Icallback)teclado);

  IupAppend(box, canvas);  // insere um elemento de interface no final do container

  dlg = IupDialog(box);  // cria uma janela de diálogo, com o container como filho
  IupSetAttribute(dlg, "TITLE", "IupGL Exemplo 1");

  IupMap(dlg);  // Mapeia os elementos de interface nativos

  IupGLMakeCurrent(canvas);  // Transforma o canvas para suporte OpenGL

  inicializa();  // define a janela de visualização 2D

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);  // Apresenta o diálogo

  IupMainLoop();  // Gerencia o controle de execução do programa até um r
  IupClose();  // Finaliza a IUP

  return 0;
}

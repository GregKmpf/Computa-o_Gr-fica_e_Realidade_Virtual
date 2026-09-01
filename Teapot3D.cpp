/*
   Exemplo de aplicação 3D com DOUBLE BUFFER
   usando OpenGL e IUP
   **** TEAPOT ****
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

static Ihandle *canvas;

//////////////////////////////////////////////////////////////////////
// Base source: https://github.com/openscenegraph/OpenSceneGraph/blob/master/examples/osgteapot/osgteapot.cpp
// The classic OpenGL teapot... taken form glut-3.7/lib/glut/glut_teapot.c
// Copyright (c) Mark J. Kilgard, OpenGL 1994 (Martin Newell's original teapot)

// History: https://www.sjbaker.org/wiki/index.php?title=The_History_of_The_Teapot

/* Rim, body, lid, and bottom data must be reflected in x and
   y; handle and spout data across the y axis only.  */
static int patchdata[][16] =
{
  /* rim */
  {102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11,
    12, 13, 14, 15},
  /* body */
  {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27},
  {24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36,
    37, 38, 39, 40},
  /* lid */
  {96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101,
    101, 0, 1, 2, 3,},
  {0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112,
    113, 114, 115, 116, 117},
  /* bottom */
  {118, 118, 118, 118, 124, 122, 119, 121, 123, 126,
    125, 120, 40, 39, 38, 37},
  /* handle */
  {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
    53, 54, 55, 56},
  {53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
    28, 65, 66, 67},
  /* spout */
  {68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83},
  {80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
    92, 93, 94, 95}
};

/* Vertices */
static float cpdata[][3] =
{
    {0.2, 0, 2.7}, {0.2, -0.112, 2.7}, {0.112, -0.2, 2.7}, {0,
    -0.2, 2.7}, {1.3375, 0, 2.53125}, {1.3375, -0.749, 2.53125},
    {0.749, -1.3375, 2.53125}, {0, -1.3375, 2.53125}, {1.4375,
    0, 2.53125}, {1.4375, -0.805, 2.53125}, {0.805, -1.4375,
    2.53125}, {0, -1.4375, 2.53125}, {1.5, 0, 2.4}, {1.5, -0.84,
    2.4}, {0.84, -1.5, 2.4}, {0, -1.5, 2.4}, {1.75, 0, 1.875},
    {1.75, -0.98, 1.875}, {0.98, -1.75, 1.875}, {0, -1.75,
    1.875}, {2, 0, 1.35}, {2, -1.12, 1.35}, {1.12, -2, 1.35},
    {0, -2, 1.35}, {2, 0, 0.9}, {2, -1.12, 0.9}, {1.12, -2,
    0.9}, {0, -2, 0.9}, {-2, 0, 0.9}, {2, 0, 0.45}, {2, -1.12,
    0.45}, {1.12, -2, 0.45}, {0, -2, 0.45}, {1.5, 0, 0.225},
    {1.5, -0.84, 0.225}, {0.84, -1.5, 0.225}, {0, -1.5, 0.225},
    {1.5, 0, 0.15}, {1.5, -0.84, 0.15}, {0.84, -1.5, 0.15}, {0,
    -1.5, 0.15}, {-1.6, 0, 2.025}, {-1.6, -0.3, 2.025}, {-1.5,
    -0.3, 2.25}, {-1.5, 0, 2.25}, {-2.3, 0, 2.025}, {-2.3, -0.3,
    2.025}, {-2.5, -0.3, 2.25}, {-2.5, 0, 2.25}, {-2.7, 0,
    2.025}, {-2.7, -0.3, 2.025}, {-3, -0.3, 2.25}, {-3, 0,
    2.25}, {-2.7, 0, 1.8}, {-2.7, -0.3, 1.8}, {-3, -0.3, 1.8},
    {-3, 0, 1.8}, {-2.7, 0, 1.575}, {-2.7, -0.3, 1.575}, {-3,
    -0.3, 1.35}, {-3, 0, 1.35}, {-2.5, 0, 1.125}, {-2.5, -0.3,
    1.125}, {-2.65, -0.3, 0.9375}, {-2.65, 0, 0.9375}, {-2,
    -0.3, 0.9}, {-1.9, -0.3, 0.6}, {-1.9, 0, 0.6}, {1.7, 0,
    1.425}, {1.7, -0.66, 1.425}, {1.7, -0.66, 0.6}, {1.7, 0,
    0.6}, {2.6, 0, 1.425}, {2.6, -0.66, 1.425}, {3.1, -0.66,
    0.825}, {3.1, 0, 0.825}, {2.3, 0, 2.1}, {2.3, -0.25, 2.1},
    {2.4, -0.25, 2.025}, {2.4, 0, 2.025}, {2.7, 0, 2.4}, {2.7,
    -0.25, 2.4}, {3.3, -0.25, 2.4}, {3.3, 0, 2.4}, {2.8, 0,
    2.475}, {2.8, -0.25, 2.475}, {3.525, -0.25, 2.49375},
    {3.525, 0, 2.49375}, {2.9, 0, 2.475}, {2.9, -0.15, 2.475},
    {3.45, -0.15, 2.5125}, {3.45, 0, 2.5125}, {2.8, 0, 2.4},
    {2.8, -0.15, 2.4}, {3.2, -0.15, 2.4}, {3.2, 0, 2.4}, {0, 0,
    3.15}, {0.8, 0, 3.15}, {0.8, -0.45, 3.15}, {0.45, -0.8,
    3.15}, {0, -0.8, 3.15}, {0, 0, 2.85}, {1.4, 0, 2.4}, {1.4,
    -0.784, 2.4}, {0.784, -1.4, 2.4}, {0, -1.4, 2.4}, {0.4, 0,
    2.55}, {0.4, -0.224, 2.55}, {0.224, -0.4, 2.55}, {0, -0.4,
    2.55}, {1.3, 0, 2.55}, {1.3, -0.728, 2.55}, {0.728, -1.3,
    2.55}, {0, -1.3, 2.55}, {1.3, 0, 2.4}, {1.3, -0.728, 2.4},
    {0.728, -1.3, 2.4}, {0, -1.3, 2.4}, {0, 0, 0}, {1.425,
    -0.798, 0}, {1.5, 0, 0.075}, {1.425, 0, 0}, {0.798, -1.425,
    0}, {0, -1.5, 0.075}, {0, -1.425, 0}, {1.5, -0.84, 0.075},
    {0.84, -1.5, 0.075}
};

static void teapot(GLint grid, GLenum type)
{
  float p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];
  long i, j, k, l;

  glPushAttrib(GL_ENABLE_BIT | GL_EVAL_BIT);
  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_MAP2_VERTEX_3);

  for (i = 0; i < 10; i++) {
    for (j = 0; j < 4; j++) {
      for (k = 0; k < 4; k++) {
        for (l = 0; l < 3; l++) {
          p[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
          q[j][k][l] = cpdata[patchdata[i][j * 4 + (3 - k)]][l];
          if (l == 1)
            q[j][k][l] *= -1.0;
          if (i < 6) {
            r[j][k][l] =
              cpdata[patchdata[i][j * 4 + (3 - k)]][l];
            if (l == 0)
              r[j][k][l] *= -1.0;
            s[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
            if (l == 0)
              s[j][k][l] *= -1.0;
            if (l == 1)
              s[j][k][l] *= -1.0;
          }
        }
      }
    }

    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &p[0][0][0]);
    glMapGrid2f(grid, 0.0, 1.0, grid, 0.0, 1.0);
    glEvalMesh2(type, 0, grid, 0, grid);
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &q[0][0][0]);
    glEvalMesh2(type, 0, grid, 0, grid);
    if (i < 6) {
      glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &r[0][0][0]);
      glEvalMesh2(type, 0, grid, 0, grid);
      glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &s[0][0][0]);
      glEvalMesh2(type, 0, grid, 0, grid);
    }
  }
  glPopAttrib();
}
//////////////////////////////////////////////////////////////////////


/* function called when the canvas is exposed in the screen */
static int desenha(Ihandle *self)
{
  IupGLMakeCurrent(self);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  glPushMatrix();
    glRotated(-90, 1, 0, 0);
    glPushMatrix();
      glColor3f(0.0f, 0.0f, 1.0f);
      glTranslated(-4, 0, -1);
      teapot(15, GL_FILL);
    glPopMatrix();
    glPushMatrix();
      glColor3f(1.0f, 1.0f, 0.0f);
      glTranslated(4, 0, -1);
      teapot(15, GL_LINE);
    glPopMatrix();
  glPopMatrix();

  IupGLSwapBuffers(self);  /* change the back buffer with the front buffer */

  return IUP_DEFAULT; /* returns the control to the main loop */
}

/* function called in the event of changes in the width or in the height of the canvas */
static int redimensiona(Ihandle *self, int width, int height)
{
  IupGLMakeCurrent(self);  /* Make the canvas current in OpenGL */

  /* define the entire canvas as the viewport  */
  glViewport(0, 0, width, height);

  // Especifica sistema de coordenadas de proje��o
  glMatrixMode(GL_PROJECTION);
  // Inicializa sistema de coordenadas de proje��o
  glLoadIdentity();

  // Especifica a proje��o perspectiva(angulo,aspecto,zMin,zMax)
  gluPerspective(60, (GLfloat)width/(GLfloat)height, 0.01, 200);

  // Especifica sistema de coordenadas do modelo
  glMatrixMode(GL_MODELVIEW);
  // Inicializa sistema de coordenadas do modelo
  glLoadIdentity();

  // Especifica posi��o do observador e do alvo
  gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);

  return IUP_DEFAULT; /* return to the IUP main loop */
}

int exit_cb(void)
{
   printf("Function to free memory and do finalizations...\n");
   return IUP_CLOSE;
}

/*-------------------------------------------------------------------------*/
/* Inicializa o programa.                                                   */
/*-------------------------------------------------------------------------*/
Ihandle* initDialog(void)
{
  Ihandle* dialog;   /* dialog containing the canvas */

  canvas = IupGLCanvas(NULL); /* create _canvas */

  IupSetAttribute(canvas,"RASTERSIZE","800x600");   /* define the initial size in pixels */
  IupSetAttribute(canvas, "BUFFER","DOUBLE");      /* define that this OpenGL _canvas has double buffer (front and back) */
  IupSetAttribute(canvas, "DEPTH_SIZE", "16");

  /* bind callback actions with callback functions */
  IupSetCallback(canvas, "ACTION", (Icallback) desenha);
  IupSetCallback(canvas, "RESIZE_CB",(Icallback) redimensiona);

  /* create the dialog and set its attributes */
  dialog = IupDialog(canvas);
  IupSetAttribute(dialog, "TITLE", "IUP 3D OpenGL TEAPOT");

  IupSetCallback(dialog, "CLOSE_CB", (Icallback) exit_cb);

  return dialog;
}

/*-----------------------*/
/* Main function.        */
/*-----------------------*/
int main(int argc, char* argv[])
{
  Ihandle* dialog;

  IupOpen(&argc, &argv);                     /* opens the IUP lib */
  IupGLCanvasOpen();                         /* enable the use of OpenGL to draw in canvas */

  dialog = initDialog();                     /* local function to create a dialog with buttons and canvas */
  IupShowXY(dialog, IUP_CENTER, IUP_CENTER); /* shows dialog in the center of screen */

  IupMainLoop();                             /* handle the program control to the IUP lib until a return IUP_CLOSE */

  IupClose();                                /* closes the IUP lib */
}

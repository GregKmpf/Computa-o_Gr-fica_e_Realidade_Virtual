/*
   Exemplo 3D - OpenGL e IUP
   Desenha o wireframe de uma pedra lapidada (cubo central e duas pirâmides nas pontas)
   usando structs (geometria e topologia).

   Professor: Rafael Rieder
   UPF - Universidade de Passo Fundo
*/
#include <GL/gl.h>
#include <GL/glu.h>
//h
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <iup.h>
#include <iupgl.h>
#include <iupkey.h>

using namespace std;

// Declaração de Protótipos
void EspecificaParametrosVisualizacao(float AspectRatio);
void PosicionaObservador(void);

// Variáveis para controles de navegação
GLfloat angle;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY, obsZ, obsX_ini, obsY_ini, obsZ_ini;
int x_ini, y_ini, bot;

// Define um vértice
typedef struct {
  float x, y, z;  // posição no espaço
} VERT;

// Define uma face
typedef struct {
  int total;   // total de vértices
  int ind[4];  // índices para o vetor de vértices
} FACE;

// Define um objeto 3D
typedef struct {
  VERT *verticesModel;   // aponta para os vértices
  FACE *facesModel;      // aponta para as faces
  int total_faces;       // total de faces no objeto
} OBJ;

// ============================================================================
// OBJETO 1: CUBO CENTRAL
// ============================================================================
VERT verticesCubo[] = {
  { -1.0f, -1.0f, -1.0f }, // 0: canto inf esquerdo tras.
  {  1.0f, -1.0f, -1.0f }, // 1: canto inf direito tras.
  {  1.0f,  1.0f, -1.0f }, // 2: canto sup direito tras.
  { -1.0f,  1.0f, -1.0f }, // 3: canto sup esquerdo tras.
  { -1.0f, -1.0f,  1.0f }, // 4: canto inf esquerdo diant.
  {  1.0f, -1.0f,  1.0f }, // 5: canto inf direito diant.
  {  1.0f,  1.0f,  1.0f }, // 6: canto sup direito diant.
  { -1.0f,  1.0f,  1.0f }  // 7: canto sup esquerdo diant.
};

FACE facesCubo[] = {
  { 4, { 0, 1, 2, 3 } }, // tras
  { 4, { 4, 5, 6, 7 } }, // frente
  { 4, { 0, 4, 7, 3 } }, // esquerda
  { 4, { 1, 5, 6, 2 } }, // direita
  { 4, { 3, 2, 6, 7 } }, // topo
  { 4, { 0, 1, 5, 4 } }  // base
};

OBJ cubo = { verticesCubo, facesCubo, 6 };

// ============================================================================
// OBJETO 2: PIRÂMIDE ESQUERDA
// ============================================================================
VERT verticesPiramideEsquerda[] = {
  { -1.0f, -1.0f, -1.0f }, // 0
  { -1.0f,  1.0f, -1.0f }, // 1
  { -1.0f,  1.0f,  1.0f }, // 2
  { -1.0f, -1.0f,  1.0f }, // 3
  { -2.5f,  0.0f,  0.0f }  // 4: ápice esquerdo
};

FACE facesPiramideEsquerda[] = {
  { 4, { 0, 1, 2, 3 } },   // base
  { 3, { 0, 1, 4, -1 } },
  { 3, { 1, 2, 4, -1 } },
  { 3, { 2, 3, 4, -1 } },
  { 3, { 3, 0, 4, -1 } }
};

OBJ piramideEsquerda = { verticesPiramideEsquerda, facesPiramideEsquerda, 5 };

// ============================================================================
// OBJETO 3: PIRÂMIDE DIREITA
// ============================================================================
VERT verticesPiramideDireita[] = {
  {  1.0f, -1.0f, -1.0f }, // 0
  {  1.0f,  1.0f, -1.0f }, // 1
  {  1.0f,  1.0f,  1.0f }, // 2
  {  1.0f, -1.0f,  1.0f }, // 3
  {  2.5f,  0.0f,  0.0f }  // 4: ápice direito
};

FACE facesPiramideDireita[] = {
  { 4, { 0, 1, 2, 3 } },   // base
  { 3, { 0, 1, 4, -1 } },
  { 3, { 1, 2, 4, -1 } },
  { 3, { 2, 3, 4, -1 } },
  { 3, { 3, 0, 4, -1 } }
};

OBJ piramideDireita = { verticesPiramideDireita, facesPiramideDireita, 5 };

// Desenha um objeto em wireframe
void DesenhaObjetoWireframe(OBJ *objeto)
{
  OBJ *obj = objeto;
  // Percorre todas as faces
  for(int f = 0; f < obj->total_faces; ++f)
  {
    glBegin(GL_LINE_LOOP);
    // Percorre todos os vértices da face
    for(int v = 0; v < obj->facesModel[f].total; ++v)
      glVertex3f(obj->verticesModel[obj->facesModel[f].ind[v]].x,
                 obj->verticesModel[obj->facesModel[f].ind[v]].y,
                 obj->verticesModel[obj->facesModel[f].ind[v]].z);
    glEnd();
  }
}

static int teclado(Ihandle *ih, int key, int pressed)
{
  int w, h;
  char *size = IupGetAttribute(ih, "RASTERSIZE");
  sscanf(size, "%dx%d", &w, &h);

  if(pressed) {
    switch(key) {
      /* HOME e END: alteram o ângulo utilizado na projeção perspectiva */
      case K_HOME:  // Zoom In
        if(angle>=10)
          angle -=5;
        break;
      case K_END:  // Zoom Out
        if(angle<=150)
          angle +=5;
        break;
      /* Operações de PAN (movimento o observador nos eixos x e y) */
      case K_LEFT:
        obsX +=0.5;
        break;
      case K_RIGHT:
        obsX -=0.5;
        break;        
      case K_UP:
        obsY -=0.5;
        break;
      case K_DOWN:
        obsY +=0.5;
        break;
      /* Operações de ZOOM (aproxima/afasta o observador do objeto no eixo z) */
      case K_PGUP:  // Zoom In
        obsZ -=0.5;
        break;
      case K_PGDN:  // Zoom Out
        obsZ +=0.5;
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

static int mouse(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  if(pressed) {
    x_ini = x;
    y_ini = y;
    obsX_ini = obsX;
    obsY_ini = obsY;
    obsZ_ini = obsZ;
    rotX_ini = rotX;
    rotY_ini = rotY;
    bot = button;
  }
  else {
    bot = -1;
  }

  (void)status;
  return IUP_DEFAULT;
}

#define SENS_ROT	5.0
#define SENS_OBS	15.0
#define SENS_TRANSL	30.0
static int movimenta(Ihandle *ih, int x, int y, char *status)
{
  if(bot==IUP_BUTTON1)
  {
    int deltax = x_ini - x;
    int deltay = y_ini - y;
    rotY = rotY_ini - deltax/SENS_ROT;
    rotX = rotX_ini - deltay/SENS_ROT;
  }
  else if(bot==IUP_BUTTON3)
  {
    int deltaz = y_ini - y;
    obsZ = obsZ_ini + deltaz/SENS_OBS;
  }
  else if(bot==IUP_BUTTON2)
  {
    int deltax = x_ini - x;
    int deltay = y_ini - y;
    obsX = obsX_ini + deltax/SENS_TRANSL;
    obsY = obsY_ini - deltay/SENS_TRANSL;
  }

  PosicionaObservador();
  IupRedraw(ih, 0);

  (void)status;
  return IUP_DEFAULT;
}

static int desenha(Ihandle *ih, float posx, float posy)
{
  IupGLMakeCurrent(ih);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(0.0f, 0.0f, 0.0f);

  // Desenha os 3 objetos da pedra lapidada
  DesenhaObjetoWireframe(&cubo);
  DesenhaObjetoWireframe(&piramideEsquerda);
  DesenhaObjetoWireframe(&piramideDireita);

  IupGLSwapBuffers(ih);

  (void)posx;
  (void)posy;
  return IUP_DEFAULT;
}

void PosicionaObservador(void)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glTranslatef(-obsX,-obsY,-obsZ);
  glRotatef(rotX,1,0,0);
  glRotatef(rotY,0,1,0);
}

void EspecificaParametrosVisualizacao(float AspectRatio)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(angle, AspectRatio, 0.1, 1200);

  PosicionaObservador();
}

static int redimensiona(Ihandle *ih, int w, int h)
{
  float fAspect;

  if(h == 0) h = 1;

  glViewport(0, 0, w, h);

  fAspect = (GLfloat)w/(GLfloat)h;

  EspecificaParametrosVisualizacao(fAspect);

  return IUP_DEFAULT;
}

static void inicializa(Ihandle *ih)
{
  int w, h;
  char *size = IupGetAttribute(ih, "RASTERSIZE");
  sscanf(size, "%dx%d", &w, &h);

  angle=60;

  rotX = 0;
  rotY = 0;
  obsX = obsY = 0;
  obsZ = 5;

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);

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

  IupSetAttribute(canvas, "COLOR", "RGBA");
  IupSetAttribute(canvas, "BUFFER", "DOUBLE");

  IupSetAttribute(canvas, "BORDER", "NO");
  IupSetAttribute(canvas, "RASTERSIZE", "800x600");

  IupSetCallback(canvas, "ACTION",      (Icallback)desenha);
  IupSetCallback(canvas, "KEYPRESS_CB", (Icallback)teclado);
  IupSetCallback(canvas, "RESIZE_CB", (Icallback)redimensiona);
  IupSetCallback(canvas, "BUTTON_CB", (Icallback)mouse);
  IupSetCallback(canvas, "MOTION_CB", (Icallback)movimenta);

  IupAppend(box, canvas);

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupGL Desenho Tridimensional");
  IupSetGlobal("UTF8MODE", "YES");

  IupMap(dlg);

  IupGLMakeCurrent(canvas);

  inicializa(canvas);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupSetFocus(canvas);

  IupMainLoop();

  IupClose();

  return 0;
}
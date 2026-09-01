/*
   Exemplo 3D - OpenGL e IUP
   Desenha o wireframe de uma pirâmide usando structs (geometria e topologia).
   Controla o ponto de vista do observador.

   Navegação via botões do mouse + movimento:
   - botão esquerdo: rotaciona objeto
   - botão direito:  aproxima/afasta
   - botão do meio:  translada objeto

  Teclas Home e End fazem zoom in/zoom out

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

// Declaração de Protótipos
void EspecificaParametrosVisualizacao(float AspectRatio);
void PosicionaObservador(void);

// Variáveis para controles de navegação
GLfloat angle;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY, obsZ, obsX_ini, obsY_ini, obsZ_ini;
int x_ini,y_ini,bot;

// Define um vértice
typedef struct {
  float x,y,z;  // posição no espaço
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
  int total_faces;  // total de faces no objeto
} OBJ;

// Definição dos vértices
VERT verticesModel[] = {
  { -1, 0, -1 },  // 0 canto inf esquerdo tras.
  {  1, 0, -1 },  // 1 canfo inf direito  tras.
  {  1, 0,  1 },  // 2 canto inf direito  diant.
  { -1, 0,  1 },  // 3 canto inf esquerdo diant.
  {  0, 2,  0 },  // 4 topo
};

// Definição das faces
FACE facesModel[] = {
  { 4, { 0,1,2,3 }},  // base
  { 3, { 0,1,4,-1 }},  // lado traseiro
  { 3, { 0,3,4,-1 }},  // lado esquerdo
  { 3, { 1,2,4,-1 }},  // lado direito
  { 3, { 3,2,4,-1 }}  // lado dianteiro
};

// Definição do objeto pirâmide
OBJ piramide = {
  verticesModel, facesModel, 5 };

// Desenha um objeto em wireframe
void DesenhaObjetoWireframe(OBJ *objeto)
{
  OBJ *obj = objeto;
  // Percorre todas as faces
  for(int f = 0; f < obj->total_faces; ++f)
  {
    glBegin(GL_LINE_LOOP);
    // Percorre todos os vértices da face
    for(int v=0; v < obj->facesModel[f].total; ++v)
      glVertex3f(obj->verticesModel[facesModel[f].ind[v]].x,
                 obj->verticesModel[facesModel[f].ind[v]].y,
                 obj->verticesModel[facesModel[f].ind[v]].z);
  }
  glEnd();
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
      /* Operações de ZOOM (aproxima/afasta *** o observador do objeto *** no eixo z) */
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
    // Se algum botão estiver pressionado,
    // salva os parâmetros atuais
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
  // Botão esquerdo: rotaciona o objeto em torno dos eixos x e y
  if(bot==IUP_BUTTON1)
  {
    // Calcula diferenças
    int deltax = x_ini - x;
    int deltay = y_ini - y;
    // E modifica ângulos
    rotY = rotY_ini - deltax/SENS_ROT;
    rotX = rotX_ini - deltay/SENS_ROT;
  }
  // Botão direito: aproxima/afasta o objeto do observador
  else if(bot==IUP_BUTTON3)
  {
    // Calcula diferença
    int deltaz = y_ini - y;
    // E modifica distância do observador
    obsZ = obsZ_ini + deltaz/SENS_OBS;
  }
  // Botão do meio: translação do objeto nos eixos x e y
  else if(bot==IUP_BUTTON2)
  {
    // Calcula diferenças
    int deltax = x_ini - x;
    int deltay = y_ini - y;
    // E modifica posições
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
  // Ativa o canvas para o contexto OpenGL
  IupGLMakeCurrent(ih);

  // glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Altera a cor do desenho para preto
  glColor3f(0.0f, 0.0f, 0.0f);

  // Desenha o objeto definido anteriormente: uma pirâmide
  DesenhaObjetoWireframe(&piramide);

  // glFlush();
  IupGLSwapBuffers(ih);

  (void)posx;
  (void)posy;
  return IUP_DEFAULT;
}

// Função usada para especificar a posição do observador virtual
void PosicionaObservador(void)
{
  // Especifica sistema de coordenadas do modelo
  glMatrixMode(GL_MODELVIEW);
  // Inicializa sistema de coordenadas do modelo
  glLoadIdentity();
  
  // Posiciona e orienta o observador
  glTranslatef(-obsX,-obsY,-obsZ);
  /*
  gluLookAt(0-obsX, 0-obsY, 0-obsZ,
            0+obsX, 0+obsY, 0+obsZ,
            0, 1, 0);
  */
  glRotatef(rotX,1,0,0);
  glRotatef(rotY,0,1,0);

}

// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao(float AspectRatio)
{
  // Especifica sistema de coordenadas de projeção
  glMatrixMode(GL_PROJECTION);
  // Inicializa sistema de coordenadas de projeção
  glLoadIdentity();

  // Define o modo de projeção perspectiva
  gluPerspective(angle, AspectRatio, 0.1, 1200);

  PosicionaObservador();
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

  // Inicializa a variável que especifica o ângulo da projeção
  // perspectiva
  angle=60;

  // Inicializa as variáveis usadas para alterar a posição do
  // observador virtual
  rotX = 0;
  rotY = 0;
  obsX = obsY = 0;
  obsZ = 5;

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);  // Habilita o buffer de profundidade

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
  // IupSetAttribute(canvas, "BUFFER", "SINGLE");
  IupSetAttribute(canvas, "BUFFER", "DOUBLE");

  IupSetAttribute(canvas, "BORDER", "NO");
  IupSetAttribute(canvas, "RASTERSIZE", "800x600");

  // Registra a callback para redesenho da janela de visualização
  IupSetCallback(canvas, "ACTION",      (Icallback)desenha);
  // Registra a callback para tratamento das teclas
  IupSetCallback(canvas, "KEYPRESS_CB", (Icallback)teclado);
  // Registra a callback para redimensionamento da janela
  IupSetCallback(canvas, "RESIZE_CB", (Icallback)redimensiona);
  // Registra a callback para tratamento dos botões do mouse
  IupSetCallback(canvas, "BUTTON_CB", (Icallback)mouse);
  // Registra a callback para tratamento dos movimentos do mouse
  IupSetCallback(canvas, "MOTION_CB", (Icallback)movimenta);

  IupAppend(box, canvas);

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupGL Desenho Tridimensional");
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

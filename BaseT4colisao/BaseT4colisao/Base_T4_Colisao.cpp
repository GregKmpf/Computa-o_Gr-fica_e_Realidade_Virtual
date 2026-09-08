/*
   OpenGL, IUP, bibutil e RAPID
   Sala Virtual com Detecção de Colisão

   Navegação com teclas + movimento do mouse:
   - UP/DOWN: deslocamento em profundidade
   - LEFT/RIGHT: deslocamento lateral
   - Botão esquerdo: ângulo da cabeça

   Detecção de colisão entre personagem (3a. pessoa)
   e objetos do cenário

   Teclas Home e End fazem zoom in/zoom out

   Professor: Rafael Rieder
   UPF - Universidade de Passo Fundo
*/
#include <GL/gl.h>
#include <GL/glu.h>
//teste de user certo no git
//teste 2
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <iup.h>
#include <iupgl.h>
#include <iupkey.h>

#include "bibutil.h"

using namespace std;

// Teste de colisão para exemplo
OBJ* listOBJCollide[2];
MATRIX objMTX[2];
MATRIX legoMTX;
string namesOBJ[2];

// Declaração de Protótipos
void EspecificaParametrosVisualizacao(float AspectRatio);
void PosicionaObservador(void);

// Variáveis para controles de navegação
GLfloat angle;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY, obsZ, obsX_ini, obsY_ini, obsZ_ini;
GLfloat rotXrestore, rotYrestore;
GLfloat obsXrestore, obsYrestore, obsZrestore;
int x_ini, y_ini, bot;

// ******************************
// Object's stuff
// ******************************
OBJ *piso, *sofa, *vaca, *lego;

// ******************************
// Light stuff
// ******************************

// Função responsável pela especificação dos parâmetros de iluminação
void DefineIluminacao (void)
{
  GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0};
  GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};     // "cor"
  GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};// "brilho"
  GLfloat posicaoLuz[4]={0.0, 50.0, 50.0, 1.0};

  // Capacidade de brilho do material
  GLfloat especularidade[4]={1.0,1.0,1.0,1.0};
  GLint especMaterial = 60;

  // Define a refletância do material
  glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
  // Define a concentração do brilho
  glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

  // Ativa o uso da luz ambiente
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

  // Define os parâmetros da luz de número 0
  glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
  glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
  glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
}

void DefineCenario(void)
{
  // Lembrete para detecção de colisão com RAPID:
  // Se usar glScalef, não fará a detecção correta
  // Dar preferência para modelos com as dimensões
  // pré-definidas (pode-se usar AccuTrans ou Blender
  // para editar e ajustar os modelos para isso!)

  // Personagem
  glPushMatrix();
    glTranslatef(obsX, obsY-1.34, obsZ-3.0);
    glRotatef(180, 0, 1, 0);
    glRotatef(-rotY, 0, 1, 0);
    DesenhaObjeto(lego);
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)legoMTX);
  glPopMatrix();

  // Objetos não colidíveis
  glPushMatrix();
    glTranslatef(0, -0.55, 0);
    DesenhaObjeto(piso);
  glPopMatrix();

  // Objetos colidíveis estáticos
  glPushMatrix();  // Sofá
    glTranslatef(0, 0, 0);
    glRotatef(180, 0, 1, 0);
    DesenhaObjeto(listOBJCollide[0]);
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)objMTX[0]);
  glPopMatrix();

  glPushMatrix();  // Vaca
    glColor3f(0, 0, 1);
    glTranslatef(0, 0.34, 2);
    DesenhaObjeto(listOBJCollide[1]);
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)objMTX[1]);
  glPopMatrix();

}

static int teclado(Ihandle *ih, int key, int pressed)
{
  int w, h;
  float sina, cosa, sinaLat, cosaLat;
  char *size = IupGetAttribute(ih, "RASTERSIZE");
  sscanf(size, "%dx%d", &w, &h);

  // Deslocamento da câmera/Lego

  // Pré-calcula o seno e o cosseno do ângulo horizontal
  sina = 0.6*sin(rotY*M_PI/180.0);
  cosa = 0.6*cos(rotY*M_PI/180.0);

  // Pré-calcula o seno e o cosseno do ângulo lateral
  sinaLat = 0.6*sin((rotY+90)*M_PI/180.0);
  cosaLat = 0.6*cos((rotY+90)*M_PI/180.0);

  if(pressed) {
    switch(key) {
      case K_HOME:
        if(angle>=10)
          angle -=5;
        break;
      case K_END:
        if(angle<=150)
          angle +=5;
        break;
      case K_UP:
        obsX = obsX + sina;
        obsZ = obsZ - cosa;
        break;
      case K_DOWN:
        obsX = obsX - sina;
        obsZ = obsZ + cosa;
        break;
      case K_LEFT:
        obsX = obsX - sinaLat;
        obsZ = obsZ + cosaLat;
        break;
      case K_RIGHT:
        obsX = obsX + sinaLat;
        obsZ = obsZ - cosaLat;
        break;
      case K_ESC:
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

#define SENS_ROT    5.0
#define SENS_OBS    10.0
static int movimenta(Ihandle *ih, int x, int y, char *status)
{
  // Botão esquerdo ?
  if(bot==IUP_BUTTON1)
  {
    // Calcula diferenças
    int deltax = x_ini - x;
    int deltay = y_ini - y;
    // Modifica ângulos para câmera/Lego
    rotY = rotY_ini - deltax/SENS_ROT;
    rotX = rotX_ini - deltay/SENS_ROT;
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

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // Chama a função que especifica os parâmetros de iluminação
  DefineIluminacao();

  // Carrega cenário
  DefineCenario();

  for(int i = 0; i < 2; i++) {
    if(MyCollided(listOBJCollide[i], objMTX[i], lego, legoMTX, 0)) {
      // Restaura câmera (e posição do Lego) para o status sem colisão
      rotX = rotXrestore;
      rotY = rotYrestore;
      obsX = obsXrestore;
      obsY = obsYrestore;
      obsZ = obsZrestore;
      cout << "#### Lego colidiu com " << namesOBJ[i] << "!! ####" << endl;
      PosicionaObservador();
      IupRedraw(IupGetHandle("MeuCanvas"), 0);
    }
  }

  ///////////////////////////////////
  // Salva a posição atual da câmera
  rotXrestore = rotX;
  rotYrestore = rotY;
  obsXrestore = obsX;
  obsYrestore = obsY;
  obsZrestore = obsZ;
  ///////////////////////////////////

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
  gluPerspective(angle, AspectRatio, 0.1, 50);

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

  // Define a cor de fundo da janela de visualização como branca
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  // Habilita a definição da cor do material a partir da cor corrente
  glEnable(GL_COLOR_MATERIAL);
  //Habilita o uso de iluminação
  glEnable(GL_LIGHTING);
  // Habilita a luz de número 0
  glEnable(GL_LIGHT0);
  // Remoção faces traseiras
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  // Habilita o depth-buffering
  glEnable(GL_DEPTH_TEST);
  // Habilita o modelo de colorização de Gouraud
  glShadeModel(GL_SMOOTH);

  // Carrega objetos
  lego = CarregaObjeto((char*)"modelos/LEGO_man2.obj", false);
  piso = CarregaObjeto((char*)"modelos/piso.obj", false);
  listOBJCollide[0] = CarregaObjeto((char*)"modelos/sofa.obj", false);
  listOBJCollide[1] = CarregaObjeto((char*)"modelos/vaquinha.obj", false);
  namesOBJ[0] = "Sofa";
  namesOBJ[1] = "Vaca";

  // Seta o modo de desenho inicial
  // w = wireframe  s = solid  t = textured
  SetaModoDesenho('t');
  
  ///////////////////////////////////////////////////////
  // Cria display lists para TODOS os objetos carregados
  // Contribui para a otimização do rendering conforme
  // se tem mais modelos 3D a serem desenhados
  CriaDisplayList(NULL);  

  // Inicializa a variável que especifica o ângulo da projeção
  // perspectiva
  angle=90;

  // Inicializa as variáveis usadas para alterar a posição do
  // observador virtual
  rotX = 0;
  rotY = 0;
  obsX = 0;
  obsY = 1.0;
  obsZ = 5.5;

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
  IupSetAttribute(canvas, "BUFFER", "DOUBLE");
  IupSetAttribute(canvas, "DEPTH_SIZE", "24");

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
  IupSetAttribute(dlg, "TITLE", "IupGL Sala Virtual");
  // Define o uso de caracteres especiais/específicos para elementos IUP
  IupSetGlobal("UTF8MODE", "YES");

  IupMap(dlg);

  IupGLMakeCurrent(canvas);

  inicializa(canvas);  // define a janela de visualização

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  // Ativa o foco de teclado para o canvas
  IupSetFocus(canvas);

  IupMainLoop();

  LiberaMateriais();
  LiberaObjeto(NULL);

  IupClose();

  return 0;
}

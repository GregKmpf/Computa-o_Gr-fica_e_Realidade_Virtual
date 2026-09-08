#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#endif

// ============================================================================
// VARIÁVEIS DE NAVEGAÇÃO E NORMAS DE VISUALIZAÇÃO
// ============================================================================
GLfloat rotX = 20.0f;
GLfloat rotY = -35.0f;
GLfloat obsX = 0.0f;
GLfloat obsY = 0.0f;
GLfloat obsZ = 8.0f;

// ============================================================================
// GEOMETRIA DOS OBJETOS
// ============================================================================

// 8 Vértices do Cubo Central (Dimensões: x ∈ [-1, 1], y ∈ [-1, 1], z ∈ [-1, 1])
GLfloat verticesCubo[8][3] = {
    {-1.0f, -1.0f, -1.0f}, // 0: Trás-Esquerda-Baixo
    { 1.0f, -1.0f, -1.0f}, // 1: Trás-Direita-Baixo
    { 1.0f,  1.0f, -1.0f}, // 2: Trás-Direita-Alto
    {-1.0f,  1.0f, -1.0f}, // 3: Trás-Esquerda-Alto
    {-1.0f, -1.0f,  1.0f}, // 4: Frente-Esquerda-Baixo
    { 1.0f, -1.0f,  1.0f}, // 5: Frente-Direita-Baixo
    { 1.0f,  1.0f,  1.0f}, // 6: Frente-Direita-Alto
    {-1.0f,  1.0f,  1.0f}  // 7: Frente-Esquerda-Alto
};

// Ápices das Pirâmides Laterais
GLfloat apexEsquerda[3] = {-2.5f, 0.0f, 0.0f};
GLfloat apexDireita[3]  = { 2.5f, 0.0f, 0.0f};

// ============================================================================
// FUNÇÕES DE DESENHO DOS OBJETOS (3 OBJETOS DISTINTOS)
// ============================================================================

// Objeto 1: Cubo Central
void DesenhaCuboWireframe() {
    // Definindo as 6 faces quadradas do cubo através dos índices dos vértices
    GLuint faces[6][4] = {
        {0, 1, 2, 3}, // Trás
        {4, 5, 6, 7}, // Frente
        {0, 4, 7, 3}, // Esquerda
        {1, 5, 6, 2}, // Direita
        {3, 2, 6, 7}, // Topo
        {0, 1, 5, 4}  // Base
    };

    for (int i = 0; i < 6; i++) {
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 4; j++) {
            glVertex3fv(verticesCubo[faces[i][j]]);
        }
        glEnd();
    }
}

// Objeto 2: Pirâmide Esquerda
void DesenhaPiramideEsquerdaWireframe() {
    // A base da pirâmide esquerda é a face esquerda do cubo: vértices 0, 3, 7, 4
    int base[4] = {0, 3, 7, 4};
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINE_LOOP);
        glVertex3fv(apexEsquerda);
        glVertex3fv(verticesCubo[base[i]]);
        glVertex3fv(verticesCubo[base[(i + 1) % 4]]);
        glEnd();
    }
}

// Objeto 3: Pirâmide Direita
void DesenhaPiramideDireitaWireframe() {
    // A base da pirâmide direita é a face direita do cubo: vértices 1, 2, 6, 5
    int base[4] = {1, 2, 6, 5};
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINE_LOOP);
        glVertex3fv(apexDireita);
        glVertex3fv(verticesCubo[base[i]]);
        glVertex3fv(verticesCubo[base[(i + 1) % 4]]);
        glEnd();
    }
}

// Função Principal de Desenho do Conjunto
void DesenhaObjetoWireframe() {
    glColor3f(1.0f, 1.0f, 1.0f); // Cor das linhas: Branco

    DesenhaCuboWireframe();
    DesenhaPiramideEsquerdaWireframe();
    DesenhaPiramideDireitaWireframe();
}

// ============================================================================
// NAVEGAÇÃO E POSICIONAMENTO DO OBSERVADOR
// ============================================================================

void PosicionaObservador() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Posicionamento da Câmera no espaço
    glTranslatef(-obsX, -obsY, -obsZ);

    // Rotações da cena em torno dos eixos X e Y
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
}

// ============================================================================
// CALLBACKS GLUT
// ============================================================================

void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    PosicionaObservador();
    DesenhaObjetoWireframe();

    glutSwapBuffers();
}

void Reshape(int width, int height) {
    if (height == 0) height = 1;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

void TeclasEspeciais(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            rotY -= 5.0f;
            break;
        case GLUT_KEY_RIGHT:
            rotY += 5.0f;
            break;
        case GLUT_KEY_UP:
            rotX -= 5.0f;
            break;
        case GLUT_KEY_DOWN:
            rotX += 5.0f;
            break;
    }
    glutPostRedisplay();
}

void Teclado(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // Tecla ESC sai do programa
            exit(0);
            break;
        case '+':
        case '=':
            obsZ -= 0.5f; // Zoom in
            break;
        case '-':
        case '_':
            obsZ += 0.5f; // Zoom out
            break;
        case 'w': case 'W':
            obsY += 0.2f;
            break;
        case 's': case 'S':
            obsY -= 0.2f;
            break;
        case 'a': case 'A':
            obsX -= 0.2f;
            break;
        case 'd': case 'D':
            obsX += 0.2f;
            break;
    }
    glutPostRedisplay();
}

void Init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Pedra Lapidada 3D - Computacao Grafica UPF");

    Init();

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Teclado);
    glutSpecialFunc(TeclasEspeciais);

    glutMainLoop();
    return 0;
}
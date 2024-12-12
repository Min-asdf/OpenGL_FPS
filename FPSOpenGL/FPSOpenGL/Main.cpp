#include <GL/freeglut.h>
#include <math.h>
#include <windows.h>
#include "ParticleSystem.h"
#include "RayCaster.h"
#include "Vector3.h"
#include "GlobalVariables.h"

// ���� ������
ParticleSystem particleSystem;
RayCaster* rayCaster = nullptr;
bool isLeftButtonPressed = false;
float fireRate = 0.1f;
float lastFireTime = 0.0f;
int prevTime = 0;
float deltaTime = 0.0f;

// �ؽ�ó �迭�� �ؽ�ó ID
GLubyte MyTexture[8][8][3];
GLuint textureID;

// ������ ����
int FlatShaded = 0;
int Wireframed = 0;

// ���콺 ���� ���� ����
int ViewX = 400, ViewY = 300;
float sensitivity = 0.2f;
bool firstMouse = true;

// ����
void InitLight() {
    GLfloat light_position[] = { 0.0, 10.0, 5.0, 1.0 }; // ���� ��ġ
    GLfloat light_ambient[] = { 0.7, 0.7, 0.7, 1.0 }; // �ֺ��� (ȸ��)
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; // Ȯ�걤 (���)
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // �ݻ籤

    glEnable(GL_DEPTH_TEST); // ���� �׽�Ʈ Ȱ��ȭ
    glEnable(GL_LIGHTING); // ���� Ȱ��ȭ
    glEnable(GL_LIGHT0); // LIGHT0 Ȱ��ȭ
    glShadeModel(GL_SMOOTH); // ���̵� ���

    glLightfv(GL_LIGHT0, GL_POSITION, light_position); // ���� ��ġ ����
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); // �ֺ��� ����
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); // Ȯ�걤 ����
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); // �ݻ籤 ����
}

// ���콺 ������ (FPS���� ȸ�� ����)
void MyMouseMove(int x, int y) {
    if (firstMouse) {
        ViewX = x;
        ViewY = y;
        firstMouse = false;
        return;
    }

    float xoffset = ViewX - x;
    float yoffset = ViewY - y;
    ViewX = x;
    ViewY = y;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cameraRotY += xoffset;
    cameraRotX += yoffset;

    // ���� ȸ�� ����
    if (cameraRotX > 89.0f) cameraRotX = 89.0f;
    if (cameraRotX < -89.0f) cameraRotX = -89.0f;

    // ���� ȸ�� ������ 360�� ���� ���� ����
    if (cameraRotY > 360.0f) cameraRotY -= 360.0f;
    if (cameraRotY < 0.0f) cameraRotY += 360.0f;

    /*// ��Ŭ�� ���¿��� ����ĳ���� ����
    if (isLeftButtonPressed && rayCaster) {
        rayCaster->castRay(x, y);
    }*/

    glutPostRedisplay();
}

// glutMotionFunc �ݹ� �߰�
void MyMotion(int x, int y) {
    // ���콺 �巡�� �� ������ ���� ���� ���� ����
    MyMouseMove(x, y);
}

// ���� �ð��� �� ������ ��ȯ�ϴ� �Լ�
float GetGameTime() {
    return glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

// ���콺�� ȭ���߾����� ���� �̵���Ű�� �Լ� (FPS ���� ��� ���� �ʿ�)
void WarpPointer(int window_width, int window_height) {
    int center_x = window_width / 2;
    int center_y = window_height / 2;

    if (ViewX != center_x || ViewY != center_y) {
        ViewX = center_x;
        ViewY = center_y;
        glutWarpPointer(center_x, center_y);
    }
}

// üũ���� �ؽ�ó
void CreateCheckerTexture() {
    int s, t;
    for (s = 0; s < 8; s++) {
        for (t = 0; t < 8; t++) {
            GLubyte intensity = ((s + t) % 2) * 127 + 128;
            MyTexture[s][t][0] = intensity;
            MyTexture[s][t][1] = intensity;
            MyTexture[s][t][2] = intensity;
        }
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_UNSIGNED_BYTE, MyTexture);
}

// ����Ʈ�� ���� ��� ����
void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / h, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

// üũ���� �ؽ�ó�� ����� �� ����
void DrawWall() {
    float wallWidth = 400.0f;
    float wallHeight = 300.0f;
    float wallDistance = -300.0f;

    GLfloat wall_mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat wall_mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat wall_mat_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, wall_mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, wall_mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, wall_mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, wallDistance);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-wallWidth / 2, -wallHeight / 2, 0.0f);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(wallWidth / 2, -wallHeight / 2, 0.0f);
    glTexCoord2f(2.0f, 2.0f); glVertex3f(wallWidth / 2, wallHeight / 2, 0.0f);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-wallWidth / 2, wallHeight / 2, 0.0f);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// ȭ�� ���� �ϴܿ� ���� �׸��� �Լ� (Cylinder�� �ѱ� ǥ��)
void DrawGun() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(45.0f, 800.0f / 600.0f, 0.1f, 200.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(0.2f, -0.2f, -0.9);
    glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(15.0f, 0.0f, 0.0f, 1.0f);

    GLfloat gun_mat[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat gun_spec[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gun_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, gun_spec);
    glMaterialf(GL_FRONT, GL_SHININESS, 32.0f);

    GLUquadricObj* cylinder = gluNewQuadric();
    gluQuadricDrawStyle(cylinder, GLU_FILL);
    gluCylinder(cylinder, 0.03, 0.03, 0.25, 16, 1);

    gluDeleteQuadric(cylinder);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// 
void MyDisplay() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = (currentTime - prevTime) / 1000.0f;
    prevTime = currentTime;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ī�޶�� ������ ����� �������� �ݴ��̱� ������ - ��ȣ ���
    glRotatef(-cameraRotX, 1.0f, 0.0f, 0.0f);
    glRotatef(-cameraRotY, 0.0f, 1.0f, 0.0f);
    glTranslatef(-cameraX, -cameraY, -cameraZ);

    GLfloat light_position[] = { 0.0, 10.0, 0.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    DrawWall();

    // ���� �߻�
    if (isLeftButtonPressed && rayCaster) {
        static float timeSinceLastShot = 0.0f;
        timeSinceLastShot += deltaTime;

        if (timeSinceLastShot >= fireRate) {
            int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
            int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
            rayCaster->castRay(centerX, centerY);
            timeSinceLastShot = 0.0f;
        }
    }

    // �� 60fps ������ ��ƼŬ �ý��� ������Ʈ �� ������
    glPushMatrix();
    particleSystem.update(0.016f);
    particleSystem.render();
    glPopMatrix();

    DrawGun();

    glutSwapBuffers();

    WarpPointer(800, 600);
    glutPostRedisplay();
}

// ���콺 �Է� ó��
void MyMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isLeftButtonPressed = true;
            int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
            int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
            if (rayCaster) {
                rayCaster->castRay(centerX, centerY);
            }
        }
        else if (state == GLUT_UP) {
            isLeftButtonPressed = false;
        }
    }
}

// Ű���� �Է� ó��
void MyKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': case 'Q': case '\033':
        delete rayCaster;
        exit(0);
        break;
    case 's':
        if (FlatShaded) {
            FlatShaded = 0;
            glShadeModel(GL_SMOOTH);
        }
        else {
            FlatShaded = 1;
            glShadeModel(GL_FLAT);
        }
        glutPostRedisplay();
        break;
    case 'w':
        if (Wireframed) {
            Wireframed = 0;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            Wireframed = 1;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        glutPostRedisplay();
        break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("FPS");

    rayCaster = new RayCaster(particleSystem);

    InitLight();
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_TEXTURE_2D);
    CreateCheckerTexture();

    glutDisplayFunc(MyDisplay);
    glutReshapeFunc(MyReshape);
    glutKeyboardFunc(MyKeyboard);
    glutPassiveMotionFunc(MyMouseMove);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMotion);
    glutIdleFunc(MyDisplay);

    glutSetCursor(GLUT_CURSOR_NONE);

    glutMainLoop();

    delete rayCaster;
    return 0;
}
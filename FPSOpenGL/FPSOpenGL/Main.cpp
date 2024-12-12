#include <GL/freeglut.h>
#include <math.h>
#include <windows.h>
#include "ParticleSystem.h"
#include "RayCaster.h"
#include "Vector3.h"
#include "GlobalVariables.h"

// 전역 변수들
ParticleSystem particleSystem;
RayCaster* rayCaster = nullptr;
bool isLeftButtonPressed = false;
float fireRate = 0.1f;
float lastFireTime = 0.0f;
int prevTime = 0;
float deltaTime = 0.0f;

// 텍스처 배열과 텍스처 ID
GLubyte MyTexture[8][8][3];
GLuint textureID;

// 렌더링 변수
int FlatShaded = 0;
int Wireframed = 0;

// 마우스 시점 제어 변수
int ViewX = 400, ViewY = 300;
float sensitivity = 0.2f;
bool firstMouse = true;

// 광원
void InitLight() {
    GLfloat light_position[] = { 0.0, 10.0, 5.0, 1.0 }; // 광원 위치
    GLfloat light_ambient[] = { 0.7, 0.7, 0.7, 1.0 }; // 주변광 (회색)
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; // 확산광 (흰색)
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // 반사광

    glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
    glEnable(GL_LIGHTING); // 조명 활성화
    glEnable(GL_LIGHT0); // LIGHT0 활성화
    glShadeModel(GL_SMOOTH); // 쉐이딩 모드

    glLightfv(GL_LIGHT0, GL_POSITION, light_position); // 광원 위치 설정
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); // 주변광 설정
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); // 확산광 설정
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); // 반사광 설정
}

// 마우스 움직임 (FPS시점 회전 구현)
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

    // 수직 회전 제한
    if (cameraRotX > 89.0f) cameraRotX = 89.0f;
    if (cameraRotX < -89.0f) cameraRotX = -89.0f;

    // 수평 회전 각도를 360도 범위 내로 유지
    if (cameraRotY > 360.0f) cameraRotY -= 360.0f;
    if (cameraRotY < 0.0f) cameraRotY += 360.0f;

    /*// 좌클릭 상태에서 레이캐스팅 실행
    if (isLeftButtonPressed && rayCaster) {
        rayCaster->castRay(x, y);
    }*/

    glutPostRedisplay();
}

// glutMotionFunc 콜백 추가
void MyMotion(int x, int y) {
    // 마우스 드래그 중 동일한 시점 제어 로직 적용
    MyMouseMove(x, y);
}

// 현재 시간을 초 단위로 반환하는 함수
float GetGameTime() {
    return glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

// 마우스를 화면중앙으로 강제 이동시키는 함수 (FPS 시점 제어를 위해 필요)
void WarpPointer(int window_width, int window_height) {
    int center_x = window_width / 2;
    int center_y = window_height / 2;

    if (ViewX != center_x || ViewY != center_y) {
        ViewX = center_x;
        ViewY = center_y;
        glutWarpPointer(center_x, center_y);
    }
}

// 체크무늬 텍스처
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

// 뷰포트와 투영 행렬 설정
void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / h, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

// 체크무늬 텍스처가 적용된 벽 생성
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

// 화면 우측 하단에 총을 그리는 함수 (Cylinder로 총구 표현)
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

    // 카메라와 월드의 상대적 움직임이 반대이기 때문에 - 부호 사용
    glRotatef(-cameraRotX, 1.0f, 0.0f, 0.0f);
    glRotatef(-cameraRotY, 0.0f, 1.0f, 0.0f);
    glTranslatef(-cameraX, -cameraY, -cameraZ);

    GLfloat light_position[] = { 0.0, 10.0, 0.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    DrawWall();

    // 연속 발사
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

    // 약 60fps 간격의 파티클 시스템 업데이트 및 렌더링
    glPushMatrix();
    particleSystem.update(0.016f);
    particleSystem.render();
    glPopMatrix();

    DrawGun();

    glutSwapBuffers();

    WarpPointer(800, 600);
    glutPostRedisplay();
}

// 마우스 입력 처리
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

// 키보드 입력 처리
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
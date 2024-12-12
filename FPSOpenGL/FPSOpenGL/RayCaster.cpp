#include "RayCaster.h"
#include <GL/freeglut.h>

// ������ (ParticleSystem ����)
RayCaster::RayCaster(ParticleSystem& ps) : particleSystem(ps) {}

// Ray Cast
void RayCaster::castRay(float mouseX, float mouseY) {
    Vector3 rayDir = getWorldRay(mouseX, mouseY);   // ���콺 ��ġ�κ��� ���� ������ ���� ���� ���
    Vector3 origin(cameraX, cameraY, cameraZ);      // Ray�� ������
    Vector3 hitPoint;                               // �浹 ������ ������ ����

    // ������ �浹 �˻� �� ��ƼŬ ����
    if (intersectWall(origin, rayDir, hitPoint)) {
        particleSystem.spawnParticles(hitPoint);
    }
}

// ��ũ�� ��ǥ�� ���� ��ǥ�� ��ȯ
Vector3 RayCaster::getWorldRay(float mouseX, float mouseY) {
    GLint viewport[4];              // ����Ʈ ����
    GLdouble modelview[16];         // �𵨺� ���
    GLdouble projection[16];        // ���� ���
    GLdouble worldX, worldY, worldZ;

    // ���� OpenGL ���� ���� ��������
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    // ���콺 ��ǥ OpenGL ��ǥ��� ��ȯ
    GLfloat winX = mouseX;
    GLfloat winY = viewport[3] - mouseY;    // Y��ǥ ����
    GLfloat winZ = 1.0f;                    // ���Ÿ� ���

    // ��ũ�� ��ǥ�� ���� ��ǥ�� ��ȯ
    gluUnProject(winX, winY, winZ,
        modelview, projection, viewport,
        &worldX, &worldY, &worldZ);

    // Ray ���� ��� �� ����ȭ
    Vector3 dir(worldX - cameraX, worldY - cameraY, worldZ - cameraZ);
    dir.normalize();
    return dir;
}

// Ray - Wall �浹 �˻�
bool RayCaster::intersectWall(const Vector3& origin, const Vector3& direction, Vector3& hitPoint) {
    float wallZ = -300.0f;                      // ���� Z��ġ
    float t = (wallZ - origin.z) / direction.z; //Ray - Wall ������ ���
    
    // �������� Ray�� ���ʿ� �ִ� ���
    if (t > 0) {
        hitPoint.x = origin.x + direction.x * t;
        hitPoint.y = origin.y + direction.y * t;
        hitPoint.z = wallZ;

        // �������� ���� ���� ���� �ִ��� Ȯ��
        if (std::abs(hitPoint.x) < 200.0f && std::abs(hitPoint.y) < 150.0f) {
            return true;
        }
    }
    return false;
}
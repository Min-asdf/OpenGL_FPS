#include "RayCaster.h"
#include <GL/freeglut.h>

// 생성자 (ParticleSystem 참조)
RayCaster::RayCaster(ParticleSystem& ps) : particleSystem(ps) {}

// Ray Cast
void RayCaster::castRay(float mouseX, float mouseY) {
    Vector3 rayDir = getWorldRay(mouseX, mouseY);   // 마우스 위치로부터 월드 공간의 레이 방향 계산
    Vector3 origin(cameraX, cameraY, cameraZ);      // Ray의 시작점
    Vector3 hitPoint;                               // 충돌 지점을 저장할 변수

    // 벽과의 충돌 검사 후 파티클 생성
    if (intersectWall(origin, rayDir, hitPoint)) {
        particleSystem.spawnParticles(hitPoint);
    }
}

// 스크린 좌표를 월드 좌표로 변환
Vector3 RayCaster::getWorldRay(float mouseX, float mouseY) {
    GLint viewport[4];              // 뷰포트 정보
    GLdouble modelview[16];         // 모델뷰 행렬
    GLdouble projection[16];        // 투영 행렬
    GLdouble worldX, worldY, worldZ;

    // 현재 OpenGL 상태 정보 가져오기
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    // 마우스 좌표 OpenGL 좌표계로 변환
    GLfloat winX = mouseX;
    GLfloat winY = viewport[3] - mouseY;    // Y좌표 반전
    GLfloat winZ = 1.0f;                    // 원거리 평면

    // 스크린 좌표를 월드 좌표로 변환
    gluUnProject(winX, winY, winZ,
        modelview, projection, viewport,
        &worldX, &worldY, &worldZ);

    // Ray 방향 계산 및 정규화
    Vector3 dir(worldX - cameraX, worldY - cameraY, worldZ - cameraZ);
    dir.normalize();
    return dir;
}

// Ray - Wall 충돌 검사
bool RayCaster::intersectWall(const Vector3& origin, const Vector3& direction, Vector3& hitPoint) {
    float wallZ = -300.0f;                      // 벽의 Z위치
    float t = (wallZ - origin.z) / direction.z; //Ray - Wall 교차점 계산
    
    // 교차점이 Ray의 앞쪽에 있는 경우
    if (t > 0) {
        hitPoint.x = origin.x + direction.x * t;
        hitPoint.y = origin.y + direction.y * t;
        hitPoint.z = wallZ;

        // 교차점이 벽의 범위 내에 있는지 확인
        if (std::abs(hitPoint.x) < 200.0f && std::abs(hitPoint.y) < 150.0f) {
            return true;
        }
    }
    return false;
}
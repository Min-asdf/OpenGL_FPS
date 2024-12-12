#pragma once
#include "Vector3.h"
#include "ParticleSystem.h"
#include "GlobalVariables.h"

class RayCaster {
public:
    // 생성자 (ParticleSystem 참조)
    RayCaster(ParticleSystem& ps);
    // Ray 발사
    void castRay(float mouseX, float mouseY);

private:
    // ParticleSystem 참조
    ParticleSystem& particleSystem;
    // 마우스 좌표를 3D Ray로 변환
    Vector3 getWorldRay(float mouseX, float mouseY);
    // Ray - Wall 충돌 검사
    bool intersectWall(const Vector3& origin, const Vector3& direction, Vector3& hitPoint);
};
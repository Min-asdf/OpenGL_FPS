#pragma once
#include "Vector3.h"
#include "ParticleSystem.h"
#include "GlobalVariables.h"

class RayCaster {
public:
    // ������ (ParticleSystem ����)
    RayCaster(ParticleSystem& ps);
    // Ray �߻�
    void castRay(float mouseX, float mouseY);

private:
    // ParticleSystem ����
    ParticleSystem& particleSystem;
    // ���콺 ��ǥ�� 3D Ray�� ��ȯ
    Vector3 getWorldRay(float mouseX, float mouseY);
    // Ray - Wall �浹 �˻�
    bool intersectWall(const Vector3& origin, const Vector3& direction, Vector3& hitPoint);
};
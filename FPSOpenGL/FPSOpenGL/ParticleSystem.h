#pragma once
#include <GL/freeglut.h>
#include <vector>
#include "Vector3.h"
#include <random>

class ParticleSystem {
public:
    ParticleSystem();   // 생성자
    void update(float deltaTime);   // 파티클 상태 업데이트
    void render();  // 파티클 렌더링
    void spawnParticles(
        const Vector3& position,    // 파티클 생성 위치
        int count = 30);    // 파티클 30개 생성

private:
    // 파티클 구조체 정의
    struct Particle {
        Vector3 position;   // 위치
        Vector3 velocity;   // 속도
        Vector3 color;      // 색상
        float life;         // 수명
        float size;         // 크기

        // 파티클 생성자
        Particle(const Vector3& pos, const Vector3& vel, const Vector3& col)
            : position(pos), velocity(vel), color(col), life(1.0f), size(0.5f) {
        }
    };

    std::vector<Particle> particles;    // 파티클 저장 컨테이너
    Vector3 generateRandomVelocity() const;
    void renderParticle(const Particle& particle);
};
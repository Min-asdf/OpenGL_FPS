#include "ParticleSystem.h"

ParticleSystem::ParticleSystem() {}

// 파티클 업데이트
void ParticleSystem::update(float deltaTime) {
    auto it = particles.begin();
    while (it != particles.end()) {
        // 파티클 수명 감소
        it->life -= deltaTime;
        // 수명이 다한 파티클 제거
        if (it->life <= 0.0f) {
            it = particles.erase(it);
        }
        else {
            // 중력 및 속도 업데이트
            it->velocity.y -= 9.8f * deltaTime;
            it->position.x += it->velocity.x * deltaTime;
            it->position.y += it->velocity.y * deltaTime;
            it->position.z += it->velocity.z * deltaTime;

            // 수명에 비례해 파티클 크기 감소
            it->size = it->life * 0.5f;
            ++it;
        }
    }
}

// 파티클 렌더링
void ParticleSystem::render() {
    if (particles.empty()) return;

    // OpenGL 상태 저장
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);     // 조명 비확성화
    glDisable(GL_DEPTH_TEST);   // 깊이 테스트 비활성화
    glEnable(GL_BLEND);         // 블렌딩 활성화
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // 알파 블렌딩 설정

    // 모든 파티클 렌더링
    for (const auto& particle : particles) {
        renderParticle(particle);
    }

    // OpenGL 상태 복원
    glPopAttrib();
}

// 파티클 생성
void ParticleSystem::spawnParticles(const Vector3& position, int count) {
    for (int i = 0; i < count; i++) {
        Vector3 velocity = generateRandomVelocity();
        Vector3 color(1.0f, 0.5f, 0.0f); // 주황색

        // 시작 위치에 약간의 랜덤성 추가
        float randX = (rand() % 100 - 50) / 500.0f;
        float randY = (rand() % 100 - 50) / 500.0f;
        float randZ = (rand() % 100 - 50) / 500.0f;
        Vector3 randomOffset(randX, randY, randZ);

        // 파티클 추가
        particles.emplace_back(position + randomOffset, velocity, color);
    }
}

// 랜덤 속도 생성
Vector3 ParticleSystem::generateRandomVelocity() const {
    float angle = (rand() % 360) * 3.14159f / 180.0f;   // 랜덤 각도
    float speed = 5.0f + (rand() % 50) / 10.0f;         // 랜덤 속도

    return Vector3(
        cos(angle) * speed,             // X방향 속도
        5.0f + (rand() % 50) / 10.0f,   // Y방향 속도
        sin(angle) * speed              // Z방향 속도
    );
}

// 개별 파티클 렌더링
void ParticleSystem::renderParticle(const Particle& particle) {
    glPushMatrix();
    // 파티클 위치로 이동
    glTranslatef(particle.position.x, particle.position.y, particle.position.z);

    // 카메라를 향하도록 회전
    GLfloat modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    // 파티클 색상과 투명도 설정
    glColor4f(particle.color.x, particle.color.y, particle.color.z, particle.life);

    // 사각형 파티클
    float halfSize = particle.size * 5.0f;
    glBegin(GL_QUADS);
    glVertex3f(-halfSize, -halfSize, 0.0f);
    glVertex3f(halfSize, -halfSize, 0.0f);
    glVertex3f(halfSize, halfSize, 0.0f);
    glVertex3f(-halfSize, halfSize, 0.0f);
    glEnd();

    glPopMatrix();
}
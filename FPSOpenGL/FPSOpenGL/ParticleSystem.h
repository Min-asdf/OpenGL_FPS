#pragma once
#include <GL/freeglut.h>
#include <vector>
#include "Vector3.h"
#include <random>

class ParticleSystem {
public:
    ParticleSystem();   // ������
    void update(float deltaTime);   // ��ƼŬ ���� ������Ʈ
    void render();  // ��ƼŬ ������
    void spawnParticles(
        const Vector3& position,    // ��ƼŬ ���� ��ġ
        int count = 30);    // ��ƼŬ 30�� ����

private:
    // ��ƼŬ ����ü ����
    struct Particle {
        Vector3 position;   // ��ġ
        Vector3 velocity;   // �ӵ�
        Vector3 color;      // ����
        float life;         // ����
        float size;         // ũ��

        // ��ƼŬ ������
        Particle(const Vector3& pos, const Vector3& vel, const Vector3& col)
            : position(pos), velocity(vel), color(col), life(1.0f), size(0.5f) {
        }
    };

    std::vector<Particle> particles;    // ��ƼŬ ���� �����̳�
    Vector3 generateRandomVelocity() const;
    void renderParticle(const Particle& particle);
};
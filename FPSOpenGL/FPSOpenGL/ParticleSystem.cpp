#include "ParticleSystem.h"

ParticleSystem::ParticleSystem() {}

// ��ƼŬ ������Ʈ
void ParticleSystem::update(float deltaTime) {
    auto it = particles.begin();
    while (it != particles.end()) {
        // ��ƼŬ ���� ����
        it->life -= deltaTime;
        // ������ ���� ��ƼŬ ����
        if (it->life <= 0.0f) {
            it = particles.erase(it);
        }
        else {
            // �߷� �� �ӵ� ������Ʈ
            it->velocity.y -= 9.8f * deltaTime;
            it->position.x += it->velocity.x * deltaTime;
            it->position.y += it->velocity.y * deltaTime;
            it->position.z += it->velocity.z * deltaTime;

            // ���� ����� ��ƼŬ ũ�� ����
            it->size = it->life * 0.5f;
            ++it;
        }
    }
}

// ��ƼŬ ������
void ParticleSystem::render() {
    if (particles.empty()) return;

    // OpenGL ���� ����
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);     // ���� ��Ȯ��ȭ
    glDisable(GL_DEPTH_TEST);   // ���� �׽�Ʈ ��Ȱ��ȭ
    glEnable(GL_BLEND);         // ���� Ȱ��ȭ
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // ���� ���� ����

    // ��� ��ƼŬ ������
    for (const auto& particle : particles) {
        renderParticle(particle);
    }

    // OpenGL ���� ����
    glPopAttrib();
}

// ��ƼŬ ����
void ParticleSystem::spawnParticles(const Vector3& position, int count) {
    for (int i = 0; i < count; i++) {
        Vector3 velocity = generateRandomVelocity();
        Vector3 color(1.0f, 0.5f, 0.0f); // ��Ȳ��

        // ���� ��ġ�� �ణ�� ������ �߰�
        float randX = (rand() % 100 - 50) / 500.0f;
        float randY = (rand() % 100 - 50) / 500.0f;
        float randZ = (rand() % 100 - 50) / 500.0f;
        Vector3 randomOffset(randX, randY, randZ);

        // ��ƼŬ �߰�
        particles.emplace_back(position + randomOffset, velocity, color);
    }
}

// ���� �ӵ� ����
Vector3 ParticleSystem::generateRandomVelocity() const {
    float angle = (rand() % 360) * 3.14159f / 180.0f;   // ���� ����
    float speed = 5.0f + (rand() % 50) / 10.0f;         // ���� �ӵ�

    return Vector3(
        cos(angle) * speed,             // X���� �ӵ�
        5.0f + (rand() % 50) / 10.0f,   // Y���� �ӵ�
        sin(angle) * speed              // Z���� �ӵ�
    );
}

// ���� ��ƼŬ ������
void ParticleSystem::renderParticle(const Particle& particle) {
    glPushMatrix();
    // ��ƼŬ ��ġ�� �̵�
    glTranslatef(particle.position.x, particle.position.y, particle.position.z);

    // ī�޶� ���ϵ��� ȸ��
    GLfloat modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    // ��ƼŬ ����� ���� ����
    glColor4f(particle.color.x, particle.color.y, particle.color.z, particle.life);

    // �簢�� ��ƼŬ
    float halfSize = particle.size * 5.0f;
    glBegin(GL_QUADS);
    glVertex3f(-halfSize, -halfSize, 0.0f);
    glVertex3f(halfSize, -halfSize, 0.0f);
    glVertex3f(halfSize, halfSize, 0.0f);
    glVertex3f(-halfSize, halfSize, 0.0f);
    glEnd();

    glPopMatrix();
}
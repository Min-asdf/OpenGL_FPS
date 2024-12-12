#pragma once
#include <cmath>

struct Vector3 {
    float x, y, z;
    // ������ (�⺻�� 0���� �ʱ�ȭ)
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : x(x), y(y), z(z) {
    }

    // ���� ���� ������ �����ε�
    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
    
    // ��Į�� ���� ������ �����ε�
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // ���� ����ȭ (���� ���ͷ� ��ȯ)
    void normalize() {
        // ���� ���� ���
        float length = std::sqrt(x * x + y * y + z * z);
        // 0���� ������ ���� �����ϰ� �� ������ ���̷� ������ ����ȭ
        if (length > 0) {
            x /= length;
            y /= length;
            z /= length;
        }
    }
};
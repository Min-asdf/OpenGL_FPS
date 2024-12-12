#pragma once
#include <cmath>

struct Vector3 {
    float x, y, z;
    // 생성자 (기본값 0으로 초기화)
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : x(x), y(y), z(z) {
    }

    // 벡터 덧셈 연산자 오버로딩
    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
    
    // 스칼라 곱셍 연산자 오버로딩
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // 벡터 정규화 (단위 벡터로 변환)
    void normalize() {
        // 벡터 길이 계산
        float length = std::sqrt(x * x + y * y + z * z);
        // 0으로 나누는 것을 방지하고 각 성분을 길이로 나누어 정규화
        if (length > 0) {
            x /= length;
            y /= length;
            z /= length;
        }
    }
};
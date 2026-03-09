//
// Created by adamd on 2026-01-28.
//

#include "Vector2D.h"

#include <cmath>


Vector2D Vector2D::operator+(const Vector2D& vec) const {
    return Vector2D(x + vec.x, y + vec.y);
}

Vector2D Vector2D::operator-(const Vector2D& vec) const {
    return Vector2D(x - vec.x, y - vec.y);
}

// member operator function so we can use Vector2D that we want to change on the left hand side
Vector2D& Vector2D::operator+=(const Vector2D& vec) {
    this->x += vec.x;
    this->y += vec.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& vec) {
    this->x -= vec.x;
    this->y -= vec.y;
    return *this;
}

// member operator function so we can use the Vector2D on the left hand side
Vector2D Vector2D::operator*(float scaler) const {
    return Vector2D(x * scaler, y * scaler);
}

Vector2D Vector2D::operator/(float scaler) const {
    return Vector2D(x / scaler, y / scaler);
}

// non-member operator function so we can make use of putting the Vector2D on the right hand side
Vector2D operator*(float scaler, Vector2D& vec) {
    return Vector2D(vec.x * scaler, vec.y * scaler);
}

// Vector2D operator/(float scaler, Vector2D& vec) {
//     return Vector2D(scaler / vec.x, scaler / vec.y);
// }

Vector2D& Vector2D::operator*=(float scaler) {
    this->x *= scaler;
    this->y *= scaler;
    return *this;
}

Vector2D& Vector2D::operator/=(float scaler) {
    this->x /= scaler;
    this->y /= scaler;
    return *this;
}

bool Vector2D::operator==(const Vector2D& other) const {
    return x == other.x && y == other.y;
}
bool Vector2D::operator!=(const Vector2D& other) const {
    return x != other.x || y != other.y;
}

Vector2D& Vector2D::operator-() {
    this->x = -this->x;
    this->y = -this->y;
    return *this;
}



Vector2D& Vector2D::normalize() {
    // pythagorean theorem
    float length = sqrt(x * x + y * y);

    // if length was 4.4
    // x = x/4.4
    // y = y/4.4
    if (length > 0.0f) {
        this->x /= length;
        this->y /= length;
    }
    return *this;
}

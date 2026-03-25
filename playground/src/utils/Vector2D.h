//
// Created by adamd on 2026-01-28.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_VECTOR2D_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_VECTOR2D_H

class Vector2D {
public:
    float x = 0.0f;
    float y = 0.0f;

    Vector2D() {
        x = 0.0f;
        y = 0.0f;
    }

    Vector2D(float x, float y) : x(x), y(y) {}

    Vector2D operator+(const Vector2D& vec) const;
    Vector2D operator-(const Vector2D& vec) const;

    // member operator function (+=)
    // Vector that we want to return is on the left
    // another vector is on the right
    Vector2D& operator+=(const Vector2D& vec);
    Vector2D& operator-=(const Vector2D& vec);

    // member operator function (*)
    // Vector2D has to be on the left, float on the right
    Vector2D operator*(float scaler) const;
    Vector2D operator/(float scaler) const;

    Vector2D& operator*=(float scaler);
    Vector2D& operator/=(float scaler);

    // Vector2D is on the right, and the float is on the left?
    // friend makes it a non-member function
    friend Vector2D operator*(float scaler, Vector2D& vec);
    //friend Vector2D operator/(float scaler, Vector2D& vec);

    bool operator==(const Vector2D& other) const;
    bool operator!=(const Vector2D& other) const;

    Vector2D& operator-();

    Vector2D& normalize();
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_VECTOR2D_H
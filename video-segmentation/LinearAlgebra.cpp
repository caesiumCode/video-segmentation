//
//  LinearAlgebra.cpp
//  video-segmentation
//
//  Created by Stephen Jaud on 29/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#include "LinearAlgebra.hpp"

// - - - - - Vector3 - - - - -
Vector3::Vector3() {
    x = 0.;
    y = 0.;
    z = 0.;
}
Vector3::Vector3(float a, float b, float c) {
    x = a;
    y = b;
    z = c;
}
Vector3::Vector3(sf::Vector3i u) {
    x = (float) u.x;
    y = (float) u.y;
    z = (float) u.z;
}
Vector3::Vector3(sf::Vector3f u) {
    x = u.x;
    y = u.y;
    z = u.z;
}
Vector3::Vector3(sf::Color col) {
    x = (float) col.r;
    y = (float) col.g;
    z = (float) col.b;
}

Matrix3 outerp(Vector3 u) {
    return Matrix3(Vector3(u.x*u.x, u.x*u.y, u.x*u.z),
                   Vector3(u.y*u.x, u.y*u.y, u.y*u.z),
                   Vector3(u.z*u.x, u.z*u.y, u.z*u.z));
}

sf::Color Vector3::toColor() {
    return sf::Color((int) x, (int) y, (int) z);
}

// Overload operations
Vector3 operator+(const Vector3 & u, const Vector3 & v) {
    return Vector3(u.x + v.x, u.y + v.y, u.z + v.z);
}

Vector3 operator-(const Vector3 & u, const Vector3 & v) {
    return Vector3(u.x - v.x, u.y - v.y, u.z - v.z);
}

float operator*(const Vector3 & u, const Vector3 & v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

Vector3 operator*(float a, const Vector3 & u) {
    return Vector3(a * u.x, a * u.y, a * u.z);
}

// - - - - - Matrix3 - - - - -
Matrix3::Matrix3() {
    x = Vector3::Zeros();
    y = Vector3::Zeros();
    z = Vector3::Zeros();
}

Matrix3::Matrix3(Vector3 u, Vector3 v, Vector3 w) {
    x = u;
    y = v;
    z = w;
}

Matrix3 Matrix3::inverse() {
    float det = x.x*(y.y*z.z - y.z*z.y) - x.y*(y.x*z.z - y.z*z.x) + x.z*(y.x*z.y - y.y*z.x);
    
    if (det == 0) {
        //std::cout << "ERROR: Inverse matrix of rank 0\n";
        return Matrix3::Zeros();
    } else {
        return 1./det * Matrix3(Vector3(y.y*z.z - y.z*z.y, x.z*z.y - x.y*z.z, x.y*y.z - x.z*y.y),
                                Vector3(y.z*z.x - y.x*z.z, x.x*z.z - x.z*z.x, x.z*y.x - x.x*y.z),
                                Vector3(y.x*z.y - y.y*z.x, x.y*z.x - x.x*z.y, x.x*y.y - x.y*y.x));
    }
}

// Overload operations
Vector3 operator*(const Matrix3 & M, const Vector3 & u) {
    return Vector3(M.x * u, M.y * u, M.z * u);
}

Matrix3 operator*(float a, const Matrix3 & M) {
    return Matrix3(a * M.x, a * M.y, a * M.z);
}

Matrix3 operator+(const Matrix3 & A, const Matrix3 & B) {
    return Matrix3(A.x + B.x, A.y + B.y, A.z + B.z);
}

Matrix3 operator-(const Matrix3 & A, const Matrix3 & B) {
    return Matrix3(A.x - B.x, A.y - B.y, A.z - B.z);
}

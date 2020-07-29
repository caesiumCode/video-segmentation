//
//  LinearAlgebra.hpp
//  video-segmentation
//
//  Created by Stephen Jaud on 29/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#ifndef LinearAlgebra_hpp
#define LinearAlgebra_hpp

#include <iostream>

#include <SFML/Graphics.hpp>

class Vector3;
class Matrix3;

// - - - - - Vector3 - - - - -
class Vector3 {
public:
    Vector3();
    Vector3(float, float, float);
    Vector3(sf::Vector3i);
    Vector3(sf::Vector3f);
    Vector3(sf::Color);
    
    float x, y, z;
    
    static Vector3 Zeros() {
        return Vector3(0, 0, 0);
    }
    static Vector3 Ones() {
        return Vector3(1, 1, 1);
    }
    
    Matrix3 outerp();
};

// Overload operations
Vector3 operator+(const Vector3 &, const Vector3 &);
Vector3 operator-(const Vector3 &, const Vector3 &);
float operator*(const Vector3 &, const Vector3 &);
Vector3 operator*(float, const Vector3 &);


// - - - - - Matrix3 - - - - -
class Matrix3 {
public:
    Matrix3();
    Matrix3(Vector3, Vector3, Vector3);
    
    Vector3 x, y, z;
    
    static Matrix3 Zeros() {
        return Matrix3(Vector3::Zeros(), Vector3::Zeros(), Vector3::Zeros());
    }
    static Matrix3 Ones() {
        return Matrix3(Vector3::Ones(), Vector3::Ones(), Vector3::Ones());
    }
    static Matrix3 Eye() {
        return Matrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1));
    }
    
    Matrix3 inverse();
};

// Overload operations
Vector3 operator*(const Matrix3 &, const Vector3 &);
Matrix3 operator*(float, const Matrix3 &);
Matrix3 operator+(const Matrix3 &, const Matrix3 &);
Matrix3 operator-(const Matrix3 &, const Matrix3 &);

#endif /* LinearAlgebra_hpp */

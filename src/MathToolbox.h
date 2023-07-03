/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "GPUPixelDef.h"

NS_GPUPIXEL_BEGIN

class Vector2 {
 public:
  float x;
  float y;

  Vector2();
  Vector2(float xx, float yy);
  Vector2(const float* array);
  Vector2(const Vector2& p1, const Vector2& p2);
  Vector2(const Vector2& copy);
  ~Vector2();

  bool isZero() const;
  bool isOne() const;
  static float angle(const Vector2& v1, const Vector2& v2);
  void add(const Vector2& v);
  static void add(const Vector2& v1, const Vector2& v2, Vector2* dst);
  void clamp(const Vector2& min, const Vector2& max);
  static void clamp(const Vector2& v,
                    const Vector2& min,
                    const Vector2& max,
                    Vector2* dst);
  float distance(const Vector2& v) const;
  float distanceSquared(const Vector2& v) const;
  float dot(const Vector2& v) const;
  static float dot(const Vector2& v1, const Vector2& v2);
  float length() const;
  float lengthSquared() const;
  void negate();
  void normalize();
  Vector2 getNormalized() const;
  void scale(float scalar);
  void scale(const Vector2& scale);
  void rotate(const Vector2& point, float angle);
  void set(float xx, float yy);
  void set(const Vector2& v);
  void set(const Vector2& p1, const Vector2& p2);
  void setZero();
  void subtract(const Vector2& v);
  static void subtract(const Vector2& v1, const Vector2& v2, Vector2* dst);
  void smooth(const Vector2& target, float elapsedTime, float responseTime);
  const Vector2 operator+(const Vector2& v) const;
  Vector2& operator+=(const Vector2& v);
  const Vector2 operator-(const Vector2& v) const;
  Vector2& operator-=(const Vector2& v);
  const Vector2 operator-() const;
  const Vector2 operator*(float s) const;
  Vector2& operator*=(float s);
  const Vector2 operator/(float s) const;
  bool operator<(const Vector2& v) const;
  bool operator>(const Vector2& v) const;
  bool operator==(const Vector2& v) const;
  bool operator!=(const Vector2& v) const;
};

class Vector3 {};

class Matrix4 {
 public:
  float m[16];
  Matrix4();
  Matrix4(const float* mat);
  Matrix4(float m11,
          float m12,
          float m13,
          float m14,
          float m21,
          float m22,
          float m23,
          float m24,
          float m31,
          float m32,
          float m33,
          float m34,
          float m41,
          float m42,
          float m43,
          float m44);
  Matrix4(const Matrix4& copy);
  ~Matrix4();

  void set(float m11,
           float m12,
           float m13,
           float m14,
           float m21,
           float m22,
           float m23,
           float m24,
           float m31,
           float m32,
           float m33,
           float m34,
           float m41,
           float m42,
           float m43,
           float m44);
  void set(const float* mat);
  void set(const Matrix4& mat);
  void setIdentity();

  void negate();
  Matrix4 getNegated() const;

  void transpose();
  Matrix4 getTransposed() const;

  void add(float scalar);
  void add(float scalar, Matrix4* dst) const;
  void add(const Matrix4& mat);
  static void add(const Matrix4& m1, const Matrix4& m2, Matrix4* dst);

  void subtract(const Matrix4& mat);
  static void subtract(const Matrix4& m1, const Matrix4& m2, Matrix4* dst);

  void multiply(float scalar);
  void multiply(float scalar, Matrix4* dst) const;
  static void multiply(const Matrix4& mat, float scalar, Matrix4* dst);
  void multiply(const Matrix4& mat);
  static void multiply(const Matrix4& m1, const Matrix4& m2, Matrix4* dst);

  const Matrix4 operator+(const Matrix4& mat) const;
  Matrix4& operator+=(const Matrix4& mat);
  const Matrix4 operator-(const Matrix4& mat) const;
  Matrix4& operator-=(const Matrix4& mat);
  const Matrix4 operator-() const;
  const Matrix4 operator*(const Matrix4& mat) const;
  Matrix4& operator*=(const Matrix4& mat);

  const Matrix4 operator+(float scalar) const;
  Matrix4& operator+=(float scalar);
  const Matrix4 operator-(float scalar) const;
  Matrix4& operator-=(float scalar);
  const Matrix4 operator*(float scalar) const;
  Matrix4& operator*=(float scalar);

  static const Matrix4 IDENTITY;
};

class Matrix3 {
 public:
  float m[9];
  Matrix3();
  Matrix3(const float* mat);
  Matrix3(float m11,
          float m12,
          float m13,
          float m21,
          float m22,
          float m23,
          float m31,
          float m32,
          float m33);
  Matrix3(const Matrix3& copy);
  ~Matrix3();

  void set(float m11,
           float m12,
           float m13,
           float m21,
           float m22,
           float m23,
           float m31,
           float m32,
           float m33);
  void set(const float* mat);
  void set(const Matrix3& mat);
  void setIdentity();

  void negate();
  Matrix3 getNegated() const;

  void transpose();
  Matrix3 getTransposed() const;

  void add(float scalar);
  void add(float scalar, Matrix3* dst) const;
  void add(const Matrix3& mat);
  static void add(const Matrix3& m1, const Matrix3& m2, Matrix3* dst);

  void subtract(const Matrix3& mat);
  static void subtract(const Matrix3& m1, const Matrix3& m2, Matrix3* dst);

  void multiply(float scalar);
  void multiply(float scalar, Matrix3* dst) const;
  static void multiply(const Matrix3& mat, float scalar, Matrix3* dst);
  void multiply(const Matrix3& mat);
  static void multiply(const Matrix3& m1, const Matrix3& m2, Matrix3* dst);

  const Matrix3 operator+(const Matrix3& mat) const;
  Matrix3& operator+=(const Matrix3& mat);
  const Matrix3 operator-(const Matrix3& mat) const;
  Matrix3& operator-=(const Matrix3& mat);
  const Matrix3 operator-() const;
  const Matrix3 operator*(const Matrix3& mat) const;
  Matrix3& operator*=(const Matrix3& mat);

  const Matrix3 operator+(float scalar) const;
  Matrix3& operator+=(float scalar);
  const Matrix3 operator-(float scalar) const;
  Matrix3& operator-=(float scalar);
  const Matrix3 operator*(float scalar) const;
  Matrix3& operator*=(float scalar);

  static const Matrix3 IDENTITY;
};

NS_GPUPIXEL_END

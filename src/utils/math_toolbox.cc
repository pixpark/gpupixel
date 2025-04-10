/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/utils/math_toolbox.h"
#include <assert.h>
#include <string>

namespace gpupixel {

Vector2::Vector2() : x(0.0f), y(0.0f) {}

Vector2::Vector2(float xx, float yy) : x(xx), y(yy) {}

Vector2::Vector2(const float* array) {
  set(array);
}

Vector2::Vector2(const Vector2& p1, const Vector2& p2) {
  set(p1, p2);
}

Vector2::Vector2(const Vector2& copy) {
  set(copy);
}

Vector2::~Vector2() {}

bool Vector2::isZero() const {
  return x == 0.0f && y == 0.0f;
}

bool Vector2::isOne() const {
  return x == 1.0f && y == 1.0f;
}

void Vector2::add(const Vector2& v) {
  x += v.x;
  y += v.y;
}

float Vector2::distanceSquared(const Vector2& v) const {
  float dx = v.x - x;
  float dy = v.y - y;
  return (dx * dx + dy * dy);
}

float Vector2::dot(const Vector2& v) const {
  return (x * v.x + y * v.y);
}

float Vector2::lengthSquared() const {
  return (x * x + y * y);
}

void Vector2::negate() {
  x = -x;
  y = -y;
}

void Vector2::scale(float scalar) {
  x *= scalar;
  y *= scalar;
}

void Vector2::scale(const Vector2& scale) {
  x *= scale.x;
  y *= scale.y;
}

void Vector2::set(float xx, float yy) {
  this->x = xx;
  this->y = yy;
}

void Vector2::set(const Vector2& v) {
  this->x = v.x;
  this->y = v.y;
}

void Vector2::set(const Vector2& p1, const Vector2& p2) {
  x = p2.x - p1.x;
  y = p2.y - p1.y;
}

void Vector2::setZero() {
  x = y = 0.0f;
}

void Vector2::subtract(const Vector2& v) {
  x -= v.x;
  y -= v.y;
}

void Vector2::smooth(const Vector2& target,
                     float elapsedTime,
                     float responseTime) {
  if (elapsedTime > 0) {
    *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
  }
}

const Vector2 Vector2::operator+(const Vector2& v) const {
  Vector2 result(*this);
  result.add(v);
  return result;
}

Vector2& Vector2::operator+=(const Vector2& v) {
  add(v);
  return *this;
}

const Vector2 Vector2::operator-(const Vector2& v) const {
  Vector2 result(*this);
  result.subtract(v);
  return result;
}

Vector2& Vector2::operator-=(const Vector2& v) {
  subtract(v);
  return *this;
}

const Vector2 Vector2::operator-() const {
  Vector2 result(*this);
  result.negate();
  return result;
}

const Vector2 Vector2::operator*(float s) const {
  Vector2 result(*this);
  result.scale(s);
  return result;
}

Vector2& Vector2::operator*=(float s) {
  scale(s);
  return *this;
}

const Vector2 Vector2::operator/(const float s) const {
  return Vector2(this->x / s, this->y / s);
}

bool Vector2::operator<(const Vector2& v) const {
  if (x == v.x) {
    return y < v.y;
  }
  return x < v.x;
}

bool Vector2::operator>(const Vector2& v) const {
  if (x == v.x) {
    return y > v.y;
  }
  return x > v.x;
}

bool Vector2::operator==(const Vector2& v) const {
  return x == v.x && y == v.y;
}

bool Vector2::operator!=(const Vector2& v) const {
  return x != v.x || y != v.y;
}

const Vector2 operator*(float x, const Vector2& v) {
  Vector2 result(v);
  result.scale(x);
  return result;
}

const Matrix4 Matrix4::IDENTITY = Matrix4(1.0f,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          1.0f,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          1.0f,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          1.0f);

Matrix4::Matrix4() {
  *this = IDENTITY;
}

Matrix4::Matrix4(const float* mat) {
  set(mat);
}

Matrix4::Matrix4(float m11,
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
                 float m44) {
  set(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43,
      m44);
}

Matrix4::Matrix4(const Matrix4& copy) {
  memcpy(m, copy.m, sizeof(float) * 16);
}

Matrix4::~Matrix4() {}

void Matrix4::set(float m11,
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
                  float m44) {
  m[0] = m11;
  m[1] = m21;
  m[2] = m31;
  m[3] = m41;
  m[4] = m12;
  m[5] = m22;
  m[6] = m32;
  m[7] = m42;
  m[8] = m13;
  m[9] = m23;
  m[10] = m33;
  m[11] = m43;
  m[12] = m14;
  m[13] = m24;
  m[14] = m34;
  m[15] = m44;
}

void Matrix4::set(const float* mat) {
  assert(mat);
  memcpy(this->m, mat, sizeof(float) * 16);
}

void Matrix4::set(const Matrix4& mat) {
  memcpy(this->m, mat.m, sizeof(float) * 16);
}

void Matrix4::setIdentity() {
  memcpy(m, &IDENTITY, sizeof(float) * 16);
}

void Matrix4::negate() {
  m[0] = -m[0];
  m[1] = -m[1];
  m[2] = -m[2];
  m[3] = -m[3];
  m[4] = -m[4];
  m[5] = -m[5];
  m[6] = -m[6];
  m[7] = -m[7];
  m[8] = -m[8];
  m[9] = -m[9];
  m[10] = -m[10];
  m[11] = -m[11];
  m[12] = -m[12];
  m[13] = -m[13];
  m[14] = -m[14];
  m[15] = -m[15];
}

Matrix4 Matrix4::getNegated() const {
  Matrix4 mat(*this);
  mat.negate();
  return mat;
}

void Matrix4::transpose() {
  float tmp;
  tmp = m[1];
  m[1] = m[4];
  m[4] = tmp;
  tmp = m[2];
  m[2] = m[8];
  m[8] = tmp;
  tmp = m[6];
  m[6] = m[9];
  m[9] = tmp;
  tmp = m[3];
  m[3] = m[12];
  m[12] = tmp;
  tmp = m[7];
  m[7] = m[13];
  m[13] = tmp;
  tmp = m[11];
  m[11] = m[14];
  m[14] = tmp;
}

Matrix4 Matrix4::getTransposed() const {
  Matrix4 mat(*this);
  mat.transpose();
  return mat;
}

void Matrix4::add(float scalar) {
  add(scalar, this);
}

void Matrix4::add(float scalar, Matrix4* dst) const {
  assert(dst);
  dst->m[0] = this->m[0] + scalar;
  dst->m[1] = this->m[1] + scalar;
  dst->m[2] = this->m[2] + scalar;
  dst->m[3] = this->m[3] + scalar;
  dst->m[4] = this->m[4] + scalar;
  dst->m[5] = this->m[5] + scalar;
  dst->m[6] = this->m[6] + scalar;
  dst->m[7] = this->m[7] + scalar;
  dst->m[8] = this->m[8] + scalar;
  dst->m[9] = this->m[9] + scalar;
  dst->m[10] = this->m[10] + scalar;
  dst->m[11] = this->m[11] + scalar;
  dst->m[12] = this->m[12] + scalar;
  dst->m[13] = this->m[13] + scalar;
  dst->m[14] = this->m[14] + scalar;
  dst->m[15] = this->m[15] + scalar;
}

void Matrix4::add(const Matrix4& mat) {
  add(*this, mat, this);
}

void Matrix4::add(const Matrix4& m1, const Matrix4& m2, Matrix4* dst) {
  assert(dst);
  dst->m[0] = m1.m[0] + m2.m[0];
  dst->m[1] = m1.m[1] + m2.m[1];
  dst->m[2] = m1.m[2] + m2.m[2];
  dst->m[3] = m1.m[3] + m2.m[3];
  dst->m[4] = m1.m[4] + m2.m[4];
  dst->m[5] = m1.m[5] + m2.m[5];
  dst->m[6] = m1.m[6] + m2.m[6];
  dst->m[7] = m1.m[7] + m2.m[7];
  dst->m[8] = m1.m[8] + m2.m[8];
  dst->m[9] = m1.m[9] + m2.m[9];
  dst->m[10] = m1.m[10] + m2.m[10];
  dst->m[11] = m1.m[11] + m2.m[11];
  dst->m[12] = m1.m[12] + m2.m[12];
  dst->m[13] = m1.m[13] + m2.m[13];
  dst->m[14] = m1.m[14] + m2.m[14];
  dst->m[15] = m1.m[15] + m2.m[15];
}

void Matrix4::subtract(const Matrix4& mat) {
  subtract(*this, mat, this);
}

void Matrix4::subtract(const Matrix4& m1, const Matrix4& m2, Matrix4* dst) {
  assert(dst);
  dst->m[0] = m1.m[0] - m2.m[0];
  dst->m[1] = m1.m[1] - m2.m[1];
  dst->m[2] = m1.m[2] - m2.m[2];
  dst->m[3] = m1.m[3] - m2.m[3];
  dst->m[4] = m1.m[4] - m2.m[4];
  dst->m[5] = m1.m[5] - m2.m[5];
  dst->m[6] = m1.m[6] - m2.m[6];
  dst->m[7] = m1.m[7] - m2.m[7];
  dst->m[8] = m1.m[8] - m2.m[8];
  dst->m[9] = m1.m[9] - m2.m[9];
  dst->m[10] = m1.m[10] - m2.m[10];
  dst->m[11] = m1.m[11] - m2.m[11];
  dst->m[12] = m1.m[12] - m2.m[12];
  dst->m[13] = m1.m[13] - m2.m[13];
  dst->m[14] = m1.m[14] - m2.m[14];
  dst->m[15] = m1.m[15] - m2.m[15];
}

void Matrix4::multiply(float scalar) {
  multiply(scalar, this);
}

void Matrix4::multiply(float scalar, Matrix4* dst) const {
  multiply(*this, scalar, dst);
}

void Matrix4::multiply(const Matrix4& mat, float scalar, Matrix4* dst) {
  assert(dst);
  dst->m[0] = mat.m[0] * scalar;
  dst->m[1] = mat.m[1] * scalar;
  dst->m[2] = mat.m[2] * scalar;
  dst->m[3] = mat.m[3] * scalar;
  dst->m[4] = mat.m[4] * scalar;
  dst->m[5] = mat.m[5] * scalar;
  dst->m[6] = mat.m[6] * scalar;
  dst->m[7] = mat.m[7] * scalar;
  dst->m[8] = mat.m[8] * scalar;
  dst->m[9] = mat.m[9] * scalar;
  dst->m[10] = mat.m[10] * scalar;
  dst->m[11] = mat.m[11] * scalar;
  dst->m[12] = mat.m[12] * scalar;
  dst->m[13] = mat.m[13] * scalar;
  dst->m[14] = mat.m[14] * scalar;
  dst->m[15] = mat.m[15] * scalar;
}

void Matrix4::multiply(const Matrix4& mat) {
  multiply(*this, mat, this);
}

void Matrix4::multiply(const Matrix4& m1, const Matrix4& m2, Matrix4* dst) {
  assert(dst);
  dst->m[0] = m1.m[0] * m2.m[0];
  dst->m[1] = m1.m[1] * m2.m[1];
  dst->m[2] = m1.m[2] * m2.m[2];
  dst->m[3] = m1.m[3] * m2.m[3];
  dst->m[4] = m1.m[4] * m2.m[4];
  dst->m[5] = m1.m[5] * m2.m[5];
  dst->m[6] = m1.m[6] * m2.m[6];
  dst->m[7] = m1.m[7] * m2.m[7];
  dst->m[8] = m1.m[8] * m2.m[8];
  dst->m[9] = m1.m[9] * m2.m[9];
  dst->m[10] = m1.m[10] * m2.m[10];
  dst->m[11] = m1.m[11] * m2.m[11];
  dst->m[12] = m1.m[12] * m2.m[12];
  dst->m[13] = m1.m[13] * m2.m[13];
  dst->m[14] = m1.m[14] * m2.m[14];
  dst->m[15] = m1.m[15] * m2.m[15];
}

const Matrix4 Matrix4::operator+(const Matrix4& mat) const {
  Matrix4 result(*this);
  result.add(mat);
  return result;
}

Matrix4& Matrix4::operator+=(const Matrix4& mat) {
  add(mat);
  return *this;
}

const Matrix4 Matrix4::operator-(const Matrix4& mat) const {
  Matrix4 result(*this);
  result.subtract(mat);
  return result;
}

Matrix4& Matrix4::operator-=(const Matrix4& mat) {
  subtract(mat);
  return *this;
}

const Matrix4 Matrix4::operator-() const {
  Matrix4 mat(*this);
  mat.negate();
  return mat;
}

const Matrix4 Matrix4::operator*(const Matrix4& mat) const {
  Matrix4 result(*this);
  result.multiply(mat);
  return result;
}

Matrix4& Matrix4::operator*=(const Matrix4& mat) {
  multiply(mat);
  return *this;
}

const Matrix4 Matrix4::operator+(float scalar) const {
  Matrix4 result(*this);
  result.add(scalar);
  return result;
}

Matrix4& Matrix4::operator+=(float scalar) {
  add(scalar);
  return *this;
}

const Matrix4 Matrix4::operator-(float scalar) const {
  Matrix4 result(*this);
  result.add(-scalar);
  return result;
}

Matrix4& Matrix4::operator-=(float scalar) {
  add(-scalar);
  return *this;
}

const Matrix4 Matrix4::operator*(float scalar) const {
  Matrix4 result(*this);
  result.multiply(scalar);
  return result;
}

Matrix4& Matrix4::operator*=(float scalar) {
  multiply(scalar);
  return *this;
}

const Matrix3 Matrix3::IDENTITY =
    Matrix3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
Matrix3::Matrix3() {
  *this = IDENTITY;
}

Matrix3::Matrix3(const float* mat) {
  set(mat);
}

Matrix3::Matrix3(float m11,
                 float m12,
                 float m13,
                 float m21,
                 float m22,
                 float m23,
                 float m31,
                 float m32,
                 float m33) {
  set(m11, m12, m13, m21, m22, m23, m31, m32, m33);
}

Matrix3::Matrix3(const Matrix3& copy) {
  memcpy(m, copy.m, sizeof(float) * 9);
}

Matrix3::~Matrix3() {}

void Matrix3::set(float m11,
                  float m12,
                  float m13,
                  float m21,
                  float m22,
                  float m23,
                  float m31,
                  float m32,
                  float m33) {
  m[0] = m11;
  m[1] = m21;
  m[2] = m31;
  m[3] = m12;
  m[4] = m22;
  m[5] = m32;
  m[6] = m13;
  m[7] = m23;
  m[8] = m33;
}

void Matrix3::set(const float* mat) {
  assert(mat);
  memcpy(this->m, mat, sizeof(float) * 9);
}

void Matrix3::set(const Matrix3& mat) {
  memcpy(this->m, mat.m, sizeof(float) * 9);
}

void Matrix3::setIdentity() {
  memcpy(m, &IDENTITY, sizeof(float) * 9);
}

void Matrix3::negate() {
  m[0] = -m[0];
  m[1] = -m[1];
  m[2] = -m[2];
  m[3] = -m[3];
  m[4] = -m[4];
  m[5] = -m[5];
  m[6] = -m[6];
  m[7] = -m[7];
  m[8] = -m[8];
}

Matrix3 Matrix3::getNegated() const {
  Matrix3 mat(*this);
  mat.negate();
  return mat;
}

void Matrix3::transpose() {
  float tmp;
  tmp = m[1];
  m[1] = m[3];
  m[3] = tmp;
  tmp = m[2];
  m[2] = m[6];
  m[6] = tmp;
  tmp = m[5];
  m[5] = m[7];
  m[7] = tmp;
}

Matrix3 Matrix3::getTransposed() const {
  Matrix3 mat(*this);
  mat.transpose();
  return mat;
}

void Matrix3::add(float scalar) {
  add(scalar, this);
}

void Matrix3::add(float scalar, Matrix3* dst) const {
  assert(dst);
  dst->m[0] = this->m[0] + scalar;
  dst->m[1] = this->m[1] + scalar;
  dst->m[2] = this->m[2] + scalar;
  dst->m[3] = this->m[3] + scalar;
  dst->m[4] = this->m[4] + scalar;
  dst->m[5] = this->m[5] + scalar;
  dst->m[6] = this->m[6] + scalar;
  dst->m[7] = this->m[7] + scalar;
  dst->m[8] = this->m[8] + scalar;
}

void Matrix3::add(const Matrix3& mat) {
  add(*this, mat, this);
}

void Matrix3::add(const Matrix3& m1, const Matrix3& m2, Matrix3* dst) {
  assert(dst);
  dst->m[0] = m1.m[0] + m2.m[0];
  dst->m[1] = m1.m[1] + m2.m[1];
  dst->m[2] = m1.m[2] + m2.m[2];
  dst->m[3] = m1.m[3] + m2.m[3];
  dst->m[4] = m1.m[4] + m2.m[4];
  dst->m[5] = m1.m[5] + m2.m[5];
  dst->m[6] = m1.m[6] + m2.m[6];
  dst->m[7] = m1.m[7] + m2.m[7];
  dst->m[8] = m1.m[8] + m2.m[8];
}

void Matrix3::subtract(const Matrix3& mat) {
  subtract(*this, mat, this);
}

void Matrix3::subtract(const Matrix3& m1, const Matrix3& m2, Matrix3* dst) {
  assert(dst);
  dst->m[0] = m1.m[0] - m2.m[0];
  dst->m[1] = m1.m[1] - m2.m[1];
  dst->m[2] = m1.m[2] - m2.m[2];
  dst->m[3] = m1.m[3] - m2.m[3];
  dst->m[4] = m1.m[4] - m2.m[4];
  dst->m[5] = m1.m[5] - m2.m[5];
  dst->m[6] = m1.m[6] - m2.m[6];
  dst->m[7] = m1.m[7] - m2.m[7];
  dst->m[8] = m1.m[8] - m2.m[8];
}

void Matrix3::multiply(float scalar) {
  multiply(scalar, this);
}

void Matrix3::multiply(float scalar, Matrix3* dst) const {
  multiply(*this, scalar, dst);
}

void Matrix3::multiply(const Matrix3& mat, float scalar, Matrix3* dst) {
  assert(dst);
  dst->m[0] = mat.m[0] * scalar;
  dst->m[1] = mat.m[1] * scalar;
  dst->m[2] = mat.m[2] * scalar;
  dst->m[3] = mat.m[3] * scalar;
  dst->m[4] = mat.m[4] * scalar;
  dst->m[5] = mat.m[5] * scalar;
  dst->m[6] = mat.m[6] * scalar;
  dst->m[7] = mat.m[7] * scalar;
  dst->m[8] = mat.m[8] * scalar;
}

void Matrix3::multiply(const Matrix3& mat) {
  multiply(*this, mat, this);
}

void Matrix3::multiply(const Matrix3& m1, const Matrix3& m2, Matrix3* dst) {
  assert(dst);
  dst->m[0] = m1.m[0] * m2.m[0];
  dst->m[1] = m1.m[1] * m2.m[1];
  dst->m[2] = m1.m[2] * m2.m[2];
  dst->m[3] = m1.m[3] * m2.m[3];
  dst->m[4] = m1.m[4] * m2.m[4];
  dst->m[5] = m1.m[5] * m2.m[5];
  dst->m[6] = m1.m[6] * m2.m[6];
  dst->m[7] = m1.m[7] * m2.m[7];
  dst->m[8] = m1.m[8] * m2.m[8];
}

const Matrix3 Matrix3::operator+(const Matrix3& mat) const {
  Matrix3 result(*this);
  result.add(mat);
  return result;
}

Matrix3& Matrix3::operator+=(const Matrix3& mat) {
  add(mat);
  return *this;
}

const Matrix3 Matrix3::operator-(const Matrix3& mat) const {
  Matrix3 result(*this);
  result.subtract(mat);
  return result;
}

Matrix3& Matrix3::operator-=(const Matrix3& mat) {
  subtract(mat);
  return *this;
}

const Matrix3 Matrix3::operator-() const {
  Matrix3 mat(*this);
  mat.negate();
  return mat;
}

const Matrix3 Matrix3::operator*(const Matrix3& mat) const {
  Matrix3 result(*this);
  result.multiply(mat);
  return result;
}

Matrix3& Matrix3::operator*=(const Matrix3& mat) {
  multiply(mat);
  return *this;
}

const Matrix3 Matrix3::operator+(float scalar) const {
  Matrix3 result(*this);
  result.add(scalar);
  return result;
}

Matrix3& Matrix3::operator+=(float scalar) {
  add(scalar);
  return *this;
}

const Matrix3 Matrix3::operator-(float scalar) const {
  Matrix3 result(*this);
  result.add(-scalar);
  return result;
}

Matrix3& Matrix3::operator-=(float scalar) {
  add(-scalar);
  return *this;
}

const Matrix3 Matrix3::operator*(float scalar) const {
  Matrix3 result(*this);
  result.multiply(scalar);
  return result;
}

Matrix3& Matrix3::operator*=(float scalar) {
  multiply(scalar);
  return *this;
}

}  // namespace gpupixel

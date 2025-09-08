#pragma once
#include "math.h"
#include "cprocessing.h"

const float	EPSILON = 0.00001f;

typedef struct vector2 {
	float x, y;
} Vector;

float v_dot(Vector* base, Vector* other) {
	return (base->x * other->x) + (base->y * other->y);
}

Vector v_cross(Vector* base, Vector* other) {
	Vector a = { base->y * other->x, base->x * other->y };
	return a;
}

Vector v_mul(Vector* base, float scalar) {
	Vector a = { base->x * scalar, base->y *= scalar };
	return a;
}

Vector v_add(Vector* base, Vector* other) {
	Vector a = { base->x + other->x, base->y + other->y };
	return a;
}

float v_length_squared(Vector* vector) {
	return pow(vector->x, 2) + pow(vector->y, 2);
}

float v_length(Vector* vector) {
	return sqrt(v_length_squared(vector));
}

Vector v_direction(Vector* origin, Vector* target) {
	Vector a = { target->x - origin->x, target->y - origin->y };
	return a;
}

void v_normalize(Vector* vector) {
	float d = v_length(vector);
	vector->x /= d;
	vector->y /= d;
}

CP_Matrix set_ortho(double left, double right, double bottom, double top, double nearVal, double farVal) {
	CP_Matrix mat = { 
		2 / (float)(right - left), 0, 0,
		0, 2 / (float)(top - bottom), 0,
		0, 0, -2 / (float)(farVal - nearVal) 
	};
	return mat;
}

#pragma once

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

typedef struct {
	int rows, columns;
	double *matrix;
} MATRIX;

typedef struct {
	int degree;
	double *equation;
} SPLINE;

SPLINE spline_create(int set_count, double *sets);
double spline_get(SPLINE *spline, double x);
MATRIX matrix_identity(MATRIX *m);
MATRIX matrix_multiply(MATRIX *m1, MATRIX *m2);
MATRIX matrix_inverse(MATRIX *m);
void matrix_print(MATRIX *m);
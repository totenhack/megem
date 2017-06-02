#include "stdafx.h"

SPLINE spline_create(unsigned int set_count, double *sets) {
	if (set_count <= 0) return{ 0 };

	double *variables = (double *)malloc((set_count * set_count) * sizeof(double));
	double *constants = (double *)malloc(set_count * sizeof(double));

	for (unsigned int i = 0, s = 0; i < set_count; i++) {
		double x = sets[s++];

		for (unsigned int e = 0, b = i * set_count; e < set_count; e++) {
			variables[b + e] = pow(x, e);
		}

		constants[i] = sets[s++];
	}

	MATRIX variable_matrix = { (int)set_count, (int)set_count, variables };
	MATRIX constant_matrix = { (int)set_count, 1, constants };

	MATRIX inverse = matrix_inverse(&variable_matrix);
	free(variables);

	variable_matrix = matrix_multiply(&inverse, &constant_matrix);
	free(inverse.matrix);
	free(constants);

	return{ (unsigned int)variable_matrix.rows, variable_matrix.matrix };
}

double spline_get(SPLINE *spline, double x) {
	double r = 0;

	for (unsigned int i = 0; i < spline->degree; i++) {
		r += pow(x, i) * spline->equation[i];
	}

	return r;
}

void spline_print(SPLINE *spline) {
	printf("y = ");
	for (unsigned int i = 0; i < spline->degree; i++) {
		printf("%lfx^%d + ", spline->equation[i], i);
	}
	printf("\b\b \n");
}

void matrix_print(MATRIX *m) {
	printf("[\n  ");

	for (int r = 0; r < m->rows; r++) {
		for (int c = 0; c < m->columns; c++) {
			printf("%lf ", m->matrix[(r * m->columns) + c]);
		}

		printf("\n  ");
	}

	printf("\b\b]\n");
}

MATRIX matrix_identity(MATRIX *m) {
	if (m->rows != m->columns) return{ 0, 0, 0 };

	MATRIX r;
	r.rows = r.columns = m->rows;

	int size = (r.rows * r.columns) * sizeof(double);

	r.matrix = (double *)malloc(size);
	memset(r.matrix, 0, size);

	for (int i = 0; i < r.rows; i++) {
		r.matrix[(i * r.columns) + i] = 1;
	}

	return r;
}

MATRIX matrix_multiply(MATRIX *m1, MATRIX *m2) {
	if (m1->columns != m2->rows) return{ 0, 0, 0 };

	MATRIX m;
	m.rows = m1->rows;
	m.columns = m2->columns;
	m.matrix = (double *)malloc(m.rows * m.columns * sizeof(double));

	for (int r = 0; r < m1->rows; r++) {
		for (int c = 0; c < m2->columns; c++) {
			double a = 0;

			for (int i = 0; i < m1->columns; i++) {
				a += m1->matrix[(r * m1->columns) + i] * m2->matrix[c + (i * m2->columns)];
			}

			m.matrix[(r * m.columns) + c] = a;
		}
	}

	return m;
}

MATRIX matrix_inverse(MATRIX *m) {
	double *identity = matrix_identity(m).matrix;
	if (!identity) {
		return{ 0, 0, 0 };
	}

	int size = m->rows;
	double *matrix = (double *)malloc((size * size) * sizeof(double));
	memcpy(matrix, m->matrix, size * size * sizeof(double));

	for (int i = 0; i < size; i++) {
		double a = matrix[(i * size) + i];

		for (int e = 0; e < size; e++) {
			matrix[(i * size) + e] /= a;
			identity[(i * size) + e] /= a;
		}

		for (int k = i + 1; k < size; k++) {
			double a = matrix[(k * size) + i];

			for (int e = 0; e < size; e++) {
				matrix[(k * size) + e] -= matrix[(i * size) + e] * a;
				identity[(k * size) + e] -= identity[(i * size) + e] * a;
			}
		}
	}

	for (int k = size - 1; k > 0; k--) {
		for (int i = k - 1; i >= 0; i--) {
			double a = matrix[(i * size) + k];

			for (int j = 0; j < size; j++) {
				matrix[(i * size) + j] -= matrix[(k * size) + j] * a;
				identity[(i * size) + j] -= identity[(k * size) + j] * a;
			}
		}
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matrix[(i * size) + j] = identity[(i * size) + j];
		}
	}

	free(identity);

	return{ size, size, matrix };
}


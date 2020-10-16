#ifndef __IIR_FLOAT_H_
#define __IIR_FLOAT_H_

#include "math.h"
#include "stdio.h"

#ifndef M_PI
#define M_PI (3.141592654)
#endif

typedef struct {
	float a0;
	float a1;
	float a2;

	float b0;
	float b1;
	float b2;
}iir_coeffs;

typedef struct {
	float prev_in_1;
	float prev_in_2;

	float prev_out_1;
	float prev_out_2;
}prev_states;

void iir_process(iir_coeffs* coeffs, prev_states* prev_st, FILE* source, FILE* dest);
void calc_coeffs(iir_coeffs* coeffs, float freq, float sampRate, float Q);
void init_states_iir(prev_states* prev_st);
void init_coeff_iir(iir_coeffs* coeffs);

#endif // !__FIR_FLOAT_H_

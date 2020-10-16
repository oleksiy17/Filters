#include "iir_float.h"

void init_coeff_iir(iir_coeffs* coeffs)
{
	coeffs->a0 = 0.0;
	coeffs->a1 = 0.0;
	coeffs->a2 = 0.0;

	coeffs->b0 = 0.0;
	coeffs->b1 = 0.0;
	coeffs->b2 = 0.0;
}

void init_states_iir(prev_states *prev_st)
{
	prev_st->prev_in_1 = 0.0;
	prev_st->prev_in_2 = 0.0;

	prev_st->prev_out_1 = 0.0;
	prev_st->prev_out_2 = 0.0;
}


void calc_coeffs(iir_coeffs* coeffs, float freq, float sampRate, float Q)
{
	float alpha;
	float omega;

	omega = (2 * M_PI * freq) / sampRate;
	alpha = sinf(omega) / (2 * Q);

	coeffs->a0 = 1.0 + alpha;
	coeffs->a1 = (-2.0) * cosf(omega);
	coeffs->a2 = 1.0 - alpha;

	coeffs->b0 = 1.0;
	coeffs->b1 = (-2.0) * cosf(omega);
	coeffs->b2 = 1.0;

	/*coeffs->a0 = 1.0035642544322675569;
	coeffs->a1 = -1.9987266078;
	coeffs->a2 = 0.9964357455677324431;

	coeffs->b0 = 1.0;
	coeffs->b1 = -1.9987266078;
	coeffs->b2 = 1.0;*/
}

void iir_process(iir_coeffs* coeffs, prev_states* prev_st, FILE* source, FILE* dest)
{
	size_t numRead;
	size_t numWrite;
	size_t seek;
	float state;
	float accum;

	
	numRead = fread(&state, sizeof(float), 1, source);
	seek = fseek(source, sizeof(float), SEEK_CUR);

	accum = (coeffs->b0 * state) + (coeffs->b1 * prev_st->prev_in_1) + (coeffs->b2 * prev_st->prev_in_2) - (coeffs->a1 * prev_st->prev_out_1) - (coeffs->a2 * prev_st->prev_out_2);

	prev_st->prev_in_2 = prev_st->prev_in_1;
	prev_st->prev_in_1 = state;

	prev_st->prev_out_2 = prev_st->prev_out_1;
	prev_st->prev_out_1 = accum;

	numWrite = fwrite(&accum, sizeof(float), 1, dest);
	numWrite = fwrite(&accum, sizeof(float), 1, dest);
}


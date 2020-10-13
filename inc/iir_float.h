#ifndef __IIR_FLOAT_H_
#define __IIR_FLOAT_H_

typedef struct {
    float a0;
    float a1;
    float a2;

    float b0;
    float b1;
    float b2;
}iir_coeff;

void IIR_tration()

//void IIR_coeff(iir_coeff* coeff);

#endif // !__IIR_FLOAT_H_

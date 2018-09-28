#ifndef LIGHTBLAS_MATH_FUNCTIONS_H_
#define LIGHTBLAS_MATH_FUNCTIONS_H_

void sgemm(const int M, const int N, const int K,
		const float* A, const float* B, float* C);

void sconv(const int kernel_size, const int pad, const int stride,
		const int ic, const int ih, const int iw,
		const int oc, const float* weight, const float* input, float* output);

void my_sgemm(const int M, const int N, const int K,
		const float* A, const float* B,float* C);

void my_sconv(const int kernel_size, const int pad, const int stride,
		const int ic, const int ih, const int iw,
		const int oc, const float* weight, const float* input, float* output);

#endif

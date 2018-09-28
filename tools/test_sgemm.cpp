#include "lightblas/test_helper.hpp"
#include "lightblas/math_functions.hpp"

int main() {

int M[2] = {239, 499};
int N[2] = {257, 463};
int K[2] = {281, 449};

Data A, B, C, NC;

CPUTimer timer;
for (int m = 0; m < 2; ++m){
  for (int n = 0; n < 2; ++n){
    for (int k = 0; k < 2; ++k){
      std::cout << "Testing sgemm, [M, N, K] = ["
	      << M[m] << ", " << N[n] << ", " << K[k]
	      << "]:\n";
      A.Reshape({M[m], K[k]});
      B.Reshape({K[k], N[n]});
      C.Reshape({M[m], N[n]});
      NC.Reshape({M[m], N[n]});

	  float blas_time = 0;
	  float custom_time = 0;

      for (int t = 0; t < 5; ++t){
        std::cout << "test iteration: " << t + 1 << std::endl;
	A.RandomInit();
	B.RandomInit();
	C.RandomInit();
	NC.RandomInit();

	TIMER_START(timer);
	sgemm(M[m], N[n], K[k], A.data(), B.data(), C.data());
	blas_time += TIMER_STOP(timer);

	TIMER_START(timer);
	my_sgemm(M[m], N[n], K[k], A.data(), B.data(), NC.data());
	custom_time += TIMER_STOP(timer);

	if (C.CheckEqual(NC)) 
	  std::cout << "Test passed.\n";
	else
	  std::cout << "Test failed.\n";
      }

	  std::cout << "OpenBLAS time: " << blas_time / 5 << " ms\n";
	  std::cout << "CustomGEMM time: " << custom_time / 5 << " ms\n";

    }
  }
}

}

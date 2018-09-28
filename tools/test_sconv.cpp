#include "lightblas/math_functions.hpp"
#include "lightblas/test_helper.hpp"

int main() {
  int kernel_size[] = {3, 5};
  const int pad[] = {0, 1};
  const int stride[] = {1, 2};
  const int id[] = {31, 67};

  const int ic = 16;
  const int oc = 16;

  Data weight, input, C, NC;

  CPUTimer timer;
  for (int k = 0; k < 2; ++k){
    for (int p = 0; p < 2; ++p){
      for (int s = 0; s < 2; ++s){
        for (int i = 0; i < 2; ++i){
	  const int kernel_size_t = kernel_size[k];
	  const int pad_t = pad[p];
	  const int stride_t = stride[s];
	  const int ih_t = id[i];
	  const int iw_t = id[i];
	  const int ic_t = ic;
	  const int oc_t = oc;

	  const int oh_t = (ih_t + 2 * pad_t - kernel_size_t) / stride_t + 1;
	  const int ow_t = (iw_t + 2 * pad_t - kernel_size_t) / stride_t + 1;

	  weight.Reshape({oc_t, ic_t, kernel_size_t, kernel_size_t});
	  input.Reshape({ic_t, ih_t, iw_t});
	  C.Reshape({oc_t, oh_t, ow_t});
	  NC.Reshape({oc_t, oh_t, ow_t});

	  weight.RandomInit();
	  input.RandomInit();
	  C.RandomInit();
	  NC.RandomInit();

	  std::cout << "Testing sconv. [kernel_size, pad, stride, ic, ih, iw, oc]: ["
		  << kernel_size_t << ", " << pad_t << ", " << stride_t 
		  << ", " << ic_t << ", " << ih_t << ", " << iw_t << ", "
		  << oc_t << "]\n";
	  float blas_time = 0;
	  float custom_time = 0;
	  for (int t = 0; t < 5; ++t){
	    std::cout << "test iteration: " << t + 1 << std::endl;

		TIMER_START(timer);
	    sconv(kernel_size_t, pad_t, stride_t, ic_t, ih_t, iw_t, oc_t,
			 weight.data(), input.data(), C.data());
		blas_time += TIMER_STOP(timer);

		TIMER_START(timer);
	    my_sconv(kernel_size_t, pad_t, stride_t, ic_t, ih_t, iw_t, oc_t,
			 weight.data(), input.data(), NC.data());    
		custom_time += TIMER_STOP(timer);

	    if (C.CheckEqual(NC))
	      std::cout << "Test passed.\n";
	    else
	      std::cout << "Test failed.\n";
	  }
	  std::cout << "OpenBLAS time: " << blas_time / 5 << " ms\n";
	  std::cout << "CustomCONV time: " << custom_time / 5 << " ms\n";
	}
      }
    }
  }
}

#include "lightblas/math_functions.hpp"

void my_sgemm(const int M, const int N, const int K,
	const float* A, const float* B, float* C){
   for(int m=0;m<M;m++){
      for(int n=0;n<N;n++){
         C[m*N+n]=0;
	 for(int k=0;k<K;k++){
          C[m*N+n]+=A[m*K+k]*B[k*N+n];
         }
      }
   }
}

void my_sconv(const int kernel_size, const int pad, const int stride,
		const int ic, const int ih, const int iw, const int oc,
		const float* weight, const float* input, float* output){
	
	//卷积之后输出的单个feature map的尺寸
    const int oh=(ih+2*pad-kernel_size)/stride+1;
    const int ow=(iw+2*pad-kernel_size)/stride+1;

	//对输入图像进行im2col操作
	
	int im2col_rows=ic*kernel_size*kernel_size;//im2col之后结果的行数
	int im2col_cols=oh*ow;//im2cols之后结果的列数
	float im_col[im2col_rows][im2col_cols];//存放图像经im2col后的结果

    for(int c=0;c<im2col_rows;c++){
		int im_channel=c/(kernel_size*kernel_size);
		int h_offset=(c/kernel_size)%kernel_size;
        int w_offset=c%kernel_size;
        for(int h=0;h<oh;h++){
			for(int w=0;w<ow;w++){
				int h_pos=h*stride+h_offset-pad;
				int w_pos=w*stride+w_offset-pad;
				if(h_pos<0 || h_pos>=ih || w_pos<0 || w_pos>=iw)
					im_col[c][h*ow+w]=0;
				else
				    im_col[c][h*ow+w]=input[im_channel*ih*iw+h_pos*iw+w_pos];
			}
		}
	}

	//对卷积核进行2col操作
	float kernel_col[oc][im2col_rows]; //存放结果
    
	//找到kernel_col中每一个元素在weight中对应的位置并进行赋值
	for(int i=0;i<oc;i++){
        for(int c=0;c<ic;c++){
			for(int j=0;j<kernel_size*kernel_size;j++){
			    int h_pos=j/kernel_size;
				int w_pos=j%kernel_size;
				kernel_col[i][c*kernel_size*kernel_size+j]=weight[i*ic*kernel_size*kernel_size+c*kernel_size*kernel_size+h_pos*kernel_size+w_pos];
			}
			
		}
	}

    //将卷积操作转化成kernel_col和im_col两矩阵相乘操作，将结果存放在output_col中
    float output_col[oc][im2col_cols];
	my_sgemm(oc,oh*ow,ic*kernel_size*kernel_size,&kernel_col[0][0],&im_col[0][0],&output_col[0][0]);
    
	//output_col转化为output
    for(int c=0;c<oc;c++){
		for(int h=0;h<oh;h++){
			for(int w=0;w<ow;w++){
				output[c*oh*ow+h*ow+w]=output_col[c][h*ow+w];
			}
		}
	}

}

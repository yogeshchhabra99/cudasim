#include<cuda.h>
#include<stdio.h>
#include<math.h>
#include<ctime>

__global__
void vecMulMatrixKernel(float* A, float* B, float* C, int n){
//	clock_t start = clock();
	int column = threadIdx.x + blockDim.x * blockIdx.x;
	int row = threadIdx.y + blockDim.y * blockIdx.y;
	//printf("%d ",blockDim.x);
	if(row<n && column <n){
			
		float val = 0.0;
		int i;
		for(i=0;i<n;i++){
				val += A[row*n+i] * B[i*n+column];			
		}
		C[row*n+column]=val;
	}
//	clock_t end =clock();
//	printf("time:%lld\n",(int)(end-start));

}

int main(){}
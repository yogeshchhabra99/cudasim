#include<cuda.h>
#include<stdio.h>
#include<math.h>

__global__
void vecMulMatrixKernel(float* A, float* B, float* C, float* D, int n){
	int column = threadIdx.x + blockDim.x * blockIdx.x;
	int row = threadIdx.y + blockDim.y * blockIdx.y;
	printf("%d ",blockDim.x);
	if(row<n && column <n){
			
		float val = 0.0;
		int i,j;
		for(i=0;i<n;i++){
			for( j=0;j<n;j++){
					val += A[row*n+i] * B[j*n+column];
			}
			 val += A[row*n+i] * C[i*n+column];
		}
		C[row*n+column]=val;
	}
}

__host__
void vecMulMatrix(float* A,float* B,float* C, float* D, int n){
	int size = n * n * sizeof(float);
	float *d_A, *d_B, *d_C, *d_D;
	
	//Allocate device memory for A,B,C
	cudaMalloc((void**)&d_A, size);
	cudaMalloc((void**)&d_B, size);
	cudaMalloc((void**)&d_C, size);
	cudaMalloc((void**)&d_D, size);	

	//copy A,B to device memory
	cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_B, B, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, B, size, cudaMemcpyHostToDevice);

	//call kernal function that the calculates the product and stores it in C
	dim3 dimBlock(16,16,1);
	dim3 dimGrid(ceil(n/16.0),ceil(n/16.0),1);
	vecMulMatrixKernel<<<dimGrid,dimBlock >>>(d_A,d_B,d_C,d_D,n);		

	//copy C from devce memory
	cudaMemcpy(D, d_D, size, cudaMemcpyDeviceToHost);
	
	//free device memories
	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);
	cudaFree(d_D);
}

//Kernal function that runs in each thread


int main(){
	int n=256;
	int i,j;
	float A[n][n],C[n][n],B[n][n],D[n][n];
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			A[i][j]=(i+j)%1024;
			B[i][j]=(i*j)%1024;
			C[i][j]=(i%j)%1024;
		}
	}
	
	vecMulMatrix(&A[0][0],&B[0][0],&C[0][0],&D[0][0],n);
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			printf("%.3f ",A[i][j]);
		}
		printf("\n");
	}
	printf("---\n");
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			printf("%.3f ",B[i][j]);
		}
		printf("\n");
	}
	printf("---\n");
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			printf("%.3f ",D[i][j]);
		}
		printf("\n");
	}
	
	return 0;
}





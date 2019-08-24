#include<cuda.h>
#include<stdio.h>
#include<math.h>

__global__
void vecConvKernel(float* A, float* B, float* C,int*D, int n,int m){
	//identify the index of the data to be read
	int i= threadIdx.x + blockDim.x * blockIdx.x;
	int j;
//	if(threadIdx.x==0)
//		printf("threadIdx.x is 0, blockid: %d\n",blockIdx.x);
	float val=0.0;
	int N=n+m-2;
	int start=0;
	int end=n;
	//calculate the sum and store
	if(i<=N){
		
		if(i-m>start)
			start=i-m;
		if(i<end)
			end=i;
		printf("start:%d   end:%d\n",start,end);
		for(j=start;j<=end;j++){
			val+=A[j]*B[i-j];
		}
	}
//	else{
//		printf("i:%d n:%d blockdim:%d\n",i,N,blockDim.x);		
//	}
	C[i]=val;
	D[i]=end;
}

__host__
void vecConv(float* A,float* B,float* C,int* D, int n, int m){
	int c=ceil(n/256.0);
	int size1 = n * sizeof(float);
	int size2 = m * sizeof(float);
	int size3 = (n+m-1) * sizeof(float);
	float *d_A, *d_B, *d_C;	//A of size n and B of size m
	int *d_D;
	//Allocate device memory for A,B,C
	cudaMalloc((void**)&d_A, size1);
	cudaMalloc((void**)&d_B, size2);
	cudaMalloc((void**)&d_C, size3);	
	cudaMalloc((void**)&d_D, size3);	

	//copy A,B to device memory
	cudaMemcpy(d_A, A, size1, cudaMemcpyHostToDevice);
	cudaMemcpy(d_B, B, size2, cudaMemcpyHostToDevice);

	//call kernal function that the calculates sum and stores it in C

	double blocks = ceil((n+m-1)/256.0);
	printf("blocks:%lf\n",blocks);
	clock_t start,end;
	start=clock();

	//printf("n+m:%d,%f blocks:%d,%f\n",(n+m-1),(n+m-1),ceil((float)(n+m-1)/256.0),ceil((float)(n+m-1)/256.0));
	vecConvKernel<<< blocks,256 >>>(d_A,d_B,d_C,d_D,n,m);		
	cudaDeviceSynchronize();
	end=clock();
	printf("time:%lf\n",(double)(end-start)/CLOCKS_PER_SEC);

	//the y and z dimensions are set to 1 by default


	//copy C from devce memory
	cudaMemcpy( C,d_C, size3, cudaMemcpyDeviceToHost);
	cudaMemcpy( D,d_D, size3, cudaMemcpyDeviceToHost);
	//free device memories
	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);
	cudaFree(d_D);
}

//Kernal function that runs in each thread


int main(){
	float *A,*B,*C;
	int*D;
	int n=1024*1024;
	A=(float*)malloc(n*sizeof(float));
	B=(float*)malloc(n*sizeof(float));
	C=(float*)malloc((n+n-1)*sizeof(float));
	D=(int*)malloc((n+n-1)*sizeof(int));
	int i;
	for(i=0;i<n;i++){
		A[i]=(float)i;
		B[i]=(float)2*i;	
	}
	vecConv(A,B,C,D,n,n);
//	for(i=1024*1023;i<1024*1024;i++){
//		printf("%d\n",D[i]);	
//	}
	free(A);
	free(B);
	free(C);
	return 0;
}





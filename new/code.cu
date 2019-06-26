#include<cuda.h>
#include<stdio.h>
#include<math.h>
#include<time.h>

__global__
void mKernal(int *A){
	int i=0;
	__shared__ int arr[10];
	arr[0]=6;
	//while(i<1000000000){		//2147483647
		i=i*1;	
		i=i+1;
		
		//printf(";");
	//}
	//*A=i;
	*A=arr[0];
}

__host__
int main(){
	time_t t1,t2;
	
	
	long n=pow(2,34);
	long nb=n/256;
	int* dA;
	cudaMalloc((void**)&dA, 4);
	//int A;
	
	//printf("n: %ld nb: %ld A: %d\n",n,nb,A);
	dim3 dim(nb,4,1);
	t1=time(NULL);
	mKernal<<<dim,256>>>(dA);
	//mKernal<<<1,1>>>(&A);
	cudaDeviceSynchronize();
	t2=time(NULL);
	cudaMemcpy( &A,dA, 4, cudaMemcpyDeviceToHost);	
	printf("Time:%ld   A: %d\n",t2-t1,A);
	
}

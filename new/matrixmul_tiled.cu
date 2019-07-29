	//
#include<cuda.h>
#include<stdio.h>
#include<math.h>
#include<ctime>
#include<time.h>
#define TILEWIDTH 32
#define TILE_WIDTH 32

__global__
void vecMulMatrixKernel(float* A, float* B, float* C, int n){
	//each block loads the corresponding row of blocks of A matrix and column of blocks of B matrix, one block at a time and then clculates the product for that part then product of a  the parts is added.
	// each thread loads 2 elements one from A and one from B in each phase
	// there are total gridDim.x phases
	// the element loaded is the element at the same position as this thread but in a different block
	//if run more thread than max then not run
	int tx=threadIdx.x; int ty=threadIdx.y;
	int bx=blockIdx.x; int by=blockIdx.y;
	
	int row=by*blockDim.y+ty;
	int col=bx*blockDim.x+tx;
	
	__shared__ float Ads[TILEWIDTH][TILEWIDTH];
	__shared__ float Bds[TILEWIDTH][TILEWIDTH];
	
	if(row<n && col <n){
		int i; float val=0.0;
		for(i=0;i<gridDim.x-1;i++){
			Ads[ty][tx] = A[ row*n + i*TILEWIDTH + tx];
			Bds[ty][tx] = B[ (i*TILEWIDTH + ty)*n + col];
			__syncthreads();
			for(int k=0;k<TILEWIDTH;k++){
				val+= Ads[ty][k]*Bds[tx][k];	
			}
			__syncthreads();
			
		}

		if(i*TILEWIDTH + tx <n )		//if n was a multiple of blockDim then this was not required
			Ads[ty][tx] = A[ row*n + i*TILEWIDTH + tx];
		if(i*TILEWIDTH + ty <n )
			Bds[ty][tx] = B[ (i*TILEWIDTH + ty)*n + col];
		__syncthreads();
		int m =n%TILEWIDTH;
		if(m==0)
			m=TILEWIDTH;
		for(int k=0;k<m;k++){//printf("add");
			val+= Ads[ty][k]*Bds[tx][k];	
		}
		__syncthreads();
		C[row*n + col]= val;
	}
}

__global__ void MatrixMulKernel(float* Md, float* Nd, float* Pd, int Width)
{  
__shared__ float Mds[TILE_WIDTH][TILE_WIDTH];
__shared__ float Nds[TILE_WIDTH][TILE_WIDTH];  
int bx = blockIdx.x;
int by = blockIdx.y;
int tx = threadIdx.x;
int ty = threadIdx.y;
// Identify the row and column of the Pd element to work on
int Row = by * TILE_WIDTH + ty;
int Col = bx * TILE_WIDTH + tx;
float Pvalue = 0;
// Loop over the Md and Nd tiles required to compute the Pd element
for (int m = 0; m < Width/TILE_WIDTH; ++m) {
// Collaborative loading of Md and Nd tiles into shared memory
Mds[ty][tx] = Md[Row*Width + (m*TILE_WIDTH + tx)];
Nds[ty][tx] = Nd[Col + (m*TILE_WIDTH + ty)*Width];
__syncthreads();
for (int k = 0; k < TILE_WIDTH; ++k)
Pvalue += Mds[ty][k] * Nds[k][tx];
__syncthreads();
}
Pd[Row*Width+Col] = Pvalue;
}


int min2Power(int x){
	int res=1;
	while(res<x){
		res*=2;	
	}
	return res/2;
}

__host__
void vecMulMatrix(float* A,float* B,float* C, int n){
	int size = n * n * sizeof(float);
	float *d_A, *d_B, *d_C;
	
	//Allocate device memory for A,B,C
	cudaMalloc((void**)&d_A, size);
	cudaMalloc((void**)&d_B, size);
	cudaMalloc((void**)&d_C, size);

	//copy A,B to device memory
	cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_B, B, size, cudaMemcpyHostToDevice);
	
	//call kernal function that the calculates the product and stores it in C
	dim3 dimBlock(TILEWIDTH,TILEWIDTH,1);
	dim3 dimGrid(ceil(n/(float)TILEWIDTH),ceil(n/(float)TILEWIDTH),1);

//	time_t t1=time(NULL);
//	for(int i=0;i<270;i++)
//	vecMulMatrixKernel<<<dimGrid,dimBlock >>>(d_A,d_B,d_C,n);		

	clock_t start,end;
	start=clock();
	MatrixMulKernel<<<dimGrid,dimBlock>>>(d_A,d_B,d_C,n);
	cudaDeviceSynchronize();
	end=clock();
	printf("time:%lf\n",(double)(end-start)/CLOCKS_PER_SEC);

//	time_t t2=time(NULL);
//	printf("%d\n",t2-t1);

	//copy C from devce memory
	cudaMemcpy(C, d_C, size, cudaMemcpyDeviceToHost);
	
	//free device memories
	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);
}

//Kernal function that runs in each thread


int main(){
	int maxThreads=640;
	
		int n=1024*2;//640;
	int i,j;
//	float A[n][n],C[n][n],B[n][n];
//	for(i=0;i<n;i++){
//		for(j=0;j<n;j++){
//			A[i][j]=(i+j)%1024;
//			B[i][j]=(i*j)%1024;
//		}
//	}
	
	float **A=(float**)malloc(1024*sizeof(float*));
	float **B=(float**)malloc(1024*sizeof(float*));
	float **C=(float**)malloc(1024*sizeof(float*));
	for(int i=0;i<1024;i++){
		A[i]=(float*)malloc(1024*sizeof(float));
		B[i]=(float*)malloc(1024*sizeof(float));
		C[i]=(float*)malloc(1024*sizeof(float));
	}
	vecMulMatrix(&A[0][0],&B[0][0],&C[0][0],n);

//for(i=0;i<n;i++){
//		for(j=0;j<n;j++){
	//		printf("%.3f ",A[i][j]);
//		}
	//	printf("\n");
//	}
//	printf("---\n");
//	for(i=0;i<n;i++){
//		for(j=0;j<n;j++){
	//		printf("%.3f ",B[i][j]);
//		}
	//	printf("\n");
//	}
//	printf("---\n");
//	for(i=0;i<n;i++){
//		for(j=0;j<n;j++){
	//		printf("%.3f ",C[i][j]);
//		}
//		printf("\n");
//	}
	
	return 0;
}





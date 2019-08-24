#pragma once

#include<iostream>

using namespace std;

typedef struct mystruct1{
    	long long numthreads;
	float bytesPerCycle;
	int loadLatency;
	int sharedLoadLatency;
} CudaConfig;

void printConfig(CudaConfig config){
	    	cout<<"numthreads:"<< config.numthreads<<endl;
	cout<< "bytesPerCycle:"<<config.bytesPerCycle<<endl;
	cout<< "loadLatency:"<<config.loadLatency<<endl;
	cout<< "sharedLoadLatency:"<<config.sharedLoadLatency<<endl;

}

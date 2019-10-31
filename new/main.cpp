#include <bits/stdc++.h> 
#include "codescanner.cpp"
#include "configscanner.cpp"
#include "structs.h"

using namespace std; 

int main () 
{ 
	//string 
  //ll
	CudaConfig config= getConfig();
	
    system("nvcc -c code.cu");
	system("cuobjdump -ptx code.o >generatedptx.txt");
    solve(config.numthreads,config.bytesPerCycle,config.loadLatency,config.sharedLoadLatency); 
  
    return 0; 
} 

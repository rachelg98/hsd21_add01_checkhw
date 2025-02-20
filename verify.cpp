#include <cstdio>
#include <cstdlib>
#include "fpga_api.h"

// g++ -I./include main.cpp ./src/fpga_api.cpp -o run.exe && sudo ./run.exe

int main(void)
{
	printf("program start\n");

	int m_size = 8;
	int v_size = 8;
	int data_size_M = m_size * v_size; 


	float* flat = new float[data_size_M*2]; // for input & weight
	float* input = flat+ data_size_M;	// for input
	float* mat = flat;  // for weight
	float* output = new float[data_size_M];	//for output	
	float* output_fpga = new float[data_size_M]; // for fpga output

	for(int i = 0 ; i < data_size_M*2 ; ++i)
		flat[i] = ((float)rand()) / RAND_MAX;

	printf("Computation start\n");
	// computation
	for(int i = 0; i < v_size; ++i)
	{
		printf("Loop i: %d\n ", i);
		for(int j = 0; j < v_size; ++j){    
			output[v_size*i+j] = 0;
			printf("\t[j:%d]: output[%d]\n", j, v_size*i+j);
			for(int k = 0; k < v_size; ++k){
				output[v_size*i+j] += mat[v_size*i+k] * input[v_size*k + j];
				printf("\t[k:%d]: Add %f,\toutput[%d]=%f\n", 
								k, 
								mat[v_size*i+k] * input[v_size*k + j], 
								v_size*i+j, 
								output[v_size*i+j]
								);
			}
		}
	}

	printf("==Computation finished!==\n");

	// FPGA offloading
	// memory load
	FPGA dev(0x40000000, 0x43c00000);

	printf("===Large MM start===\n\n");
	dev.largeMM(mat, input, output_fpga, v_size, v_size, v_size);
	printf("===Large MM Complete!===\n");

	// display
	printf("%-10s%-10s%-10s\n", "index", "CPU", "FPGA");
	for (int i = 0; i < data_size_M; i++)
	{
		printf("%-10d%-10f%-10f%\n", i, output[i], output_fpga[i]);
	}

	delete[] flat;
	delete[] output;
	delete[] output_fpga;
	return 0;
}

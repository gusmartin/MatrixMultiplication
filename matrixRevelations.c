/**
 * Copyright (c) 2018 
 *
 * @file    matrixThreads.c
 *
 * @author  Gustavo Martin
 *
 * @date    Fri 25 Mar 2018
 *
 * @brief   Square matrix multiplication and trace calcultion using threads and implementing master slave model
 *
 * About: 
 *			The program uses the standard matrix multiplication algorithm of complexity O(n^3).
 *			Generates n child threads, each one performs calculations for a designated column of the matrix product
 *			Each child does the addition necesary for calculating the trace
 *
 *			I have been illustrated and this is the simpler version of the program
 *
 * References:
 *          File reading is based and on the professor큦 code from TC2008, modified for reading size n of matrix, 
 *			matrix A then matrix B.
 *
 * File format:
 *          File has an int n at the beginning to define size (n * n) of the square matrix     
 * 			it reads the next numbers and stores them into A, then it keeps reading until B 
 * 			is also read.
 *
 *			Ex.
 *			2
 *			1 2 3 4
 * 			1 0 0 1
 *
 * Revision history:
 *          Fri 23 Mar 2018 -- File created, implement matrix multiplication using threads
 *			Sun 25 Mar 2018 -- Added trace calculation using mutex for synchronixation, added comments
 *			Sat 14 Apr 2018 -- Changed trace calculations to do it in a more concurrent way, having a flag 
 *							   for each element that indicates that the element can be added to the trace
 *
 *
 * @note    Assignment for TC2025
 *
 */

#include<stdio.h>
#include<stdlib.h>     	/* We need malloc */
#include<string.h>
#include "FileIO.h"    	/* Definition of file access support functions */
#include <pthread.h>	/* For thread management */
#include <unistd.h>

#define NUMPARAMS 2

//These are READ ONLY, except for C, but each thread is going to modify diferent parts of the array so it큦 okay
int n;

int **A, **B, **C;

//we need to synchronize to change these values
pthread_mutex_t traceMutex = PTHREAD_MUTEX_INITIALIZER;

int trace = 0;

//Used to check wheter if the trace has been calculated or not
unsigned short int trace_flag = 0;


//the thread structure
typedef struct thread_attribute{
	
	int id;  //Used to traverse thread structure
	int ready; // used as a flag to let know the diagonal value is ready
	
}thread_attributes;

//Used to know which thread you really are
typedef struct thread_ID{
	
	thread_attributes **attributes;
	int number; 	//know the column assigned
	
}thread_ID;

//initializing thread structure
void init(thread_attributes * t, int i){
	
	t->id = i;
	t->ready = 0;
}


//Print the specified matrix
void printMatrix(int **M, int n){
	
	int i,j;
	
	for(i = 0; i < n; i++){
		for(j = 0; j < n; j++){
			
				printf("%d ", M[i][j]);
		}
		printf("\n");
	}		
}

//Perform the multiplication in the section(divided by columns) specified by data and add value of diagonal to trace
void *multiply(void *data) {
	
	thread_ID *info = (thread_ID *)data;
			
	int i = *(int*) &info->number;
				
	//matrix multiplication n^2, since this is just the calculations for column i
	for(int j = 0; j < n; j++){
			
		C[i][j] = 0;
		
		for(int k = 0; k < n; k++){

			C[i][j] += A[i][k] * B[k][j];
			
		}
		//if multiplication of value of main diagonal is done, release the mutex
		if(j == i)
			info->attributes[i]->ready = 1; //diagonal can be calculated know
	}
	
//print answer of column, useful to see order of thread execution
#ifdef DEBUG
	
	for(int j = 0; j < n; j++){
		
		printf("%d ", C[i][j]);

	}
	
	printf("\n");
	
#endif

	//The first thread that finishes calculates the trace of the matrix, if 0 then the mutex was aquired
	if(!pthread_mutex_trylock(&traceMutex)){
		
		//if the trace has already been calculated, and somehow a thread gets the mutex, ignore and exit
		if(!trace_flag){
			
			int count = 0, i = 0; // count how many values have been added
			
			while(count != n){
				
				if(info->attributes[i]->ready){
										
					trace += C[i][i];
					info->attributes[i]->ready = 0;
					count++;																	
				}
				
				//if there큦 still missing diagonals to add, restart loop
				i = (i+1) % n;
			}
			
			pthread_mutex_unlock(&traceMutex);	
			
			trace_flag = 1;		
		}
	}
	return EXIT_SUCCESS;
}

int main(int argc, const char * argv[]){
	
	FILE *fp;                                /* Pointer to the file */
						
	/* Check if the number of parameters is correct */
    if (argc < NUMPARAMS){  
    
        printf("Need a file with the process information\n");
        printf("Abnormal termination\n");
        return (EXIT_FAILURE);
        
    } else {
    	
        /* Open the file and check that it exists */
        fp = fopen (argv[1],"r");	  /* Open file for read operation */
        
        if (!fp) { printf("Error opening file \n"); return EXIT_FAILURE;}
		
		else {
			
			//get N
            n = GetInt(fp);
            
            // Malloc of array of pointers (we use an array of pointers since we are going to be using 2d arrays)
            A = malloc(sizeof(int*) * n);
            B = malloc(sizeof(int*) * n);
            C = malloc(sizeof(int*) * n);
            
            if(A == NULL || B  == NULL|| C == NULL)
            	return EXIT_FAILURE;
            
            // malloc for each pointer of array A,B and C, of size n * int 
            for (int i = 0 ; i < n ; i++){
		
			    A[i] = malloc(sizeof(int) * n);     
			    B[i] = malloc(sizeof(int) * n);     
			    C[i] = malloc(sizeof(int) * n);     
			    
			    if (A[i] == NULL || B[i] == NULL || C[i] == NULL)
			        return EXIT_FAILURE;
			}
                        
            if (n == EXIT_FAILURE) { printf("Error getting n\n"); return EXIT_FAILURE;} 
			
			else {
                    
                //get matrix A from file
                for (int i = 0; ((i < n) && (!feof(fp))); i++) {
                	 for (int j = 0; j < n; j++) {
                	 	
                    	A[i][j] =  GetInt(fp);
                    }
                }
                
                //get matrix B from file
                for (int i = 0; ((i < n) && (!feof(fp))); i++) {
                	 for (int j = 0; j < n; j++) {
                	 	
                    	B[i][j] =  GetInt(fp);
                    }
                }
            }
        }
        
#ifdef DEBUG

        //print A 
		printMatrix(A,n);
		
		printf("\n");
		
		//print B 
		printMatrix(B,n);
		
		printf("\n");
		
#endif
		  
        pthread_t thread[n];
        
        //declare the structure that is going to have the mutex and index of the thread
        thread_attributes * thread_struct[n];
        
        /* 	it큦 necessary to have this struct for the thread to know what column to calculate, otherwise it will not
        	it큦 correspondent thread since we are passing the entire array of thread_struct to the multiply function  */
        
        thread_ID * thread_id[n];
        
        //initialize thread_struct
        for (int i = 0; i < n; i++){
        	
        	thread_struct[i] = malloc(sizeof(thread_attributes));
        	
        	//initialize struct
        	init(thread_struct[i],i);
        }
        			        
        for (int i = 0; i < n; i++){
        	
        	thread_id[i] = malloc(sizeof(thread_ID));
        	
        	//initialize thread_id
        	thread_id[i]->number = i;
        	thread_id[i]->attributes = thread_struct;
        	
		  	if (pthread_create(&thread[i], NULL, multiply, thread_id[i]) != 0)
			  	perror("error creating thread.");
		
		}
						
		/* Now wait for the threads to finish*/
		for (int i = 0; i < n; i++){
			
		  	if (pthread_join (thread[i], NULL ) != 0)
				perror("error joining thread.");
		
		}
		
#ifdef DEBUG	
		printf("\n");
#endif
        		
		//print C
		printMatrix(C,n);
		
		printf("\ntrace  = %d\n", trace);
				
		printf("\n");
	}
}

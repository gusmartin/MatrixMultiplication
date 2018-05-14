////Matrix Multiplication Assignment////

Brief description   - 	Square matrix multiplication and trace calcultion using 
			threads and implementing the master slave model. Reads input
			from a text file


Design of algorithm - 	The solution required and proposed uses the divide and 
			conquer design paradigm, which is commonly used in shared
			memory systems for multi-processing.


Solution	    -	For this algorithm we create N threads for the matrix 
			multiplication calculation, each thread is in charge of 
			calculating the result of a column of the output matrix
			so if we are multiplying two 2x2 matrices 2 threads are 
			created for the calculations. I chose this algorithm over 
			creating N threads for each element of the output matrix 
			because this method will result in too many context switches.
			In other words there´s too much overhead in relation to the 
			task performed. In fact, the granularity is so low that using 
			only one thread would be the optimal solution.

			For the trace calculation the first thread that finishes 
			calculating it´s respective column is assigned the task of
			calculating the trace. For this calculation the thread is 
			constantly checking if a value is ready to be added by 
			traversing the diagonal until it adds all the elements. The 
			other threads update a flag as soon as the diagonal element
			has been calculated so the thread can add the value right
			right after the flag is updated.

Complexity	    -   For the matrix calculation the standard algorithm of 
			complexity O(n^3) is used, since we are using threads the 
			complexity of each thread is of O(n^2) and n threads perform 
			this operation.(Complexity for each thread could be reduced
			to O(n) with n^2 performing this operation, but as mentioned
			before this would result in a lot of context switches).

Command for compiling:

gcc FileIO.c matrixRevelations.c -pthread -Wall -o matrixRevelations

Run:
./matrixRevelations test.txt

Textfile format:

2
1 2 3 4
1 0 0 1

*or*

2
1 2
3 4

1 0
0 1


	
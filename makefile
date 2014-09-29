
reduce : reduce/reduce.c
	mpicc reduce/reduce.c -o reduce/reduce.o

salary : salary/salary_reduction.c
	gcc salary/salary_reduction -fopenmp salary/salary_reduction.o

warmup : warmup/hello.c warmup/pingpong.c
	mpicc warmup/hello.c -o warmup/hello.o
	mpicc warmup/pingpong.c -o warmup/pingpong.o

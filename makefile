.PHONY: reduce salary warmup

reduce : reduce/reduce.c
	mpicc reduce/reduce.c -o reduce/reduce.o

salary : salary/salary_reduction.c
	gcc salary/salary_reduction.c -fopenmp -o salary/salary_reduction.o

warmup : warmup/hello.c warmup/pingpong.c
	mpicc warmup/hello.c -o warmup/hello.o
	mpicc warmup/pingpong.c -o warmup/pingpong.o

clean :
	rm -rf */*.o

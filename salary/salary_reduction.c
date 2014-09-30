#include <omp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
        int i, nthreads, tid,  programmers, managers, accountants, ceos, num_workers;
        float accounterFee, programmerSalary, managerSalary, ceoSalary, totalSalary;

        /* Some initializations */
        programmers = 10;
        managers = 3;
        ceos = 1;
        accounterFee = 500.0;
        programmerSalary = 4000.0;
        managerSalary = 6000.0;
        ceoSalary = 9000.0;
        totalSalary = 0.0;

        #pragma omp parallel private(nthreads, tid)
        {
                tid = omp_get_thread_num();
                printf("\nAccounter %d ready to calculate total salary.\n", tid);
                if(tid == 0)
                {
                        nthreads = omp_get_num_threads();
                        accountants = nthreads;
                        printf("\n%d Accountants were hired to calculate total salary, therefor they are included in the calculations.\n", accountants);
                }
        }

        #pragma omp parallel for reduction(+:totalSalary)
        for(i = 0; i < programmers; i++){
            printf("\nAccounter %d calculating in loop 2.\n", omp_get_thread_num());
            totalSalary = totalSalary + programmerSalary;
        }
        #pragma omp parallel for reduction(+:totalSalary)
        for(i = 0; i < managers ; i++){
            printf("\nAccounter %d calculating in loop 3.\n", omp_get_thread_num());
            totalSalary = totalSalary + managerSalary;
        }
        #pragma omp parallel for reduction(+:totalSalary)
        for(i = 0; i < ceos; i++){
            printf("\nAccounter %d calculating in loop 4.\n", omp_get_thread_num());
            totalSalary = totalSalary + ceoSalary;
        }

        num_workers = programmers + managers + ceos;
        #pragma omp parallel for num_threads(num_workers) private(tid)
        for(i = 0; i < num_workers; i++){
            tid = omp_get_thread_num();
            printf("I'm employee #%d and I do exist\n", tid);
        }
        
        #pragma omp parallel for reduction(+:totalSalary)
        for(i = 0; i < accountants; i++){
            printf("\nAccounter %d calculating in loop 1.\n", omp_get_thread_num());
            totalSalary = totalSalary + accounterFee;
            }

        printf("\nA company has to pay $%f total salary for the current month. The company consists of %d programmers ($%f a month), %d managers ($%f a month), %d ceos ($%f a month) and additionally, %d accountants were hired to calculate the total salary and they charged $%f each for their work.\n\n", totalSalary, programmers, programmerSalary, managers, managerSalary, ceos, ceoSalary, accountants, accounterFee);
}


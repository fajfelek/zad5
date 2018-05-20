//
// Created by Filip Stajniak on 22.04.2018.
//


int *pNumbers = new int[numbers.size()];
clock_t begin = clock();

//init mpi
MPI_Init(NULL, NULL);

//find out how many processes are there
MPI_Comm_size(MPI_COMM_WORLD, &world_size);
//find out which process are we in
int world_rank;
MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
std::vector <long> numbers2;

//calculate prime numbers each process will calculate its own portion
for (i = 0; i < numbers.size(); i++) {

if (world_rank == 0) {
MPI_Send(&numbers[i], 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
MPI_Recv(&recvNum, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
long recvNum;

if (world_rank!=0)
{
//go through all the recieved primes and add them to our vector
for (i=1;i<world_size;i++)
{
MPI_Recv(&recvNum, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
if(checkNumber(recvNum)){
MPI_Send(&recvNum, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
} else {
recvNum *= -1;
MPI_Send(&recvNum, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
}

}

cout <<"overall "<<allPrimes.size()<<" primes"<<endl;
//output all primes to a file
freopen("primes.txt","w",stdout);

for(i=0;i<allPrimes.size();i++)
printf("%d ", allPrimes[i]);
printf("\n");
}
}
#include "a.h"

#include <iostream>
#include <deque>
#include <vector>
#include <fstream>
#include <mpi.h>
#include <cmath>
#include <cstdlib>

bool checkNumber(long number){

    if (number%2==0)
        return false;
    for (int i=3;i<sqrt(number);i+=2)
        if (number%i==0)
            return false;
    return true;
}

int configure(char *progam_name)
{
    std::cout << "To use write: " << progam_name << " [num_process] [input_data_txt]" << std::endl;

    return 1;
}

void master(std::deque <long> &data, std::vector <long> &numbers)
{
    int	tasks, rank;
    long result, work;
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    long data_size = data.size();
    //for(int r = 0;r < data_size; r++) {
    //while(!data.empty()){
        //sending to slaves
        for (rank = 1; rank < tasks; ++rank) {
            if (!data.empty()) {
                work = data[0];
                data.pop_front();
                MPI_Send(&work, 1, MPI_LONG, rank, 0, MPI_COMM_WORLD);
            }
        }
        //receiving from slaves
        for (rank = 1; rank < tasks; ++rank) {
            MPI_Recv(&result, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            numbers.push_back(result);
        }
    //}
    //if (data.empty()) {
        for (int i = 1; i < tasks; i++) {
            MPI_Send(&i, 0, MPI_LONG, i, 42, MPI_COMM_WORLD);
        }
    //}
}

void slaves(std::deque <long> &data)
{
    long number;
    while(true) {
        MPI_Status status;
        MPI_Recv(&number, 1, MPI_LONG, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (status.MPI_TAG == 42) {
            break;
        }

        if (checkNumber(number)) {
            MPI_Send(&number, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
        } else {
            number *= -1;
            MPI_Send(&number, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
        }
    }
}

int main(int argc, char** argv) {

    if(argc < 2)
    {
        return configure(argv[0]);
    }
    int myrank;
    double start, end;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    std::deque <long> data;
    std::vector <long> numbers;
    std::string number;
    std::ifstream input(argv[1]);

    if (myrank == 0) {
        //data load
        if (input.is_open()) {
            while (!input.eof()) {
                getline(input, number);
                data.push_back(atol(number.c_str()));
            }
        } else {
            std::cout << "Nie znaleziono pliku z danymi" << std::endl;
            return 1;
        }
    }

    //start = MPI_Wtime();
    if (myrank == 0) {
        master(data, numbers);
    } else {
        slaves(data);
    }
    //end = MPI_Wtime();


    if(myrank == 0) {
        //printing time
        std::cout << "Time: " << (end-start)*1000 << std::endl;
        //printing result
        for (int i = 0; i < numbers.size(); i++) {
            if (numbers[i] >= 0) {
                std::cout << numbers[i] << ": prime" << std::endl;
            } else {
                std::cout << -numbers[i] << ": composite" << std::endl;
            }
        }
    }
    MPI_Finalize();

    return 0;
}


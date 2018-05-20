#include <iostream>
#include <deque>
#include <vector>
#include <fstream>
#include <mpi.h>
#include <cmath>
#include <cstdlib>

//struct liczba{
//    long number;
//    bool odd = false;
//
//    liczba(long number,bool odd){
//        this->number=number;
//        this->odd=odd;
//    }
//};

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
    std::cout << "To use write: " << progam_name << " [num_threads] [input_data_txt]" << std::endl;

    return 1;
}

void master(std::deque <long> &data, std::vector <long> &numbers)
{
    int	tasks, rank;
    long result;
    long work;
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    long data_size = data.size();
    for(int r = 0;r < data_size; r++) {
        //sending to slaves
        for (rank = 1; rank < tasks; ++rank) {
            if (!data.empty()) {
                work = data[0];
                data.pop_front();
                MPI_Send(&work, 1, MPI_LONG, rank, 0, MPI_COMM_WORLD);
                std::cout << "Master: Sending number: " << work << " to " << rank << std::endl;
            }
        }

        //receiving from slaves
        for (rank = 1; rank < tasks; ++rank) {
            MPI_Recv(&result, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "Master: Receiving number: " << work << " from " << rank << std::endl;
            numbers.push_back(result);
        }
        std::cout << data.size() << std::endl;
    }
    if (data.empty()) {
        std::cout << "AAAAAAAAAAAAAAA" << std::endl;
        long a = 123456789;
        for (int i = 1; i < tasks; i++) {
            MPI_Send(&i, 0, MPI_LONG, i, 42, MPI_COMM_WORLD);
        }
    }

}

void slaves(std::deque <long> &data)
{
    long result;
    long number;
    while(true) {
        MPI_Status status;
        MPI_Recv(&number, 1, MPI_LONG, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        std::cout << "Slave: Receiving number: " << number << " from 0" << std::endl;

        if (status.MPI_TAG == 42) {
            std::cout << "Process exiting work loop." << std::endl;
            break;
        }

        if (checkNumber(number)) {
            MPI_Send(&number, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
            std::cout << "Slave: Sending number: " << number << " to 0" << std::endl;
        } else {
            number *= -1;
            MPI_Send(&number, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
            std::cout << "Slave: Sending number: " << number << " to 0" << std::endl;
        }
    }
}

//int world_size;

int main(int argc, char** argv) {

    if(argc < 2)
    {
        return configure(argv[0]);
    }
    std::deque <long> data;
    std::vector <long> numbers;
    std::string number;
    std::ifstream input(argv[1]);

    if(input.is_open()){

        while(!input.eof()){
            getline(input,number);
            data.push_back(atol(number.c_str()));
        }

    } else {
        std::cout << "Nie znaleziono pliku z danymi" << std::endl;
        return 1;
    }

    int myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
int data_size = static_cast<int>(data.size());
int g = 0;
    //while(!data.empty() || numbers.size()!=data_size) {
        if (myrank == 0) {
            master(data, numbers);
        } else {
            slaves(data);
        }
        std::cout << "YOLO " << g++ << std::endl;
   // }
    std::cout << "KONIEC YOLO " << std::endl;
    //MPI_Finalize();

//    if (myrank != 0) {
//        MPI_Finalize();
//    }

    for(int i = 0; i < numbers.size(); i++){
        std::cout << "Liczba: " << numbers[i] << std::endl;
    }


//    /*
//* Tell all the slaves to exit.
//*/
//    int rank;
//    for (rank = 1; rank < 2; ++rank) {
//        MPI_Send(0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD);
//    }
//}
//

    MPI_Finalize();

    return 0;
}


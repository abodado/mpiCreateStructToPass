#include <iostream>
#include <mpi.h>
#include <string>
#include <cstring>

class Student {
public:
    int id;
    char name[50];
    double gpa;
};

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Student student_data;
    if (rank == 0) {
        student_data.id = 123;
        strcpy(student_data.name, "Alice");
        student_data.gpa = 3.8;
    }

    int blocklengths[] = {1, 50, 1};
    MPI_Aint displacements[3];
    MPI_Datatype types[] = {MPI_INT, MPI_CHAR, MPI_DOUBLE};
    MPI_Datatype mpi_student_type;

    MPI_Aint base_address;
    MPI_Get_address(&student_data, &base_address);
    MPI_Get_address(&student_data.id, &displacements[0]);
    MPI_Get_address(&student_data.name, &displacements[1]);
    MPI_Get_address(&student_data.gpa, &displacements[2]);

    displacements[0] -= base_address;
    displacements[1] -= base_address;
    displacements[2] -= base_address;

    MPI_Type_create_struct(3, blocklengths, displacements, types, &mpi_student_type);
    MPI_Type_commit(&mpi_student_type);

    MPI_Bcast(&student_data, 1, mpi_student_type, 0, MPI_COMM_WORLD); // Broadcast from rank 0

    std::cout << "Rank " << rank << " received Student:" << std::endl;
    std::cout << "ID: " << student_data.id << std::endl;
    std::cout << "Name: " << student_data.name << std::endl;
    std::cout << "GPA: " << student_data.gpa << std::endl;

    MPI_Type_free(&mpi_student_type);
    MPI_Finalize();

    return 0;
}

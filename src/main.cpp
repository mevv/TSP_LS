#include "tsp.h"

int main(int argc, char** argv)
{
    TSP a;

//    std::cout << argv[1] << " " << argv[2] << std::endl;

    if (argc != 3)
    {
       std::cout << "Wrong arguments number:" << argc << std::endl;
       return -1;
    }

    if (!a.readFromFile(argv[1]))
    {
        std::cout << "Task read failed!" << std::endl;
        return -1;
    }

    if (!a.readInitial(argv[2]))
    {
        std::cout << "Initial failed!" << std::endl;
        return -1;
    }

    std::cout << "Name: " << a.getName() << std::endl;
    std::cout << "Description: " << a.getDescription() << std::endl;
    std::cout << "Size: " << a.getSize() << std::endl;
//    std::cout << "Matrix: ";
//    a.showMatrix();
//    std::cout << "Initial: ";
//    a.showInitial();

    std::cout << ">>>>>>>>>>ALGO: LS<<<<<<<<<<" << std::endl;
    a.solve(ALGO::LS, true);
    std::cout << ">>>>>>>>>>ALGO: GLS<<<<<<<<<<" << std::endl;;
    a.solve(ALGO::GLS, true);

    return 0;
}

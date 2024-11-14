#include <iostream>
#include "../ynlib.h"

int main(int argc, char* argv[])
{
    if(argc > 3 || argc < 3){
        std::cout << "Provided " << argc << "arguements. Usage: child [name] [age]" << std::endl;
        return 1;
    }

    char* child_name = argv[1];
    int child_age = ynlib::str_to_int(argv[2]);
    if(child_age < 0){
        std::cout << "Invalid age. Usage: child [name] [age]" << std::endl;
        return -1;
    }

    std::cout << "I am " << child_name << ", " << child_age << " years old" << std::endl;
    return 0;
}

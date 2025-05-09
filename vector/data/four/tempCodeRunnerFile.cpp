//provided by ivy

#include "/home/entong/STLite-ACM-2025-main/vector/src/vector.hpp"
#include "/home/entong/STLite-ACM-2025-main/vector/data/class-matrix.hpp"
#include "/home/entong/STLite-ACM-2025-main/vector/data/class-bint.hpp"
#include <iostream>

void complex_test()
{
    std::cout << "Supplementary test for large amounts of data ..." << std::endl;
    sjtu::vector<Diamond::Matrix<Util::Bint>> myVec;
    for (int i = 1; i <= 1926; ++i) {
        std::cout << i << std::endl;
        myVec.push_back(Diamond::Matrix<Util::Bint>(i % 8 + 1, i % 17 + 1, Util::Bint(i * 817)));
    }
    int o = 1234;
    while (o--)
        myVec.pop_back();
    myVec = myVec;
    int _ = 20, __ = myVec.size();
    while (_--)
    {
        if (_ % 2 == 0)
            std::cout << myVec[_][0][0] << std::endl;
        else
            std::cout << myVec[__ - _][0][0] << std::endl;
    }
    std::cout << "Finished!" << std::endl;
}

int main()
{
    complex_test();
}
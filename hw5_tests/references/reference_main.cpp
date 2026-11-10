#include <gtest/gtest.h>
#include <cstring>
#include <iostream>
#include "reference_info.h"

int main(int argc, char** argv)
{
    if (argc == 2 && std::strcmp(argv[1], "--reference-info") == 0)
    {
        std::cout << REFERENCE_INFO << '\n';
        return 0;
    }
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

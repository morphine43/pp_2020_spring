// Copyright 2020 Danilov Nikita

#include <gtest/gtest.h>

#include <vector>
#include <random>

#include "./crs_matrix.h"

// Need only one test that tbb multiply is equal to seq
// other functions of class have tested in seq main.cpp
TEST(CRSMatrix, Test_Correct_Matrix_Multiplication_TBB) {
    std::vector<std::vector<double>> a {{ 14.2,  0.0,  0.0, 0.0, },
                                        {  4.4, 14.4, 93.1, 0.0, },
                                        {  0.0,  0.0, 73.7, 0.0, },
                                        { 35.7,  0.0,  0.0, 0.0  }};

    std::vector<std::vector<double>> b {{ 0.0, 0.0,  90.3,  0.0, },
                                        { 0.0, 0.0,  65.2,  3.2, },
                                        { 0.0, 0.0,   0.0,  0.0, },
                                        { 0.0,   6,   0.0,  0.0  }};

    std::vector<std::vector<double>> true_result {{ 0.0, 0.0, 1282.26,   0.0, },
                                                  { 0.0, 0.0,  1336.2, 46.08, },
                                                  { 0.0, 0.0,     0.0,   0.0, },
                                                  { 0.0, 0.0, 3223.71,   0.0, }};

    auto result = CRSMatrix<double>(a) * CRSMatrix<double>(b);
    auto result_tbb = tbbMultiply(CRSMatrix<double>(a), CRSMatrix<double>(b));

    ASSERT_EQ(result, true_result);
    ASSERT_EQ(result_tbb, true_result);
    ASSERT_EQ(result, result_tbb);
}

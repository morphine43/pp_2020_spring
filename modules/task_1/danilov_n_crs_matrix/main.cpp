// Copyright 2020 Danilov Nikita

#include <gtest/gtest.h>

#include <vector>
#include <random>

#include "./crs_matrix.h"

TEST(CRSMatrix, Test_Correct_Create_CRSMatrix_From_Classic_Matrix) {
    std::vector<std::vector<double>> a {{ 38.16,     0, 37.38,    0, },
                                        {  62.1, 11.28,  4.18, 32.9, },
                                        { 67.41, 36.48,     0,    0, },
                                        {     0,     0,     0, 8.56  }};

    CRSMatrix<double> b { a };

    ASSERT_EQ(a, b);
}

TEST(CRSMatrix, Test_Correct_Matrix_Multiplication) {
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

    std::vector<std::vector<double>> not_true_result {{ 1.0, 0.0, 1282.26,   0.0, },
                                                      { 0.0, 0.0,  1336.2, 46.08, },
                                                      { 0.0, 0.0,     0.0,   0.0, },
                                                      { 0.0, 0.0, 3223.71,   0.0, }};

    auto result = CRSMatrix<double>(a) * CRSMatrix<double>(b);

    ASSERT_EQ(true_result, result);
    ASSERT_NE(not_true_result, result);
}

TEST(CRSMatrix, Test_Not_Correct_Matrix_Multiplication) {
    std::vector<std::vector<double>> a {{ 14.2,  0.0,  0.0, 0.0, },
                                        {  4.4, 14.4, 93.1, 0.0, },
                                        {  0.0,  0.0, 73.7, 0.0, },
                                        { 35.7,  0.0,  0.0, 0.0  }};

    std::vector<std::vector<double>> b {{ 0.0, 0.0,  90.3,  0.0, },
                                        { 0.0, 0.0,  65.2,  3.2, },
                                        { 0.0, 0.0,   0.0,  0.0, },
                                        { 0.0,   6,   0.0,  0.0  }};

    std::vector<std::vector<double>> not_true_result {{ 1.0/*0.0*/, 0.0, 1282.26,   0.0, },
                                                      {        0.0, 0.0,  1336.2, 46.08, },
                                                      {        0.0, 0.0,     0.0,   0.0, },
                                                      {        0.0, 0.0, 3223.71,   0.0, }};

    auto result = CRSMatrix<double>(a) * CRSMatrix<double>(b);

    ASSERT_NE(not_true_result, result);
}

TEST(CRSMatrix, Test_Correct_Matrix_Multiplication_To_Scalar) {
    std::vector<std::vector<double>> a {{ 14.2,  0.0,  0.0, 0.0, },
                                        {  4.4, 14.4, 93.1, 0.0, },
                                        {  0.0,  0.0, 73.7, 0.0, },
                                        { 35.7,  0.0,  0.0, 0.0  }};

    std::vector<std::vector<double>> true_result {{ 5 * 14.2,      0.0,      0.0, 0.0, },
                                                  {  5 * 4.4, 5 * 14.4, 5 * 93.1, 0.0, },
                                                  {      0.0,      0.0, 5 * 73.7, 0.0, },
                                                  { 5 * 35.7,      0.0,      0.0, 0.0  }};

    auto result = 5 * CRSMatrix<double>(a);

    ASSERT_EQ(true_result, result);
}

TEST(CRSMatrix, Test_Correct_Matrix_Multiplication_To_Negative) {
    std::vector<std::vector<double>> a {{ 14.2,  0.0,  0.0, 0.0, },
                                        {  4.4, 14.4, 93.1, 0.0, },
                                        {  0.0,  0.0, 73.7, 0.0, },
                                        { 35.7,  0.0,  0.0, 0.0  }};

    std::vector<std::vector<double>> true_result {{ -201.64,     0.0,      0.0, 0.0, },
                                                  { -125.84, -207.36, -8202.11, 0.0, },
                                                  {     0.0,     0.0, -5431.69, 0.0, },
                                                  { -506.94,     0.0,      0.0, 0.0  }};

    auto result = CRSMatrix<double>(a) * (-CRSMatrix<double>(a));

    ASSERT_EQ(true_result, result);
}

TEST(CRSMatrix, Test_Correct_Matrix_Multiplication_Random_Numbers) {
    std::vector<std::vector<double>> a(4, std::vector<double>(4, 0));
    std::vector<std::vector<double>> b(4, std::vector<double>(4, 0));

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<> dist(0, 300);

    for (auto& i : a) {
        for (auto& j : i) {
            j = dist(rng);
        }
    }

    auto result = CRSMatrix<double>(a) * CRSMatrix<double>(b);

    // just easy math
    // A * 0 = 0
    ASSERT_EQ(result, b);
}

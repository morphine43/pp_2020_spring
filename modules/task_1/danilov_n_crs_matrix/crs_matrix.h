// Copyright 2020 Danilov Nikita

#ifndef MODULES_TASK_1_DANILOV_N_CRS_MATRIX_CRS_MATRIX_H_
#define MODULES_TASK_1_DANILOV_N_CRS_MATRIX_CRS_MATRIX_H_

#include <limits>
#include <utility>
#include <algorithm>
#include <vector>

// Compressed Row Storage (CRS) format for storing sparse matrices.
template <typename T>
class CRSMatrix {
 private:
    std::size_t rowsNumber;
    std::size_t columnsNumber;
    std::vector<T> values;
    std::vector<std::size_t> columns;
    std::vector<std::size_t> rows;

 public:
    inline CRSMatrix<T>() : rowsNumber {0}, columnsNumber {0}
    {
    }

    inline CRSMatrix<T>(std::size_t m, std::size_t n) {
        if (m < 0 || n < 0)
            throw "Error: invalid dimensions.";

        rowsNumber = m;
        columnsNumber = n;
        rows.assign(m + 1, 0);
    }

    inline CRSMatrix<T>(const std::vector<std::vector<T>>& array) {
        auto largestSubarray = std::max_element(array.begin(), array.end(),
                                                [](const auto& a, const auto& b) {
                                                    return a.size() < b.size();
                                                });

        std::size_t m = array.size();
        std::size_t n = largestSubarray->size();

        if (m < 0 || n < 0)
            throw "Error: invalid dimensions.";

        rowsNumber = m;
        columnsNumber = n;
        rows.assign(m + 1, 0);

        for (std::size_t i = 0; i < m; ++i)
            for (std::size_t j = 0; j < array[i].size(); ++j)
                if (array[i][j] != T{0})
                    set(i, j, array[i][j]);
    }

    inline CRSMatrix(const CRSMatrix<T>& matrix) {
        rowsNumber = matrix.rowsNumber;
        columnsNumber = matrix.columnsNumber;
        values = matrix.values;
        rows = matrix.rows;
        columns = matrix.columns;
    }

    inline CRSMatrix<T>& operator=(const CRSMatrix<T>& matrix) {
        if (this != &matrix)
            *this(matrix);

        return *this;
    }

    inline void resizeRows(std::size_t m) {
        if (m > rowsNumber)
            addRows(m - rowsNumber);
        else if (m < rowsNumber)
            removeRows(rowsNumber - m);
    }

    inline void resizeColumns(std::size_t n) {
        if (n > columnsNumber)
            addColumns(n - columnsNumber);
        else if (n < columnsNumber)
            removeColumns(columnsNumber - n);
    }

    inline void resize(std::size_t m, std::size_t n) {
        resizeRows(m);
        resizeColumns(n);
    }

    inline std::size_t getrowsNumber() const {
        return rowsNumber;
    }

    inline std::size_t getcolumnsNumber() const {
        return columnsNumber;
    }

    inline T get(std::size_t m, std::size_t n) const {
        if (m < 0 || n < 0 || m >= rowsNumber || n >= columnsNumber)
            throw "Error: indices out of range";

        for (std::size_t i = rows[m]; i < rows[m + 1]; ++i)
            if (columns[i] >= n)
                return (columns[i] == n) ? values[i] : T{0};

        return T{0};
    }

    inline std::size_t getRow(std::size_t i) const {
        auto row = std::find_if(rows.begin(), rows.end(),
                                [&](const auto& f) {
                                    return f >= i;
                                });

        return row - rows.begin();
    }

    inline void set(std::size_t m, std::size_t n, const T& value) {
        if (m < 0 || n < 0)
            std::cout << "Incorrect coordinates to set: [" << m << ", " << n << "]" << std::endl;
        else if (m >= rowsNumber)
            resizeRows(m);
        else if (n >= columnsNumber)
            resizeColumns(n);

        std::size_t i;

        for (i = rows[m]; i < rows[m + 1]; ++i)
            if (columns[i] >= n)
                break;

        bool isZeroTarget = (i >= columns.size() || columns[i] != n);
        bool isZeroSource = (value == T{0});

        if (isZeroSource && isZeroTarget) {
            // Source and target are both zero do nothing
            // Skip
        } else if (isZeroSource && !isZeroTarget) {
            // Zeroing target
            remove(m, i);
        } else if (!isZeroSource && !isZeroTarget) {
            // Replace value
            // Maybe they match?
            // Minor optimization possible
            values[i] = value;
        } else if (!isZeroSource && isZeroTarget) {
            // Insert nonzero value in zero place
            insert(m, n, i, value);
        }
    }

    inline bool empty() const {
        return (rowsNumber == 0) || (columnsNumber == 0) || values.empty();
    }

    inline std::size_t size() const {
        return rowsNumber * columnsNumber;
    }

    inline std::size_t countNonzeros() const {
        return rows.back();
    }

    inline std::size_t rowSize(std::size_t i) const {
        return rows[i + 1] - rows[i];
    }

    inline std::size_t countZeros() const {
        return size() - countNonzeros();
    }

    inline void showFormat() const {
        std::cout << (*this);

        std::cout << "Values:  { ";
        for (const auto& i : values)
            std::cout << i << ' ';
        std::cout << "}" << std::endl;

        std::cout << "Columns: { ";
        for (const auto& i : columns)
            std::cout << i << ' ';
        std::cout << "}" << std::endl;

        std::cout << "Rows:    { ";
        for (const auto& i : rows)
            std::cout << i << ' ';
        std::cout << "}" << std::endl;
    }

    inline const T operator()(std::size_t m, std::size_t n) const {
        return get(m, n);
    }

    inline T operator()(std::size_t m, std::size_t n) {
        return get(m, n);
    }

    inline CRSMatrix<T> operator-() const {
        return -1 * (*this);
    }

    friend inline CRSMatrix<T> operator*(const CRSMatrix<T>& left, const CRSMatrix<T>& right) {
        if (left.columnsNumber != right.rowsNumber)
            throw "Error: the number of columns in the left matrix does not equal "
                  "the number of rows in the right matrix.";

        CRSMatrix<T> result(left.rowsNumber, right.columnsNumber);

        T sum = T{0};

        for (std::size_t i = 0; i < left.rowsNumber; ++i) {
            for (std::size_t j = 0; j < right.columnsNumber; ++j) {
                for (std::size_t k = 0; k < left.columnsNumber; ++k)
                    sum += left.get(i, k) * right.get(k, j);

                result.set(i, j, sum);

                sum = T{0};
            }
        }

        return result;
    }

    friend inline CRSMatrix<T> operator+(const CRSMatrix<T>& left, const CRSMatrix<T>& right) {
        if (left.rowsNumber != right.rowsNumber || left.columnsNumber != right.columnsNumber)
            throw "Error: matrices' dimensions do not match.";

        CRSMatrix<T> sum = left;

        for (std::size_t i = 0; i < right.rowsNumber; ++i)
            for (std::size_t j = right.rows[i]; j < right.rows[i + 1]; ++j)
                sum.set(i, right.columns[j], sum.get(i, right.columns[j]) + right.values[j]);

        return sum;
    }

    friend inline CRSMatrix<T> operator-(const CRSMatrix<T>& left, const CRSMatrix<T>& right) {
        return left + (-right);
    }

    friend inline CRSMatrix<T> operator*(const T& scalar, const CRSMatrix<T>& right) {
        auto matrix = right;

        std::for_each(matrix.values.begin(), matrix.values.end(),
                      [&](auto& i) {
                        i *= scalar;
                      });

        return matrix;
    }

    friend inline bool operator==(const CRSMatrix<T>& left, const CRSMatrix<T>& right) {
        return ((left.rowsNumber == right.rowsNumber) &&
                (left.columnsNumber == right.columnsNumber) &&
                (std::equal(left.values.begin(), left.values.end(), right.values.begin(),
                            [&](auto& a, auto& b) {
                                // Some magic with doubles compare
                                return (std::abs(a - b) <= (std::numeric_limits<T>::epsilon() *
                                                            std::max(std::abs(a), std::abs(b)) * 7) ||
                                        std::abs(a - b) < std::numeric_limits<T>::min());
                            })) &&
                (left.columns == right.columns) &&
                (left.rows == right.rows));
    }

    friend inline bool operator==(const std::vector<std::vector<T>>& left, const CRSMatrix<T>& right) {
        return (CRSMatrix<T>{left} == right);
    }

    friend inline bool operator==(const CRSMatrix<T>& left, const std::vector<std::vector<T>>& right) {
        return (right == left);
    }

    friend inline bool operator!=(const CRSMatrix<T>& left, const CRSMatrix<T>& right) {
        return !(left == right);
    }

    friend inline std::ostream& operator<<(std::ostream& out, const CRSMatrix<T>& matrix) {
        for (std::size_t i = 0; i < matrix.rowsNumber; ++i) {
            out << '[';
            for (std::size_t j = 0; j < matrix.columnsNumber; ++j)
                out << matrix.get(i, j) << ((j == matrix.columnsNumber - 1) ? "]\n" : " ");
        }

        return out;
    }

 private:
    inline void clear() {
        values.clear();
        columns.clear();

        std::fill(rows.begin(), rows.end(), 0);
    }

    inline void insert(std::size_t m, std::size_t n, std::size_t i, const T& value) {
        values.insert(values.begin() + i, value);
        columns.insert(columns.begin() + i, n);

        std::for_each(rows.begin() + m + 1, rows.end(),
                      [](auto& f) {
                          ++f;
                      });
    }

    inline void remove(std::size_t m, std::size_t i) {
        values.erase(values.begin() + i);
        columns.erase(columns.begin() + i);

        std::for_each(rows.begin() + m + 1, rows.end(),
                      [](auto& f) {
                          --f;
                      });
    }

    inline void addRows(std::size_t k) {
        rowsNumber += k;
        rows.resize(rowsNumber + 1, rows.back());
    }

    inline void addColumns(std::size_t k) {
        columnsNumber += k;
    }

    inline void removeRows(std::size_t k) {
        rowsNumber -= k;
        values.erase(values.begin() + rows[rowsNumber], values.end());
        columns.erase(columns.begin() + rows[rowsNumber], columns.end());
        rows.erase(rows.begin() + rowsNumber + 1, rows.end());
    }

    inline void removeColumns(std::size_t k) {
        columnsNumber -= k;

        auto colsBeg = columns.begin();
        auto colsEnd = columns.end();

        auto rowsBeg = rows.begin();
        auto rowsEnd = rows.end();

        auto colsIter   = colsBeg;
        auto colsNewEnd = colsBeg;

        auto valsIter   = values.begin();
        auto valsNewEnd = valsIter;

        for (; colsIter != colsEnd; ++colsIter, ++valsIter) {
            if ((*colsIter) < columnsNumber) {
                *(colsNewEnd++) = std::move(*colsIter);
                *(valsNewEnd++) = std::move(*valsIter);
            } else {
                std::for_each(rowsBeg + getRow(colsNewEnd - colsBeg) + 1, rowsEnd,
                              [](auto& f) {
                                  --f;
                              });
            }
        }

        columns.assign(colsBeg, colsNewEnd);
        values.assign(values.begin(), valsNewEnd);
    }
};

#endif  // MODULES_TASK_1_DANILOV_N_CRS_MATRIX_CRS_MATRIX_H_

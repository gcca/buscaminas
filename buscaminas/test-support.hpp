#pragma once

#include <cstddef>
#include <ostream>
#include <string>

#include "gmock/gmock.h"

#include "buscaminas.hpp"

namespace buscaminas {

inline void PrintTo(const Dump &dump, std::ostream *os) {
  *os << dump.rows() << "x" << dump.cols();
  for (std::size_t row = 0; row < dump.rows(); ++row) {
    *os << "\n  ";
    for (std::size_t col = 0; col < dump.cols(); ++col) {
      *os << dump.At(row, col);
    }
  }
}

namespace testing_support {

struct BoardSpec {
  std::size_t rows;
  std::size_t cols;
  std::size_t mines;
};

inline void PrintTo(const BoardSpec &spec, std::ostream *os) {
  *os << spec.rows << "x" << spec.cols << " with " << spec.mines << " mines";
}

inline std::string
BoardSpecName(const ::testing::TestParamInfo<BoardSpec> &info) {
  return "Rows" + std::to_string(info.param.rows) + "Cols" +
         std::to_string(info.param.cols) + "Mines" +
         std::to_string(info.param.mines);
}

inline std::string Cells(const Dump &dump) {
  return std::string(dump.begin(), dump.end());
}

inline std::string Row(const Dump &dump, const std::size_t row) {
  std::string cells;
  for (std::size_t col = 0; col < dump.cols(); ++col) {
    cells.push_back(dump.At(row, col));
  }
  return cells;
}

inline std::size_t MinesAround(const Dump &dump, const std::size_t row,
                               const std::size_t col) {
  const std::size_t first_row = row == 0 ? 0 : row - 1;
  const std::size_t last_row = row + 1 < dump.rows() ? row + 1 : row;
  const std::size_t first_col = col == 0 ? 0 : col - 1;
  const std::size_t last_col = col + 1 < dump.cols() ? col + 1 : col;

  std::size_t mines = 0;
  for (std::size_t r = first_row; r <= last_row; ++r) {
    for (std::size_t c = first_col; c <= last_col; ++c) {
      if ((r != row || c != col) && dump.At(r, c) == '*') { ++mines; }
    }
  }
  return mines;
}

MATCHER(HasConsistentNeighborCounts,
        std::string(negation ? "has" : "has no") +
            " safe cell whose digit differs from its neighboring mines") {
  const Dump &dump = arg;
  for (std::size_t row = 0; row < dump.rows(); ++row) {
    for (std::size_t col = 0; col < dump.cols(); ++col) {
      const char cell = dump.At(row, col);
      if (cell == '*') { continue; }
      const std::size_t mines = MinesAround(dump, row, col);
      if (cell != static_cast<char>('0' + mines)) {
        *result_listener << "cell (" << row << ", " << col << ") is '" << cell
                         << "' but touches " << mines << " mines";
        return false;
      }
    }
  }
  return true;
}

class MockDump : public Dump {
public:
  MockDump() = default;

  MOCK_METHOD(const_iterator, begin, (), (const, noexcept, override));
  MOCK_METHOD(const_iterator, end, (), (const, noexcept, override));
  MOCK_METHOD(char, At, (std::size_t row, std::size_t col),
              (const, noexcept, override));
  MOCK_METHOD(std::size_t, rows, (), (const, noexcept, override));
  MOCK_METHOD(std::size_t, cols, (), (const, noexcept, override));
};

} // namespace testing_support
} // namespace buscaminas

#include <sstream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "buscaminas.hpp"
#include "test-support.hpp"

using buscaminas::testing_support::Cells;
using buscaminas::testing_support::HasConsistentNeighborCounts;
using buscaminas::testing_support::Row;
using ::testing::Contains;
using ::testing::Each;

template <std::size_t Rs, std::size_t Cs, std::size_t Ms>
class BoardRxCMTest : public testing::Test {
protected:
  void SetUp() final {
    board = buscaminas::Board::Make(Rs, Cs, Ms);
    board->Shuffle();
  }
  std::unique_ptr<buscaminas::Board> board;
};

class Board3x3M3Test : public BoardRxCMTest<3, 3, 3> {};

TEST_F(Board3x3M3Test, Dumps) {
  const auto uncover = board->DumpUncover();
  ASSERT_NE(nullptr, uncover);
  ASSERT_EQ(3, uncover->rows());
  ASSERT_EQ(3, uncover->cols());

  std::string expected;
  for (std::size_t row = 0; row < 3; ++row) {
    for (std::size_t col = 0; col < 3; ++col) {
      expected.push_back(uncover->At(row, col));
      expected.push_back(col == 2 ? '\n' : ' ');
    }
  }

  std::ostringstream output;
  board->Dumps(output);
  EXPECT_EQ(expected, output.str());
}

TEST_F(Board3x3M3Test, DumpUncover) {
  auto dump = board->DumpUncover();
  ASSERT_NE(nullptr, dump);
  ASSERT_EQ(3, dump->rows());
  ASSERT_EQ(3, dump->cols());
  const std::string cells = Cells(*dump);
  ASSERT_EQ(9, cells.size());
  EXPECT_THAT(cells, Contains('*').Times(3));
  EXPECT_THAT(cells, Each(::testing::AnyOfArray(std::string("*012345678"))));
  EXPECT_THAT(*dump, HasConsistentNeighborCounts());
}

TEST_F(Board3x3M3Test, DumpCover) {
  auto dump = board->DumpCover();
  ASSERT_NE(nullptr, dump);
  EXPECT_EQ(3, dump->rows());
  EXPECT_EQ(3, dump->cols());
  EXPECT_EQ(std::string(9, '#'), Cells(*dump));
}

TEST_F(Board3x3M3Test, DumpSplit) {
  board->Open(0, 0);
  board->Open(1, 1);
  board->Open(2, 2);
  auto dump = board->DumpSplit();
  const auto uncover = board->DumpUncover();
  ASSERT_NE(nullptr, dump);
  ASSERT_NE(nullptr, uncover);
  ASSERT_EQ(3, dump->rows());
  ASSERT_EQ(7, dump->cols());
  ASSERT_EQ(21, Cells(*dump).size());
  ASSERT_EQ(3, uncover->rows());
  ASSERT_EQ(3, uncover->cols());
  for (std::size_t row = 0; row < 3; ++row) {
    std::string covered(3, '#');
    covered[row] = uncover->At(row, row);
    EXPECT_EQ(covered + " " + Row(*uncover, row), Row(*dump, row))
        << "row " << row;
  }
}

TEST_F(Board3x3M3Test, DumpCoverAfterOpen) {
  board->Open(0, 0);
  auto dump = board->DumpCover();
  const auto uncover = board->DumpUncover();
  ASSERT_NE(nullptr, dump);
  ASSERT_NE(nullptr, uncover);
  ASSERT_EQ(3, uncover->rows());
  ASSERT_EQ(3, uncover->cols());
  EXPECT_EQ(3, dump->rows());
  EXPECT_EQ(3, dump->cols());
  std::string expected(9, '#');
  expected[0] = uncover->At(0, 0);
  EXPECT_EQ(expected, Cells(*dump));
}

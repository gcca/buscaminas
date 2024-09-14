#include "gtest/gtest.h"

#include "buscaminas.hpp"

std::ostream &operator<<(std::ostream &os, const buscaminas::Dump &dump) {
  for (std::size_t row = 0; row < dump.rows(); ++row) {
    for (std::size_t col = 0; col < dump.cols(); ++col) {
      os << dump.At(row, col);
    }
    os << std::endl;
  }
  return os;
}

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

TEST_F(Board3x3M3Test, Dumps) { board->Dumps(std::cout); }

TEST_F(Board3x3M3Test, DumpUncover) {
  auto dump = board->DumpUncover();
  std::cout << *dump;

  std::size_t size_counter = 0;
  std::size_t mine_counter = 0;
  for (char c : *dump) {
    ++size_counter;
    if (c == '*') { ++mine_counter; }
  }

  EXPECT_EQ(9, size_counter);
  EXPECT_EQ(3, mine_counter);
}

TEST_F(Board3x3M3Test, DumpCover) {
  auto dump = board->DumpCover();
  std::cout << *dump;

  std::size_t counter = 0;
  for (char c : *dump) {
    if (c == '#') { ++counter; }
  }

  EXPECT_EQ(9, counter);
}

TEST_F(Board3x3M3Test, DumpSplit) {
  board->Open(0, 0);
  board->Open(1, 1);
  board->Open(2, 2);
  auto dump = board->DumpSplit();
  std::cout << *dump;
}

TEST_F(Board3x3M3Test, DumpCoverAfterOpen) {
  board->Open(0, 0);
  auto dump = board->DumpCover();
  std::cout << *dump;

  std::size_t counter = 0;
  for (char c : *dump) {
    if (c == '#') { ++counter; }
  }

  EXPECT_EQ(8, counter);
}

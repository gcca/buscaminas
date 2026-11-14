#include <cstddef>
#include <memory>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "buscaminas.hpp"
#include "test-support.hpp"

namespace {

using buscaminas::Board;
using buscaminas::Dump;
using buscaminas::testing_support::BoardSpec;
using buscaminas::testing_support::BoardSpecName;
using buscaminas::testing_support::Cells;
using buscaminas::testing_support::HasConsistentNeighborCounts;
using ::testing::AllOf;
using ::testing::AnyOfArray;
using ::testing::Contains;
using ::testing::Each;
using ::testing::SizeIs;

constexpr int kShuffles = 50;

const std::string kSymbols = "*012345678";

class BoardPropertiesTest : public ::testing::TestWithParam<BoardSpec> {
protected:
  void SetUp() override {
    board_ = Board::Make(spec().rows, spec().cols, spec().mines);
  }

  const BoardSpec &spec() const { return GetParam(); }
  std::size_t size() const { return spec().rows * spec().cols; }

  void ExpectValidUncover(const Dump &dump) const {
    EXPECT_THAT(Cells(dump), AllOf(SizeIs(size()), Each(AnyOfArray(kSymbols)),
                                   Contains('*').Times(spec().mines)));
    EXPECT_THAT(dump, HasConsistentNeighborCounts());
  }

  void ExpectAccessorsAgree(const Dump &dump) const {
    ASSERT_EQ(spec().rows, dump.rows());
    ASSERT_EQ(spec().cols, dump.cols());
    ASSERT_EQ(static_cast<std::ptrdiff_t>(size()), dump.end() - dump.begin());
    for (std::size_t row = 0; row < dump.rows(); ++row) {
      for (std::size_t col = 0; col < dump.cols(); ++col) {
        EXPECT_EQ(dump.begin()[row * dump.cols() + col], dump.At(row, col))
            << "at (" << row << ", " << col << ")";
      }
    }
  }

  std::unique_ptr<Board> board_;
};

TEST_P(BoardPropertiesTest, UnshuffledBoardIsEmptyAndCovered) {
  EXPECT_EQ(std::string(size(), '0'), Cells(*board_->DumpUncover()));
  EXPECT_EQ(std::string(size(), '#'), Cells(*board_->DumpCover()));
}

TEST_P(BoardPropertiesTest, ShuffleKeepsCoverClosed) {
  board_->Shuffle();
  EXPECT_EQ(std::string(size(), '#'), Cells(*board_->DumpCover()));
}

TEST_P(BoardPropertiesTest, ShuffleProducesValidUncover) {
  for (int i = 0; i < kShuffles; ++i) {
    SCOPED_TRACE("fresh board " + std::to_string(i));
    std::unique_ptr<Board> board =
        Board::Make(spec().rows, spec().cols, spec().mines);
    board->Shuffle();
    ExpectValidUncover(*board->DumpUncover());
  }
}

TEST_P(BoardPropertiesTest, ReshuffleReplacesPreviousMines) {
  for (int i = 0; i < kShuffles; ++i) {
    SCOPED_TRACE("reshuffle " + std::to_string(i));
    board_->Shuffle();
    ExpectValidUncover(*board_->DumpUncover());
  }
}

TEST_P(BoardPropertiesTest, DumpAccessorsAgree) {
  board_->Shuffle();
  board_->Open(0, 0);
  board_->Open(spec().rows - 1, spec().cols - 1);
  {
    SCOPED_TRACE("uncover");
    ExpectAccessorsAgree(*board_->DumpUncover());
  }
  {
    SCOPED_TRACE("cover");
    ExpectAccessorsAgree(*board_->DumpCover());
  }
}

INSTANTIATE_TEST_SUITE_P(
    Sizes, BoardPropertiesTest,
    ::testing::Values(BoardSpec{2, 2, 1}, BoardSpec{3, 3, 3},
                      BoardSpec{3, 5, 4}, BoardSpec{5, 3, 4},
                      BoardSpec{8, 8, 10}, BoardSpec{16, 30, 99},
                      BoardSpec{9, 9, 0}, BoardSpec{4, 4, 16}),
    BoardSpecName);

TEST(BoardExtremesTest, NoMinesGivesAllZeros) {
  std::unique_ptr<Board> board = Board::Make(6, 4, 0);
  board->Shuffle();
  EXPECT_EQ(std::string(24, '0'), Cells(*board->DumpUncover()));
}

TEST(BoardExtremesTest, AllMinesGivesAllMines) {
  std::unique_ptr<Board> board = Board::Make(4, 6, 24);
  board->Shuffle();
  EXPECT_EQ(std::string(24, '*'), Cells(*board->DumpUncover()));
}

TEST(BoardExtremesTest, SingleSafeCellCountsAllNeighbors) {
  std::unique_ptr<Board> board = Board::Make(3, 3, 8);
  for (int i = 0; i < kShuffles; ++i) {
    SCOPED_TRACE("shuffle " + std::to_string(i));
    board->Shuffle();
    const std::string cells = Cells(*board->DumpUncover());
    EXPECT_THAT(cells, Contains('*').Times(8));
    if (cells[4] != '*') { EXPECT_EQ('8', cells[4]); }
  }
}

} // namespace

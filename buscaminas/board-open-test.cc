#include <cstddef>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>

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
using buscaminas::testing_support::Row;
using ::testing::Each;
using ::testing::Ne;

using Cell = std::pair<std::size_t, std::size_t>;

class BoardOpenTest : public ::testing::TestWithParam<BoardSpec> {
protected:
  void SetUp() override {
    board_ = Board::Make(spec().rows, spec().cols, spec().mines);
    board_->Shuffle();
  }

  const BoardSpec &spec() const { return GetParam(); }
  std::size_t size() const { return spec().rows * spec().cols; }

  std::set<Cell> SomeCells() const {
    const std::size_t last_row = spec().rows - 1;
    const std::size_t last_col = spec().cols - 1;
    return {{0, 0},
            {0, last_col},
            {last_row, 0},
            {last_row, last_col},
            {spec().rows / 2, spec().cols / 2}};
  }

  void OpenAll(const std::set<Cell> &cells) {
    for (const Cell &cell : cells) { board_->Open(cell.first, cell.second); }
  }

  std::string ExpectedCover(const std::set<Cell> &opened) const {
    const std::string uncover = Cells(*board_->DumpUncover());
    std::string cover(size(), '#');
    for (const Cell &cell : opened) {
      const std::size_t index = cell.first * spec().cols + cell.second;
      cover[index] = uncover[index];
    }
    return cover;
  }

  std::unique_ptr<Board> board_;
};

TEST_P(BoardOpenTest, CoverRevealsOnlyOpenedCells) {
  const std::set<Cell> opened = SomeCells();
  OpenAll(opened);
  EXPECT_EQ(ExpectedCover(opened), Cells(*board_->DumpCover()));
}

TEST_P(BoardOpenTest, OpeningTwiceChangesNothing) {
  board_->Open(0, 0);
  const std::string once = Cells(*board_->DumpCover());
  board_->Open(0, 0);
  EXPECT_EQ(once, Cells(*board_->DumpCover()));
  EXPECT_EQ(ExpectedCover({{0, 0}}), once);
}

TEST_P(BoardOpenTest, OpeningEveryCellUncoversTheBoard) {
  for (std::size_t row = 0; row < spec().rows; ++row) {
    for (std::size_t col = 0; col < spec().cols; ++col) {
      board_->Open(row, col);
    }
  }
  const std::string cover = Cells(*board_->DumpCover());
  EXPECT_THAT(cover, Each(Ne('#')));
  EXPECT_EQ(Cells(*board_->DumpUncover()), cover);
}

TEST_P(BoardOpenTest, DumpsAreSnapshots) {
  const std::unique_ptr<Dump> cover = board_->DumpCover();
  const std::unique_ptr<Dump> uncover = board_->DumpUncover();
  const std::unique_ptr<Dump> split = board_->DumpSplit();
  const std::string cover_cells = Cells(*cover);
  const std::string uncover_cells = Cells(*uncover);
  const std::string split_cells = Cells(*split);

  OpenAll(SomeCells());
  board_->Shuffle();
  OpenAll({{0, 0}});

  EXPECT_EQ(std::string(size(), '#'), cover_cells);
  EXPECT_EQ(cover_cells, Cells(*cover));
  EXPECT_EQ(uncover_cells, Cells(*uncover));
  EXPECT_EQ(split_cells, Cells(*split));
}

TEST_P(BoardOpenTest, OpenedCellsStayOpenAcrossShuffle) {
  const std::set<Cell> opened = SomeCells();
  OpenAll(opened);
  for (int i = 0; i < 20; ++i) {
    SCOPED_TRACE("shuffle " + std::to_string(i));
    board_->Shuffle();
    EXPECT_EQ(ExpectedCover(opened), Cells(*board_->DumpCover()));
  }
}

TEST_P(BoardOpenTest, SplitJoinsCoverAndUncoverRows) {
  OpenAll(SomeCells());
  const std::unique_ptr<Dump> split = board_->DumpSplit();
  const std::unique_ptr<Dump> cover = board_->DumpCover();
  const std::unique_ptr<Dump> uncover = board_->DumpUncover();

  ASSERT_EQ(spec().rows, split->rows());
  ASSERT_EQ(2 * spec().cols + 1, split->cols());
  ASSERT_EQ(static_cast<std::ptrdiff_t>(spec().rows * (2 * spec().cols + 1)),
            split->end() - split->begin());
  for (std::size_t row = 0; row < spec().rows; ++row) {
    EXPECT_EQ(Row(*cover, row) + " " + Row(*uncover, row), Row(*split, row))
        << "row " << row;
  }
}

TEST_P(BoardOpenTest, DumpsStreamsUncoverWithSpacesAndNewlines) {
  OpenAll(SomeCells());
  const std::unique_ptr<Dump> uncover = board_->DumpUncover();
  std::string expected;
  for (std::size_t row = 0; row < spec().rows; ++row) {
    for (std::size_t col = 0; col < spec().cols; ++col) {
      expected.push_back(uncover->At(row, col));
      expected.push_back(col + 1 == spec().cols ? '\n' : ' ');
    }
  }

  std::ostringstream os;
  board_->Dumps(os);
  EXPECT_EQ(expected, os.str());
}

INSTANTIATE_TEST_SUITE_P(
    Sizes, BoardOpenTest,
    ::testing::Values(BoardSpec{2, 2, 1}, BoardSpec{3, 3, 3},
                      BoardSpec{2, 4, 3}, BoardSpec{5, 7, 8},
                      BoardSpec{7, 5, 8}, BoardSpec{16, 30, 99}),
    BoardSpecName);

} // namespace

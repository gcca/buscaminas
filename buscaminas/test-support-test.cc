#include <cstddef>
#include <ostream>
#include <string>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "buscaminas.hpp"
#include "test-support.hpp"

namespace {

using buscaminas::Dump;
using buscaminas::testing_support::Cells;
using buscaminas::testing_support::HasConsistentNeighborCounts;
using buscaminas::testing_support::MinesAround;
using buscaminas::testing_support::MockDump;
using buscaminas::testing_support::Row;
using ::testing::_;
using ::testing::Return;

class LayoutDump {
public:
  LayoutDump(const std::size_t rows, const std::size_t cols, std::string cells)
      : rows_{rows}, cols_{cols}, cells_{std::move(cells)} {
    ON_CALL(mock_, rows()).WillByDefault(Return(rows_));
    ON_CALL(mock_, cols()).WillByDefault(Return(cols_));
    ON_CALL(mock_, begin()).WillByDefault(Return(cells_.data()));
    ON_CALL(mock_, end()).WillByDefault(Return(cells_.data() + cells_.size()));
    ON_CALL(mock_, At(_, _))
        .WillByDefault([this](const std::size_t row, const std::size_t col) {
          return cells_[row * cols_ + col];
        });
  }

  const Dump &dump() const { return mock_; }
  ::testing::NiceMock<MockDump> &mock() { return mock_; }

private:
  const std::size_t rows_, cols_;
  const std::string cells_;
  ::testing::NiceMock<MockDump> mock_;
};

const char kSquare[] = "*211"
                       "12*1"
                       "0122"
                       "001*";

const char kWide[] = "**1"
                     "221";

TEST(TestSupportTest, CellsReadsThroughIteratorsOnly) {
  LayoutDump layout{4, 4, kSquare};
  EXPECT_CALL(layout.mock(), begin()).Times(1);
  EXPECT_CALL(layout.mock(), end()).Times(1);
  EXPECT_CALL(layout.mock(), At(_, _)).Times(0);

  EXPECT_EQ(kSquare, Cells(layout.dump()));
}

TEST(TestSupportTest, RowReadsEachColumnOnce) {
  LayoutDump layout{4, 4, kSquare};
  EXPECT_CALL(layout.mock(), begin()).Times(0);
  for (std::size_t col = 0; col < 4; ++col) {
    EXPECT_CALL(layout.mock(), At(2, col)).Times(1);
  }

  EXPECT_EQ("0122", Row(layout.dump(), 2));
}

TEST(TestSupportTest, MinesAroundCountsCornersEdgesAndInterior) {
  LayoutDump square{4, 4, kSquare};
  EXPECT_EQ(0, MinesAround(square.dump(), 0, 0));
  EXPECT_EQ(1, MinesAround(square.dump(), 0, 3));
  EXPECT_EQ(0, MinesAround(square.dump(), 3, 0));
  EXPECT_EQ(0, MinesAround(square.dump(), 3, 3));
  EXPECT_EQ(2, MinesAround(square.dump(), 0, 1));
  EXPECT_EQ(1, MinesAround(square.dump(), 1, 0));
  EXPECT_EQ(2, MinesAround(square.dump(), 2, 3));
  EXPECT_EQ(1, MinesAround(square.dump(), 3, 2));
  EXPECT_EQ(2, MinesAround(square.dump(), 1, 1));
  EXPECT_EQ(2, MinesAround(square.dump(), 2, 2));

  LayoutDump wide{2, 3, kWide};
  EXPECT_EQ(1, MinesAround(wide.dump(), 0, 0));
  EXPECT_EQ(1, MinesAround(wide.dump(), 0, 2));
  EXPECT_EQ(2, MinesAround(wide.dump(), 1, 0));
  EXPECT_EQ(2, MinesAround(wide.dump(), 1, 1));
  EXPECT_EQ(1, MinesAround(wide.dump(), 1, 2));
}

TEST(TestSupportTest, ConsistentLayoutsMatch) {
  LayoutDump square{4, 4, kSquare};
  EXPECT_THAT(square.dump(), HasConsistentNeighborCounts());

  LayoutDump wide{2, 3, kWide};
  EXPECT_THAT(wide.dump(), HasConsistentNeighborCounts());

  LayoutDump empty{2, 2, "0000"};
  EXPECT_THAT(empty.dump(), HasConsistentNeighborCounts());

  LayoutDump full{2, 2, "****"};
  EXPECT_THAT(full.dump(), HasConsistentNeighborCounts());
}

struct Corruption {
  const char *where;
  std::size_t row;
  std::size_t col;
  char digit;
};

void PrintTo(const Corruption &corruption, std::ostream *os) {
  *os << corruption.where;
}

class CorruptedLayoutTest : public ::testing::TestWithParam<Corruption> {};

TEST_P(CorruptedLayoutTest, IsRejectedAtTheWrongCell) {
  const Corruption &corruption = GetParam();
  std::string cells = kSquare;
  cells[corruption.row * 4 + corruption.col] = corruption.digit;
  LayoutDump layout{4, 4, cells};

  ::testing::StringMatchResultListener listener;
  EXPECT_FALSE(::testing::ExplainMatchResult(HasConsistentNeighborCounts(),
                                             layout.dump(), &listener));
  EXPECT_THAT(listener.str(),
              ::testing::HasSubstr("(" + std::to_string(corruption.row) + ", " +
                                   std::to_string(corruption.col) + ")"));
}

INSTANTIATE_TEST_SUITE_P(
    Cells, CorruptedLayoutTest,
    ::testing::Values(Corruption{"Interior", 1, 1, '3'},
                      Corruption{"TopEdge", 0, 1, '1'},
                      Corruption{"RightEdge", 2, 3, '1'},
                      Corruption{"BottomEdge", 3, 1, '1'},
                      Corruption{"LeftEdge", 1, 0, '0'},
                      Corruption{"TopRightCorner", 0, 3, '0'},
                      Corruption{"BottomLeftCorner", 3, 0, '1'}),
    [](const ::testing::TestParamInfo<Corruption> &info) {
      return std::string(info.param.where);
    });

} // namespace

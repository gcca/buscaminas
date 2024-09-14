#include <algorithm>
#include <chrono>
#include <memory>
#include <ostream>
#include <random>

#include "buscaminas.hpp"

using namespace buscaminas;

namespace {

template <class T> class Grid {
public:
  constexpr explicit Grid(std::size_t rows, std::size_t cols, std::size_t size,
                          T *array)
      : rows{rows}, cols{cols}, size{size}, array{array} {}

  [[nodiscard]] constexpr T *operator[](std::size_t row) const noexcept {
    return array + row * cols;
  }

  constexpr void Clean() noexcept { std::fill(array, array + size, 0); }

  const std::size_t rows;
  const std::size_t cols;
  const std::size_t size;
  T *array;
};

class DumpImpl : public Dump {
public:
  explicit DumpImpl(const std::size_t rows, const std::size_t cols)
      : array_{std::make_unique<char[]>(rows * cols)},
        grid_{rows, cols, rows * cols, array_.get()} {}

  const_iterator begin() const noexcept final { return array_.get(); }

  const_iterator end() const noexcept final {
    return array_.get() + grid_.size;
  }

  char At(const std::size_t row, const std::size_t col) const noexcept final {
    return grid_[row][col];
  }

  std::size_t rows() const noexcept final { return grid_.rows; }

  std::size_t cols() const noexcept final { return grid_.cols; }

  static inline std::unique_ptr<DumpImpl> Make(const std::size_t rows,
                                               const std::size_t cols) {
    return std::make_unique<DumpImpl>(rows, cols);
  }

  const std::unique_ptr<char[]> array_;
  const Grid<char> grid_;
};

static const char endl[]{' ', '\n'};
static char print[]{"*012345678"};

class BoardImpl : public Board {
public:
  explicit BoardImpl(const std::size_t rows, const std::size_t cols,
                     const std::size_t mines)
      : mines_{mines}, grid_size_{rows * cols},
        array_{std::make_unique<int[]>(grids_count_ * grid_size_)},
        counts_grid_{rows, cols, grid_size_, array_.get()},
        mines_grid_{rows, cols, grid_size_, counts_grid_.array + grid_size_},
        cover_grid_{rows, cols, grid_size_, mines_grid_.array + grid_size_} {}

  void Shuffle() noexcept final {
    GenerateMines();
    CalculateCounts();
  }

  void Open(const std::size_t row, const std::size_t col) noexcept final {
    cover_grid_[row][col] = 1;
  }

  void Dumps(std::ostream &os) const noexcept final {
    std::size_t i = 0;
    while (i < counts_grid_.size) {
      os << print[(1 + counts_grid_.array[i]) * (1 - mines_grid_.array[i])]
         << endl[++i % counts_grid_.cols == 0];
    }
  }

  std::unique_ptr<Dump> DumpUncover() const noexcept final {
    std::unique_ptr<DumpImpl> dump =
        DumpImpl::Make(counts_grid_.rows, counts_grid_.cols);

    for (std::size_t i = 0; i < counts_grid_.size; ++i) {
      dump->array_[i] =
          print[(1 + counts_grid_.array[i]) * (1 - mines_grid_.array[i])];
    }

    return dump;
  }

  std::unique_ptr<Dump> DumpCover() const noexcept final {
    std::unique_ptr<DumpImpl> dump =
        DumpImpl::Make(counts_grid_.rows, counts_grid_.cols);

    for (std::size_t i = 0; i < counts_grid_.size; ++i) {
      dump->array_[i] =
          cover_grid_.array[i]
              ? print[(1 + counts_grid_.array[i]) * (1 - mines_grid_.array[i])]
              : '#';
    }

    return dump;
  }

  std::unique_ptr<Dump> DumpSplit() const noexcept final {
    std::unique_ptr<DumpImpl> dump =
        DumpImpl::Make(counts_grid_.rows, 2 * counts_grid_.cols + 1);

    std::size_t k = 0;
    for (std::size_t i = 0; i < counts_grid_.rows; ++i) {
      for (std::size_t j = 0; j < counts_grid_.cols; ++j) {
        dump->array_[k++] =
            cover_grid_[i][j]
                ? print[(1 + counts_grid_[i][j]) * (1 - mines_grid_[i][j])]
                : '#';
      }
      dump->array_[k++] = ' ';
      for (std::size_t j = 0; j < counts_grid_.cols; ++j) {
        dump->array_[k++] =
            print[(1 + counts_grid_[i][j]) * (1 - mines_grid_[i][j])];
      }
    }

    return dump;
  }

private:
  const std::size_t mines_, grid_size_;
  std::unique_ptr<int[]> array_;
  Grid<int> counts_grid_, mines_grid_, cover_grid_;
  static constexpr std::size_t grids_count_ = 3;

  inline void GenerateMines() {
    mines_grid_.Clean();
    long long seed =
        std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    int poss[grid_size_];
    std::iota(poss, poss + grid_size_, 0);
    std::shuffle(poss, poss + grid_size_, generator);

    for (std::size_t i = 0; i < mines_; ++i) { mines_grid_.array[poss[i]] = 1; }
  }

  inline void CalculateCounts() {
    counts_grid_.Clean();
    CalculateInnerCounts();
    CalculateCornerCounts();
    CalculateTopCounts();
    CalculateBottomCounts();
    CalculateLeftCounts();
    CalculateRightCounts();
  }

  inline void CalculateInnerCounts() {
    for (std::size_t row = 1; row < counts_grid_.rows - 1; ++row) {
      for (std::size_t col = 1; col < counts_grid_.cols - 1; ++col) {
        if (mines_grid_[row - 1][col - 1]) { ++counts_grid_[row][col]; }
        if (mines_grid_[row - 1][col]) { ++counts_grid_[row][col]; }
        if (mines_grid_[row - 1][col + 1]) { ++counts_grid_[row][col]; }
        if (mines_grid_[row][col - 1]) { ++counts_grid_[row][col]; }
        if (mines_grid_[row][col + 1]) { ++counts_grid_[row][col]; }
        if (mines_grid_[row + 1][col - 1]) { ++counts_grid_[row][col]; }
        if (mines_grid_[row + 1][col]) { ++counts_grid_[row][col]; }
        if (mines_grid_[row + 1][col + 1]) { ++counts_grid_[row][col]; }
      }
    }
  }

  inline void CalculateTopCounts() {
    for (std::size_t col = 1; col < counts_grid_.cols - 1; ++col) {
      if (mines_grid_[0][col - 1]) { ++counts_grid_[0][col]; }
      if (mines_grid_[0][col + 1]) { ++counts_grid_[0][col]; }
      if (mines_grid_[1][col - 1]) { ++counts_grid_[0][col]; }
      if (mines_grid_[1][col]) { ++counts_grid_[0][col]; }
      if (mines_grid_[1][col + 1]) { ++counts_grid_[0][col]; }
    }
  }

  inline void CalculateBottomCounts() {
    const std::size_t row = counts_grid_.rows - 1;
    for (std::size_t col = 1; col < counts_grid_.cols - 1; ++col) {
      if (mines_grid_[row][col - 1]) { ++counts_grid_[row][col]; }
      if (mines_grid_[row][col + 1]) { ++counts_grid_[row][col]; }
      if (mines_grid_[row - 1][col - 1]) { ++counts_grid_[row][col]; }
      if (mines_grid_[row - 1][col]) { ++counts_grid_[row][col]; }
      if (mines_grid_[row - 1][col + 1]) { ++counts_grid_[row][col]; }
    }
  }

  inline void CalculateLeftCounts() {
    for (std::size_t row = 1; row < counts_grid_.rows - 1; ++row) {
      if (mines_grid_[row - 1][0]) { ++counts_grid_[row][0]; }
      if (mines_grid_[row + 1][0]) { ++counts_grid_[row][0]; }
      if (mines_grid_[row - 1][1]) { ++counts_grid_[row][0]; }
      if (mines_grid_[row][1]) { ++counts_grid_[row][0]; }
      if (mines_grid_[row + 1][1]) { ++counts_grid_[row][0]; }
    }
  }

  inline void CalculateRightCounts() {
    const std::size_t col = counts_grid_.cols - 1;
    for (std::size_t row = 1; row < counts_grid_.rows - 1; ++row) {
      if (mines_grid_[row - 1][col]) { ++counts_grid_[row][col]; }
      if (mines_grid_[row + 1][col]) { ++counts_grid_[row][col]; }
      if (mines_grid_[row - 1][col - 1]) { ++counts_grid_[row][col]; }
      if (mines_grid_[row][col - 1]) { ++counts_grid_[row][col]; }
      if (mines_grid_[row + 1][col - 1]) { ++counts_grid_[row][col]; }
    }
  }

  inline void CalculateCornerCounts() {
    if (mines_grid_[0][1]) { ++counts_grid_[0][0]; }
    if (mines_grid_[1][0]) { ++counts_grid_[0][0]; }
    if (mines_grid_[1][1]) { ++counts_grid_[0][0]; }

    const std::size_t col = counts_grid_.cols - 1;
    if (mines_grid_[0][col - 1]) { ++counts_grid_[0][col]; }
    if (mines_grid_[1][col]) { ++counts_grid_[0][col]; }
    if (mines_grid_[1][col - 1]) { ++counts_grid_[0][col]; }

    const std::size_t row = counts_grid_.rows - 1;
    if (mines_grid_[row - 1][0]) { ++counts_grid_[row][0]; }
    if (mines_grid_[row][1]) { ++counts_grid_[row][0]; }
    if (mines_grid_[row - 1][1]) { ++counts_grid_[row][0]; }

    if (mines_grid_[row - 1][col]) { ++counts_grid_[row][col]; }
    if (mines_grid_[row][col - 1]) { ++counts_grid_[row][col]; }
    if (mines_grid_[row - 1][col - 1]) { ++counts_grid_[row][col]; }
  }
};

} // namespace

std::unique_ptr<Board> Board::Make(const std::size_t rows,
                                   const std::size_t cols,
                                   const std::size_t mines) {
  return std::make_unique<BoardImpl>(rows, cols, mines);
}

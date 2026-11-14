#pragma once

#include <cstddef>
#include <memory>

#include "buscaminas.hpp"

namespace bmios {

class GameBoard {
public:
  GameBoard(std::size_t rows, std::size_t cols, std::size_t mines)
      : board_{buscaminas::Board::Make(rows, cols, mines)} {
    board_->Shuffle();
    Refresh();
  }

  void Open(std::size_t row, std::size_t col) {
    board_->Open(row, col);
    Refresh();
  }

  void Shuffle() {
    board_->Shuffle();
    Refresh();
  }

  char At(std::size_t row, std::size_t col) const {
    return cover_->At(row, col);
  }

  std::size_t Rows() const { return cover_->rows(); }

  std::size_t Cols() const { return cover_->cols(); }

private:
  void Refresh() { cover_ = board_->DumpCover(); }

  std::shared_ptr<buscaminas::Board> board_;
  std::shared_ptr<const buscaminas::Dump> cover_;
};

} // namespace bmios

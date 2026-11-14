#pragma once

#include <cstddef>
#include <memory>

namespace buscaminas {

class Dump {
public:
  virtual ~Dump() = default;
  Dump(const Dump &) = delete;
  Dump(const Dump &&) = delete;
  Dump &operator=(const Dump &) = delete;
  Dump &operator=(const Dump &&) = delete;

  using const_iterator = const char *;

  virtual const_iterator begin() const noexcept = 0;
  virtual const_iterator end() const noexcept = 0;

  virtual char At(std::size_t row, std::size_t col) const noexcept = 0;

  virtual std::size_t rows() const noexcept = 0;
  virtual std::size_t cols() const noexcept = 0;

protected:
  Dump() = default;
};

class Board {
public:
  virtual ~Board() = default;
  Board(const Board &) = delete;
  Board(const Board &&) = delete;
  Board &operator=(const Board &) = delete;
  Board &operator=(const Board &&) = delete;

  virtual void Shuffle() noexcept = 0;
  virtual void Open(std::size_t row, std::size_t col) noexcept = 0;

  virtual void Dumps(std::ostream &os) const noexcept = 0;

  [[nodiscard]] virtual std::unique_ptr<Dump> DumpUncover() const noexcept = 0;
  [[nodiscard]] virtual std::unique_ptr<Dump> DumpCover() const noexcept = 0;
  [[nodiscard]] virtual std::unique_ptr<Dump> DumpSplit() const noexcept = 0;

  static std::unique_ptr<Board> Make(std::size_t rows, std::size_t cols,
                                     std::size_t mines);

protected:
  Board() = default;
};

} // namespace buscaminas

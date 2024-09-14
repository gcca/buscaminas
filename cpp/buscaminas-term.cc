#include <cstddef>
#include <cstdlib>
#include <iostream>

#include "termbox2.h"

#include "buscaminas.hpp"

class State {
public:
  explicit State(std::size_t rows, std::size_t cols, std::size_t mines)
      : rows(rows), cols(cols), mines(mines) {}

  void Start() {
    const std::size_t height = tb_height(), width = tb_width();
    padding_x = (height - rows) / 2;
    padding_y = width / 2 - cols;
  }

  const std::size_t rows, cols, mines;
  std::size_t padding_x, padding_y;
};

class Cursor {
public:
  explicit Cursor(const State &state)
      : state{state}, x{state.padding_x}, y{state.padding_y} {}

  void Up() {
    x = x <= state.padding_x ? state.padding_x + state.rows - 1 : x - 1;
  }

  void Down() {
    x = x >= state.padding_x + state.rows - 1 ? state.padding_x : x + 1;
  }

  void Left() {
    y = y <= state.padding_y ? state.padding_y + 2 * state.cols - 2 : y - 2;
  }

  void Right() {
    y = y >= state.padding_y + 2 * state.cols - 2 ? state.padding_y : y + 2;
  }

  std::size_t x, y;
  const State &state;
};

void Show(auto &board, const State &state, const Cursor &cursor) {
  auto dump = board->DumpCover();

  char outcell[] = " ";
  for (std::size_t i = 0; i < dump->rows(); ++i) {
    for (std::size_t j = 0; j < dump->cols(); ++j) {
      outcell[0] = dump->At(i, j);
      uintattr_t fg = TB_DEFAULT;
      switch (outcell[0]) {
      case '#': fg = TB_DEFAULT | TB_DIM; break;
      case '0': fg = TB_BLACK; break;
      case '1': fg = TB_GREEN; break;
      case '2': fg = TB_BLUE; break;
      case '3': fg = TB_YELLOW; break;
      case '4': fg = TB_MAGENTA; break;
      case '5': fg = TB_CYAN; break;
      case '6': fg = TB_WHITE; break;
      case '7': fg = TB_CYAN; break;
      case '8': fg = TB_MAGENTA; break;
      case '*': fg = TB_RED;
      }
      tb_print(2 * j + state.padding_y, i + state.padding_x, fg, 0, outcell);
    }
  }
  tb_print(cursor.y, cursor.x, TB_WHITE, TB_BLACK, "@");
  tb_present();
}

inline void ShowUsage() {
  std::cerr << "Usage: buscaminas-term <rows> <cols> <mines>" << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    ShowUsage();
    return EXIT_FAILURE;
  }

  State state =
      State(std::stoul(argv[1]), std::stoul(argv[2]), std::stoul(argv[3]));

  tb_init();

  state.Start();
  Cursor cursor(state);

  auto board = buscaminas::Board::Make(state.rows, state.cols, state.mines);
  board->Shuffle();

  tb_event ev;
  for (;;) {
    Show(board, state, cursor);
    tb_poll_event(&ev);

    switch (ev.ch) {
    case 'k': cursor.Up(); break;
    case 'j': cursor.Down(); break;
    case 'h': cursor.Left(); break;
    case 'l': cursor.Right(); break;
    case 'x':
      board->Open(cursor.x - state.padding_x, (cursor.y - state.padding_y) / 2);
      cursor.Right();
      break;
    case 's': board->Shuffle(); break;
    case 'q': goto end;
    }
  }

end:
  tb_shutdown();
  return EXIT_SUCCESS;
}

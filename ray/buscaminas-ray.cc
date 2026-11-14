#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <iostream>
#include <string_view>

#include "raylib.h"

#include "buscaminas.hpp"

namespace {

struct Settings {
  int rows = 9;
  int cols = 9;
  int mines = 10;
};

bool ParseNumber(std::string_view text, int &value) {
  if (text.empty() || !std::all_of(text.begin(), text.end(), [](char c) {
        return c >= '0' && c <= '9';
      })) {
    return false;
  }
  auto result = std::from_chars(text.data(), text.data() + text.size(), value);
  return result.ec == std::errc{} && result.ptr == text.data() + text.size();
}

bool ParseSettings(int argc, char **argv, Settings &settings) {
  if (argc == 1) { return true; }
  return argc == 4 && ParseNumber(argv[1], settings.rows) &&
         ParseNumber(argv[2], settings.cols) &&
         ParseNumber(argv[3], settings.mines) && settings.rows >= 2 &&
         settings.rows <= 100 && settings.cols >= 2 && settings.cols <= 100 &&
         settings.mines <= settings.rows * settings.cols;
}

struct Layout {
  float x;
  float y;
  float cell;

  Rectangle Cell(int row, int col) const {
    return {x + col * cell, y + row * cell, cell, cell};
  }
};

Layout FitBoard(const Settings &settings, int width, int height) {
  float available_width = std::max(1, width - 32);
  float available_height = std::max(1, height - 80);
  float cell = std::min(available_width / settings.cols,
                        available_height / settings.rows);
  return {(width - settings.cols * cell) / 2.0f,
          16 + (available_height - settings.rows * cell) / 2.0f, cell};
}

Color CellColor(char value) {
  switch (value) {
  case '1': return BLUE;
  case '2': return DARKGREEN;
  case '3': return RED;
  case '4': return DARKBLUE;
  case '5': return MAROON;
  case '6': return DARKPURPLE;
  case '7': return BLACK;
  case '8': return DARKGRAY;
  case '*': return RED;
  default: return BLACK;
  }
}

void DrawBoard(const buscaminas::Dump &dump, const Settings &settings,
               const Layout &layout, int row, int col) {
  for (int r = 0; r < settings.rows; ++r) {
    for (int c = 0; c < settings.cols; ++c) {
      Rectangle cell = layout.Cell(r, c);
      char value = dump.At(r, c);
      DrawRectangleRec(cell, value == '#' ? GRAY : RAYWHITE);
      DrawRectangleLinesEx(cell, 1, DARKGRAY);
      if (value != '#' && value != '0' && layout.cell >= 14) {
        char text[]{value, '\0'};
        int size = std::max(10, static_cast<int>(layout.cell * 0.55f));
        DrawText(text,
                 static_cast<int>(cell.x +
                                  (cell.width - MeasureText(text, size)) / 2),
                 static_cast<int>(cell.y + (cell.height - size) / 2), size,
                 CellColor(value));
      }
    }
  }
  DrawRectangleLinesEx(layout.Cell(row, col), std::min(3.0f, layout.cell / 4),
                       ORANGE);
}

}

int main(int argc, char **argv) {
  Settings settings;
  if (!ParseSettings(argc, argv, settings)) {
    std::cerr << "Usage: buscaminas-ray [rows cols mines]\n"
                 "Rows and columns: 2-100; mines: 0 through rows * cols.\n";
    return EXIT_FAILURE;
  }

  auto board =
      buscaminas::Board::Make(settings.rows, settings.cols, settings.mines);
  board->Shuffle();
  auto dump = board->DumpCover();

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(960, 720, "Buscaminas");
  if (!IsWindowReady()) {
    std::cerr << "Unable to initialize the raylib window.\n";
    return EXIT_FAILURE;
  }
  SetWindowMinSize(320, 240);
  SetTargetFPS(60);

  int row = 0;
  int col = 0;
  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_Q)) { break; }
    Layout layout = FitBoard(settings, GetScreenWidth(), GetScreenHeight());
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_K)) {
      row = (row + settings.rows - 1) % settings.rows;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_J)) {
      row = (row + 1) % settings.rows;
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_H)) {
      col = (col + settings.cols - 1) % settings.cols;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_L)) {
      col = (col + 1) % settings.cols;
    }

    bool changed = false;
    Vector2 mouse = GetMousePosition();
    Rectangle bounds{layout.x, layout.y, settings.cols * layout.cell,
                     settings.rows * layout.cell};
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse.x >= bounds.x &&
        mouse.y >= bounds.y && mouse.x < bounds.x + bounds.width &&
        mouse.y < bounds.y + bounds.height) {
      row = std::clamp(static_cast<int>((mouse.y - layout.y) / layout.cell), 0,
                       settings.rows - 1);
      col = std::clamp(static_cast<int>((mouse.x - layout.x) / layout.cell), 0,
                       settings.cols - 1);
      board->Open(row, col);
      changed = true;
    }
    if (IsKeyPressed(KEY_X)) {
      board->Open(row, col);
      col = (col + 1) % settings.cols;
      changed = true;
    }
    if (IsKeyPressed(KEY_S)) {
      board->Shuffle();
      changed = true;
    }
    if (IsKeyPressed(KEY_R)) {
      board =
          buscaminas::Board::Make(settings.rows, settings.cols, settings.mines);
      board->Shuffle();
      changed = true;
    }
    if (changed) { dump = board->DumpCover(); }

    BeginDrawing();
    ClearBackground(Color{35, 38, 44, 255});
    DrawBoard(*dump, settings, layout, row, col);
    DrawText("Click / X: open   Arrows / HJKL: move", 16,
             GetScreenHeight() - 48, 16, RAYWHITE);
    DrawText("R: restart   S: shuffle   Q / Esc: quit", 16,
             GetScreenHeight() - 26, 16, RAYWHITE);
    EndDrawing();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}

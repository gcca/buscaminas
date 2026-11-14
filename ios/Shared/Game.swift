import BuscaminasBridge
import Observation

@MainActor
@Observable
final class Game {
  let rows: Int
  let cols: Int
  let mines: Int
  private var board: bmios.GameBoard

  init(rows: Int = 9, cols: Int = 9, mines: Int = 10) {
    self.rows = rows
    self.cols = cols
    self.mines = mines
    board = bmios.GameBoard(rows, cols, mines)
  }

  func cell(_ row: Int, _ col: Int) -> Character {
    Character(UnicodeScalar(UInt8(bitPattern: board.At(row, col))))
  }

  func open(_ row: Int, _ col: Int) {
    board.Open(row, col)
  }

  func shuffle() {
    board.Shuffle()
  }

  func restart() {
    board = bmios.GameBoard(rows, cols, mines)
  }
}

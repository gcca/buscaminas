import BuscaminasBridge
import Observation

@MainActor
@Observable
final class Game {
  static let sizes = 2...30

  private(set) var rows: Int
  private(set) var cols: Int
  private(set) var mines: Int
  private var board: bmios.GameBoard

  var maxMines: Int { rows * cols - 1 }

  init(rows: Int = 9, cols: Int = 9, mines: Int = 10) {
    let rows = Self.clampSize(rows)
    let cols = Self.clampSize(cols)
    let mines = min(max(mines, 1), rows * cols - 1)
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

  func resize(rows: Int, cols: Int) {
    let rows = Self.clampSize(rows)
    let cols = Self.clampSize(cols)
    guard rows != self.rows || cols != self.cols else { return }
    self.rows = rows
    self.cols = cols
    mines = min(mines, maxMines)
    restart()
  }

  func setMines(_ count: Int) {
    let count = min(max(count, 1), maxMines)
    guard count != mines else { return }
    mines = count
    restart()
  }

  private static func clampSize(_ value: Int) -> Int {
    min(max(value, sizes.lowerBound), sizes.upperBound)
  }
}

import BuscaminasBridge
import Observation

@MainActor
@Observable
final class Game {
  static let sizes = 2...30

  private(set) var rows: Int
  private(set) var cols: Int
  private var board: bmios.GameBoard

  var mines: Int { Self.mines(rows, cols) }

  init(rows: Int = 9, cols: Int = 9) {
    self.rows = rows
    self.cols = cols
    board = bmios.GameBoard(rows, cols, Self.mines(rows, cols))
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
    let rows = min(max(rows, Self.sizes.lowerBound), Self.sizes.upperBound)
    let cols = min(max(cols, Self.sizes.lowerBound), Self.sizes.upperBound)
    guard rows != self.rows || cols != self.cols else { return }
    self.rows = rows
    self.cols = cols
    restart()
  }

  private static func mines(_ rows: Int, _ cols: Int) -> Int {
    max(1, (rows * cols * 10 + 40) / 81)
  }
}

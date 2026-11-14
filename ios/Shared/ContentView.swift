import SwiftUI

struct ContentView: View {
  let game: Game

  var body: some View {
    VStack(spacing: 8) {
      BoardView(game: game)
      VStack(spacing: 4) {
        HStack {
          Button("Restart") { game.restart() }
          Button("Shuffle") { game.shuffle() }
          StepControl(title: "Mines", value: game.mines) { step in
            game.setMines(game.mines + step)
          }
        }
        HStack {
          StepControl(title: "Rows", value: game.rows) { step in
            game.resize(rows: game.rows + step, cols: game.cols)
          }
          StepControl(title: "Cols", value: game.cols) { step in
            game.resize(rows: game.rows, cols: game.cols + step)
          }
        }
      }
      .buttonStyle(.borderless)
      .font(.footnote)
      .lineLimit(1)
      .minimumScaleFactor(0.5)
    }
    .padding(4)
  }
}

private struct StepControl: View {
  let title: String
  let value: Int
  let change: (Int) -> Void

  var body: some View {
    HStack(spacing: 6) {
      Button("−") { change(-1) }
      Text("\(title) \(value)")
        .monospacedDigit()
      Button("+") { change(1) }
    }
  }
}

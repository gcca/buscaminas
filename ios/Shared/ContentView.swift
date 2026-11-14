import SwiftUI

struct ContentView: View {
  let game: Game

  var body: some View {
    VStack(spacing: 8) {
      BoardView(game: game)
      HStack {
        Button("Restart") { game.restart() }
        Button("Shuffle") { game.shuffle() }
      }
      .buttonStyle(.borderless)
    }
    .padding(4)
  }
}

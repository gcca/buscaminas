import SwiftUI

@main
struct BuscaminasApp: App {
  @State private var game = Game()

  var body: some Scene {
    WindowGroup {
      ContentView(game: game)
    }
  }
}

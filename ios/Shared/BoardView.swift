import SwiftUI

struct BoardView: View {
  let game: Game

  var body: some View {
    GeometryReader { proxy in
      let size = min(proxy.size.width / CGFloat(game.cols),
                     proxy.size.height / CGFloat(game.rows))
      VStack(spacing: 0) {
        ForEach(0..<game.rows, id: \.self) { row in
          HStack(spacing: 0) {
            ForEach(0..<game.cols, id: \.self) { col in
              Button {
                game.open(row, col)
              } label: {
                Text(CellLabel.text(game.cell(row, col)))
              }
              .buttonStyle(CellStyle(value: game.cell(row, col), size: size))
            }
          }
        }
      }
      .frame(width: proxy.size.width, height: proxy.size.height)
    }
  }
}

enum CellLabel {
  static func text(_ value: Character) -> String {
    value == "#" || value == "0" ? "" : String(value)
  }

  static func color(_ value: Character) -> Color {
    switch value {
    case "1": .blue
    case "2": .green
    case "3": .red
    case "4": .indigo
    case "5": .brown
    case "6": .purple
    case "7": .black
    case "8": .gray
    case "*": .red
    default: .black
    }
  }
}

struct CellStyle: ButtonStyle {
  let value: Character
  let size: CGFloat

  func makeBody(configuration: Configuration) -> some View {
    CellFace(label: configuration.label, value: value, size: size,
             pressed: configuration.isPressed)
  }
}

struct CellFace<Label: View>: View {
  let label: Label
  let value: Character
  let size: CGFloat
  let pressed: Bool
  @Environment(\.isFocused) private var focused

  var body: some View {
    label
      .font(.system(size: size * 0.55, weight: .bold, design: .monospaced))
      .foregroundStyle(CellLabel.color(value))
      .frame(width: size, height: size)
      .background(value == "#" ? (pressed ? Color.gray.opacity(0.6) : .gray)
                               : Color(white: 0.95))
      .border(focused ? Color.orange : Color(white: 0.3),
              width: focused ? max(2, size / 8) : 0.5)
  }
}

//
//  ContentView.swift
//  Buscaminas
//
//  Created by (gcca) on 5/10/24.
//

import SwiftUI

struct ContentView: View {
    static let rows: Int = 16
    static let cols: Int = 10
    static let mines: Int = 15
    
    var body: some View {
        VStack {
            BuscaminasGrid(
                rows: ContentView.rows,
                cols: ContentView.cols,
                mines: ContentView.mines)
        }
        .padding()
    }
}

#Preview {
    ContentView()
}

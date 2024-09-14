//
//  BuscaminasGrid.swift
//  Buscaminas
//
//  Created by (gcca) on 5/10/24.
//

import SwiftUI

struct BuscaminasGrid: View {
    let rows: Int
    let cols: Int
    let mines: Int
    
    var body: some View {
        VStack {
            ForEach(0..<rows, id: \.self) { row in
                HStack {
                    ForEach(0..<cols, id: \.self) { column in
                        Image(systemName: "globe")
                            .resizable()
                            .scaledToFit()
                            .frame(width: UIScreen.main.bounds.width / CGFloat(cols), height: UIScreen.main.bounds.width / CGFloat(rows))
                            .foregroundStyle(.tint)
                    }
                }
            }
        }
    }
}

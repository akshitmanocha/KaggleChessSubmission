#!/bin/bash

# Ensure Stockfish binary is executable
chmod +x ./stockfish

# Run Stockfish with memory profiling (e.g., Valgrind Massif)
valgrind --tool=massif --massif-out-file=massif.out ./stockfish << EOF
uci
setoption name Hash value 1
setoption name Threads value 1
ucinewgame
isready
position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
go movetime 100
quit
EOF

# Analyze memory profiling results
ms_print massif.out
# Tem's Chess Engine v1.3

<p align="center">
  <img src="https://github.com/user-attachments/assets/622801ad-dc28-4e3b-9eb6-09718a58125a" width="31%">
  <img src="https://github.com/user-attachments/assets/ba6b46ae-a0ca-4ceb-9138-6431c150a071" width="31%">
  <img src="https://github.com/user-attachments/assets/b75c9f11-84bf-459e-bf87-cf11fd4bd63c" width="31%">
</p>

A chess engine written from scratch in **C++20**, featuring a custom **OpenGL GUI**, 64-bit bitboards, and a single-threaded search engine capable of evaluating **4M+ positions per second**.

The engine was optimized in two complementary ways:

* **Processing each position faster**
* **Searching fewer positions while preserving the strongest calculated move**

---

# Performance

* **4M+ positions evaluated per second**
* Consistently reaches approximately **8 ply** in under one second in many positions
* Some positions that required **35–40 seconds** in the original v1.0 engine can now be processed in roughly **1 millisecond**

Performance improvements came from two areas.

## 1. Raw Processing Speed

Raw-speed improvements reduced the cost of processing each board state.

### C++20

Rewriting the original Python engine in C++ removed interpreter overhead and provided greater control over memory layout, allocation, and data representation.

### Bitboards

The current engine represents pieces and board properties using **64-bit integers**, where each bit corresponds to one square.

This replaced many board-wide loops with fast bitwise operations and improved:

* Piece lookup
* Move generation
* Attack generation
* Memory usage
* Cache locality
* Make and undo performance

The bitboard rewrite improved raw processing speed by approximately **8×** in several tests.

---

## 2. Search Efficiency

The larger overall gains came from reducing the number of positions searched without changing the strongest calculated result.

### Alpha-Beta Pruning

Alpha-Beta pruning stops searching branches that cannot influence the final move.

This reduces the exponential Minimax search tree and allows the engine to reach much greater depths.

### Transposition Tables

The same position can be reached through multiple move sequences.

A **Zobrist-hashed Transposition Table** stores previously searched positions, including their evaluation, depth, bound type, and best move. Repeated positions can then reuse the stored result instead of being searched again.

### Move Ordering

Alpha-Beta pruning is most effective when strong moves are searched first.

Finding a strong move early creates tighter bounds and causes more branches to be discarded.

Current move ordering:

1. Transposition Table move
2. Winning captures using MVV-LVA
3. Killer moves
4. Remaining quiet moves

### Iterative Deepening

The engine searches progressively deeper:

```text
Depth 1 → Depth 2 → Depth 3 → ...
```

Results from shallower searches provide strong candidate moves for deeper searches, improving move ordering and producing more Alpha-Beta cutoffs.

---

# Features

## Chess Rules

* Complete legal move generation
* Check and checkmate detection
* Castling
* En passant
* Pawn promotion

## Search

* Minimax
* Alpha-Beta pruning
* Iterative Deepening
* Zobrist Hashing
* Transposition Tables
* Killer Move Heuristic
* MVV-LVA capture ordering
* Search profiling and benchmarking

## Architecture and Graphics

* 64-bit bitboards
* Single-threaded search
* Custom OpenGL GUI
  
---

# v1.3 Speed Range By Depth (Moves Ahead)

| Depth | Typical Time |
| ----: | -----------: |
|     5 | ~0.01–0.03 s |
|     6 | ~0.03–0.15 s |
|     7 | ~0.15–0.90 s |
|     8 | ~0.50–1.50 s |

> **Test System**
>
> * Intel Core i7-14700F
> * Release build
> * Single-threaded search

Search time varies depending on the position, move ordering, and number of legal moves.

---

# Estimated Playing Strength

**Estimated Elo:** *To be determined*

The engine has not yet been tested against a formal rating pool.

Planned testing includes:

* Matches against friends with known ratings
* A recorded game against a player rated approximately **1600 Elo**
* Matches against established engines at controlled strength levels
* Win, draw, and loss testing across a larger number of games

The engine currently plays significantly better than I do, but a reliable Elo estimate requires more than a few individual games.

---

# Evolution

## v1.0 — Python Lists

The original engine was written in Python during high school.

The board and pieces were stored using Python lists. To locate pieces, determine whether a square was occupied, or generate attacks, the engine frequently had to iterate through those lists.

This made even basic operations expensive.

Performance:

* **2 ply:** approximately **3–10 seconds**
* Searches beyond that quickly became impractical
* A search near **5 ply** could take hours

This version introduced me to legal move generation, recursive Minimax, and board evaluation, but its data structures and implementation were not designed for performance.

---

## v1.2 — C++ Arrays and Vectors

The engine was rewritten from scratch in C++.

Pieces were stored inside vectors, while the board itself used a fixed array containing pointers to those pieces. This allowed the engine to determine which piece occupied a square through an immediate array index lookup instead of repeatedly scanning a list.

The C++ rewrite improved raw processing speed enough to reach approximately:

* **4 ply in under one second**

This version then introduced the major search optimizations:

* Alpha-Beta pruning
* Iterative Deepening
* Zobrist Hashing
* Transposition Tables
* Move ordering
* Killer moves

Together, these algorithms allowed the engine to reach approximately:

* **6 ply in under one second**

The largest gain came from searching fewer positions rather than only processing positions faster.

---

## v1.3 — 64-Bit Bitboards

The current version replaced the array-and-pointer board with 64-bit bitboards.

Each group of pieces is stored inside a 64-bit integer, allowing the engine to locate pieces, generate attacks, and manipulate board states using really fast CPU bitwise operations.

This rewrite primarily improved raw processing speed.

A search that previously required approximately **8 seconds** now takes around **1 second**, and the engine consistently reaches approximately:

* **8 ply in under one second** in many positions
* **4M+ evaluated positions per second**

The progression was therefore:

```text
Python lists
    → 2 ply in 3–10 seconds

C++ arrays and vectors
    → 4 ply in under one second

Search pruning and caching
    → 6 ply in under one second

64-bit bitboards
    → 8 ply in under one second
```

---

# What I Learned

The main lesson from this project was that performance can be improved in two fundamentally different ways.

The first is making each operation faster. Rewriting the engine in C++ and later converting it to bitboards improved memory access, piece lookup, move generation, and raw positions-per-second performance.

The second—and more important—is reducing how much work must be performed.

Alpha-Beta pruning, Transposition Tables, move ordering, Killer Moves, and Iterative Deepening allowed the engine to search fewer positions while still arriving at the same strongest calculated move.

The bitboard rewrite produced an approximately **8× raw-speed improvement** in several tests. However, the search algorithms produced a much larger overall gain by reducing the number of positions the engine needed to examine.

There is still a LOT of room to improve both areas moving forward from V1.3.

## Future Raw-Speed Improvements

* Incremental Zobrist hashing
* Incremental board evaluation
* Faster attack generation - for sliding pieces because I still cast a loop for sliding piece move gen
* More compact move representations
* Reduced copying and memory overhead

## Future Search Improvements

* Stronger move ordering
* Quiescence search
* Principal Variation Search
* Aspiration windows
* Null-move pruning
* Late Move Reductions
* History heuristic
* Static Exchange Evaluation

The next major improvement will likely come from making the engine more selective, not simply increasing the number of positions it can process.




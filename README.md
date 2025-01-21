# FIDE & Google Efficient Chess AI Challenge Submission
## Ethereal Chess Engine - Memory Optimized Version

### Overview
Ethereal Chess Engine is a competitive chess engine optimized for memory-constrained environments. This version has been specifically tuned to operate within strict memory limits while maintaining strong playing strength.

### Competition Specifications

#### Time Controls
- 10 seconds per game
- 0.1 second Simple Delay (unused time is not banked)
- Games played in pairs with color swapping

#### Hardware Constraints
- 5 MiB RAM limit
- Single CPU core @ 2.20GHz
- 64KiB compressed submission size limit

#### Game Rules
- Standard chess rules apply
- Automatic draws:
  - Threefold repetition
  - 50-move rule (no captures or pawn moves)
- Random opening positions from a curated selection

### Memory Optimizations

#### Transposition Table
- Reduced to 1 MB from original size
- Modified `tt_init` to work with smaller keysize
- Switched from 2 MB hugepages to standard 4096 byte memory pages

#### Search Parameters
- Reduced `MAX_PLY` and `MAX_MOVES` parameters in `types.h` by 50%
- Optimized for memory efficiency while maintaining search depth

#### Pawn King Cache
- Reduced PK cache size from 2^16 to 2^13 elements
- Updated PK cache mask accordingly
- Balanced between memory usage and evaluation accuracy

### Competition System

#### Rating System
- Skill rating modeled as Gaussian N(μ,σ²)
  - μ: estimated skill
  - σ: uncertainty factor (decreases over time)
- Initial rating: μ₀ = 600
- Ratings adjusted based on game outcomes

#### Validation Process
- Self-play validation episode on submission
- Automatic error detection and logging
- Performance monitoring throughout competition

### Technical Implementation Notes
- Memory allocations optimized for 5 MiB RAM constraint
- Critical attention to memory page alignment
- Efficient state management for time control handling

### Development Focus
- Memory efficiency without sacrificing playing strength
- Robust time management for 10s games
- Optimization for single-core performance

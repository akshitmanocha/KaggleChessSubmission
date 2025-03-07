/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2016 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

<<<<<<<< Updated upstream:Cfish-cfish_12/src/main.c
#include <stdio.h>

#include "bitboard.h"
#include "endgame.h"
#include "pawns.h"
#include "polybook.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "tt.h"
#include "uci.h"
#include "tbprobe.h"

int main(int argc, char **argv)
{
  print_engine_info(false);

  psqt_init();
  bitboards_init();
  zob_init();
  bitbases_init();
  endgames_init();
  threads_init();
  options_init();
  search_clear();

  uci_loop(argc, argv);

  threads_exit();
  TB_free();
  options_free();
  tt_free();
  pb_free();

  return 0;
}
========
#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"

#define GEN_CAPTURES     0
#define GEN_QUIETS       1
#define GEN_QUIET_CHECKS 2
#define GEN_EVASIONS     3
#define GEN_NON_EVASIONS 4
#define GEN_LEGAL        5

ExtMove *generate_captures(const Pos *pos, ExtMove *list);
ExtMove *generate_quiets(const Pos *pos, ExtMove *list);
ExtMove *generate_quiet_checks(const Pos *pos, ExtMove *list);
ExtMove *generate_evasions(const Pos *pos, ExtMove *list);
ExtMove *generate_non_evasions(const Pos *pos, ExtMove *list);
ExtMove *generate_legal(const Pos *pos, ExtMove *list);

#endif

>>>>>>>> Stashed changes:CfishLite/movegen.h

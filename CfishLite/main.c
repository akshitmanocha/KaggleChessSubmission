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

#include <stdio.h>

#include "bitboard.h"
#include "endgame.h"
#include "pawns.h"
//#include "polybook.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "tt.h"
#include "uci.h"
#include "tbprobe.h"

// #include <sys/resource.h>
// #include <sys/time.h>

// void print_memory_usage(const char *step) {
//     struct rusage usage;
//     getrusage(RUSAGE_SELF, &usage);
//     printf("Memory usage after %s: %ld KB\n", step, usage.ru_maxrss);
// }

int main(int argc, char **argv) {
    //print_engine_info(0);

    //print_memory_usage("startup");

    psqt_init();
    //print_memory_usage("psqt_init");

    bitboards_init();
    //print_memory_usage("bitboards_init");

    zob_init();
    //print_memory_usage("zob_init");

    bitbases_init();
    //print_memory_usage("bitbases_init");

    search_init();
    //print_memory_usage("search_init");

    pawn_init();
    //print_memory_usage("pawn_init");

    endgames_init();
    //print_memory_usage("endgames_init");

    threads_init();
    //print_memory_usage("threads_init");

    options_init();
    //print_memory_usage("options_init");

    search_clear();
    //print_memory_usage("search_clear");

    uci_loop(argc, argv);

    threads_exit();
    //print_memory_usage("threads_exit");

    options_free();
    //print_memory_usage("options_free");

    tt_free();
    //print_memory_usage("tt_free");

    return 0;
}

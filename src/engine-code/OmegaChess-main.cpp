//This is the main cpp file for the OmegaChess Engine

#include <iostream>
#include <memory>

#include "BitBoard.h"
#include "Pieces/Pawn.h"
#include "Pieces/Knight.h"
#include "Pieces/King.h"
#include "Pieces/Bishop.h"
#include "Pieces/Rook.h"
#include "Pieces/Queen.h"
#include "Magic.h"
#include "Timer.h"

#include "BoardState.h"
#include "Move.h"

#include "../GUI-code/UCI/UCI.h"

#include "Evaluation/BasicEval.h"

using namespace std;

int main ()
{
    //ALL INITS to be moved to single function later
    //Initialize pawn attack tables
    PawnAttacks::init();
    //Initialize knight attack tables
    KnightAttacks::init();
    //Initialize king attack tables
    KingAttacks::init();
    //Init magic numbers
    //Magic::init_magic_numbers();
    //KEEP MAGIC INITS
    //Initialize Bishop attack tables
    BishopAttacks::init();
    //Initialize Rook attack tables
    RookAttacks::init();

    // FEN dedug positions
    //char* empty_board = "8/8/8/8/8/8/8/8 w - - 0 0";
    //char* start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
    char* tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
    //char* killer_position = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
    char* cmk_position = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 ";
    //rnbqkbnr/pppppppp/2p5/3p4/4P3/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    //"r1b5/kpQp3p/6r1/2p5/1P2PP2/1P6/2P4P/R1BK3R b - - 0 30"


    auto board = std::make_shared<Boardstate>();
    board->FEN_parse(cmk_position);

    bool debug = true;

    if (debug)
    {
        printf("Debug:\n\n");
        board->print_board();
        MoveList moves;
        board->generate_moves(moves);
        //moves.print_move_list();
        Search::search_position(board,7,NegaMaxSearch);
        //NegaMax::PV_table[0][NegaMax::get_ply()].print_move_UCI();
        //NegaMax::killer_moves[0][NegaMax::get_ply()] = moves.get_move(3);
        //NegaMax::killer_moves[1][NegaMax::get_ply()] = moves.get_move(2);
        //NegaMax::history_moves[moves.get_move(0).get_move_piece()][moves.get_move(0).get_move_target_square()] = 35;
        //std::cout << Search::search_position(board,1,NegaMaxSearch) << std::endl;
        //BasicEval::print_move_score(board, moves);
        //Search::sort_moves(board,moves);
        std::cout << "\n\n";
        //BasicEval::print_move_score(board, moves);
    }
    else
    {
        auto UCI = UCI_Link{};
        UCI.UCI_loop();
    }


    return EXIT_SUCCESS;
}




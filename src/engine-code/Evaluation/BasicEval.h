#ifndef BASICEVAL_H
#define BASICEVAL_H

#include <memory>
#include <cstdlib>
#include <time.h>

#include "../BitBoard.h"
#include "../BoardState.h"
#include "../Move.h"


/** \file BasicEval.h
    \brief Contains simple evalution information and functions for testing of engine
 */

namespace BasicEval
{
    constexpr int NUMBER_OF_PIECES = 12;
    inline constexpr int material_scores[NUMBER_OF_PIECES] =
    {
        100,    //White Pawn
        300,    //White Knight
        350,    //White Bishop
        500,    //White Rook
        1000,   //White Queen
        10000,  //White King
        -100,   //Black Pawn
        -300,   //Black Knight
        -350,   //Black Bishop
        -500,   //Black Rook
        -1000,  //Black Queen
        -10000  //Black King
    };
    constexpr int NUMBER_OF_SQUARES = 64;
    constexpr int DOUBLE_NUMBER_OF_SQUARES = 64;
    // pawn positional score
    inline constexpr int pawn_scores[NUMBER_OF_SQUARES] =
    {
        90,  90,  90,  90,  90,  90,  90,  90,
        30,  30,  30,  40,  40,  30,  30,  30,
        20,  20,  20,  30,  30,  30,  20,  20,
        10,  10,  10,  20,  20,  10,  10,  10,
         5,   5,  10,  20,  20,   5,   5,   5,
         0,   0,   0,   5,   5,   0,   0,   0,
         0,   0,   0, -10, -10,   0,   0,   0,
         0,   0,   0,   0,   0,   0,   0,   0
    };

    // knight positional score
    inline constexpr int knight_scores[NUMBER_OF_SQUARES] =
    {
        -5,   0,   0,   0,   0,   0,   0,  -5,
        -5,   0,   0,  10,  10,   0,   0,  -5,
        -5,   5,  20,  20,  20,  20,   5,  -5,
        -5,  10,  20,  30,  30,  20,  10,  -5,
        -5,  10,  20,  30,  30,  20,  10,  -5,
        -5,   5,  20,  10,  10,  20,   5,  -5,
        -5,   0,   0,   0,   0,   0,   0,  -5,
        -5, -10,   0,   0,   0,   0, -10,  -5
    };

    // bishop positional score
    inline constexpr int bishop_scores[NUMBER_OF_SQUARES] =
    {
         0,   0,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,   0,   0,
         0,   0,   0,  10,  10,   0,   0,   0,
         0,   0,  10,  20,  20,  10,   0,   0,
         0,   0,  10,  20,  20,  10,   0,   0,
         0,  10,   0,   0,   0,   0,  10,   0,
         0,  30,   0,   0,   0,   0,  30,   0,
         0,   0, -10,   0,   0, -10,   0,   0

    };

    // rook positional score
    inline constexpr int rook_scores[NUMBER_OF_SQUARES] =
    {
        50,  50,  50,  50,  50,  50,  50,  50,
        50,  50,  50,  50,  50,  50,  50,  50,
         0,   0,  10,  20,  20,  10,   0,   0,
         0,   0,  10,  20,  20,  10,   0,   0,
         0,   0,  10,  20,  20,  10,   0,   0,
         0,   0,  10,  20,  20,  10,   0,   0,
         0,   0,  10,  20,  20,  10,   0,   0,
         0,   0,   0,  20,  20,   0,   0,   0

    };

    // king positional score
    inline constexpr int king_scores[NUMBER_OF_SQUARES] =
    {
         0,   0,   0,   0,   0,   0,   0,   0,
         0,   0,   5,   5,   5,   5,   0,   0,
         0,   5,   5,  10,  10,   5,   5,   0,
         0,   5,  10,  20,  20,  10,   5,   0,
         0,   5,  10,  20,  20,  10,   5,   0,
         0,   0,   5,  10,  10,   5,   0,   0,
         0,   5,   5,  -5,  -5,   0,   5,   0,
         0,   0,   5,   0, -15,   0,  10,   0
    };

    // mirror positional score tables for opposite side
    inline constexpr int mirror_scores[DOUBLE_NUMBER_OF_SQUARES] =
    {
        a1, b1, c1, d1, e1, f1, g1, h1,
        a2, b2, c2, d2, e2, f2, g2, h2,
        a3, b3, c3, d3, e3, f3, g3, h3,
        a4, b4, c4, d4, e4, f4, g4, h4,
        a5, b5, c5, d5, e5, f5, g5, h5,
        a6, b6, c6, d6, e6, f6, g6, h6,
        a7, b7, c7, d7, e7, f7, g7, h7,
        a8, b8, c8, d8, e8, f8, g8, h8
    };
    //Evaluate a position
    int evaluate(std::shared_ptr<Boardstate> board_state);

    //Score a move from a movelist to enable Move ordering and reduction of Alpha Beta search
    int score_move(std::shared_ptr<Boardstate> board_state, Move move);
    void print_move_score(std::shared_ptr<Boardstate> board_state, MoveList &move_list);

    // Using most valuable victim & less valuable attacker method to prune alpha beta search
    // by increasing the value of low value pieces capturing high value pieces, i.e. pawn
    // captures queen is worth more than queen captures pawn. Given by table below:
    /*
        (Victims) Pawn Knight Bishop   Rook  Queen   King
      (Attackers)
            Pawn   105    205    305    405    505    605
          Knight   104    204    304    404    504    604
          Bishop   103    203    303    403    503    603
            Rook   102    202    302    402    502    602
           Queen   101    201    301    401    501    601
            King   100    200    300    400    500    600
    */
    constexpr auto ATTACKER = 12;
    constexpr auto VICTIM = 12;
    //Indexed using standard enumeration for pieces
    //i.e. P, N, B ... q, k
    inline constexpr int mvv_lva[ATTACKER][VICTIM] = {
        105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
        104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
        103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
        102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
        101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
        100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

        105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
        104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
        103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
        102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
        101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
        100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
    };
}

#endif



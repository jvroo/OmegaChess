#ifndef QUEEN_H
#define QUEEN_H

#include "../BitBoard.h"
#include "Rook.h"
#include "Bishop.h"
#include "../Magic.h"

/** \file Queen.h
    \brief Contains queen info
 */

//Used to lookup Queen attacks from precalculated Bishop and Rook attacks
namespace QueenAttacks
{
    constexpr auto NUM_SQUARES = 64;

    //FIND A WAY TO MAKE THIS STATIC INLINE TO IMPROVE MOVE GEN SPEED
    bitboard get_queen_attacks(int square, bitboard occupancy);
};



#endif



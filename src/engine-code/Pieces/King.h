#ifndef KING_H
#define KING_H

#include "../BitBoard.h"
#include "Pawn.h"

/** \file King.h
    \brief Contains King information
 */

namespace KingAttacks
{
    constexpr auto NUM_SQUARES = 64;

    bitboard get_king_attacks(int square);

    //Constants used in precalculated functions
    constexpr auto EMPTY_BITMAP = 0ull;
    constexpr auto ONE_SHIFT = 1ull;
    //Set for left shift, invert to use in right shift for mirror effect
    constexpr auto MOVE_UP = 8ull;
    constexpr auto MOVE_UP_RIGHT = 7ull;
    constexpr auto MOVE_UP_LEFT = 9ull;
    constexpr auto MOVE_LEFT = 1ull;

    //Array of precomputed king attack masks
    inline bitboard king_attacks[NUM_SQUARES];

    //Mask king attacks for specific piece
    constexpr bitboard mask_king_attacks(int square)
    {
        auto attacks = EMPTY_BITMAP;
        auto piece = EMPTY_BITMAP;
        piece |= (ONE_SHIFT << square);
        //TOP AND LEFT ATTACKS
        if (piece >> MOVE_UP)
            attacks |= (piece >> MOVE_UP);
        if ((piece >> MOVE_UP_LEFT) & PawnAttacks::NOT_H_FILE)
            attacks |= (piece >> MOVE_UP_LEFT);
        if ((piece >> MOVE_UP_RIGHT) & PawnAttacks::NOT_A_FILE)
            attacks |= (piece >> MOVE_UP_RIGHT);
        if ((piece >> MOVE_LEFT) & PawnAttacks::NOT_H_FILE)
            attacks |= (piece >> MOVE_LEFT);
        //BOTTOM AND RIGHT ATTACKS
        if (piece << MOVE_UP)
            attacks |= (piece << MOVE_UP);
        if ((piece << MOVE_UP_LEFT) & PawnAttacks::NOT_A_FILE)
            attacks |= (piece << MOVE_UP_LEFT);
        if ((piece << MOVE_UP_RIGHT) & PawnAttacks::NOT_H_FILE)
            attacks |= (piece << MOVE_UP_RIGHT);
        if ((piece << MOVE_LEFT) & PawnAttacks::NOT_A_FILE)
            attacks |= (piece << MOVE_LEFT);

        return attacks;
    }

    //Initialize king attacks
    constexpr void init()
    {
        for (auto square = 0; square < NUM_SQUARES; square++)
        {
            king_attacks[square] = mask_king_attacks(square);
        }
    }
}


#endif



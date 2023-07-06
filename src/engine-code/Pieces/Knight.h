#ifndef KNIGHT_H
#define KNIGHT_H

#include "../BitBoard.h"
#include "Pawn.h"

/** \file Knight.h
    \brief Contains Knight information
 */

namespace KnightAttacks
{
    constexpr auto NUM_SQUARES = 64;

    //Not HG-File constant definition
    /*
          8  1 1 1 1 1 1 0 0
          7  1 1 1 1 1 1 0 0
          6  1 1 1 1 1 1 0 0
          5  1 1 1 1 1 1 0 0
          4  1 1 1 1 1 1 0 0
          3  1 1 1 1 1 1 0 0
          2  1 1 1 1 1 1 0 0
          1  1 1 1 1 1 1 0 0

             a b c d e f g h
    */
    constexpr auto NOT_HG_FILE = 4557430888798830399ull;

    //Not AB-File constant definition
    /*
          8  0 0 1 1 1 1 1 1
          7  0 0 1 1 1 1 1 1
          6  0 0 1 1 1 1 1 1
          5  0 0 1 1 1 1 1 1
          4  0 0 1 1 1 1 1 1
          3  0 0 1 1 1 1 1 1
          2  0 0 1 1 1 1 1 1
          1  0 0 1 1 1 1 1 1

             a b c d e f g h
    */
    constexpr auto NOT_AB_FILE = 18229723555195321596ull;

    bitboard get_knight_attacks(int square);

    //Constants used in precalculated functions
    constexpr auto EMPTY_BITMAP = 0ull;
    constexpr auto ONE_SHIFT = 1ull;
    constexpr auto LEAP_TOP_RIGHT = 15ull;
    constexpr auto LEAP_TOP_LEFT = 17ull;
    constexpr auto LEAP_MID_RIGHT = 6ull;
    constexpr auto LEAP_MID_LEFT = 10ull;

    //Array of precomputed knight attack masks
    inline bitboard knight_attacks[NUM_SQUARES];

    //Mask knight attacks for specific piece
    constexpr bitboard mask_knight_attacks(int square)
    {
        auto attacks = EMPTY_BITMAP;
        auto piece = EMPTY_BITMAP;
        piece |= (ONE_SHIFT << square);
        //TOP ATTACKS
        if ((piece >> LEAP_TOP_LEFT) & PawnAttacks::NOT_H_FILE)
            attacks |= (piece >> LEAP_TOP_LEFT);
        if ((piece >> LEAP_TOP_RIGHT) & PawnAttacks::NOT_A_FILE)
            attacks |= (piece >> LEAP_TOP_RIGHT);
        if ((piece >> LEAP_MID_LEFT) & NOT_HG_FILE)
            attacks |= (piece >> LEAP_MID_LEFT);
        if ((piece >> LEAP_MID_RIGHT) & NOT_AB_FILE)
            attacks |= (piece >> LEAP_MID_RIGHT);
        //BOTTOM ATTACKS
        if ((piece << LEAP_TOP_LEFT) & PawnAttacks::NOT_A_FILE)
            attacks |= (piece << LEAP_TOP_LEFT);
        if ((piece << LEAP_TOP_RIGHT) & PawnAttacks::NOT_H_FILE)
            attacks |= (piece << LEAP_TOP_RIGHT);
        if ((piece << LEAP_MID_LEFT) & NOT_AB_FILE)
            attacks |= (piece << LEAP_MID_LEFT);
        if ((piece << LEAP_MID_RIGHT) & NOT_HG_FILE)
            attacks |= (piece << LEAP_MID_RIGHT);

        return attacks;
    }

    //Initialize knight attacks
    constexpr void init()
    {
        for (auto square = 0; square < NUM_SQUARES; square++)
        {
            knight_attacks[square] = mask_knight_attacks(square);
        }
    }
}


#endif



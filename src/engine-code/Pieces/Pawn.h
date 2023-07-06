#ifndef PAWN_H
#define PAWN_H

#include "../BitBoard.h"

/** \file Pawn.h
    \brief Contains Pawn class
 */


//Used to define precalculated pawn attacks in an array for later lookups
//Before this can be used the PawnAttacks::init() function must be called
//in main to ensure the array is populated.
namespace PawnAttacks
{
    //Define constants for array definition
    constexpr auto NUM_COLORS = 2;
    constexpr auto NUM_SQUARES = 64;

    //Not A-File constant definition
    /*
          8  0 1 1 1 1 1 1 1
          7  0 1 1 1 1 1 1 1
          6  0 1 1 1 1 1 1 1
          5  0 1 1 1 1 1 1 1
          4  0 1 1 1 1 1 1 1
          3  0 1 1 1 1 1 1 1
          2  0 1 1 1 1 1 1 1
          1  0 1 1 1 1 1 1 1

             a b c d e f g h
    */
    constexpr auto NOT_A_FILE = 18374403900871474942ull;

    //Not H-File constant definition
    /*
          8  1 1 1 1 1 1 1 0
          7  1 1 1 1 1 1 1 0
          6  1 1 1 1 1 1 1 0
          5  1 1 1 1 1 1 1 0
          4  1 1 1 1 1 1 1 0
          3  1 1 1 1 1 1 1 0
          2  1 1 1 1 1 1 1 0
          1  1 1 1 1 1 1 1 0

             a b c d e f g h
    */
    constexpr auto NOT_H_FILE = 9187201950435737471ull;

    bitboard get_pawn_attacks(int color, int square);

    //Constants used in precalculated functions
    constexpr auto EMPTY_BITMAP = 0ull;
    constexpr auto ONE_SHIFT = 1ull;
    constexpr auto BOTTOM_LEFT_AND_TOP_RIGHT_DIAG_SHIFT = 7ull;
    constexpr auto TOP_LEFT_AND_BOTTOM_RIGHT_DIAG_SHIFT = 9ull;

    //Array of precomputed pawn attack masks
    inline bitboard pawn_attacks[NUM_COLORS][NUM_SQUARES];

    //Mask pawn attacks for specific piece
    constexpr bitboard mask_pawn_attacks(int color, int square)
    {
        auto attacks = EMPTY_BITMAP;
        auto piece = EMPTY_BITMAP;
        piece |= (ONE_SHIFT << square);

        if (!color)
        {
            if ((piece >> BOTTOM_LEFT_AND_TOP_RIGHT_DIAG_SHIFT)&NOT_A_FILE)
                attacks |= (piece >> BOTTOM_LEFT_AND_TOP_RIGHT_DIAG_SHIFT);
            if ((piece >> TOP_LEFT_AND_BOTTOM_RIGHT_DIAG_SHIFT)&NOT_H_FILE)
                attacks |= (piece >> TOP_LEFT_AND_BOTTOM_RIGHT_DIAG_SHIFT);
        }
        else if (color)
        {
            if ((piece << BOTTOM_LEFT_AND_TOP_RIGHT_DIAG_SHIFT)&NOT_H_FILE)
                attacks |= (piece << BOTTOM_LEFT_AND_TOP_RIGHT_DIAG_SHIFT);
            if ((piece << TOP_LEFT_AND_BOTTOM_RIGHT_DIAG_SHIFT)&NOT_A_FILE)
                attacks |= (piece << TOP_LEFT_AND_BOTTOM_RIGHT_DIAG_SHIFT);
        }
        return attacks;
    }

    //Initialize Pawn attacks
    constexpr void init()
    {
        for (auto square = 0; square < NUM_SQUARES; square++)
        {
            pawn_attacks[white][square] = mask_pawn_attacks(white,square);
            pawn_attacks[black][square] = mask_pawn_attacks(black,square);
        }
    }

};


#endif



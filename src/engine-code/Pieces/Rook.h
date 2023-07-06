#ifndef ROOK_H
#define ROOK_H

#include "../BitBoard.h"

/** \file Rook.h
    \brief Contains Rook class
 */

//Used to define precalculated Rook attacks and occupancy bits
//in an array for later lookups
namespace RookAttacks
{
    constexpr auto NUM_SQUARES = 64;
    constexpr auto SQUARES_IN_ROW = 8;
    constexpr auto X_LAST_R_F = 6;
    constexpr auto X_FIRST_R_F = 1;
    constexpr auto IN_LAST_R_F = 7;
    constexpr auto IN_FIRST_R_F = 0;

    //Helper function to get occupancy bits for specific square
    bitboard get_rook_occupancy_bits(int square);

    //Constants used in precalculated functions
    constexpr auto EMPTY_BITMAP = 0ull;
    constexpr auto ONE_SHIFT = 1ull;

    //Mask rook occupancy attacks for magic bitboard hashing
    constexpr bitboard mask_rook_occupancies(int square)
    {
        //Initialize attack bitmap output variable
        auto attacks = EMPTY_BITMAP;
        //Initialize rank and file variables
        auto rank = 0;
        auto file = 0;
        auto targRank = square / SQUARES_IN_ROW;
        auto targFile = square % SQUARES_IN_ROW;
        //Mask occupancy bits for each direction:
        //Down
        for (rank = targRank + 1; rank <= X_LAST_R_F; rank++)
            attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + targFile));
        //Up
        for (rank = targRank - 1; rank >= X_FIRST_R_F; rank--)
            attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + targFile));
        //Right
        for (file = targFile + 1; file <= X_LAST_R_F; file++)
            attacks |= (ONE_SHIFT << (targRank * SQUARES_IN_ROW + file));
        //Left
        for (file = targFile - 1; file >= X_FIRST_R_F; file--)
            attacks |= (ONE_SHIFT << (targRank * SQUARES_IN_ROW + file));
        return attacks;
    }

    //Used to generate the rook attacks accounting for blocking pieces
    //Used in precalc but not constexpr for table creation
    bitboard gen_rook_attacks_on_the_fly(int square, bitboard blocker);

    //Rook occupancy bit count for every square on the board
    constexpr int rook_occupancy_bit_count[NUM_SQUARES] = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12
    };

    //Defines Rook attack masks
    inline bitboard rook_masks[NUM_SQUARES] = {};
    //Total number of occupancy bytes
    const auto BYTES_FOR_OCCUPANCIES = 4096;
    //Defines Rook attack table
    inline bitboard rook_attacks[NUM_SQUARES][BYTES_FOR_OCCUPANCIES] = {};
    //Init Rook attacks
    void init();

    //FIND A WAY TO MAKE THIS STATIC INLINE TO IMPROVE MOVE GEN SPEED
    bitboard get_rook_attacks(int square, bitboard occupancy);
};



#endif



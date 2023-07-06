#ifndef BISHOP_H
#define BISHOP_H

#include "../BitBoard.h"

/** \file Bishop.h
    \brief Contains Bishop class
 */

//Used to define precalculated Bishop attacks and occupancy bits
//in an array for later lookups
namespace BishopAttacks
{
    constexpr auto NUM_SQUARES = 64;
    constexpr auto SQUARES_IN_ROW = 8;
    constexpr auto X_LAST_R_F = 6;
    constexpr auto X_FIRST_R_F = 1;
    constexpr auto IN_LAST_R_F = 7;
    constexpr auto IN_FIRST_R_F = 0;

    //Helper function to get occupancy bits for specific square
    bitboard get_bishop_occupancy_bits(int square);

    //Constants used in precalculated functions
    constexpr auto EMPTY_BITMAP = 0ull;
    constexpr auto ONE_SHIFT = 1ull;

    //Mask bishop occupancy attacks for magic bitboard hashing
    constexpr bitboard mask_bishop_occupancies(int square)
    {
        //Initialize attack bitmap output variable
        auto attacks = EMPTY_BITMAP;
        //Initialize rank and file variables
        auto rank = 0;
        auto file = 0;
        auto targRank = square / SQUARES_IN_ROW;
        auto targFile = square % SQUARES_IN_ROW;
        //Mask occupancy bits for each direction
        //Down and right
        for (rank = targRank + 1, file = targFile + 1; rank <= X_LAST_R_F && file <= X_LAST_R_F; rank++, file++)
        {
            attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + file));
        }
        //Up and right
        for (rank = targRank - 1, file = targFile + 1; rank >= X_FIRST_R_F && file <= X_LAST_R_F; rank--, file++)
        {
            attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + file));
        }
        //Down and left
        for (rank = targRank + 1, file = targFile - 1; rank <= X_LAST_R_F && file >= X_FIRST_R_F; rank++, file--)
        {
            attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + file));
        }
        //Up and left
        for (rank = targRank - 1, file = targFile - 1; rank >= X_FIRST_R_F && file >= X_FIRST_R_F; rank--, file--)
        {
            attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + file));
        }
        return attacks;
    }

    //Used to generate the bishop attacks accounting for blocking pieces
    //Used in precalc but not constexpr for table creation
    bitboard gen_bishop_attacks_on_the_fly(int square, bitboard blocker);

    //Bishop occupancy bit count for every square on the board
    constexpr int bishop_occupancy_bit_count[NUM_SQUARES] = {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6
    };

    //Defines bishop attack masks
    inline bitboard bishop_masks[NUM_SQUARES] = {};
    //Total number of occupancy bytes
    const auto BYTES_FOR_OCCUPANCIES = 512;
    //Defines bishop attack table
    inline bitboard bishop_attacks[NUM_SQUARES][BYTES_FOR_OCCUPANCIES] = {};
    //Init Bishop attacks
    void init();

    //FIND A WAY TO MAKE THIS STATIC INLINE TO IMPROVE MOVE GEN SPEED
    bitboard get_bishop_attacks(int square, bitboard occupancy);
};



#endif



#include "Rook.h"

bitboard RookAttacks::get_rook_occupancy_bits(int square)
{
    return mask_rook_occupancies(square);
}

bitboard RookAttacks::gen_rook_attacks_on_the_fly(int square, bitboard blocker)
{
    //Initialize attack bitmap output variable
    auto attacks = EMPTY_BITMAP;
    //Initialize rank and file variables
    auto rank = 0;
    auto file = 0;
    auto targRank = square / SQUARES_IN_ROW;
    auto targFile = square % SQUARES_IN_ROW;
    //Mask attack bits for each direction:
    //Down
    for (rank = targRank + 1; rank <= IN_LAST_R_F; rank++)
    {
        attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + targFile));
        if (ONE_SHIFT << (rank * SQUARES_IN_ROW + targFile) & blocker) break;
    }
    //Up
    for (rank = targRank - 1; rank >= IN_FIRST_R_F; rank--)
    {
        attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + targFile));
        if (ONE_SHIFT << (rank * SQUARES_IN_ROW + targFile) & blocker) break;
    }
    //Right
    for (file = targFile + 1; file <= IN_LAST_R_F; file++)
    {
        attacks |= (ONE_SHIFT << (targRank * SQUARES_IN_ROW + file));
        if (ONE_SHIFT << (targRank * SQUARES_IN_ROW + file) & blocker) break;
    }
    //Left
    for (file = targFile - 1; file >= IN_FIRST_R_F; file--)
    {
        attacks |= (ONE_SHIFT << (targRank * SQUARES_IN_ROW + file));
        if (ONE_SHIFT << (targRank * SQUARES_IN_ROW + file) & blocker) break;
    }
    return attacks;
}

//Necessary or the Magic:: functions are not available
#include "../Magic.h"

void RookAttacks::init()
{
    for (auto square = 0; square < NUM_SQUARES; square++)
    {
        rook_masks[square] = mask_rook_occupancies(square);
        auto attack_mask = rook_masks[square];
        auto occupancy_bit_count = Bitboard::count_bits(attack_mask);
        auto occupancy_indices = (ONE_SHIFT << occupancy_bit_count);
        for (auto index = 0; index < occupancy_indices; index++)
        {
            auto occ = Magic::set_occupancy(index, occupancy_bit_count, attack_mask);
            auto magic_index = (occ * Magic::rook_magic_numbers[square]) >> (NUM_SQUARES - rook_occupancy_bit_count[square]);
            rook_attacks[square][magic_index] = gen_rook_attacks_on_the_fly(square, occ);
        }

    }
}

bitboard RookAttacks::get_rook_attacks(int square, bitboard occupancy)
{
    occupancy &= rook_masks[square];
    occupancy *= Magic::rook_magic_numbers[square];
    occupancy >>= NUM_SQUARES - rook_occupancy_bit_count[square];
    return rook_attacks[square][occupancy];
}



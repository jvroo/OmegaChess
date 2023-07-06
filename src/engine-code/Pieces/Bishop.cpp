#include "Bishop.h"

bitboard BishopAttacks::get_bishop_occupancy_bits(int square)
{
    return mask_bishop_occupancies(square);
}

bitboard BishopAttacks::gen_bishop_attacks_on_the_fly(int square, bitboard blocker)
{
    //Initialize attack bitmap output variable
    auto attacks = EMPTY_BITMAP;
    //Initialize rank and file variables
    auto rank = 0;
    auto file = 0;
    auto targRank = square / SQUARES_IN_ROW;
    auto targFile = square % SQUARES_IN_ROW;
    //Mask attack bits for each direction with blockers accounted for
    //Down and right
    for (rank = targRank + 1, file = targFile + 1; rank <= IN_LAST_R_F && file <= IN_LAST_R_F; rank++, file++)
    {
        attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + file));
        if (ONE_SHIFT << (rank * SQUARES_IN_ROW + file) & blocker) break;
    }
    //Up and right
    for (rank = targRank - 1, file = targFile + 1; rank >= IN_FIRST_R_F && file <= IN_LAST_R_F; rank--, file++)
    {
        attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + file));
        if (ONE_SHIFT << (rank * SQUARES_IN_ROW + file) & blocker) break;
    }
    //Down and left
    for (rank = targRank + 1, file = targFile - 1; rank <= IN_LAST_R_F && file >= IN_FIRST_R_F; rank++, file--)
    {
        attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + file));
        if (ONE_SHIFT << (rank * SQUARES_IN_ROW + file) & blocker) break;
    }
    //Up and left
    for (rank = targRank - 1, file = targFile - 1; rank >= IN_FIRST_R_F && file >= IN_FIRST_R_F; rank--, file--)
    {
        attacks |= (ONE_SHIFT << (rank * SQUARES_IN_ROW + file));
        if (ONE_SHIFT << (rank * SQUARES_IN_ROW + file) & blocker) break;
    }
    return attacks;
}

//Necessary or the Magic:: functions are not available
#include "../Magic.h"

void BishopAttacks::init()
{
    for (auto square = 0; square < NUM_SQUARES; square++)
    {
        bishop_masks[square] = mask_bishop_occupancies(square);
        auto attack_mask = bishop_masks[square];
        auto occupancy_bit_count = Bitboard::count_bits(attack_mask);
        auto occupancy_indices = (ONE_SHIFT << occupancy_bit_count);
        for (auto index = 0; index < occupancy_indices; index++)
        {
            auto occ = Magic::set_occupancy(index, occupancy_bit_count, attack_mask);
            auto magic_index = (occ * Magic::bishop_magic_numbers[square]) >> (NUM_SQUARES - bishop_occupancy_bit_count[square]);
            bishop_attacks[square][magic_index] = gen_bishop_attacks_on_the_fly(square, occ);
        }
    }
}

bitboard BishopAttacks::get_bishop_attacks(int square, bitboard occupancy)
{
    occupancy &= bishop_masks[square];
    occupancy *= Magic::bishop_magic_numbers[square];
    occupancy >>= NUM_SQUARES - bishop_occupancy_bit_count[square];
    return bishop_attacks[square][occupancy];
}

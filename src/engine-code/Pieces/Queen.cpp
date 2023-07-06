#include "Queen.h"

bitboard QueenAttacks::get_queen_attacks(int square, bitboard occupancy)
{
    auto result = bitboard{};
    //Init bishop occupancy
    auto bishopOccupancy = occupancy;
    //Init bishop attacks
    bishopOccupancy &= BishopAttacks::bishop_masks[square];
    bishopOccupancy *= Magic::bishop_magic_numbers[square];
    bishopOccupancy >>= NUM_SQUARES - BishopAttacks::bishop_occupancy_bit_count[square];
    result |= BishopAttacks::bishop_attacks[square][bishopOccupancy];
    //Init Rook occupancy
    auto rookOccupancy = occupancy;
    //Init rook attacks
    rookOccupancy &= RookAttacks::rook_masks[square];
    rookOccupancy *= Magic::rook_magic_numbers[square];
    rookOccupancy >>= NUM_SQUARES - RookAttacks::rook_occupancy_bit_count[square];
    result |= RookAttacks::rook_attacks[square][rookOccupancy];
    //Return Queen attacks
    return result;
}



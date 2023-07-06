#include "Knight.h"

bitboard KnightAttacks::get_knight_attacks(int square)
{
    return knight_attacks[square];
}


#include "King.h"

bitboard KingAttacks::get_king_attacks(int square)
{
    return king_attacks[square];
}


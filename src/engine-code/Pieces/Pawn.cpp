#include "Pawn.h"

bitboard PawnAttacks::get_pawn_attacks(int color, int square)
{
    return pawn_attacks[color][square];
}

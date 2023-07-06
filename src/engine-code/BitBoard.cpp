#include "BitBoard.h"

const auto BOARD_SIZE = 8;
const auto EMPTY_BITMAP = 0ull;
const auto ONE_SHIFT = 1ull;

void Bitboard::printBoard(bitboard bmap)
{
    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto square = rank*BOARD_SIZE+file;
            if (!file)
                printf("  %d  ", BOARD_SIZE - rank);
            printf("%d ", get_bit(bmap,square));
        }
        printf("\n");
    }
    printf("\n     a b c d e f g h\n\n\n");
    printf("Bitboard: %llud \n\n", bmap);
}

bool Bitboard::get_bit(bitboard &bmap, const int square)
{
    auto bit = (bmap & (ONE_SHIFT << square)) ? true : false;
    return bit;
}

void Bitboard::set_bit(bitboard &bmap, const int square)
{
    bmap |= (ONE_SHIFT << square);
}

void Bitboard::pop_bit(bitboard &bmap, const int square)
{
    get_bit(bmap,square) ? bmap ^= (ONE_SHIFT << square) : EMPTY_BITMAP;
}




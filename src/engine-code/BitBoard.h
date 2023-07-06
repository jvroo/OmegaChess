#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdio.h>

using bitboard = unsigned long long;

/** \file BitBoard.h
    \brief Contains BitBoard class
 */


 /*
    Enumeration of the squares mapping them to an index from 0 to 63.
    a8 is given index 0 and counted left to right, then top to bottom to
    determine the remaining indices. I.E. h1 = 63.
 */
enum
{
	a8 = 0, b8,c8,d8,e8,f8,g8,h8,
	a7,b7,c7,d7,e7,f7,g7,h7,
	a6,b6,c6,d6,e6,f6,g6,h6,
	a5,b5,c5,d5,e5,f5,g5,h5,
	a4,b4,c4,d4,e4,f4,g4,h4,
	a3,b3,c3,d3,e3,f3,g3,h3,
	a2,b2,c2,d2,e2,f2,g2,h2,
	a1,b1,c1,d1,e1,f1,g1,h1, no_sq
};

/*
    Converts the index mapping of squares to their relevant
    string names. If square_to_coordinate[63] is called
    it will return the string "h1"
*/
inline const char* square_to_coordinate[] = {
	"a8","b8","c8","d8","e8","f8","g8","h8",
	"a7","b7","c7","d7","e7","f7","g7","h7",
	"a6","b6","c6","d6","e6","f6","g6","h6",
	"a5","b5","c5","d5","e5","f5","g5","h5",
	"a4","b4","c4","d4","e4","f4","g4","h4",
	"a3","b3","c3","d3","e3","f3","g3","h3",
	"a2","b2","c2","d2","e2","f2","g2","h2",
	"a1","b1","c1","d1","e1","f1","g1","h1"
};

/*
    Enumeration of the colors. In was chosen over bool to
    allow for a both colors flag as needed.
*/
enum
{
    white = 0, black, both
};

/*
    Enumeration of pieces to be used in functions easily
*/
enum
{
    rook = 0, bishop, knight, queen, king, pawn
};

namespace Bitboard
{
    bool get_bit(bitboard &bmap,const int square);
    void set_bit(bitboard &bmap,const int square);
    void pop_bit(bitboard &bmap,const int square);
    void printBoard(const bitboard bmap);
    //Static and inline to increase speed as heavily used
    static inline int count_bits(bitboard bmap)
    {
        auto bitCount = 0;
        //Count bits using Brian Kernighan's method
        while (bmap)
        {
            bitCount++;
            //remove LSB on each loop
            bmap &= bmap - 1;
        }
        return bitCount;
    }
    //Static and inline to increase speed as heavily used
    static inline int get_lsb_index(bitboard bmap)
    {
        if (bmap)
        {
            return count_bits((bmap & (-bmap)) - 1);
        }
        else
            return -1; //Illegal index
    }
}

#endif



#ifndef MOVE_H
#define MOVE_H

#include <vector>
#include <string>

#include "BitBoard.h"


/** \file Move.h
    \brief Contains move encoding
 */

/*
    Move is encoded using a 24 bit number - I.E. 6 Nibbles

    BINARY VALUE                    ENCODED INFO    HEXADECIMAL
    0000 0000 0000 0000 0011 1111   Source square   0x3F
    0000 0000 0000 1111 1100 0000   Target square   0xFC0
    0000 0000 1111 0000 0000 0000   Piece           0xF000
    0000 1111 0000 0000 0000 0000   Promotion Val   0xF0000
    0001 0000 0000 0000 0000 0000   Capture Flag    0x100000
    0010 0000 0000 0000 0000 0000   DoublePushFlg   0x200000
    0100 0000 0000 0000 0000 0000   EnPassant Flg   0x400000
    1000 0000 0000 0000 0000 0000   Castling Flag   0x800000
*/

//This class defines a move and its relevent encodings
class Move
{
public:
    //ENCODERS
    Move();
    Move(unsigned long move);
    Move(int source, int target, int piece, int promotion_piece,
         bool capture, bool doublePush, bool en_passant, bool castling);
    void encode_move(int source, int target, int piece, int promotion_piece,
         bool capture, bool doublePush, bool en_passant, bool castling);
    //DECODERS
    int get_move_source_square();
    int get_move_target_square();
    int get_move_piece();
    int get_move_promotion_type();
    bool get_move_capture_flag();
    bool get_move_double_push_flag();
    bool get_move_en_passant_flag();
    bool get_move_castling_flag();
    bool is_no_move();

    //Printer
    void print_move();
    void print_move_UCI();

    //get move UCI string
    std::string get_move_UCI();

    //Overloaded operators for equality
    bool operator==(Move& rhs);

private:
    unsigned long encoded_move = 0ul; //This has 32 - bits
};

//Maximum number of moves on 8 x 8 board seems to be 218
const auto MAX_MOVES_PER_POS = 256;

//Defines the move list class
class MoveList
{
public:
    void print_move_list();
    void add_move(Move move);
    void replace_move(Move move, unsigned int index);
    void remove_move(unsigned int index);
    void clear_moves();
    int get_num_moves();
    Move get_move(int index) const;
private:
    std::vector<Move> moves = {};
    int num_move_count = 0; //Can use moves.size() instead
};

//Note that the pawn and king pieces cannot be promoted to,
//additionally the doubled chars are to handle the correct indexes
/*
    promotion_pieces[12] = {
        [Q] = 'q',
        [R] = 'r',
        ...,
        ...etc
    };
    This syntax is not supported in c++ thus work around used below
*/
constexpr auto NUM_PROMOTION_PIECES = 12;
constexpr char promotion_pieces[NUM_PROMOTION_PIECES] =
{
    ' ','N','B','R','Q',' ',' ','n','b','r','q',' '
};

#endif



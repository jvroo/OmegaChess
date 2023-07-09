#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include "BitBoard.h"
#include "Pieces/Pawn.h"
#include "Pieces/Bishop.h"
#include "Pieces/Knight.h"
#include "Pieces/Rook.h"
#include "Pieces/Queen.h"
#include "Pieces/King.h"
#include "Magic.h"
#include "Move.h"
#include "Timer.h"
#include <map>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <iostream>

constexpr auto NUM_PIECE_BITBOARDS = 12;
constexpr auto NUM_OCC_BITBOARDS = 3;
constexpr auto NO_CASTLES = 0;

/*
    The enum of the castling rights is given below
    wk == 1 == 0001 for white king castling King side
    wq == 2 == 0010 for white king castling Queen side
    bk == 4 == 0100 for black king castling King side
    bq == 8 == 1000 for black king castling Queen side
    This allows efficient bitwise manipulations to show
    which side can castle. Where 15 == 1111 represents
    that all castles are available. Also 9 = 1001 which
    represents White Kingside castling and Black Queenside
    castling are both available.
*/

enum
{
    wk = 1, wq = 2, bk = 4, bq = 8
};

/*
    Encode pieces from [0 , 11] where Capitals {PNBRQK} represents
    white pieces and lowercase {pnbrqk} represents black pieces.
*/
enum
{
    P = 0, N, B, R, Q, K, p, n, b, r, q, k
};

/*
    Maps char encodings to the numbers [0 , 11] by index for later reference
*/
inline constexpr char ASCII_Pieces[NUM_PIECE_BITBOARDS] = {
	'P','N','B','R','Q','K','p','n','b','r','q','k'
};

/*
    Provides an char index mapping to the encoded piece values [0 , 11]
*/
inline const std::map<char, int> char_pieces = {
        {'P', P},
        {'N', N},
        {'B', B},
        {'R', R},
        {'Q', Q},
        {'K', K},
        {'p', p},
        {'n', n},
        {'b', b},
        {'r', r},
        {'q', q},
        {'k', k}
};

enum { all_moves = false, captures_only = true};

//A copy of relevant board state information to use in copy/make move functions of BoardState
class BoardstateCopy
{
public:
    //----------------//
    //MEMBER VARIABLES//
    //----------------//
    //These define the board state in its entirety
    bitboard piece_bitboards[NUM_PIECE_BITBOARDS] = {bitboard{}};
    bitboard occupancy_bitboards[NUM_OCC_BITBOARDS] = {bitboard{}};
    int side_to_move_ = white;
    int en_passant_square_ = no_sq;
    int castling_rights_ = NO_CASTLES;
    unsigned int halfmove_count_ = 0u;
    unsigned int fullmove_count_ = 0u;
};

class Boardstate
{
public:
    void print_board();
    void print_moves(MoveList &move_list);
    void FEN_parse(std::string fen);
    void set_side_to_move(bool sideToMove);
    void set_en_passant_square(int square);
    void generate_moves(MoveList &move_list);
    bool make_move(Move &move, bool move_type);
    void operator=(Boardstate& oldBoardState);

    void perft_display(int depth);
    void perft_driver(int depth);
    void perft_test(int depth);

    //Copy/Make approach helper functions
    void make_copy(BoardstateCopy &copy_of_state);
    void restore_copy(BoardstateCopy &copy_of_state);

    //Getters (Can be made public if needed)
    bitboard* get_piece_bitboards();
    bitboard* get_occupancy_bitboards();
    int get_side_to_move();
    int get_en_passant_square();
    int get_castling_rights();
    unsigned int get_halfmove_count();
    unsigned int get_fullmove_count();
    int get_num_moves(MoveList &move_list);

    //Checking for attacked squares
    //Make faster by making static inline
    bool is_square_attacked(int square, int side_attacking);

private:
    //----------------//
    //HELPER FUNCTIONS//
    //----------------//
    //The FEN_check_valid is NOT my own code.
    //Repurposed from Stockfish
    bool FEN_check_valid(const std::string &fen);
    bool piece_type_is_ok(char piece_type);
    void clear_boardstate();

    //Make move helper functions
    void remove_captured_pieces(int target_square);
    void handle_pawn_promotions(int promoted_piece, int target_square);
    void handle_en_passant_captures(int target_square);
    void handle_castling(int target_square);
    void update_occupancies();

    //Checking for attacked squares
    //Make faster by making static inline
    void print_attacked_squares(int side_attacking);

    //Move generation helper functions
    void generate_white_pawn_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                                  bitboard &currentPieceType, bitboard &pawnAttacks); //Variables passed in to increase speed
    void generate_black_pawn_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                                  bitboard &currentPieceType, bitboard &pawnAttacks); //Variables passed in to increase speed
    void generate_white_king_castle(MoveList &move_list);
    void generate_black_king_castle(MoveList &move_list);
    void generate_knight_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                              bitboard &currentPieceType, bitboard &knightAttacks, int side);
    void generate_bishop_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                              bitboard &currentPieceType, bitboard &bishopAttacks, int side);
    void generate_rook_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                              bitboard &currentPieceType, bitboard &rookAttacks, int side);
    void generate_queen_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                              bitboard &currentPieceType, bitboard &queenAttacks, int side);
    void generate_king_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                              bitboard &currentPieceType, bitboard &kingAttacks, int side);

    //----------------//
    //MEMBER VARIABLES//
    //----------------//
    //These define the board state in its entirety
    bitboard piece_bitboards[NUM_PIECE_BITBOARDS] = {bitboard{}};
    bitboard occupancy_bitboards[NUM_OCC_BITBOARDS] = {bitboard{}};
    int side_to_move_ = white;
    int en_passant_square_ = no_sq;
    int castling_rights_ = NO_CASTLES;
    unsigned int halfmove_count_ = 0u;
    unsigned int fullmove_count_ = 0u;

    //Perft node count
    long long nodes_ = 0;
};

#endif



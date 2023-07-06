#include "Move.h"

const auto NO_MOVE = 0ul;
//SHIFTERS
const auto SHIFT_TARGET_SQUARE = 6ul;
const auto SHIFT_PIECE = 12ul;
const auto SHIFT_PROM_PIECE = 16ul;
const auto SHIFT_IS_CAPTURE = 20ul;
const auto SHIFT_IS_DOUBLE_PUSH = 21ul;
const auto SHIFT_IS_EN_PASSANT = 22ul;
const auto SHIFT_IS_CASTLES = 23ul;
//ENCODERS
const auto SOURCE_SQUARE = 0x3Ful;
const auto TARGET_SQUARE = 0xFC0ul;
const auto PIECE_TYPE = 0xF000ul;
const auto PROMOTION_TYPE = 0xF0000ul;
const auto CAPTURE_FLAG = 0x100000ul;
const auto DOUBLE_PUSH_FLAG = 0x200000ul;
const auto EN_PASSANT_FLAG = 0x400000ul;
const auto CASTLING_FLAG = 0x800000ul;

////////////////////////////////////////////
//               MOVE FUNCTIONS           //
////////////////////////////////////////////

Move::Move()
{
    encoded_move = NO_MOVE;
}

Move::Move(unsigned long move)
{
    encoded_move = move;
}

Move::Move(int source, int target, int piece, int promotion_piece,
         bool capture, bool doublePush, bool en_passant, bool castling)
{
    encoded_move = (source) | (target << SHIFT_TARGET_SQUARE) | (piece << SHIFT_PIECE) |
                   (promotion_piece << SHIFT_PROM_PIECE) | (capture << SHIFT_IS_CAPTURE) |
                   (doublePush << SHIFT_IS_DOUBLE_PUSH) | (en_passant << SHIFT_IS_EN_PASSANT) |
                   (castling << SHIFT_IS_CASTLES);
}

void Move::encode_move(int source, int target, int piece, int promotion_piece,
         bool capture, bool doublePush, bool en_passant, bool castling)
{
    encoded_move = (source) | (target << SHIFT_TARGET_SQUARE) | (piece << SHIFT_PIECE) |
                   (promotion_piece << SHIFT_PROM_PIECE) | (capture << SHIFT_IS_CAPTURE) |
                   (doublePush << SHIFT_IS_DOUBLE_PUSH) | (en_passant << SHIFT_IS_EN_PASSANT) |
                   (castling << SHIFT_IS_CASTLES);
}

int Move::get_move_source_square()
{
    return encoded_move & SOURCE_SQUARE;
}

int Move::get_move_target_square()
{
    return ((encoded_move & TARGET_SQUARE) >> SHIFT_TARGET_SQUARE);
}

int Move::get_move_piece()
{
    return ((encoded_move & PIECE_TYPE) >> SHIFT_PIECE);
}

int Move::get_move_promotion_type()
{
    return ((encoded_move & PROMOTION_TYPE) >> SHIFT_PROM_PIECE);
}

bool Move::get_move_capture_flag()
{
    return encoded_move & CAPTURE_FLAG;
}

bool Move::get_move_double_push_flag()
{
    return encoded_move & DOUBLE_PUSH_FLAG;
}

bool Move::get_move_en_passant_flag()
{
    return encoded_move & EN_PASSANT_FLAG;
}

bool Move::get_move_castling_flag()
{
    return encoded_move & CASTLING_FLAG;
}

#include "BoardState.h"

void Move::print_move()
{
    printf("    %s%s%c    %c        %s         %s        %s           %s    \n",
           square_to_coordinate[get_move_source_square()],
           square_to_coordinate[get_move_target_square()],
           promotion_pieces[get_move_promotion_type()],
           ASCII_Pieces[get_move_piece()],
           (get_move_capture_flag() ? "Yes" : "No "),
           (get_move_double_push_flag() ? "Yes" : "No "),
           (get_move_en_passant_flag() ? "Yes" : "No "),
           (get_move_castling_flag() ? "Yes" : "No "));
}

void Move::print_move_UCI()
{
    printf("%s%s%c", square_to_coordinate[get_move_source_square()],
                       square_to_coordinate[get_move_target_square()],
                       promotion_pieces[get_move_promotion_type()]);
}

std::string Move::get_move_UCI()
{
    std::string move_string = "";
    move_string = move_string + square_to_coordinate[get_move_source_square()];
    move_string = move_string + square_to_coordinate[get_move_target_square()];
    std::string promotion_string(1, promotion_pieces[get_move_promotion_type()]);
    move_string = move_string + promotion_string;
    move_string = move_string + "\n";
    return move_string;
}


////////////////////////////////////////////
//          MOVE LIST FUNCTIONS           //
////////////////////////////////////////////

constexpr auto NO_MOVES = 0;

void MoveList::print_move_list()
{
    if (num_move_count == NO_MOVES)
    {
        printf("    Total number of moves: %d\n", num_move_count);
        return;
    }
    printf("\n    Move:    Piece:   Capture:    Double:    Enpassant:    Castling:\n");
    for (auto iCount = 0; iCount < num_move_count; iCount++)
    {
        moves[iCount].print_move();
    }
    printf("    Total number of moves: %d\n", num_move_count);
}

void MoveList::add_move(Move move)
{
    moves.push_back(move);
    num_move_count++;
}

void MoveList::replace_move(Move move, unsigned int index)
{
    if (index >= num_move_count)
        return;
    moves[index] = move;
}

void MoveList::remove_move(unsigned int index)
{
    if (index >= num_move_count)
        return;
    moves.erase(moves.begin() + index);
    num_move_count--;
}

void MoveList::clear_moves()
{
    moves.clear();
    num_move_count = 0;
}

int MoveList::get_num_moves()
{
    return num_move_count;
}

Move MoveList::get_move(int index) const
{
    return moves[index];
}

bool Move::is_no_move()
{
    if (encoded_move == 0)
        return true;
    else
        return false;
}

bool Move::operator==(Move& rhs)
{
    if (get_move_capture_flag() != rhs.get_move_capture_flag())
        return false;
    if (get_move_castling_flag() != rhs.get_move_castling_flag())
        return false;
    if (get_move_double_push_flag() != rhs.get_move_double_push_flag())
        return false;
    if (get_move_en_passant_flag() != rhs.get_move_en_passant_flag())
        return false;
    if (get_move_piece() != rhs.get_move_piece())
        return false;
    if (get_move_promotion_type() != rhs.get_move_promotion_type())
        return false;
    if (get_move_source_square() != rhs.get_move_source_square())
        return false;
    if (get_move_target_square() != rhs.get_move_target_square())
        return false;
    return true;
}

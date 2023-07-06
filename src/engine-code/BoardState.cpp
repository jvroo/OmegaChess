#include "BoardState.h"

constexpr auto BOARD_SIZE = 8;
constexpr auto SINGLE_ROW_SHIFT = 8;
constexpr auto NUM_SQUARES = 64;
constexpr auto NO_PIECE = -1;
constexpr auto NO_PIECE_CHAR = '.';
constexpr auto WHITE_STRING = "White";
constexpr auto BLACK_STRING = "Black";
constexpr auto NO_EN_PASSANT = "N/A";
constexpr auto WK_CASTLE_STR = 'K';
constexpr auto WQ_CASTLE_STR = 'Q';
constexpr auto BK_CASTLE_STR = 'k';
constexpr auto BQ_CASTLE_STR = 'q';
constexpr auto NO_CASTLE_CHR = '-';
constexpr size_t BYTES_IN_PIECE_BITBOARD_ARR = 96;
constexpr size_t BYTES_IN_OCC_BITBOARD_ARR = 24;
constexpr auto CHANGE_COLOR = 1;
constexpr auto PERFT_EXIT = 0;
constexpr auto ONE_LESS = 1;

/*
INFO REGARDING UPDATNG CASTLING LOGIC

                           castling   move     in      in
                              right update     binary  decimal
 king & rooks didn't move:     1111 & 1111  =  1111    15
        white king  moved:     1111 & 1100  =  1100    12
  white king's rook moved:     1111 & 1110  =  1110    14
 white queen's rook moved:     1111 & 1101  =  1101    13

         black king moved:     1111 & 0011  =  1011    3
  black king's rook moved:     1111 & 1011  =  1011    11
 black queen's rook moved:     1111 & 0111  =  0111    7
*/

// castling rights update constants
constexpr int castling_rights[NUM_SQUARES] = {
     7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};

void Boardstate::perft_driver(int depth)
{
    if (depth == PERFT_EXIT)
    {
        nodes_++;
        return;
    }
    MoveList move_list;
    generate_moves(move_list);
    for (auto iCount = 0; iCount < move_list.get_num_moves(); iCount++)
    {
        Move move = move_list.get_move(iCount);
        //Copy boardstate
        BoardstateCopy copy_of_state;
        make_copy(copy_of_state);
        //Make legal moves
        if (!make_move(move, all_moves))
            continue;
        //Call perft_driver recursively
        //This traverses all the nodes of given depth
        perft_driver(depth - ONE_LESS);
        //restore previous state
        restore_copy(copy_of_state);
    }
}

void Boardstate::perft_display(int depth)
{
    Timer::reset();
    Timer::start();
    perft_driver(depth);
    Timer::stop();
    printf("\nNumber of nodes: %llu\n", nodes_);
    printf("\nTime taken: %d\n", Timer::readTime());
    nodes_ = 0;
}

void Boardstate::perft_test(int depth)
{
    printf("    Performance Test: \n\n");
    MoveList move_list;
    generate_moves(move_list);
    Timer::reset();
    Timer::start();
    for (auto iCount = 0; iCount < move_list.get_num_moves(); iCount++)
    {
        Move move = move_list.get_move(iCount);
        //Copy boardstate
        BoardstateCopy copy_of_state;
        make_copy(copy_of_state);
        //Make legal moves
        if (!make_move(move, all_moves))
            continue;
        unsigned long long cumulative_nodes = nodes_;
        //Call perft_driver recursively
        //This traverses all the nodes of given depth
        perft_driver(depth - ONE_LESS);
        unsigned long long old_nodes = nodes_ - cumulative_nodes;
        //restore previous state
        restore_copy(copy_of_state);
        printf("    Move: %s%s%c    Nodes: %llu\n",
                square_to_coordinate[move.get_move_source_square()],
                square_to_coordinate[move.get_move_target_square()],
                promotion_pieces[move.get_move_promotion_type()],
                old_nodes);
    }
    Timer::stop();
    printf("\n    Depth: %d\n", depth);
    printf("    Nodes: %llu\n", nodes_);
    printf("    Time:  %ld\n", Timer::readTime());
    nodes_ = 0;
}

void Boardstate::print_board()
{
    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto square = rank*BOARD_SIZE+file;
            if (!file)
                printf("  %d  ", BOARD_SIZE - rank);
            int piece = NO_PIECE;
            for (int iCount = P; iCount <= k; iCount++)
            {
                if (Bitboard::get_bit(piece_bitboards[iCount], square))
                    piece = iCount;
            }
            printf("%c ", (piece == NO_PIECE) ? NO_PIECE_CHAR : ASCII_Pieces[piece]);
        }
        printf("\n");
    }
    printf("\n     a b c d e f g h\n\n");
    printf("     To move: %s \n\n", (!side_to_move_) ? WHITE_STRING : BLACK_STRING);
    printf("     En passant: %s \n\n", (en_passant_square_ != no_sq) ?
                           square_to_coordinate[en_passant_square_] : NO_EN_PASSANT);
    printf("     Castling: %c%c%c%c \n\n", (castling_rights_ & wk) ? WK_CASTLE_STR : NO_CASTLE_CHR,
                                           (castling_rights_ & wq) ? WQ_CASTLE_STR : NO_CASTLE_CHR,
                                           (castling_rights_ & bk) ? BK_CASTLE_STR : NO_CASTLE_CHR,
                                           (castling_rights_ & bq) ? BQ_CASTLE_STR : NO_CASTLE_CHR);
    printf("     Half moves:  %i \n\n", halfmove_count_);
    printf("     Full moves:  %i \n\n", fullmove_count_);
}

void Boardstate::print_moves(MoveList &move_list)
{
    move_list.print_move_list();
}

void Boardstate::set_side_to_move(bool sideToMove)
{
    side_to_move_ = sideToMove;
}

bool Boardstate::is_square_attacked(int square, int side_attacking)
{
    // --- WHITE PAWNS --- //
    if ((side_attacking == white) && (PawnAttacks::pawn_attacks[black][square] & piece_bitboards[P]))
        return true;

    // --- BLACK PAWNS --- //
    if ((side_attacking == black) && (PawnAttacks::pawn_attacks[white][square] & piece_bitboards[p]))
        return true;

    // --- KNIGHTS --- //
    if ((KnightAttacks::knight_attacks[square]) & ((side_attacking == white) ? piece_bitboards[N] : piece_bitboards[n]))
        return true;

    // --- BISHOPS --- //
    if (BishopAttacks::get_bishop_attacks(square, occupancy_bitboards[both])
        & ((side_attacking == white) ? piece_bitboards[B] : piece_bitboards[b]))
        return true;

    // --- ROOKS --- //
    if (RookAttacks::get_rook_attacks(square, occupancy_bitboards[both])
        & ((side_attacking == white) ? piece_bitboards[R] : piece_bitboards[r]))
        return true;

    // --- QUEEN --- //
    if (QueenAttacks::get_queen_attacks(square, occupancy_bitboards[both])
        & ((side_attacking == white) ? piece_bitboards[Q] : piece_bitboards[q]))
        return true;

    // --- KINGS --- //
    if ((KingAttacks::king_attacks[square]) & ((side_attacking == white) ? piece_bitboards[K] : piece_bitboards[k]))
        return true;


    //By default return False
    return false;
}

void Boardstate::print_attacked_squares(int side_attacking)
{
    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto square = rank*BOARD_SIZE+file;
            if (!file)
                printf("  %d  ", BOARD_SIZE - rank);
            printf("%d ", is_square_attacked(square, side_attacking) ? true : false);
        }
        printf("\n");
    }
    printf("\n     a b c d e f g h\n\n\n");
}

void Boardstate::generate_moves(MoveList &move_list)
{
    //move_list.clear_moves();
    //Init helper variables
    int source_square = no_sq;
    int target_square = no_sq;
    auto attacks = bitboard{};
    auto current_piece_type = bitboard{};
    //Loop over all bitboards in piece_bitboards
    for (int piece_type = P; piece_type <= k; piece_type++)
    {
        current_piece_type = piece_bitboards[piece_type];
        if (piece_type == P)
        {
            generate_white_pawn_move(source_square,target_square,move_list,current_piece_type, attacks);
        }
        if (piece_type == p)
        {
            generate_black_pawn_move(source_square,target_square,move_list,current_piece_type, attacks);
        }
        if (piece_type == K)
        {
            generate_king_move(source_square, target_square, move_list,current_piece_type, attacks, white);
            generate_white_king_castle(move_list);
        }
        if (piece_type == k)
        {
            generate_king_move(source_square, target_square,move_list, current_piece_type, attacks, black);
            generate_black_king_castle(move_list);
        }
        if ((piece_type == N) || (piece_type == n))
        {
            generate_knight_move(source_square, target_square, move_list, current_piece_type, attacks,
                                 ((piece_type == N) ? white : black));
        }
        if ((piece_type == B) || (piece_type == b))
        {
            generate_bishop_move(source_square, target_square, move_list, current_piece_type, attacks,
                                 ((piece_type == B) ? white : black));
        }
        if ((piece_type == R) || (piece_type == r))
        {
            generate_rook_move(source_square, target_square, move_list, current_piece_type, attacks,
                                 ((piece_type == R) ? white : black));
        }
        if ((piece_type == Q) || (piece_type == q))
        {
            generate_queen_move(source_square, target_square, move_list, current_piece_type, attacks,
                                 ((piece_type == Q) ? white : black));
        }
    }
}

constexpr auto DONT_MAKE_MOVE = false;
constexpr auto MAKE_MOVE = true;

bool Boardstate::make_move(Move &move, bool move_type)
{
    if (move.is_no_move())
        return DONT_MAKE_MOVE;
    //Quiet moves
    if (move_type == all_moves)
    {
        //Preserve board state
        BoardstateCopy copy_of_state;
        make_copy(copy_of_state);

        //Parse move info
        auto source_square = move.get_move_source_square();
        auto target_square = move.get_move_target_square();
        auto piece = move.get_move_piece();
        auto promotion_type = move.get_move_promotion_type();
        auto capture = move.get_move_capture_flag();
        auto double_push = move.get_move_double_push_flag();
        auto enpassant = move.get_move_en_passant_flag();
        auto castling = move.get_move_castling_flag();

        //Make quiet moves
        Bitboard::pop_bit(piece_bitboards[piece], source_square);
        Bitboard::set_bit(piece_bitboards[piece], target_square);

        //Increment halfmove count
        halfmove_count_++;
        //Handle reset halfmove if pawn move
        if ((piece == P) || (piece == p))
            halfmove_count_ = 0;
        //Handle captues and reset halfmove
        if (capture)
        {
            remove_captured_pieces(target_square);
            halfmove_count_ = 0;
        }
        //Handle promotions
        handle_pawn_promotions(promotion_type, target_square);
        //Handle enpassant
        if (enpassant)
        {
            handle_en_passant_captures(target_square);
        }
        //Reset enpassant square
        en_passant_square_ = no_sq;
        //Handle double pawn push
        if (double_push)
        {
            //Set enpassant square
            (side_to_move_ == white) ? (en_passant_square_ = target_square + SINGLE_ROW_SHIFT) :
                                       (en_passant_square_ = target_square - SINGLE_ROW_SHIFT);
        }
        //Handle castling
        if (castling)
        {
            handle_castling(target_square);
        }
        //Update castling rights after every move using predefined castling_rights[64] array
        castling_rights_ &= castling_rights[source_square];
        castling_rights_ &= castling_rights[target_square];
        //Update occupancy bitboards
        update_occupancies();
        //Update side to move
        side_to_move_ ^= CHANGE_COLOR;
        //Ensure king is not in check
        auto king_square = (side_to_move_ == white) ? Bitboard::get_lsb_index(piece_bitboards[k]) :
                                                      Bitboard::get_lsb_index(piece_bitboards[K]);
        //Make move if king not in check
        if (is_square_attacked(king_square, side_to_move_))
        {
            restore_copy(copy_of_state);
            return DONT_MAKE_MOVE;
        }
        else
        {
            //If it was just black to move then increment fullmove counter
            if (side_to_move_ == white) fullmove_count_++;
            return MAKE_MOVE;
        }
    }
    //Captures
    else if (move_type == captures_only)
    {
        if (move.get_move_capture_flag())
            return make_move(move, all_moves);
        //Return move is not capture
        else
            //Dont make move
            return DONT_MAKE_MOVE;
    }
    return DONT_MAKE_MOVE;
}

void Boardstate::remove_captured_pieces(int target_square)
{
    int start_piece, end_piece;
    if (side_to_move_ == white)
    {
        start_piece = p;
        end_piece = k;
    }
    else
    {
        start_piece = P;
        end_piece = K;
    }
    for (auto bb_piece = start_piece; bb_piece <= end_piece; bb_piece++)
    {
    if (Bitboard::get_bit(piece_bitboards[bb_piece], target_square))
        {
            Bitboard::pop_bit(piece_bitboards[bb_piece], target_square);
            return;
        }
    }
}

void Boardstate::handle_pawn_promotions(int promoted_piece, int target_square)
{
    if (promoted_piece)
    {
        Bitboard::pop_bit(piece_bitboards[(side_to_move_ == white) ? P : p], target_square);
        Bitboard::set_bit(piece_bitboards[promoted_piece], target_square);
    }
}

void Boardstate::handle_en_passant_captures(int target_square)
{
    if (side_to_move_ == white)
        Bitboard::pop_bit(piece_bitboards[p], target_square + SINGLE_ROW_SHIFT);
    else
        Bitboard::pop_bit(piece_bitboards[P], target_square - SINGLE_ROW_SHIFT);
}

void Boardstate::handle_castling(int target_square)
{
    switch (target_square)
    {
        //White Kingside
        case (g1):
            Bitboard::pop_bit(piece_bitboards[R], h1);
            Bitboard::set_bit(piece_bitboards[R], f1);
            break;
        //White Queenside
        case (c1):
            Bitboard::pop_bit(piece_bitboards[R], a1);
            Bitboard::set_bit(piece_bitboards[R], d1);
            break;
        //Black Kingside
        case (g8):
            Bitboard::pop_bit(piece_bitboards[r], h8);
            Bitboard::set_bit(piece_bitboards[r], f8);
            break;
        //Black Queenside
        case (c8):
            Bitboard::pop_bit(piece_bitboards[r], a8);
            Bitboard::set_bit(piece_bitboards[r], d8);
            break;
        default:
            return;
    }
}

constexpr auto SHIFT_PIECE_INDEX_COLOR = 6;

void Boardstate::update_occupancies()
{
    memset(occupancy_bitboards, bitboard{}, BYTES_IN_OCC_BITBOARD_ARR);
    for (int bb_piece = P; bb_piece <= K; bb_piece++)
    {
        occupancy_bitboards[white] |= piece_bitboards[bb_piece];
        occupancy_bitboards[black] |= piece_bitboards[bb_piece + SHIFT_PIECE_INDEX_COLOR];
    }
    occupancy_bitboards[both] = occupancy_bitboards[white] | occupancy_bitboards[black];
}

void Boardstate::operator=(Boardstate& oldBoardState)
{
    //Use different method to sizeof to increase speed. Sizeof is the number of bytes
    memcpy(piece_bitboards, oldBoardState.get_piece_bitboards(), BYTES_IN_PIECE_BITBOARD_ARR);
    memcpy(occupancy_bitboards, oldBoardState.get_occupancy_bitboards(), BYTES_IN_OCC_BITBOARD_ARR);
    side_to_move_ = oldBoardState.get_side_to_move();
    en_passant_square_ = oldBoardState.get_en_passant_square();
    castling_rights_ = oldBoardState.get_castling_rights();
    halfmove_count_ = oldBoardState.get_halfmove_count();
    fullmove_count_ = oldBoardState.get_fullmove_count();
}

void Boardstate::make_copy(BoardstateCopy &copy_of_state)
{
    memcpy(copy_of_state.piece_bitboards, piece_bitboards, BYTES_IN_PIECE_BITBOARD_ARR);
    memcpy(copy_of_state.occupancy_bitboards, occupancy_bitboards, BYTES_IN_OCC_BITBOARD_ARR);
    copy_of_state.side_to_move_ = side_to_move_;
    copy_of_state.en_passant_square_ = en_passant_square_;
    copy_of_state.castling_rights_ = castling_rights_;
    copy_of_state.halfmove_count_ = halfmove_count_;
    copy_of_state.fullmove_count_ = fullmove_count_;
}

void Boardstate::restore_copy(BoardstateCopy &copy_of_state)
{
    memcpy(piece_bitboards, copy_of_state.piece_bitboards, BYTES_IN_PIECE_BITBOARD_ARR);
    memcpy(occupancy_bitboards, copy_of_state.occupancy_bitboards, BYTES_IN_OCC_BITBOARD_ARR);
    side_to_move_ = copy_of_state.side_to_move_;
    en_passant_square_ = copy_of_state.en_passant_square_;
    castling_rights_ = copy_of_state.castling_rights_;
    halfmove_count_ = copy_of_state.halfmove_count_;
    fullmove_count_ = copy_of_state.fullmove_count_;
}

//Getters
bitboard* Boardstate::get_piece_bitboards()
{
    return piece_bitboards;
}

bitboard* Boardstate::get_occupancy_bitboards()
{
    return occupancy_bitboards;
}

int Boardstate::get_side_to_move()
{
    return side_to_move_;
}

int Boardstate::get_en_passant_square()
{
    return en_passant_square_;
}

int Boardstate::get_castling_rights()
{
    return castling_rights_;
}

unsigned int Boardstate::get_halfmove_count()
{
    return halfmove_count_;
}

unsigned int Boardstate::get_fullmove_count()
{
    return fullmove_count_;
}

int Boardstate::get_num_moves(MoveList &move_list)
{
    move_list.get_num_moves();
}

const auto ONE_ROW_SHIFT = 8;
const auto ONE_SHIFT = 1ull;

void Boardstate::generate_white_pawn_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                                          bitboard &currentPieceType, bitboard &pawnAttacks)
{
    //For white pawns when white to move
    if (side_to_move_ == white)
    {
        //Check the current white pawn bitboard in piece_bitboards[P] and get all pawn indices
        while (currentPieceType)
        {
            //Init Source square and target square
            sourceSquare = Bitboard::get_lsb_index(currentPieceType);
            targetSquare = sourceSquare - ONE_ROW_SHIFT;
            //Check move is still on board and no piece in front of pawn
            if (!(targetSquare < a8) && (!Bitboard::get_bit(occupancy_bitboards[both],targetSquare)))
            {
                //Promotion logic if true
                if ((sourceSquare >= a7) && (sourceSquare <= h7))
                {
                    //Define white pawn queening without captures
                    move_list.add_move(Move{sourceSquare, targetSquare, P, Q, false, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, P, R, false, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, P, B, false, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, P, N, false, false, false, false});
                }
                //Single and double pawn pushes if before 7th rank
                else
                {
                    //Single push white pawn
                    move_list.add_move(Move{sourceSquare, targetSquare, P, false, false, false, false, false});

                    //Double push white pawn
                    if ((sourceSquare >= a2 && sourceSquare <= h2) &&
                        (!Bitboard::get_bit(occupancy_bitboards[both],targetSquare - ONE_ROW_SHIFT)))
                    {
                        move_list.add_move(Move{sourceSquare, targetSquare - ONE_ROW_SHIFT, P, false, false, true, false, false});
                    }
                }
            }
            pawnAttacks = PawnAttacks::pawn_attacks[side_to_move_][sourceSquare] & occupancy_bitboards[black];
            //Generate white pawn captures
            while (pawnAttacks)
            {
                targetSquare = Bitboard::get_lsb_index(pawnAttacks);
                //Promotion logic if true
                if ((sourceSquare >= a7) && (sourceSquare <= h7))
                {
                    //Define white pawn queening with captures
                    move_list.add_move(Move{sourceSquare, targetSquare, P, Q, true, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, P, R, true, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, P, B, true, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, P, N, true, false, false, false});
                }
                //Whie Pawn Normal captures
                else
                {
                    move_list.add_move(Move{sourceSquare, targetSquare, P, false, true, false, false, false});
                }
                Bitboard::pop_bit(pawnAttacks, targetSquare);
            }
            //Handle en passant captures
            if (en_passant_square_ != no_sq)
            {
                bitboard enPassantAttacks = PawnAttacks::pawn_attacks[side_to_move_][sourceSquare] &
                                        (ONE_SHIFT << en_passant_square_);
                if (enPassantAttacks)
                {
                    int target_enpassant = Bitboard::get_lsb_index(enPassantAttacks);
                    //Must be black pawn on rank below En Passant square
                    if ((ONE_SHIFT << (en_passant_square_ + ONE_ROW_SHIFT)) & (piece_bitboards[p]))
                    {
                        move_list.add_move(Move{sourceSquare, target_enpassant, P, false, true, false, true, false});
                    }
                }
            }
            //Move to next white pawn
            Bitboard::pop_bit(currentPieceType, sourceSquare);
        }
    }
}

void Boardstate::generate_black_pawn_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                                          bitboard &currentPieceType, bitboard &pawnAttacks)
{
    //For black pawns when black to move
    if (side_to_move_ == black)
    {
        //Check the current black pawn bitboard in piece_bitboards[p] and get all pawn indices
        while (currentPieceType)
        {
            //Init Source square and target square
            sourceSquare = Bitboard::get_lsb_index(currentPieceType);
            targetSquare = sourceSquare + ONE_ROW_SHIFT;
            //Check move is still on board and no piece in front of pawn
            if (!(targetSquare > h1) && (!Bitboard::get_bit(occupancy_bitboards[both],targetSquare)))
            {
                //Promotion logic if true
                if ((sourceSquare >= a2) && (sourceSquare <= h2))
                {
                    //Define black pawn queening without captures
                    move_list.add_move(Move{sourceSquare, targetSquare, p, q, false, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, p, r, false, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, p, b, false, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, p, n, false, false, false, false});
                }
                //Single and double pawn pushes if before 7th rank
                else
                {
                    //Single push black pawn
                    move_list.add_move(Move{sourceSquare, targetSquare, p, false, false, false, false, false});

                    //Double push black pawn
                    if ((sourceSquare >= a7 && sourceSquare <= h7) &&
                        (!Bitboard::get_bit(occupancy_bitboards[both],targetSquare + ONE_ROW_SHIFT)))
                    {
                        move_list.add_move(Move{sourceSquare, targetSquare + ONE_ROW_SHIFT, p, false, false, true, false, false});
                    }
                }
            }
            pawnAttacks = PawnAttacks::pawn_attacks[side_to_move_][sourceSquare] & occupancy_bitboards[white];
            //Generate black pawn captures
            while (pawnAttacks)
            {
                targetSquare = Bitboard::get_lsb_index(pawnAttacks);
                //Promotion logic if true
                if ((sourceSquare >= a2) && (sourceSquare <= h2))
                {
                    //Define black pawn queening with captures
                    move_list.add_move(Move{sourceSquare, targetSquare, p, q, true, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, p, r, true, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, p, b, true, false, false, false});
                    move_list.add_move(Move{sourceSquare, targetSquare, p, n, true, false, false, false});
                }
                //Black Pawn Normal captures
                else
                {
                    move_list.add_move(Move{sourceSquare, targetSquare, p, false, true, false, false, false});
                }
                Bitboard::pop_bit(pawnAttacks, targetSquare);
            }
            //Handle en passant captures
            if (en_passant_square_ != no_sq)
            {
                bitboard enPassantAttacks = PawnAttacks::pawn_attacks[side_to_move_][sourceSquare] &
                                        (ONE_SHIFT << en_passant_square_);
                if (enPassantAttacks)
                {
                    int target_enpassant = Bitboard::get_lsb_index(enPassantAttacks);
                    //Must be white pawn on rank above En Passant square
                    if ((ONE_SHIFT << (en_passant_square_ - ONE_ROW_SHIFT)) & (piece_bitboards[P]))
                    {
                        move_list.add_move(Move{sourceSquare, target_enpassant, p, false, true, false, true, false});
                    }
                }
            }
            //Move to next black pawn
            Bitboard::pop_bit(currentPieceType, sourceSquare);
        }
    }
}

void Boardstate::generate_white_king_castle(MoveList &move_list)
{
    //Return if king not on e1
    if (!(piece_bitboards[K] & (ONE_SHIFT << e1)))
    {
        return;
    }
    if (side_to_move_ == white)
    {
        //King side castles
        if (wk & castling_rights_)
        {
            //Ensure f1 and g1 are clear, and h1 contains rook
            if (!Bitboard::get_bit(occupancy_bitboards[both],f1) && !Bitboard::get_bit(occupancy_bitboards[both],g1)
                && (piece_bitboards[R] & (ONE_SHIFT << h1)))
            {
                //Ensure e1 is not attacked and f1 square is not attacked
                //Thus king does not pass through check. g1 is checked in make_move() function
                if ((!is_square_attacked(e1, black)) && (!is_square_attacked(f1, black)))
                {
                    //White kingside castle
                    move_list.add_move(Move{e1, g1, K, false, false, false, false, true});
                }
            }
        }
        //Queen side castles
        if (wq & castling_rights_)
        {
            //Ensure d1, c1 and b1 are clear, and a1 contains rook
            if (!Bitboard::get_bit(occupancy_bitboards[both],d1) && !Bitboard::get_bit(occupancy_bitboards[both],c1)
                && (!Bitboard::get_bit(occupancy_bitboards[both],b1)) && (piece_bitboards[R] & (ONE_SHIFT << a1)))
            {
                //Ensure e1 is not attacked and f1 square is not attacked
                //Thus king does not pass through check. g1 is checked in make_move() function
                if ((!is_square_attacked(e1, black)) && (!is_square_attacked(d1, black)))
                {
                    //White queenside castle
                    move_list.add_move(Move{e1, c1, K, false, false, false, false, true});
                }
            }
        }
    }
}

void Boardstate::generate_black_king_castle(MoveList &move_list)
{
    //Return if king not on e8
    if (!(piece_bitboards[k] & (ONE_SHIFT << e8)))
    {
        return;
    }
    if (side_to_move_ == black)
    {
        //King side castles
        if (bk & castling_rights_)
        {
            //Ensure f1 and g1 are clear, and h1 contains rook
            if (!Bitboard::get_bit(occupancy_bitboards[both],f8) && !Bitboard::get_bit(occupancy_bitboards[both],g8)
                && (piece_bitboards[r] & (ONE_SHIFT << h8)))
            {
                //Ensure e1 is not attacked and f1 square is not attacked
                //Thus king does not pass through check. g1 is checked in make_move() function
                if ((!is_square_attacked(e8, white)) && (!is_square_attacked(f8, white)))
                {
                    //Black kingside castle
                    move_list.add_move(Move{e8, g8, k, false, false, false, false, true});
                }
            }
        }
        //Queen side castles
        if (bq & castling_rights_)
        {
            //Ensure d1, c1 and b1 are clear, and a1 contains rook
            if (!Bitboard::get_bit(occupancy_bitboards[both],d8) && !Bitboard::get_bit(occupancy_bitboards[both],c8)
                && (!Bitboard::get_bit(occupancy_bitboards[both],b8)) && (piece_bitboards[r] & (ONE_SHIFT << a8)))
            {
                //Ensure e1 is not attacked and f1 square is not attacked
                //Thus king does not pass through check. g1 is checked in make_move() function
                if ((!is_square_attacked(e8, white)) && (!is_square_attacked(d8, white)))
                {
                    //Black queenside castle
                    move_list.add_move(Move{e8, c8, k, false, false, false, false, true});
                }
            }
        }
    }
}

void Boardstate::generate_king_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                              bitboard &currentPieceType, bitboard &kingAttacks, int side)
{
    if (side != side_to_move_)
    {
        return;
    }
    while (currentPieceType)
    {
        //Initialize source square
        sourceSquare = Bitboard::get_lsb_index(currentPieceType);
        //Init King attacks by inverting same color occupancies and checking valid moves
        kingAttacks = KingAttacks::king_attacks[sourceSquare] &
                        (~occupancy_bitboards[side_to_move_]);
        while (kingAttacks)
        {
            //Init target square
            targetSquare = Bitboard::get_lsb_index(kingAttacks);

            //Quiet moves
            if (!Bitboard::get_bit(occupancy_bitboards[!side_to_move_], targetSquare))
            {
                //King move without capture
                move_list.add_move(Move{sourceSquare, targetSquare, (side_to_move_ == white) ? K : k,
                                   false, false, false, false, false});
            }
            else
            //Captures
            {
                //King move with capture
                move_list.add_move(Move{sourceSquare, targetSquare, (side_to_move_ == white) ? K : k,
                                   false, true, false, false, false});
            }
            //Move to next available move
            Bitboard::pop_bit(kingAttacks, targetSquare);
        }
        //Move to next King
        Bitboard::pop_bit(currentPieceType, sourceSquare);
    }
}

void Boardstate::generate_knight_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                                      bitboard &currentPieceType, bitboard &knightAttacks, int side)
{
    if (side != side_to_move_)
    {
        return;
    }
    while (currentPieceType)
    {
        //Initialize source square
        sourceSquare = Bitboard::get_lsb_index(currentPieceType);
        //Init knight attacks by inverting same color occupancies and checking valid moves
        knightAttacks = KnightAttacks::knight_attacks[sourceSquare] &
                        (~occupancy_bitboards[side_to_move_]);
        while (knightAttacks)
        {
            //Init target square
            targetSquare = Bitboard::get_lsb_index(knightAttacks);

            //Quiet moves
            if (!Bitboard::get_bit(occupancy_bitboards[!side_to_move_], targetSquare))
            {
                //Knight move without capture
                move_list.add_move(Move{sourceSquare, targetSquare, (side_to_move_ == white) ? N : n,
                                   false, false, false, false, false});
            }
            else
            //Captures
            {
                //Knight move with capture
                move_list.add_move(Move{sourceSquare, targetSquare, (side_to_move_ == white) ? N : n,
                                   false, true, false, false, false});
            }
            //Move to next available move
            Bitboard::pop_bit(knightAttacks, targetSquare);
        }
        //Move to next Knight
        Bitboard::pop_bit(currentPieceType, sourceSquare);
    }
}

void Boardstate::generate_bishop_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                              bitboard &currentPieceType, bitboard &bishopAttacks, int side)
{
    if (side != side_to_move_)
    {
        return;
    }
    while (currentPieceType)
    {
        //Initialize source square
        sourceSquare = Bitboard::get_lsb_index(currentPieceType);
        //Init bishop attacks by inverting same color occupancies and checking valid moves
        bishopAttacks = BishopAttacks::get_bishop_attacks(sourceSquare, occupancy_bitboards[both]) &
                        (~occupancy_bitboards[side_to_move_]);
        while (bishopAttacks)
        {
            //Init target square
            targetSquare = Bitboard::get_lsb_index(bishopAttacks);

            //Quiet moves
            if (!Bitboard::get_bit(occupancy_bitboards[!side_to_move_], targetSquare))
            {
                //Bishop move without capture
                move_list.add_move(Move{sourceSquare, targetSquare, (side_to_move_ == white) ? B : b,
                                   false, false, false, false, false});
            }
            else
            //Captures
            {
                //Bishop move with capture
                move_list.add_move(Move{sourceSquare, targetSquare, (side_to_move_ == white) ? B : b,
                                   false, true, false, false, false});
            }
            //Move to next available move
            Bitboard::pop_bit(bishopAttacks, targetSquare);
        }
        //Move to next Bishop
        Bitboard::pop_bit(currentPieceType, sourceSquare);
    }
}

void Boardstate::generate_rook_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                              bitboard &currentPieceType, bitboard &rookAttacks, int side)
{
    if (side != side_to_move_)
    {
        return;
    }
    while (currentPieceType)
    {
        //Initialize source square
        sourceSquare = Bitboard::get_lsb_index(currentPieceType);
        //Init rook attacks by inverting same color occupancies and checking valid moves
        rookAttacks = RookAttacks::get_rook_attacks(sourceSquare, occupancy_bitboards[both]) &
                        (~occupancy_bitboards[side_to_move_]);
        while (rookAttacks)
        {
            //Init target square
            targetSquare = Bitboard::get_lsb_index(rookAttacks);

            //Quiet moves
            if (!Bitboard::get_bit(occupancy_bitboards[!side_to_move_], targetSquare))
            {
                //Rook move without capture
                move_list.add_move(Move{sourceSquare, targetSquare, (side_to_move_ == white) ? R : r,
                                   false, false, false, false, false});
            }
            else
            //Captures
            {
                //Rook move with capture
                move_list.add_move(Move{sourceSquare, targetSquare, (side_to_move_ == white) ? R : r,
                                   false, true, false, false, false});
            }
            //Move to next available move
            Bitboard::pop_bit(rookAttacks, targetSquare);
        }
        //Move to next Rook
        Bitboard::pop_bit(currentPieceType, sourceSquare);
    }
}

void Boardstate::generate_queen_move(int &sourceSquare, int &targetSquare, MoveList &move_list,
                              bitboard &currentPieceType, bitboard &queenAttacks, int side)
{
    if (side != side_to_move_)
    {
        return;
    }
    while (currentPieceType)
    {
        //Initialize source square
        sourceSquare = Bitboard::get_lsb_index(currentPieceType);
        //Init queen attacks by inverting same color occupancies and checking valid moves
        queenAttacks = QueenAttacks::get_queen_attacks(sourceSquare, occupancy_bitboards[both]) &
                        (~occupancy_bitboards[side_to_move_]);
        while (queenAttacks)
        {
            //Init target square
            targetSquare = Bitboard::get_lsb_index(queenAttacks);

            //Quiet moves
            if (!Bitboard::get_bit(occupancy_bitboards[!side_to_move_], targetSquare))
            {
                //Queen move without capture
                move_list.add_move(Move{sourceSquare, targetSquare, (side_to_move_ == white) ? Q : q,
                                   false, false, false, false, false});
            }
            else
            //Captures
            {
                //Queen move with capture
                move_list.add_move(Move{sourceSquare, targetSquare, (side_to_move_ == white) ? Q : q,
                                   false, true, false, false, false});
            }
            //Move to next available move
            Bitboard::pop_bit(queenAttacks, targetSquare);
        }
        //Move to next Queen
        Bitboard::pop_bit(currentPieceType, sourceSquare);
    }
}

void Boardstate::FEN_parse(std::string fen)
{
    //Check validity of FEN string
    std::string str(fen);
    auto check = FEN_check_valid(str);
    //Initialize board to zero state
    clear_boardstate();
    //Error if FEN is invalid
    if (!check)
    {
        printf("Invalid FEN, did not parse!\n\n");
        return;
    }
    //Parse FEN into pieces
    std::string fenParse(fen);
    std::istringstream iss(fenParse);
    std::string board, side, castleRights, ep, fullmove, halfmove;
    iss >> board;
    iss >> side;
    if (!iss) {
       castleRights = "-";
       ep = "-";
    } else {
       iss >> castleRights;
       if (iss)
          iss >> ep;
       else
          ep = "-";
    }
    iss >> halfmove;
    iss >> fullmove;
    //Handle side to move
    if (side == "w")
        side_to_move_ = white;
    else if (side == "b")
        side_to_move_ = black;
    //Handle Castle rights
    for (auto iCount = 0u; iCount < castleRights.size(); iCount++)
    {
        switch (castleRights[iCount])
        {
            case (WK_CASTLE_STR): castling_rights_ |= wk; break;
            case (WQ_CASTLE_STR): castling_rights_ |= wq; break;
            case (BK_CASTLE_STR): castling_rights_ |= bk; break;
            case (BQ_CASTLE_STR): castling_rights_ |= bq; break;
            default: castling_rights_ = 0; break;
        }
    }
    //Handle en passant
    if (ep.size() > 1)
    {
        int ep_file = ep[0] - 'a';
        int ep_rank = BOARD_SIZE - (ep[1] - '0');
        en_passant_square_ = ep_rank * BOARD_SIZE + ep_file;
    }
    else
        en_passant_square_ = no_sq;
    //Handle halfmove and fullmove clocks
    fullmove_count_ = std::stoi(fullmove);
    halfmove_count_ = std::stoi(halfmove);
    //Read pieces onto board
    auto square = 0;
    auto rank_counter = 0;
    auto failed = false;
    for (unsigned int iCount = 0; iCount < board.length(); iCount++)
    {
        if ((piece_type_is_ok(board[iCount])) && (board[iCount] != '/'))
        {
            auto piece_type = char_pieces.at(board[iCount]);
            Bitboard::set_bit(piece_bitboards[piece_type], square);
            rank_counter++;
            square++;
        }
        else
        {
            if (board[iCount] != '/')
                failed = true;
        }

        if (board[iCount] >= '1' && board[iCount] <= '8')
        {
            rank_counter += static_cast<int>(board[iCount]) - static_cast<int>('0');
            square += static_cast<int>(board[iCount]) - static_cast<int>('0');
            failed = false;
        }
        if (board[iCount] == '/')
        {
            if (rank_counter != BOARD_SIZE)
            {
                failed = true;
            }
            rank_counter = 0;
        }
        if ((square > NUM_SQUARES) || (rank_counter > BOARD_SIZE))
            failed = true;
        if (failed)
        {
            clear_boardstate();
            printf("Invalid FEN, did not parse!\n\n");
            break;
        }
    }
    //Populate occupancies
    for (int iCount = P; iCount <= K; iCount++)
    {
        occupancy_bitboards[white] |= piece_bitboards[iCount];
    }
    for (int iCount = p; iCount <= k; iCount++)
    {
        occupancy_bitboards[black] |= piece_bitboards[iCount];
    }
    occupancy_bitboards[both] = occupancy_bitboards[white] | occupancy_bitboards[black];
}

/*
///////////////////////////////////////////////////////////////
FEN_check_valid() function is taken from Stockfish with modification
THIS IS NOT MY OWN CODE AND I DO NOT USE IT AS SUCH.
I use it under the fair use and licenses provided by Stockfish
///////////////////////////////////////////////////////////////
*/
bool Boardstate::FEN_check_valid(const std::string &fen)
{
   std::istringstream iss(fen);
   std::string board, side, castleRights, ep, fullmove = "", halfmove = "";
   if (!iss) return false;
   iss >> board;
   if (!iss) return false;
   iss >> side;
   if (!iss) {
      castleRights = "-";
      ep = "-";
   } else {
      iss >> castleRights;
      if (iss)
         iss >> ep;
      else
         ep = "-";
   }
   if (!iss) return false;
   iss >> halfmove;
   if (halfmove.size() == 0) return false;
   printf("\n");
   if (!iss) return false;
   iss >> fullmove;
   if (fullmove.size() == 0) return false;
   // Let's check that all components of the supposed FEN are OK.
   if (side != "w" && side != "b") return false;
   if (castleRights != "-" && castleRights != "K" && castleRights != "Kk"
       && castleRights != "Kkq" && castleRights != "Kq" && castleRights !="KQ"
       && castleRights != "KQk" && castleRights != "KQq" && castleRights != "KQkq"
       && castleRights != "k" && castleRights != "q" && castleRights != "kq"
       && castleRights != "Q" && castleRights != "Qk" && castleRights != "Qq"
       && castleRights != "Qkq")
      return false;
   if (ep != "-") {
      if (ep.length() != 2) return false;
      if (!(ep[0] >= 'a' && ep[0] <= 'h')) return false;
      if (!((side == "w" && ep[1] == '6') || (side == "b" && ep[1] == '3')))
         return false;
   }
   // The tricky part: The board.
   // Seven slashes?
   if (std::count(board.begin(), board.end(), '/') != 7) return false;
   // Only legal characters?
   for (unsigned int i = 0u; i < board.length(); i++)
   {
       if (!(board[i] == '/' || (board[i] >= '1' && board[i] <= '8')
            || piece_type_is_ok(board[i])))
         return false;
   }
   // Exactly one king per side?
   //IF NEEDED TO EXIST FOR VALID POSITIONS FROM STARTING POSITION -------------------------------------
   //if (std::count(board.begin(), board.end(), 'K') != 1) return false;
   //if (std::count(board.begin(), board.end(), 'k') != 1) return false;
   // Other piece counts reasonable?
   //size_t wp = std::count(board.begin(), board.end(), 'P'),
   //   bp = std::count(board.begin(), board.end(), 'p'),
   //   wn = std::count(board.begin(), board.end(), 'N'),
   //   bn = std::count(board.begin(), board.end(), 'n'),
   //   wb = std::count(board.begin(), board.end(), 'B'),
   //   bb = std::count(board.begin(), board.end(), 'b'),
   //   wr = std::count(board.begin(), board.end(), 'R'),
   //   br = std::count(board.begin(), board.end(), 'r'),
   //   wq = std::count(board.begin(), board.end(), 'Q'),
   //   bq = std::count(board.begin(), board.end(), 'q');
      //IF NEEDED TO EXIST FOR VALID POSITIONS FROM STARTING POSITION -------------------------------------------
   //if (wp > 8 || bp > 8 || wn > 10 || bn > 10 || wb > 10 || bb > 10
   //    || wr > 10 || br > 10 || wq > 9 || bq > 10
    //   || wp + wn + wb + wr + wq > 15 || bp + bn + bb + br + bq > 15)
     // return false;
   return true;
}

bool Boardstate::piece_type_is_ok(char piece_type)
{
    switch (piece_type)
    {
        case ('P'):
            return true;
        case ('N'):
            return true;
        case ('B'):
            return true;
        case ('R'):
            return true;
        case ('Q'):
            return true;
        case ('K'):
            return true;
        case ('p'):
            return true;
        case ('n'):
            return true;
        case ('b'):
            return true;
        case ('r'):
            return true;
        case ('q'):
            return true;
        case ('k'):
            return true;
        default:
            return false;
    }
}

void Boardstate::clear_boardstate()
{
    memset(piece_bitboards, bitboard{}, sizeof(piece_bitboards));
    memset(occupancy_bitboards, bitboard{}, sizeof(occupancy_bitboards));
    side_to_move_ = white;
    en_passant_square_ = no_sq;
    castling_rights_ = NO_CASTLES;
    fullmove_count_ = 0u;
    halfmove_count_ = 0u;
}

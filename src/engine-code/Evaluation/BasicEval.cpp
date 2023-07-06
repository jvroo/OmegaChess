#include "BasicEval.h"

int BasicEval::evaluate(std::shared_ptr<Boardstate> board_state)
{
    auto position_score = 0;
    auto material_score = 0;
    auto bitmap = bitboard{};
    auto piece = 0;
    auto square = 0;
    for (int bb_piece = P; bb_piece <= k; bb_piece++)
    {
        bitmap = board_state->get_piece_bitboards()[bb_piece];
        while (bitmap)
        {
            piece = bb_piece;
            square = Bitboard::get_lsb_index(bitmap);
            material_score += material_scores[piece];

            switch (piece)
            {
                //White pieces
                case (P): position_score += pawn_scores[square]; break;
                case (N): position_score += knight_scores[square]; break;
                case (B): position_score += bishop_scores[square]; break;
                case (R): position_score += rook_scores[square]; break;
                case (Q): break;
                case (K): position_score += king_scores[square]; break;
                //Black pieces
                case (p): position_score -= pawn_scores[mirror_scores[square]]; break;
                case (n): position_score -= knight_scores[mirror_scores[square]]; break;
                case (b): position_score -= bishop_scores[mirror_scores[square]]; break;
                case (r): position_score -= rook_scores[mirror_scores[square]]; break;
                case (q): break;
                case (k): position_score -= king_scores[mirror_scores[square]]; break;
                default: break;
            }

            Bitboard::pop_bit(bitmap,square);
        }
    }
    auto final_score = position_score + material_score;
    //Consider returning + for white and - for black at all times
    return (board_state->get_side_to_move() == white) ? final_score : -final_score;
}

#include "../Search/Search.h"
constexpr int PV_BASE_SCORE = 20000;
constexpr int CAPTURE_MOVE_BASE_SCORE = 10000;
constexpr int FIRST_KILLER_MOVE_SCORE = 9000;
constexpr int SECOND_KILLER_MOVE_SCORE = 8000;
constexpr int FIRST_KILLER_MOVE_INDEX = 0;
constexpr int SECOND_KILLER_MOVE_INDEX = 1;

int BasicEval::score_move(std::shared_ptr<Boardstate> board_state, Move move)
{
    //Score Principle Variation Higher
    if (NegaMax::get_evaluate_PV()) {
        auto pv_move = NegaMax::PV_table[0][NegaMax::get_ply()];
        if (pv_move == move) {
            NegaMax::disable_evaluate_PV();
            return PV_BASE_SCORE;
        }
    }
    //For capture move scoring
    if (move.get_move_capture_flag())
    {
        //Init target piece as pawn in event of en-passant capture
        int target_piece = P;
        //Determine side to move
        int start_piece = 0, end_piece = 0;
        if (board_state->get_side_to_move() == white) {start_piece = p; end_piece = k;}
        else {start_piece = P; end_piece = K;}
        //Loop over bitboards to determine target piece
        for (auto bb_piece = start_piece; bb_piece <= end_piece; bb_piece++)
        {
            if (Bitboard::get_bit(board_state->get_piece_bitboards()[bb_piece], move.get_move_target_square()))
            {
                target_piece = bb_piece;
            }
        }
        return mvv_lva[move.get_move_piece()][target_piece] + CAPTURE_MOVE_BASE_SCORE;
    }
    else
    //For Quiet move scoring
    {
        if (NegaMax::killer_moves[FIRST_KILLER_MOVE_INDEX][NegaMax::get_ply()] == move)
            return FIRST_KILLER_MOVE_SCORE;
        else if (NegaMax::killer_moves[SECOND_KILLER_MOVE_INDEX][NegaMax::get_ply()] == move)
            return SECOND_KILLER_MOVE_SCORE;
        else
            return NegaMax::history_moves[move.get_move_piece()][move.get_move_target_square()];
    }
    return 0;
}

void BasicEval::print_move_score(std::shared_ptr<Boardstate> board_state, MoveList &move_list)
{
    //move.print_move_UCI();
    //printf("Source Piece: %c\n", ASCII_Pieces[move.get_move_piece()]);
    //printf("Target Piece: %c\n", ASCII_Pieces[target_piece]);
    for (int iCount = 0; iCount < move_list.get_num_moves(); iCount++)
    {
        move_list.get_move(iCount).print_move_UCI();
        std::cout << "      " << BasicEval::score_move(board_state, move_list.get_move(iCount)) << std::endl;
    }
}

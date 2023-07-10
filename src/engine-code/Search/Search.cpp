#include "Search.h"

#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////
//                       Random                          //
///////////////////////////////////////////////////////////

Move RandomEngine::best_move{};

int RandomEngine::find_random_move(std::shared_ptr<Boardstate> board_state)
{
    MoveList move_list;
    board_state->generate_moves(move_list);
    auto move_list_size = move_list.get_num_moves();
    if (move_list_size == 0)
    {
        return 0;
    }
    srand(time(NULL));
    auto move_index = rand()%move_list_size;
    auto move = move_list.get_move(move_index);
    BoardstateCopy copy_of_state;
    board_state->make_copy(copy_of_state);
    while (!board_state->make_move(move, all_moves))
    {
        board_state->restore_copy(copy_of_state);
        move_index = rand()%move_list_size;
        move = move_list.get_move(move_index);
    }
    board_state->restore_copy(copy_of_state);
    best_move = move;
    return 0;
}

Move RandomEngine::get_best_move()
{
    return best_move;
}

///////////////////////////////////////////////////////////
//                       NegaMax                         //
///////////////////////////////////////////////////////////

constexpr auto SIDE_TO_MOVE_SHIFT = 1;

constexpr auto MAXIMIM_SCORE = 50000;
constexpr auto MINIMUM_SCORE = -50000;
constexpr auto ASPIRATION_WINDOW_SCORE = 50;
constexpr auto CHECK_MATE_SCORE = 49000;
constexpr auto DRAW_SCORE = 0;
constexpr int FIRST_KILLER_MOVE_INDEX = 0;
constexpr int SECOND_KILLER_MOVE_INDEX = 1;
constexpr int NULL_MOVE_PRUNING_DEPTH = 3;
constexpr int REDUCTION_LIMIT = 2;
constexpr int NODE_POLL_FREQ = 2047;

long long NegaMax::nodes_{0};
int NegaMax::ply_{0};
//Move NegaMax::best_move{};
Move NegaMax::killer_moves[][MAX_PLY];
int NegaMax::history_moves[][MAX_PLY];
int NegaMax::PV_length[];
Move NegaMax::PV_table[][MAX_PLY];
bool NegaMax::following_PV_{false};
bool NegaMax::evaluate_PV_{false};
int NegaMax::reduction_limit{3};
int NegaMax::full_depth_moves{4};
UCITimer NegaMax::gameTimer{};

int NegaMax::find_best_move(std::shared_ptr<Boardstate> board_state, int alpha, int beta, int depth)
{
    //Quick stop as needed
    if((nodes_ & NODE_POLL_FREQ ) == 0) {
        // "listen" to the GUI/user input
		gameTimer.communicate();
    }
    //Init the principle value length
    PV_length[ply_] = ply_;
    //Exit recursive loop with evaluation of position
    if (depth == 0)
        return NegaMax::quiescence_search(board_state,alpha,beta);
    //Ensure that engine does not crash by searching at a depth that is too large
    if (ply_ >= MAX_PLY)
        return BasicEval::evaluate(board_state);
    //Increment the number of nodes traversed
    nodes_++;
    //Init king is in check or not or given boardstate
    bool is_king_in_check = board_state->is_square_attacked(
                            (board_state->get_side_to_move() == white) ?
                            Bitboard::get_lsb_index(board_state->get_piece_bitboards()[K]) :
                            Bitboard::get_lsb_index(board_state->get_piece_bitboards()[k]),
                            board_state->get_side_to_move() ^ SIDE_TO_MOVE_SHIFT);
    //If king in check then increase search depth to ensure no unforeseen mates
    if (is_king_in_check) depth++;
    //Init legal move counter
    auto legal_moves = 0;
    //Null move pruning
    if ((depth >= NULL_MOVE_PRUNING_DEPTH) && (is_king_in_check == false) && (ply_ >= 1)) {
        //Make copy of state
        auto copy_of_null_state = BoardstateCopy{};
        board_state->make_copy(copy_of_null_state);
        board_state->set_side_to_move(!board_state->get_side_to_move());
        board_state->set_en_passant_square(no_sq);
        auto score = -NegaMax::find_best_move(board_state, -beta, -beta + 1, depth - 1 - REDUCTION_LIMIT);
        board_state->restore_copy(copy_of_null_state);
        if (score >= beta) {
            return beta;
        }
    }
    //Generate empty movelist and populate for current boardstate
    auto move_list = MoveList{};
    board_state->generate_moves(move_list);
    //Find principle variation
    if (following_PV_) {
        enable_PV_scoring(move_list);
    }
    //Order moves to increase alpha beta pruning speed
    Search::sort_moves(board_state, move_list);
    //Number of moves searched
    auto moves_searched = 0;
    //Loop over moves in move list
    for (auto iCount = 0; iCount < move_list.get_num_moves(); iCount++)
    {
        //Make copy of state
        auto copy_of_state = BoardstateCopy{};
        board_state->make_copy(copy_of_state);
        //Increment the number of moves in given branch traversed
        ply_++;
        //Make only legal moves
        Move move = move_list.get_move(iCount);
        if (board_state->make_move(move,all_moves) == 0)
        {
            //Restore state for illegal move
            ply_--;
            board_state->restore_copy(copy_of_state);
            continue;
        }
        //Increment legal moves
        legal_moves++;
        //Initialize the score
        int score = 0;
        //Find principle value using enhanced search
        if (moves_searched == 0) {
            //Iterate to next node in tree
            score = -NegaMax::find_best_move(board_state, -beta, -alpha, depth - 1);
        }
        else {
            if ((moves_searched >= full_depth_moves) &&
                (depth >= reduction_limit) &&
                (is_king_in_check == false) &&
                (move.get_move_capture_flag() == false) &&
                (move.get_move_promotion_type() == 0)) {
                score = -NegaMax::find_best_move(board_state, -alpha - 1, -alpha, depth - 2);
            }
            else {
                score = alpha + 1;
            }
            if (score > alpha) {
                score = -NegaMax::find_best_move(board_state, -alpha - 1, -alpha, depth - 1);
                if ((score > alpha) && (score < beta)) {
                    score = -NegaMax::find_best_move(board_state, -beta, -alpha, depth - 1);
                }
            }
        }
        //Restore state
        ply_--;
        board_state->restore_copy(copy_of_state);
        if (gameTimer.get_stopped()) {
            return score;
        }
        //Increment moves searched
        moves_searched++;
        //Using Fail - Hard framework
        if (score >= beta)
        {
            if (move.get_move_capture_flag() == 0)
            {
                //Set killer moves
                killer_moves[SECOND_KILLER_MOVE_INDEX][ply_] = killer_moves[FIRST_KILLER_MOVE_INDEX][ply_];
                killer_moves[FIRST_KILLER_MOVE_INDEX][ply_] = move;
            }
            //Node fails high
            return beta;
        }
        //If a better move is found
        if (score > alpha)
        {
            if (move.get_move_capture_flag() == 0)
            {
                //Set history moves
                history_moves[move.get_move_piece()][move.get_move_target_square()] += depth;
            }
            //Set new alpha
            alpha = score;
            //Write PV move to PV table
            PV_table[ply_][ply_] = move;
            for (int next_ply = ply_ + 1; next_ply < PV_length[ply_+1]; next_ply++)
                PV_table[ply_][next_ply] = PV_table[ply_ + 1][next_ply];
            //Adjust PV length
            PV_length[ply_] = PV_length[ply_+1];
        }
    }
    //No legal moves for given boardstate, handles Checkmate and Stalemate
    if (legal_moves == 0)
    {
        if (is_king_in_check)
        {
            //The + ply_ ensures that the quickest mate is played
            return -CHECK_MATE_SCORE + ply_;
        }
        else
        {
            return DRAW_SCORE;
        }
    }
    //Node (move) fails low
    return alpha;
}

//Searches captures only until quiet position with no more captures
int NegaMax::quiescence_search(std::shared_ptr<Boardstate> board_state, int alpha, int beta)
{
    //Quick stop as needed
    if((nodes_ & NODE_POLL_FREQ ) == 0) {
        // "listen" to the GUI/user input
		gameTimer.communicate();
    }
    //Increments nodes
    nodes_++;
    //Find position evaluation
    int evaluation = BasicEval::evaluate(board_state);
    //Using Fail - Hard framework
    if (evaluation >= beta)
    {
        return beta;
    }
    //If a better move is found
    if (evaluation > alpha)
    {
        alpha = evaluation;
    }
    //Generate empty movelist and populate for current boardstate
    auto move_list = MoveList{};
    board_state->generate_moves(move_list);
    //Order moves to increase alpha beta pruning speed
    Search::sort_moves(board_state, move_list);
    //Loop over moves in move list
    for (auto iCount = 0; iCount < move_list.get_num_moves(); iCount++)
    {
        //Make copy of state
        auto copy_of_state = BoardstateCopy{};
        board_state->make_copy(copy_of_state);
        //Increment the number of moves in given branch traversed
        ply_++;
        //Make only legal moves
        Move move = move_list.get_move(iCount);
        if (board_state->make_move(move,captures_only) == 0)
        {
            //Restore state for illegal move
            ply_--;
            board_state->restore_copy(copy_of_state);
            continue;
        }
        //Iterate to next node in tree
        int score = -NegaMax::quiescence_search(board_state, -beta, -alpha);
        //Restore state
        ply_--;
        board_state->restore_copy(copy_of_state);
        //Using Fail - Hard framework
        if (score >= beta)
        {
            return beta;
        }
        //If a better move is found
        if (score > alpha)
        {
            alpha = score;
        }
    }
    return alpha;
}

int NegaMax::nega_search(std::shared_ptr<Boardstate> board_state, int alpha, int beta, int depth)
{
    //Reset all data used in search
    //reset_nodes();
    reset_ply();
    enable_following_PV();
    //Find best move
    return find_best_move(board_state, alpha, beta, depth);
}

void NegaMax::enable_PV_scoring(MoveList move_list){
      disable_following_PV();
      for (auto iCount = 0; iCount < move_list.get_num_moves(); iCount++) {
        auto pv_move = PV_table[0][ply_];
        auto current_move = move_list.get_move(iCount);
        if (pv_move == current_move) {
            enable_evaluate_PV();
            enable_following_PV();
        }
      }
}

constexpr auto BEST_MOVE_INDEX = 0;
Move NegaMax::get_best_move()
{
    return PV_table[BEST_MOVE_INDEX][BEST_MOVE_INDEX];
}

void NegaMax::reset_nodes()
{
    nodes_ = 0;
}

void NegaMax::reset_ply()
{
    ply_ = 0;
}

void NegaMax::disable_following_PV()
{
    following_PV_ = false;
}

void NegaMax::enable_following_PV()
{
    following_PV_ = true;
}

void NegaMax::disable_evaluate_PV()
{
    evaluate_PV_ = false;
}

void NegaMax::enable_evaluate_PV()
{
    evaluate_PV_ = true;
}

bool NegaMax::get_following_PV()
{
    return following_PV_;
}

bool NegaMax::get_evaluate_PV()
{
    return evaluate_PV_;
}

int NegaMax::get_ply()
{
    return ply_;
}

long long NegaMax::get_nodes()
{
    return nodes_;
}

///////////////////////////////////////////////////////////
//                       Search                          //
///////////////////////////////////////////////////////////

std::string Search::search_position(std::shared_ptr<Boardstate> board_state, int depth, int search_type)
{
    auto score = 0;
    std::string move_string = "";
    auto alpha = MINIMUM_SCORE;
    auto beta = MAXIMIM_SCORE;
    NegaMax::gameTimer.set_stopped(false);
    //Add searches as needed
    switch (search_type)
    {
    case (RandomSearch):
        score = RandomEngine::find_random_move(board_state);
        UCI_Link::set_search_info(score,1,0);
        move_string = RandomEngine::get_best_move().get_move_UCI();
        return move_string;
    case (NegaMaxSearch):
        //implementing iterative deepening
        NegaMax::disable_following_PV();
        NegaMax::disable_evaluate_PV();
        //Clear Move lists
        memset(NegaMax::killer_moves, 0, sizeof(NegaMax::killer_moves));
        memset(NegaMax::history_moves, 0, sizeof(NegaMax::history_moves));
        memset(NegaMax::PV_table, 0, sizeof(NegaMax::PV_table));
        memset(NegaMax::PV_length, 0, sizeof(NegaMax::PV_length));
        for (int current_depth = 1; current_depth <= depth; current_depth++)
        {
            if (NegaMax::gameTimer.get_stopped()) {
                break;
            }
            score = NegaMax::nega_search(board_state, alpha, beta, current_depth);
            if ((score <= alpha) || (score >= beta)) {
                alpha = MINIMUM_SCORE;
                beta = MAXIMIM_SCORE;
                continue;
            }
            alpha = score - ASPIRATION_WINDOW_SCORE;
            beta = score + ASPIRATION_WINDOW_SCORE;
            UCI_Link::set_search_info(score,current_depth,NegaMax::get_nodes());
            UCI_Link::print_search_info(NegaMaxSearch);
        }
        //returning best move
        move_string = NegaMax::get_best_move().get_move_UCI();
        return move_string;
    default:
        return move_string;
    }
    return move_string;
}

int Search::sort_moves(std::shared_ptr<Boardstate> board_state, MoveList &moves)
{
    auto move_scores = std::vector<int>{};
    for (auto iCount = 0; iCount < moves.get_num_moves(); iCount++)
    {
        move_scores.push_back(BasicEval::score_move(board_state, moves.get_move(iCount)));
    }
    //Sort the move list
    //THIS IS A SLOW SORT ALGORITHM (O(n^2)), IMPROVEMENTS HERE WILL INCREASE SPEED OF SEARCH
    for (auto current_move = 0; current_move < moves.get_num_moves(); current_move++)
    {
        for (auto next_move = current_move + 1; next_move < moves.get_num_moves(); next_move++)
        {
            if (move_scores[current_move] < move_scores[next_move])
            {
                //Order scores
                auto temp_score = move_scores[current_move];
                move_scores[current_move] = move_scores[next_move];
                move_scores[next_move] = temp_score;
                //Order moves
                auto temp_move = moves.get_move(current_move);
                moves.replace_move(moves.get_move(next_move), current_move);
                moves.replace_move(temp_move, next_move);
            }
        }
    }
    return 0;
}


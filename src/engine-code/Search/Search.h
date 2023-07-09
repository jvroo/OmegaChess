#ifndef SEARCH_H
#define SEARCH_H

#include <memory>
#include <vector>

#include "../BoardState.h"
#include "../Evaluation/BasicEval.h"
#include "../../GUI-code/UCI/UCI.h"

/** \file Search.h
    \brief Contains various search algorithms
 */

 class RandomEngine
 {
 public:
    static int find_random_move(std::shared_ptr<Boardstate> board_state);
    static Move get_best_move();
 private:
    static Move best_move;
 };

 constexpr auto MAX_PLY = 64;
 constexpr auto NUM_SQUARES = 64;
 constexpr auto NUM_PIECE_TYPES = 12;
 constexpr auto NUM_KILLER_IDS = 2;

 class NegaMax
 {
 public:
    static int nega_search(std::shared_ptr<Boardstate> board_state, int alpha, int beta, int depth);
    static void reset_nodes();
    static void reset_ply();
    static void disable_following_PV();
    static void enable_following_PV();
    static void disable_evaluate_PV();
    static void enable_evaluate_PV();
    static bool get_following_PV();
    static bool get_evaluate_PV();
    static int get_ply();
    static long long get_nodes();
    static Move get_best_move();

    //Move ordering for negamax
    //killer_moves[id][ply] //Can increase ply for greater depth search
    static Move killer_moves[NUM_KILLER_IDS][MAX_PLY];
    //killer_moves[piece][square]
    static int history_moves[NUM_PIECE_TYPES][NUM_SQUARES];
    /*
      ================================
            Triangular PV table
      --------------------------------
        PV line: e2e4 e7e5 g1f3 b8c6
      ================================
           0    1    2    3    4    5

      0    m1   m2   m3   m4   m5   m6

      1    0    m2   m3   m4   m5   m6

      2    0    0    m3   m4   m5   m6

      3    0    0    0    m4   m5   m6

      4    0    0    0    0    m5   m6

      5    0    0    0    0    0    m6
    */
    static int PV_length[MAX_PLY];
    static Move PV_table[MAX_PLY][MAX_PLY];
 private:
    static int quiescence_search(std::shared_ptr<Boardstate> board_state, int alpha, int beta);
    static int find_best_move(std::shared_ptr<Boardstate> board_state, int alpha, int beta, int depth);
    static long long nodes_;
    static int ply_;
    static bool following_PV_;
    static bool evaluate_PV_;
    //static Move best_move;
    //Enable PV move scoring
    static void enable_PV_scoring(MoveList move_list);
    static int reduction_limit;
    static int full_depth_moves;
 };

 //Expand types of search as needed
 enum {RandomSearch = 0, NegaMaxSearch};

 namespace Search
 {
     //Returns best move string i.e. e2e4
     std::string search_position(std::shared_ptr<Boardstate> board_state, int depth, int search_type);
     //Sort Moves in a movelist for given boardstate
     int sort_moves(std::shared_ptr<Boardstate> board_state, MoveList &moves);
 }

#endif



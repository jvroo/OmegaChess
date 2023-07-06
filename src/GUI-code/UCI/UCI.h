#ifndef UCI_H
#define UCI_H

#include <string>
#include <memory>
#include <sstream>

#include "../../engine-code/BoardState.h"
#include "../../engine-code/Evaluation/BasicEval.h"
#include "../../engine-code/Search/Search.h"

/** \file UCI.h
    \brief Contains UCI implementation for GUI
 */

using ptr_board = std::shared_ptr<Boardstate>;

class UCI_Link
{
public:
    UCI_Link();
    UCI_Link(const ptr_board board_state);
    void UCI_loop();
    Move parse_move(std::string move_string);
    void parse_position(std::string command);
    void parse_go(std::string command);
    void set_board_state(const ptr_board board_state);
    static void set_search_info(int score, int depth,int nodes);
    static void print_search_info(int search_type);
private:
    static void print_score_info(int score);
    static void print_depth_info(int depth);
    static void print_node_info(int nodes);
    static void print_PV_info();
    void print_UCI_ID_Info();
    ptr_board board_state_;
    static int score_;
    static int depth_;
    static int nodes_;
};

#endif



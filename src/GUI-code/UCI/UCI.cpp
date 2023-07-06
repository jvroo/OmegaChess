#include "UCI.h"

constexpr auto EMPTY_BOARD_FEN = "8/8/8/8/8/8/8/8 w - - 0 0";

UCI_Link::UCI_Link():
    board_state_{std::make_shared<Boardstate>()}
{
    board_state_->FEN_parse(EMPTY_BOARD_FEN);
}

UCI_Link::UCI_Link(const ptr_board board_state):
    board_state_{board_state}
{

}

constexpr auto MAX_CHARACTERS = 10000;
constexpr auto IS_READY = "isready";
constexpr auto IS_READY_SIZE = 7;
constexpr auto READY_OK = "readyok";
constexpr auto POSITION = "position";
constexpr auto POSITION_SIZE = 8;
constexpr auto UCI_NEW_GAME = "ucinewgame";
constexpr auto UCI_NEW_GAME_SIZE = 10;
constexpr auto GO_COMMAND = "go";
constexpr auto GO_COMMAND_SIZE = 2;
constexpr auto START_POS = "position startpos";
constexpr auto QUIT = "quit";
constexpr auto QUIT_SIZE = 4;
constexpr auto UCI = "uci";
constexpr auto UCI_SIZE = 3;

void UCI_Link::UCI_loop()
{
    //reset STDIN and STDOUT buffers to ensure correct information from GUI
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    //Define user input
    char input[MAX_CHARACTERS];
    //Print engine information
    print_UCI_ID_Info();
    //Start loop
    auto looping = true;
    while (looping)
    {
        //Reset input
        memset(input, 0, sizeof(input));
        //Flush output to clear
        fflush(stdout);
        //Get input and continue if none
        if (!fgets(input, MAX_CHARACTERS, stdin)) continue;
        if (input[0] == '\n') continue;
        //Parse GUI is ready
        if (strncmp(input, IS_READY, IS_READY_SIZE) == 0)
        {
            printf(READY_OK);
            continue;
        }
        //Parse GUI position command
        if (strncmp(input, POSITION, POSITION_SIZE) == 0)
        {
            parse_position(input);
            continue;
        }
        //Parse GUI ucinewgame command
        if (strncmp(input, UCI_NEW_GAME, UCI_NEW_GAME_SIZE) == 0)
        {
            parse_position(START_POS);
            continue;
        }
        //Parse GUI go command
        if (strncmp(input, GO_COMMAND, GO_COMMAND_SIZE) == 0)
        {
            parse_go(input);
            continue;
        }
        //Parse GUI quit command
        if (strncmp(input, QUIT, QUIT_SIZE) == 0)
        {
            break;
        }
        //Parse GUI uci command
        if (strncmp(input, UCI, UCI_SIZE) == 0)
        {
            //Print engine information
            print_UCI_ID_Info();
            continue;
        }
    }
}

constexpr auto ALPHABET_SHIFTER = 'a';
constexpr auto NUMERIC_SHIFTER = '0';
constexpr auto NUMBER_OF_ROWS = 8;
constexpr auto MINIMUM_MOVE_STRING_LENGTH = 4;
constexpr auto MINIMUM_PROMOTION_STRING_LENGTH = 5;
const auto FAIL_PARSE = Move{};
constexpr auto NO_PIECE = 0;

bool isPromotion(int promotion_piece);

//Recieves and input such as "a7a8q" or "b4b2"
Move UCI_Link::parse_move(std::string move_string)
{
    //Check string is correct size
    if (move_string.size() < MINIMUM_MOVE_STRING_LENGTH)
        return FAIL_PARSE;
    //Init Movelist from current Linker Boardstate
    auto move_list = MoveList{};
    board_state_->generate_moves(move_list);
    int start_square = (move_string[0] - ALPHABET_SHIFTER) +
                       (NUMBER_OF_ROWS - (move_string[1] - NUMERIC_SHIFTER)) * NUMBER_OF_ROWS;
    int target_square = (move_string[2] - ALPHABET_SHIFTER) +
                        (NUMBER_OF_ROWS - (move_string[3] - NUMERIC_SHIFTER)) * NUMBER_OF_ROWS;
    for (auto iCount = 0; iCount < move_list.get_num_moves(); iCount++)
    {
        auto move = move_list.get_move(iCount);
        if ((start_square == move.get_move_source_square()) &&
            (target_square == move.get_move_target_square()))
        {
            auto promotion_piece = move.get_move_promotion_type();
            if (isPromotion(promotion_piece))
            {
                if (move_string.size() < MINIMUM_PROMOTION_STRING_LENGTH)
                {
                    return FAIL_PARSE;
                }
                //Promoted to queen
                if ((promotion_piece == Q || promotion_piece == q) && move_string[4] == 'q')
                    return move;
                //Promoted to rook
                else if ((promotion_piece == R || promotion_piece == r) && move_string[4] == 'r')
                    return move;
                //Promoted to bishop
                else if ((promotion_piece == B || promotion_piece == b) && move_string[4] == 'b')
                    return move;
                //Promoted to knight
                else if ((promotion_piece == N || promotion_piece == n) && move_string[4] == 'n')
                    return move;
                continue;
            }
            //Move is legal non-promotion
            return move;
        }
    }
    //Move not found
    return FAIL_PARSE;
}

constexpr auto INVALD_UCI_COMMAND = "INVALID UCI COMMAND!\n";
constexpr auto POSITION_STRING = "position";
constexpr auto FEN_STRING_CONSTANT = "fen";
constexpr auto STARTPOS_STRING = "startpos";
constexpr auto STARTPOS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr auto DELETE_INDEX_TO_FEN = 13;
constexpr auto MINIMUM_FEN = 15;
constexpr auto MOVES_STRING_SIZE = 6;
constexpr auto NO_MOVE = -1;
constexpr auto MOVES_STRING = "moves";

void UCI_Link::parse_position(std::string command)
{
    std::istringstream command_string_stream(command);
    std::string position_string = "";
    if (!command_string_stream)
    {
        std::cout << INVALD_UCI_COMMAND << std::endl;
        return;
    }
    command_string_stream >> position_string;
    if (position_string != POSITION_STRING)
    {
        std::cout << INVALD_UCI_COMMAND << std::endl;
        return;
    }
    std::string position_type = "";
    if (!command_string_stream)
    {
        std::cout << INVALD_UCI_COMMAND << std::endl;
        return;
    }
    command_string_stream >> position_type;
    if (position_type == STARTPOS_STRING)
    {
        board_state_->FEN_parse(STARTPOS_FEN);
    }
    else if (position_type == FEN_STRING_CONSTANT)
    {
        if (!command_string_stream)
        {
            std::cout << INVALD_UCI_COMMAND << std::endl;
            return;
        }
        std::string fen_string = command_string_stream.str();
        if (fen_string.size() < MINIMUM_FEN)
        {
            std::cout << INVALD_UCI_COMMAND << std::endl;
            return;
        }
        fen_string.erase(fen_string.begin(), fen_string.begin()+DELETE_INDEX_TO_FEN);
        board_state_->FEN_parse(fen_string);
    }
    else
    {
        std::cout << INVALD_UCI_COMMAND << std::endl;
        return;
    }
    std::string fen_string = command_string_stream.str();
    auto moves_index = fen_string.find(MOVES_STRING);
    if (moves_index == NO_MOVE)
    {
        board_state_->print_board();
        return;
    }
    fen_string.erase(fen_string.begin(), fen_string.begin()+moves_index+MOVES_STRING_SIZE);
    std::istringstream move_string_stream(fen_string);
    while (move_string_stream)
    {
        std::string move_string = "";
        move_string_stream >> move_string;
        auto move = parse_move(move_string);
        if (move.is_no_move())
        {
            break;
        }
        board_state_->make_move(move, all_moves);
    }
    board_state_->print_board();
}

constexpr auto NO_DEPTH = -1;
constexpr auto GO_STRING = "go";
constexpr auto DEPTH_STRING = "depth";
constexpr auto PLACEHOLDER_DEPTH = 5;

void UCI_Link::parse_go(std::string command)
{
    auto depth = NO_DEPTH;
    std::istringstream command_string_stream(command);
    std::string go_string = "";
    if (!command_string_stream)
    {
        std::cout << INVALD_UCI_COMMAND << std::endl;
        return;
    }
    command_string_stream >> go_string;
    if (go_string != GO_STRING)
    {
        std::cout << INVALD_UCI_COMMAND << std::endl;
        return;
    }
    std::string depth_string = "";
    if (!command_string_stream)
    {
        std::cout << INVALD_UCI_COMMAND << std::endl;
        return;
    }
    command_string_stream >> depth_string;
    if (depth_string.find(DEPTH_STRING) == NO_MOVE)
    {
        depth = PLACEHOLDER_DEPTH;
    }
    else
    {
        depth_string = "";
        command_string_stream >> depth_string;
        depth = stoi(depth_string);
    }
    //CHANGE AS NEEDED
    std::string best_move = Search::search_position(board_state_,depth,NegaMaxSearch);
    UCI_Link::print_search_info(NegaMaxSearch);
    std::cout << "bestmove " << best_move << std::endl;
}

void UCI_Link::set_board_state(const ptr_board board_state)
{
    board_state_ = board_state;
}

void UCI_Link::print_UCI_ID_Info()
{
    printf("id name OmegaChess\n");
    printf("id author Keon Roohani\n");
    printf("uciok\n");
}

int UCI_Link::score_{0};
int UCI_Link::nodes_{0};
int UCI_Link::depth_{0};

void UCI_Link::set_search_info(int score, int depth,int nodes)
{
    score_ = score;
    depth_ = depth;
    nodes_ = nodes;
}

void UCI_Link::print_search_info(int search_type)
{
    printf("info ");
    print_score_info(score_);
    print_depth_info(depth_);
    print_node_info(nodes_);
    if (search_type == NegaMaxSearch)
        print_PV_info();
    printf("\n");
}

void UCI_Link::print_score_info(int score)
{
    printf("score cp %d ", score);
}

void UCI_Link::print_depth_info(int depth)
{
    printf("depth %d ", depth);
}

void UCI_Link::print_node_info(int nodes)
{
    printf("nodes %ld ", nodes);
}

void UCI_Link::print_PV_info()
{
    printf("pv ");
    for (int iCount = 0; iCount < NegaMax::PV_length[0]; iCount++)
    {
        NegaMax::PV_table[0][iCount].print_move_UCI();
    }
}


bool isPromotion(int promotion_piece)
{
    switch (promotion_piece)
    {
    case (Q):
        return true;
    case (q):
        return true;
    case (R):
        return true;
    case (r):
        return true;
    case (B):
        return true;
    case (b):
        return true;
    case (N):
        return true;
    case (n):
        return true;
    default:
        return false;
    }
    return false;
}

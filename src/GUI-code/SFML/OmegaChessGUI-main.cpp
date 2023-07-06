//GUI implementation for OmegaChess using SFML

#include <iostream>
#include <SFML/Graphics.hpp>
#include "Board.h"

using namespace std;

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 700;
const int BITS_PER_PIXEL = 32;

const auto NUM_SQUARES_BOARD = 8;

const auto TOP_LEFT_COL = sf::Color{232,235,239};
const auto TOP_RIGHT_COL = sf::Color{125,135,150};
const auto BACKGROUND_COLOR = sf::Color{150,150,150};

int main()
{
    sf::RenderWindow window{sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BITS_PER_PIXEL),
                            "OmegaChess", sf::Style::Titlebar | sf::Style::Close};

    sf::Event e{};
    auto board = Board{NUM_SQUARES_BOARD, TOP_LEFT_COL, TOP_RIGHT_COL};
    board.readFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //Test pos: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq
    while (window.isOpen())
    {
        while (window.pollEvent(e))
        {
            if((e.type == sf::Event::Closed) ||
                    ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::Escape)))
            {
                window.close();
                break;
            }
            if (e.type == sf::Event::MouseButtonPressed)
            {
                if (e.mouseButton.button == sf::Mouse::Right)
                {
                    board.highlightSquare(sf::Vector2i{e.mouseButton.x,e.mouseButton.y});
                }
                if (e.mouseButton.button == sf::Mouse::Left)
                {
                    board.clearBoardHighlights();
                    board.selectSquare(sf::Vector2i{e.mouseButton.x, e.mouseButton.y});
                }
            }
            if (e.type == sf::Event::MouseButtonReleased)
            {
                if (e.mouseButton.button == sf::Mouse::Left)
                {
                    board.selectSquare(sf::Vector2i{e.mouseButton.x, e.mouseButton.y});
                }
            }
            if((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::F))
            {
                board.flipBoard();
            }
            if (e.type == sf::Event::MouseMoved)
            {
                board.dragPiece(sf::Mouse::getPosition(window));
            }
        }
        window.clear(BACKGROUND_COLOR);
        window.draw(board);
        window.display();
    }
    return 0;
};

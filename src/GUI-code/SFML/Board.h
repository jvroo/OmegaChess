#ifndef BOARD_H
#define BOARD_H

/** \file Board.h
    \brief Contains Board GUI class
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "Square.h"

class InvalidFEN{};

/** \class Board
    \brief A graphical representation of a board

    This class represents a board graphically using the SFML library.
    A board is formed using a vector of squares of the Square class.
 */
class Board : public sf::Drawable
{
public:
    Board(int boardSize, sf::Color topLeftCol, sf::Color topRightCol);
    void highlightSquare(sf::Vector2i mousePos);
    void clearBoardHighlights();
    void flipBoard();
    void addPiece(const PieceType &newPieceType, const PieceColor &newPieceColor, const SquareNames &newSquareName);
    void movePiece(const SquareNames &oldSquareName, const SquareNames &newSquareName);
    void readFEN(std::string FEN);
    void selectSquare(sf::Vector2i mousePos);
    void dragPiece(sf::Vector2i mousePos);
    /** \brief A virtual destructor to be called by the parents of Board */
    virtual ~Board(){}
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void initSquares(sf::Color topLeftCol, sf::Color topRightCol);
    void initTextures();
    void createFENPieces(const char &character, int &squareCount);
    void countFENSquares(const char &character, int &squareCount);
    int boardSize_;
    SquareNames nameSquares(int squareIndex);
    int selectedSquareIndex_;
    std::vector<Square> squareList_;
};

#endif



#ifndef SQUARE_H
#define SQUARE_H

/** \file Square.h
    \brief Contains Square GUI class
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include "Piece.h"

enum class SquareNames
{
	a8 = 0, b8,c8,d8,e8,f8,g8,h8,
	a7,b7,c7,d7,e7,f7,g7,h7,
	a6,b6,c6,d6,e6,f6,g6,h6,
	a5,b5,c5,d5,e5,f5,g5,h5,
	a4,b4,c4,d4,e4,f4,g4,h4,
	a3,b3,c3,d3,e3,f3,g3,h3,
	a2,b2,c2,d2,e2,f2,g2,h2,
	a1,b1,c1,d1,e1,f1,g1,h1,
	NO_NAME
};

/** \class Square
    \brief A graphical representation of a Square to form the board

    This class represents a square graphically using the SFML library,
    The Square class is a decendent of the sf::Drawable virtual class.
    The square is represented by an sf::Sprite with either black or white
    colours. This class can be used as a constituent of the Board Class.
 */
class Square : public sf::Drawable
{
public:
    Square(sf::Color color, sf::Vector2f pos, sf::Vector2f squareSize, sf::Vector2i coordinate,
           SquareNames name, bool isLightSquare);
    void highlightToggle(sf::Color newColour);
    void dehighlight();
    bool pointWithinSquare(sf::Vector2i point);
    void invertFlip();
    static void setBoardSize(int boardSize);
    void addPiece(const PieceType &newPieceType, const PieceColor &newPieceColor);
    void addPiece(std::shared_ptr<Piece> newPiece);
    void givePiece(Square &newPieceSquare);
    void removePiece();
    void dragPiece(sf::Vector2i mousePos);
    bool operator==(const SquareNames rhs) const;
    bool hasPiece() const;
    void centerPieceInSquare();
    void drawSquare(sf::RenderTarget& target, sf::RenderStates states) const;
    void drawPiece(sf::RenderTarget& target, sf::RenderStates states) const;
    /** \brief A virtual destructor to be called by the parents of Square */
    virtual ~Square(){}
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    sf::RectangleShape square_;
    SquareNames squareName_;
    sf::Color originalColor_;
    sf::Vector2i coord_;
    bool isHighlighted_;
    bool isLightSquare_;
    static int boardSize_;
    std::vector<std::shared_ptr<Piece>> piece_;
};

#endif



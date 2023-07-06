#ifndef PIECE_H
#define PIECE_H

/** \file Piece.h
    \brief Contains Piece GUI class
 */

#include <SFML/Graphics.hpp>

class PieceImageCannotBeOpened{};

enum class PieceType
{
    King = 0,
    Queen,
    Bishop,
    Knight,
    Rook,
    Pawn
};

enum class PieceColor
{
    White = 0,
    Black
};

//Error exception classes
class WhiteBishopImageCannotBeOpened{};
class BlackBishopImageCannotBeOpened{};
class WhiteRookImageCannotBeOpened{};
class BlackRookImageCannotBeOpened{};
class WhiteKnightImageCannotBeOpened{};
class BlackKnightImageCannotBeOpened{};
class WhiteQueenImageCannotBeOpened{};
class BlackQueenImageCannotBeOpened{};
class WhiteKingImageCannotBeOpened{};
class BlackKingImageCannotBeOpened{};
class WhitePawnImageCannotBeOpened{};
class BlackPawnImageCannotBeOpened{};

/** \class Piece
    \brief A graphical representation of a Piece

    This class represents a piece graphically using the SFML library.
    A piece is formed using a sprite containing a piece image and information.
 */

class Piece : public sf::Drawable
{
public:
    Piece(const sf::Vector2f &squareSize, const sf::Vector2f &squarePos,
          const PieceType &newPieceType, const PieceColor &newPieceColor);
    void setToSquarePosition(const sf::Vector2f &currentPos);
    static void loadPieceTextures(std::string imagePathName, PieceType pieceType, PieceColor color);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    /** \brief A virtual destructor to be called by the parents of Piece */
    virtual ~Piece(){}
private:
    void initSprite(sf::Vector2f squareSize);
    sf::Sprite piece_;

    void setPieceTexture(const PieceColor &color, const PieceType &newPieceType);
    void setPawnTextures(const PieceColor &color);
    void setRookTextures(const PieceColor &color);
    void setKnightTextures(const PieceColor &color);
    void setBishopTextures(const PieceColor &color);
    void setQueenTextures(const PieceColor &color);
    void setKingTextures(const PieceColor &color);

    //Textures for Piece Types
    static sf::Texture whiteBishopTexture_;
    static sf::Texture blackBishopTexture_;
    static sf::Texture whitePawnTexture_;
    static sf::Texture blackPawnTexture_;
    static sf::Texture whiteRookTexture_;
    static sf::Texture blackRookTexture_;
    static sf::Texture whiteKnightTexture_;
    static sf::Texture blackKnightTexture_;
    static sf::Texture whiteQueenTexture_;
    static sf::Texture blackQueenTexture_;
    static sf::Texture whiteKingTexture_;
    static sf::Texture blackKingTexture_;
    //Helper Functions for loading
    static void loadPawnTextures(const PieceColor &color, const std::string &imagePathName);
    static void loadRookTextures(const PieceColor &color, const std::string &imagePathName);
    static void loadKnightTextures(const PieceColor &color, const std::string &imagePathName);
    static void loadBishopTextures(const PieceColor &color, const std::string &imagePathName);
    static void loadQueenTextures(const PieceColor &color, const std::string &imagePathName);
    static void loadKingTextures(const PieceColor &color, const std::string &imagePathName);

};

#endif



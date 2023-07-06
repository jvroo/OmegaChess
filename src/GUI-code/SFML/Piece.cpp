#include "Piece.h"
#include "Square.h"

//Class functions

sf::Texture Piece::whiteBishopTexture_;
sf::Texture Piece::blackBishopTexture_;
sf::Texture Piece::whitePawnTexture_;
sf::Texture Piece::blackPawnTexture_;
sf::Texture Piece::whiteRookTexture_;
sf::Texture Piece::blackRookTexture_;
sf::Texture Piece::whiteKnightTexture_;
sf::Texture Piece::blackKnightTexture_;
sf::Texture Piece::whiteQueenTexture_;
sf::Texture Piece::blackQueenTexture_;
sf::Texture Piece::whiteKingTexture_;
sf::Texture Piece::blackKingTexture_;

Piece::Piece(const sf::Vector2f &squareSize, const sf::Vector2f &squarePos,
            const PieceType &newPieceType, const PieceColor &newPieceColor)
{
    setPieceTexture(newPieceColor, newPieceType);
    initSprite(squareSize);
    piece_.setPosition(squarePos);
}

void Piece::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(piece_, states);
}

void Piece::initSprite(sf::Vector2f squareSize)
{
    auto height = piece_.getGlobalBounds().height;
    auto width = piece_.getGlobalBounds().width;
    auto scaleX = squareSize.x / width;
    auto scaleY = squareSize.y / height;
    piece_.setScale(scaleX,scaleY);
}

void Piece::setToSquarePosition(const sf::Vector2f &currentPos)
{
    piece_.setPosition(currentPos);
}

void Piece::setPieceTexture(const PieceColor &color, const PieceType &newPieceType)
{
    switch (newPieceType)
    {
        case (PieceType::Bishop):
            setBishopTextures(color);
            break;
        case (PieceType::King):
            setKingTextures(color);
            break;
        case (PieceType::Knight):
            setKnightTextures(color);
            break;
        case (PieceType::Pawn):
            setPawnTextures(color);
            break;
        case (PieceType::Queen):
            setQueenTextures(color);
            break;
        case (PieceType::Rook):
            setRookTextures(color);
            break;
    }
}

void Piece::setPawnTextures(const PieceColor &color)
{
    if (color == PieceColor::White)
        piece_.setTexture(whitePawnTexture_);
    else
        piece_.setTexture(blackPawnTexture_);
}

void Piece::setRookTextures(const PieceColor &color)
{
    if (color == PieceColor::White)
        piece_.setTexture(whiteRookTexture_);
    else
        piece_.setTexture(blackRookTexture_);
}

void Piece::setKnightTextures(const PieceColor &color)
{
    if (color == PieceColor::White)
        piece_.setTexture(whiteKnightTexture_);
    else
        piece_.setTexture(blackKnightTexture_);
}

void Piece::setBishopTextures(const PieceColor &color)
{
    if (color == PieceColor::White)
        piece_.setTexture(whiteBishopTexture_);
    else
        piece_.setTexture(blackBishopTexture_);
}

void Piece::setQueenTextures(const PieceColor &color)
{
    if (color == PieceColor::White)
        piece_.setTexture(whiteQueenTexture_);
    else
        piece_.setTexture(blackQueenTexture_);
}

void Piece::setKingTextures(const PieceColor &color)
{
        if (color == PieceColor::White)
        piece_.setTexture(whiteKingTexture_);
    else
        piece_.setTexture(blackKingTexture_);
}


void Piece::loadPieceTextures(std::string imagePathName, PieceType pieceType, PieceColor color)
{
    switch (pieceType)
    {
        case PieceType::Pawn:
        {
            loadPawnTextures(color,imagePathName);
            break;
        }
        case PieceType::Rook:
        {
            loadRookTextures(color,imagePathName);
            break;
        }
        case PieceType::Knight:
        {
            loadKnightTextures(color,imagePathName);
            break;
        }
        case PieceType::Bishop:
        {
            loadBishopTextures(color,imagePathName);
            break;
        }
        case PieceType::Queen:
        {
            loadQueenTextures(color,imagePathName);
            break;
        }
        case PieceType::King:
        {
            loadKingTextures(color,imagePathName);
            break;
        }
        default:
            std::cerr << "NO PIECE TYPE GIVEN" << std::endl;
    }

}

void Piece::loadPawnTextures(const PieceColor &color, const std::string &imagePathName)
{
    if (color == PieceColor::White)
    {
        if (!whitePawnTexture_.loadFromFile(imagePathName))
        {
            throw WhitePawnImageCannotBeOpened{};
        }
    }
    else
    {
        if (!blackPawnTexture_.loadFromFile(imagePathName))
        {
            throw BlackPawnImageCannotBeOpened{};
        }
    }
}

void Piece::loadRookTextures(const PieceColor &color, const std::string &imagePathName)
{
    if (color == PieceColor::White)
    {
        if (!whiteRookTexture_.loadFromFile(imagePathName))
        {
            throw WhiteRookImageCannotBeOpened{};
        }
    }
    else
    {
        if (!blackRookTexture_.loadFromFile(imagePathName))
        {
            throw BlackRookImageCannotBeOpened{};
        }
    }
}

void Piece::loadKnightTextures(const PieceColor &color, const std::string &imagePathName)
{
   if (color == PieceColor::White)
    {
        if (!whiteKnightTexture_.loadFromFile(imagePathName))
        {
            throw WhiteKnightImageCannotBeOpened{};
        }
    }
    else
    {
        if (!blackKnightTexture_.loadFromFile(imagePathName))
        {
            throw BlackKnightImageCannotBeOpened{};
        }
    }
}

void Piece::loadBishopTextures(const PieceColor &color, const std::string &imagePathName)
{
   if (color == PieceColor::White)
    {
        if (!whiteBishopTexture_.loadFromFile(imagePathName))
        {
            throw WhiteBishopImageCannotBeOpened{};
        }
    }
    else
    {
        if (!blackBishopTexture_.loadFromFile(imagePathName))
        {
            throw BlackBishopImageCannotBeOpened{};
        }
    }
}

void Piece::loadQueenTextures(const PieceColor &color, const std::string &imagePathName)
{
    if (color == PieceColor::White)
    {
        if (!whiteQueenTexture_.loadFromFile(imagePathName))
        {
            throw WhiteQueenImageCannotBeOpened{};
        }
    }
    else
    {
        if (!blackQueenTexture_.loadFromFile(imagePathName))
        {
            throw BlackQueenImageCannotBeOpened{};
        }
    }
}

void Piece::loadKingTextures(const PieceColor &color, const std::string &imagePathName)
{
    if (color == PieceColor::White)
    {
        if (!whiteKingTexture_.loadFromFile(imagePathName))
        {
            throw WhiteKingImageCannotBeOpened{};
        }
    }
    else
    {
        if (!blackKingTexture_.loadFromFile(imagePathName))
        {
            throw BlackKingImageCannotBeOpened{};
        }
    }
}



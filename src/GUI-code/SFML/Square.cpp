#include "Square.h"

const auto GRAPHICS_BUFFER = 0.05f;
const auto BOARD_OFFSET = 30;

int Square::boardSize_;

Square::Square(sf::Color color, sf::Vector2f pos, sf::Vector2f squareSize, sf::Vector2i coordinate,
               SquareNames name, bool isLightSquare)
{
    square_.setPosition(pos);
    square_.setSize(squareSize);
    square_.setFillColor(color);
    originalColor_ = color;
    isHighlighted_ = false;
    coord_ = coordinate;
    squareName_ = name;
    isLightSquare_ = isLightSquare;
}

void Square::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(square_, states);
}

void Square::drawSquare(sf::RenderTarget& target, sf::RenderStates states) const
{
    draw(target,states);
}

void Square::drawPiece(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (piece_.size() == 1)
    {
        piece_[0]->draw(target, states);
    }
}

void Square::highlightToggle(sf::Color newColour)
{
    if (isHighlighted_)
    {
        dehighlight();
    }
    else
    {
        if (isLightSquare_)
        {
            newColour = newColour + sf::Color{10,10,10};
            square_.setFillColor(newColour);
        }
        else
        {
            square_.setFillColor(newColour);
        }
        isHighlighted_ = true;
    }

}

bool Square::pointWithinSquare(sf::Vector2i point)
{
    if ((point.x < square_.getGlobalBounds().left) ||
        (point.x > square_.getGlobalBounds().left + square_.getGlobalBounds().width))
    {
        return false;
    }
    if ((point.y < square_.getGlobalBounds().top) ||
        (point.y > square_.getGlobalBounds().top + square_.getGlobalBounds().height))
    {
        return false;
    }
    return true;

}

void Square::dehighlight()
{
    square_.setFillColor(originalColor_);
    isHighlighted_ = false;
}

void Square::invertFlip()
{
    auto left = coord_.x;
    auto top = coord_.y;
    //Transforms coordinates to flip board
    left = (left - (boardSize_ - 1))*(-1);
    top = (top - (boardSize_ - 1))*(-1);
    coord_.x = left;
    coord_.y = top;
    auto newPos = sf::Vector2f{static_cast<float>(BOARD_OFFSET+left*(square_.getGlobalBounds().width + GRAPHICS_BUFFER)),
                               static_cast<float>(BOARD_OFFSET+top*(square_.getGlobalBounds().height + GRAPHICS_BUFFER))};
    square_.setPosition(newPos);
    if (piece_.size() == 1)
    {
        piece_[0]->setToSquarePosition(sf::Vector2f{square_.getGlobalBounds().left,square_.getGlobalBounds().top});
    }
}

void Square::setBoardSize(int boardSize)
{
    boardSize_ = boardSize;
}

void Square::addPiece(const PieceType &newPieceType, const PieceColor &newPieceColor)
{
    piece_.clear();
    auto squareSize = sf::Vector2f{square_.getGlobalBounds().height, square_.getGlobalBounds().width};
    auto squarePos = square_.getPosition();
    piece_.push_back(std::make_shared<Piece>(squareSize, squarePos, newPieceType, newPieceColor));
}

void Square::addPiece(std::shared_ptr<Piece> newPiece)
{
    piece_.clear();
    piece_.push_back(newPiece);
    if (piece_.size() == 1)
    {
        piece_[0]->setToSquarePosition(sf::Vector2f{square_.getGlobalBounds().left,square_.getGlobalBounds().top});
    }
}

void Square::removePiece()
{
    piece_.clear();
}

bool Square::operator==(const SquareNames rhs) const
{
    if (squareName_ == rhs)
        return true;
    else
        return false;

}

void Square::givePiece(Square &newPieceSquare)
{
    if (piece_.size() == 1)
    {
        newPieceSquare.addPiece(piece_[0]);
        piece_.clear();
    }
}

void Square::dragPiece(sf::Vector2i mousePos)
{
    if (piece_.size() == 1)
    {
        auto newPos = sf::Vector2f{static_cast<float>(mousePos.x),static_cast<float>(mousePos.y)};
        piece_[0]->setToSquarePosition(newPos);
    }
}

bool Square::hasPiece() const
{
    if (piece_.size() == 1)
        return true;
    else
        return false;
}

void Square::centerPieceInSquare()
{
    if (piece_.size() == 1)
    {
        piece_[0]->setToSquarePosition(sf::Vector2f{square_.getGlobalBounds().left, square_.getGlobalBounds().top});
    }
}

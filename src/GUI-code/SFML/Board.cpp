#include "Board.h"

const auto ORIGIN = sf::Vector2f{0,0};
const auto SQUARE_SIZE = 75;
const auto GRAPHICS_BUFFER = 0.05;
const auto BOARD_OFFSET = 30;
const auto HIGHLIGHT_COLOR = sf::Color{200,50,50};
const auto CHAR_NUMBER_ASCII_OFFSET = 48;
const auto NO_SQUARE_INDEX = -1;

Board::Board(int boardSize, sf::Color topLeftCol, sf::Color topRightCol)
{
    boardSize_ = boardSize;
    Square::setBoardSize(boardSize);
    selectedSquareIndex_ = NO_SQUARE_INDEX;
    initSquares(topLeftCol, topRightCol);
    initTextures();
}

void Board::readFEN(std::string FEN)
{
    auto squareCount = 0;
    for (auto character : FEN)
    {
        if (squareCount >= boardSize_*boardSize_+1)
        {
            throw InvalidFEN{};
            return;
        }
        countFENSquares(character,squareCount);
        createFENPieces(character,squareCount);
    }
}

void Board::countFENSquares(const char &character, int &squareCount)
{
    if (squareCount >= boardSize_*boardSize_)
    {
        return;
    }
    if ((character >= '1') && (character <= '8'))
    {
        auto charNumber = static_cast<int>(character) - CHAR_NUMBER_ASCII_OFFSET;
        squareCount += charNumber;
    }
}

void Board::createFENPieces(const char &character, int &squareCount)
{
    if (squareCount >= boardSize_*boardSize_)
    {
        return;
    }
    switch (character)
    {
        case ('p'):
            squareList_[squareCount].addPiece(PieceType::Pawn,PieceColor::Black);
            squareCount++;
            break;
        case ('r'):
            squareList_[squareCount].addPiece(PieceType::Rook,PieceColor::Black);
            squareCount++;
            break;
        case ('n'):
            squareList_[squareCount].addPiece(PieceType::Knight,PieceColor::Black);
            squareCount++;
            break;
        case ('b'):
            squareList_[squareCount].addPiece(PieceType::Bishop,PieceColor::Black);
            squareCount++;
            break;
        case ('q'):
            squareList_[squareCount].addPiece(PieceType::Queen,PieceColor::Black);
            squareCount++;
            break;
        case ('k'):
            squareList_[squareCount].addPiece(PieceType::King,PieceColor::Black);
            squareCount++;
            break;
        case ('P'):
            squareList_[squareCount].addPiece(PieceType::Pawn,PieceColor::White);
            squareCount++;
            break;
        case ('R'):
            squareList_[squareCount].addPiece(PieceType::Rook,PieceColor::White);
            squareCount++;
            break;
        case ('N'):
            squareList_[squareCount].addPiece(PieceType::Knight,PieceColor::White);
            squareCount++;
            break;
        case ('B'):
            squareList_[squareCount].addPiece(PieceType::Bishop,PieceColor::White);
            squareCount++;
            break;
        case ('Q'):
            squareList_[squareCount].addPiece(PieceType::Queen,PieceColor::White);
            squareCount++;
            break;
        case ('K'):
            squareList_[squareCount].addPiece(PieceType::King,PieceColor::White);
            squareCount++;
            break;
        default:
            break;
    }
}

void Board::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (auto &square : squareList_)
    {
        square.drawSquare(target,states);
    }
    for (auto &square : squareList_)
    {
        square.drawPiece(target,states);
    }
    if (selectedSquareIndex_ != NO_SQUARE_INDEX)
    {
        squareList_[selectedSquareIndex_].drawPiece(target,states);
    }
}

void Board::initSquares(sf::Color topLeftCol, sf::Color topRightCol)
{
    auto pos = ORIGIN;
    auto isLightSquare = true;
    for (auto iCount = 0; iCount < boardSize_*boardSize_; iCount++)
    {
        auto coordinate = sf::Vector2i{iCount % boardSize_, iCount / boardSize_};
        pos = sf::Vector2f{static_cast<float>(BOARD_OFFSET+(SQUARE_SIZE+GRAPHICS_BUFFER)*(iCount%boardSize_)),
                           static_cast<float>(BOARD_OFFSET+(iCount/boardSize_)*(SQUARE_SIZE+GRAPHICS_BUFFER))};
        if ((iCount / boardSize_) % 2 == 0)
        {
            if (iCount % 2 == 0)
            {
                squareList_.push_back(Square{topLeftCol,pos,sf::Vector2f{SQUARE_SIZE,SQUARE_SIZE},
                                      coordinate,nameSquares(iCount),isLightSquare});
            }
            else
            {
                squareList_.push_back(Square{topRightCol,pos,sf::Vector2f{SQUARE_SIZE,SQUARE_SIZE},
                                      coordinate,nameSquares(iCount),!isLightSquare});
            }
        }
        else
        {
            if (iCount % 2 == 1)
            {
                squareList_.push_back(Square{topLeftCol,pos,sf::Vector2f{SQUARE_SIZE,SQUARE_SIZE},
                                      coordinate,nameSquares(iCount),isLightSquare});
            }
            else
            {
                squareList_.push_back(Square{topRightCol,pos,sf::Vector2f{SQUARE_SIZE,SQUARE_SIZE},
                                      coordinate,nameSquares(iCount),!isLightSquare});
            }
        }

    }
}

void Board::highlightSquare(sf::Vector2i mousePos)
{
    for (auto &squares : squareList_)
    {
        if (squares.pointWithinSquare(mousePos))
        {
            squares.highlightToggle(HIGHLIGHT_COLOR);
        }
    }
}

void Board::clearBoardHighlights()
{
    for (auto &squares : squareList_)
    {
        squares.dehighlight();
    }
}

void Board::flipBoard()
{
    for (auto &square : squareList_)
    {
        square.invertFlip();
    }
}

void Board::addPiece(const PieceType &newPieceType, const PieceColor &newPieceColor, const SquareNames &newSquareName)
{
    for (auto &square : squareList_)
    {
        if (square == newSquareName)
        {
            square.addPiece(newPieceType, newPieceColor);
        }
    }
}

void Board::selectSquare(sf::Vector2i mousePos)
{
    for (auto iCount = 0u; iCount < squareList_.size(); iCount++)
    {
        if (squareList_[iCount].pointWithinSquare(mousePos))
        {
            auto oldSquareIndex = NO_SQUARE_INDEX;
            if (selectedSquareIndex_ != NO_SQUARE_INDEX)
            {
                oldSquareIndex = selectedSquareIndex_;
            }
            selectedSquareIndex_ = iCount;
            if (oldSquareIndex == NO_SQUARE_INDEX)
            {
                if (!squareList_[selectedSquareIndex_].hasPiece())
                {
                    selectedSquareIndex_ = NO_SQUARE_INDEX;
                }
                return;
            }
            movePiece(nameSquares(oldSquareIndex),nameSquares(selectedSquareIndex_));
        }
    }
    squareList_[selectedSquareIndex_].centerPieceInSquare();
    selectedSquareIndex_ = NO_SQUARE_INDEX;
}

void Board::dragPiece(sf::Vector2i mousePos)
{
    if (selectedSquareIndex_ != NO_SQUARE_INDEX)
    {
        squareList_[selectedSquareIndex_].dragPiece(sf::Vector2i{mousePos.x-SQUARE_SIZE/2,mousePos.y-SQUARE_SIZE/2});
    }
}

void Board::initTextures()
{
    Piece::loadPieceTextures("resources\\wP.png", PieceType::Pawn, PieceColor::White);
    Piece::loadPieceTextures("resources\\bP.png", PieceType::Pawn, PieceColor::Black);
    Piece::loadPieceTextures("resources\\wR.png", PieceType::Rook, PieceColor::White);
    Piece::loadPieceTextures("resources\\bR.png", PieceType::Rook, PieceColor::Black);
    Piece::loadPieceTextures("resources\\wN.png", PieceType::Knight, PieceColor::White);
    Piece::loadPieceTextures("resources\\bN.png", PieceType::Knight, PieceColor::Black);
    Piece::loadPieceTextures("resources\\wB.png", PieceType::Bishop, PieceColor::White);
    Piece::loadPieceTextures("resources\\bB.png", PieceType::Bishop, PieceColor::Black);
    Piece::loadPieceTextures("resources\\wQ.png", PieceType::Queen, PieceColor::White);
    Piece::loadPieceTextures("resources\\bQ.png", PieceType::Queen, PieceColor::Black);
    Piece::loadPieceTextures("resources\\wK.png", PieceType::King, PieceColor::White);
    Piece::loadPieceTextures("resources\\bK.png", PieceType::King, PieceColor::Black);
}

void Board::movePiece(const SquareNames &oldSquareName, const SquareNames &newSquareName)
{
    for (auto &oldSquare : squareList_)
    {
        if (oldSquare == oldSquareName)
        {
            for (auto &newSquare : squareList_)
            {
                if (newSquare == newSquareName)
                {
                    if (oldSquareName != newSquareName)
                        oldSquare.givePiece(newSquare);
                    else
                        oldSquare.centerPieceInSquare();
                }
            }
        }
    }
}

SquareNames Board::nameSquares(int squareIndex)
{
    switch (squareIndex)
    {
        case 0:
            return SquareNames::a8;
        case 1:
            return SquareNames::b8;
        case 2:
            return SquareNames::c8;
        case 3:
            return SquareNames::d8;
        case 4:
            return SquareNames::e8;
        case 5:
            return SquareNames::f8;
        case 6:
            return SquareNames::g8;
        case 7:
            return SquareNames::h8;
        case 8:
            return SquareNames::a7;
        case 9:
            return SquareNames::b7;
        case 10:
            return SquareNames::c7;
        case 11:
            return SquareNames::d7;
        case 12:
            return SquareNames::e7;
        case 13:
            return SquareNames::f7;
        case 14:
            return SquareNames::g7;
        case 15:
            return SquareNames::h7;
        case 16:
            return SquareNames::a6;
        case 17:
            return SquareNames::b6;
        case 18:
            return SquareNames::c6;
        case 19:
            return SquareNames::d6;
        case 20:
            return SquareNames::e6;
        case 21:
            return SquareNames::f6;
        case 22:
            return SquareNames::g6;
        case 23:
            return SquareNames::h6;
        case 24:
            return SquareNames::a5;
        case 25:
            return SquareNames::b5;
        case 26:
            return SquareNames::c5;
        case 27:
            return SquareNames::d5;
        case 28:
            return SquareNames::e5;
        case 29:
            return SquareNames::f5;
        case 30:
            return SquareNames::g5;
        case 31:
            return SquareNames::h5;
        case 32:
            return SquareNames::a4;
        case 33:
            return SquareNames::b4;
        case 34:
            return SquareNames::c4;
        case 35:
            return SquareNames::d4;
        case 36:
            return SquareNames::e4;
        case 37:
            return SquareNames::f4;
        case 38:
            return SquareNames::g4;
        case 39:
            return SquareNames::h4;
        case 40:
            return SquareNames::a3;
        case 41:
            return SquareNames::b3;
        case 42:
            return SquareNames::c3;
        case 43:
            return SquareNames::d3;
        case 44:
            return SquareNames::e3;
        case 45:
            return SquareNames::f3;
        case 46:
            return SquareNames::g3;
        case 47:
            return SquareNames::h3;
        case 48:
            return SquareNames::a2;
        case 49:
            return SquareNames::b2;
        case 50:
            return SquareNames::c2;
        case 51:
            return SquareNames::d2;
        case 52:
            return SquareNames::e2;
        case 53:
            return SquareNames::f2;
        case 54:
            return SquareNames::g2;
        case 55:
            return SquareNames::h2;
        case 56:
            return SquareNames::a1;
        case 57:
            return SquareNames::b1;
        case 58:
            return SquareNames::c1;
        case 59:
            return SquareNames::d1;
        case 60:
            return SquareNames::e1;
        case 61:
            return SquareNames::f1;
        case 62:
            return SquareNames::g1;
        case 63:
            return SquareNames::h1;
        default:
            return SquareNames::NO_NAME;
    }
}


#include <array>
#include <unordered_map>

#include "Board.hpp"

namespace
{
enum class BoardInput
{
   EmptySpace = 'e',
   Wall = 'x',

   HeadUp = 'U',
   HeadRight = 'R',
   HeadDown = 'D',
   HeadLeft = 'L',

   BodyUp = '^',
   BodyRight = '>',
   BodyDown = 'V',
   BodyLeft = '<'
};

const Location ORIGIN{ 0, 0 };
const std::array<Snake::SnakePart, 2> PARTS = { Snake::SnakePart::Head, Snake::SnakePart::Tail };
const std::array<Direction, 4> DIRECTIONS = { Direction::Up, Direction::Right, Direction::Down, Direction::Left };
const int ASCII_ZERO = 48;

constexpr Direction NextDirection(const BoardInput aInput)
{
   switch (aInput)
   {
   case BoardInput::HeadUp:
   case BoardInput::BodyUp:
      return Direction::Up;
      break;

   case BoardInput::HeadRight:
   case BoardInput::BodyRight:
      return Direction::Right;
      break;

   case BoardInput::HeadDown:
   case BoardInput::BodyDown:
      return Direction::Down;
      break;

   case BoardInput::HeadLeft:
   case BoardInput::BodyLeft:
      return Direction::Left;
      break;

   default:
      return Direction::Null;
      break;
   }
}
}

bool Board::Move::operator==(const Board::Move& aRhs) const
{
   return mSnakeIdx == aRhs.mSnakeIdx
      && mDirection == aRhs.mDirection
      && mSnakePart == aRhs.mSnakePart;
}

bool Board::IsLocationEmpty(const Location& aLocation) const
{
   // check snakes
   for (const auto& snake : mSnakes)
   {
      if (snake.OccupiesLocation(aLocation))
      {
         return false;
      }
   }

   // check walls
   return mWalls.count(aLocation) == 0;
}

bool Board::IsLocationInside(const Location& aLocation) const
{
   int x = aLocation.GetX();
   int y = aLocation.GetY();
   return x >= 0 && x < mSize.GetX() && y >= 0 && y < mSize.GetY();
}

const Location& Board::GetSnakePartLocation(const int aSnakeIdx, const Snake::SnakePart aSnakePart) const
{
   return mSnakes[aSnakeIdx].GetPartLocation(aSnakePart);
}

bool Board::IsSolved() const
{
   return mSnakes[0].OccupiesLocation(mExit);
}

std::vector<Board::Move> Board::LegalMoves() const
{
   std::vector<Move> validMoves;
   validMoves.reserve(2 * 3 * mSnakes.size());

   for (const auto& snake : mSnakes)
   {
      for (const auto part : PARTS)
      {
         for (const auto direction : DIRECTIONS)
         {
            Location tryLoc = snake.GetPartLocation(part).Nudge(direction);
            if (IsLocationEmpty(tryLoc) && IsLocationInside(tryLoc))
            {
               validMoves.push_back({ snake.GetIdx(), part, direction });
            }
         }
      }
   }

   return validMoves;
}

void Board::MakeMove(const Move& aMove)
{
   mSnakes[aMove.mSnakeIdx].MakeMove(aMove.mSnakePart, aMove.mDirection);
}

size_t Board::Hash() const
{
   size_t hash = 0;
   for (const auto& snake : mSnakes)
   {
      hash = snake.Hash() ^ (hash << 12);
   }

   return hash;
}

bool Board::operator==(const Board& aRhs) const
{
   return mSnakes == aRhs.mSnakes;
}

void Board::Builder::FromStream(std::istream& aIn)
{
   int width;
   int height;
   int numSnakes;
   char input;
   std::unordered_map<Location, char> textBoard;

   auto TraceSnake = [&textBoard](const Location& aHead) -> Snake
   {
      Snake::Builder builder;
      Location loc = aHead;
      builder.SetHead(loc);
      auto input = static_cast<BoardInput>(textBoard.at(loc));
      Direction nextDirection = NextDirection(input);

      while (nextDirection != Direction::Null)
      {
         builder.AddSegment(nextDirection);
         loc = loc.Nudge(nextDirection);
         input = static_cast<BoardInput>(textBoard.at(loc));
         nextDirection = NextDirection(input);
      }

      builder.SetIndex(static_cast<int>(textBoard.at(loc)) - ASCII_ZERO);
      return builder.Build();
   };

   aIn >> width;
   aIn >> height;
   mBoardPtr->mSize = { width, height };
   mBoardPtr->mExit = { width - 1, height - 1 };

   aIn >> numSnakes;
   mBoardPtr->mSnakes.resize(numSnakes);

   for (int j = 0; j < height; ++j)
   {
      for (int i = 0; i < width; ++i)
      {
         aIn >> input;
         textBoard[{ i, j }] = input;
      }
   }

   for (int i = 0; i < width; ++i)
   {
      for (int j = 0; j < height; ++j)
      {
         Location loc{ i, j };
         Snake snake;

         switch (static_cast<BoardInput>(textBoard[loc]))
         {
         case BoardInput::Wall:
            mBoardPtr->mWalls.insert(loc);
            break;

         case BoardInput::HeadUp:
         case BoardInput::HeadRight:
         case BoardInput::HeadDown:
         case BoardInput::HeadLeft:
            snake = TraceSnake(loc);
            mBoardPtr->mSnakes[snake.GetIdx()] = snake;

         default: // ignore all others
            break;
         }
      }
   }

}

Board Board::Builder::Build()
{
   return *mBoardPtr;
}

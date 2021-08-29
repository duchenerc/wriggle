
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
   return aLocation >= ORIGIN && aLocation < mSize;
}

std::unique_ptr<std::list<Board::Move>> Board::LegalMoves() const
{
   std::unique_ptr<std::list<Move>> validMoves;

   for (const auto& snake : mSnakes)
   {
      for (const auto part : PARTS)
      {
         for (const auto direction : DIRECTIONS)
         {
            Location tryLoc = snake.GetPartLocation(part).Nudge(direction);
            if (IsLocationEmpty(tryLoc) && IsLocationInside(tryLoc))
            {
               validMoves->push_back({ snake.GetIdx(), part, direction });
            }
         }
      }
   }

   return std::move(validMoves);
}

void Board::MakeMove(const Move& aMove)
{
   mSnakes[aMove.mSnakeIdx].MakeMove(aMove.mSnakePart, aMove.mDirection);
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
      return *builder.Build();
   };

   aIn >> width;
   aIn >> height;
   mBoardPtr->mSize = { width, height };

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

std::unique_ptr<Board> Board::Builder::Build()
{
   return std::move(mBoardPtr);
}

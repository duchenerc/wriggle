
#ifndef PUZZLEFILE_HPP
#define PUZZLEFILE_HPP

#include <iostream>
#include <list>
#include <memory>
#include <unordered_set>
#include <vector>

#include "Location.hpp"
#include "Snake.hpp"

class Board
{
public:

   struct Move
   {
      int mSnakeIdx;
      Snake::SnakePart mSnakePart;
      Direction mDirection;

      bool operator==(const Move& aRhs) const;
   };

   Board() = default;
   Board(const Board& aRhs) = default;

   bool IsLocationEmpty(const Location& aLocation) const;
   bool IsLocationInside(const Location& aLocation) const;
   bool IsLocationOccupiedBySnake(const Location& aLocation) const;
   bool IsLocationOccupiedByWall(const Location& aLocation) const;

   const Location& GetSnakePartLocation(const int aSnakeIdx, const Snake::SnakePart aSnakePart) const;
   const Location& GetExitLocation() const;

   bool IsSolved() const;

   std::vector<Move> LegalMoves() const;
   void MakeMove(const Move& aMove);

   size_t Hash() const;

   bool operator==(const Board& aRhs) const;

   void PrintToStream(std::ostream& aOut) const;

private:
   Location mSize;
   Location mExit;
   std::vector<Snake> mSnakes;
   std::unordered_set<Location> mWalls;

public:
   class Builder
   {
   public:
      Builder()
         : mBoardPtr{ std::make_unique<Board>() }
      {}

      Board Build();
      void FromStream(std::istream& aIn);

   private:
      std::unique_ptr<Board> mBoardPtr;
   };
};

namespace std
{
template<>
struct hash<Board>
{
   size_t operator()(Board const& aRhs) const
   {
      return aRhs.Hash();
   }
};

template<>
struct hash<Board::Move>
{
   size_t operator()(Board::Move const& aRhs) const
   {
      return aRhs.mSnakeIdx ^ (static_cast<int>(aRhs.mSnakePart) << 3) ^ (static_cast<int>(aRhs.mDirection) << 4);
   }
};
}

#endif

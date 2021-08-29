
#ifndef PUZZLEFILE_HPP
#define PUZZLEFILE_HPP

#include <iostream>
#include <list>
#include <memory>
#include <set>
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
   };

   Board() = default;

   bool IsLocationEmpty(const Location& aLocation) const;
   bool IsLocationInside(const Location& aLocation) const;

   std::unique_ptr<std::list<Move>> LegalMoves() const;
   void MakeMove(const Move& aMove);

private:
   Location mSize;
   std::vector<Snake> mSnakes;
   std::set<Location> mWalls;

public:
   class Builder
   {
   public:
      Builder()
         : mBoardPtr{ std::make_unique<Board>() }
      {}

      std::unique_ptr<Board> Build();
      void FromStream(std::istream& aIn);

   private:
      std::unique_ptr<Board> mBoardPtr;
   };
};

#endif

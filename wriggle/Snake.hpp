
#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <deque>
#include <memory>

#include "Location.hpp"

class Snake
{
public:
   enum class SnakePart
   {
      Head,
      Tail
   };

   Snake() = default;

   int GetIdx() const { return mIdx; }
   bool OccupiesLocation(const Location& aLocation) const;
   const Location& GetPartLocation(const SnakePart aPart) const;

   void MakeMove(const SnakePart aPart, const Direction aDirection);

   size_t Hash() const;

   bool operator==(const Snake& aRhs) const;

private:
   std::deque<Location> mBody;
   int mIdx = 0;

public:
   class Builder
   {
   public:
      Builder()
         : mSnakePtr{ std::make_unique<Snake>() }
      {}

      void SetHead(const Location& aLoc);
      void AddSegment(const Direction aDirection);
      void SetIndex(const int aIdx);
      Snake Build();

   private:
      std::unique_ptr<Snake> mSnakePtr;
   };
};

namespace std
{
template<>
struct hash<Snake>
{
   size_t operator()(Snake const& aRhs) const
   {
      return aRhs.Hash();
   }
};
}

#endif

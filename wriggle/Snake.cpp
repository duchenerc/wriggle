
#include "Snake.hpp"

bool Snake::OccupiesLocation(const Location& aLocation) const
{
   for (const auto& loc : mBody)
   {
      if (loc == aLocation)
      {
         return true;
      }
   }
   return false;
}

const Location& Snake::GetPartLocation(const SnakePart aPart) const
{
   if (aPart == SnakePart::Head)
   {
      return mBody.front();
   }
   else // if aPart == SnakePart::Tail
   {
      return mBody.back();
   }
}

void Snake::MakeMove(const Snake::SnakePart aPart, const Direction aDirection)
{
   const Location& partMoving = GetPartLocation(aPart);
   Location newPart = partMoving.Nudge(aDirection);
   if (aPart == SnakePart::Head)
   {
      mBody.push_front(newPart);
      mBody.pop_back();
   }
   else // if aPart == SnakePart::Tail
   {
      mBody.push_back(newPart);
      mBody.pop_front();
   }
}

void Snake::Builder::SetHead(const Location& aLocation)
{
   if (!mSnakePtr->mBody.empty())
   {
      mSnakePtr->mBody.clear();
   }
   mSnakePtr->mBody.emplace_back(aLocation);
}

void Snake::Builder::AddSegment(const Direction aDirection)
{
   mSnakePtr->mBody.emplace_back(mSnakePtr->mBody.back().Nudge(aDirection));
}

void Snake::Builder::SetIndex(const int aIdx)
{
   mSnakePtr->mIdx = aIdx;
}

std::unique_ptr<Snake> Snake::Builder::Build()
{
   return std::move(mSnakePtr);
}

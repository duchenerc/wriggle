
#include "Location.hpp"

const Location Location::Up    = {  0, -1 };
const Location Location::Right = {  1,  0 };
const Location Location::Down  = {  0,  1 };
const Location Location::Left  = { -1,  0 };

Location Location::operator+(const Location& aRhs) const
{
   return Location{ mX + aRhs.mX, mY + aRhs.mY };
}

Location Location::operator-(const Location& aRhs) const
{
   return Location{ mX - aRhs.mX, mY - aRhs.mY };
}

Location& Location::operator+=(const Location& aRhs)
{
   mX += aRhs.mX;
   mY += aRhs.mY;
   return *this;
}

Location& Location::operator-=(const Location& aRhs)
{
   mX -= aRhs.mX;
   mY -= aRhs.mY;
   return *this;
}

bool Location::operator==(const Location& aRhs) const
{
   return mX == aRhs.mX && mY == aRhs.mY;
}

bool Location::operator!=(const Location& aRhs) const
{
   return mX != aRhs.mX || mY != aRhs.mY;
}

Location Location::Nudge(const Direction aDir) const
{
   switch (aDir)
   {
   case Direction::Up:
      return *this + Location::Up;
      break;
   case Direction::Right:
      return *this + Location::Right;
      break;
   case Direction::Down:
      return *this + Location::Down;
      break;
   case Direction::Left:
   default:
      return *this + Location::Left;
      break;
   }
}

int Location::Taxicab() const
{
   return std::abs(mX + mY);
}

size_t Location::Hash() const
{
   return std::hash<int>{}(mX ^ (mY << 3));
}

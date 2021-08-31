
#include "Location.hpp"

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
      return *this + Location{ 0, -1 };
      break;
   case Direction::Right:
      return *this + Location{ 1, 0 };
      break;
   case Direction::Down:
      return *this + Location{ 0, 1 };
      break;
   case Direction::Left:
   default:
      return *this + Location{ -1, 0 };
      break;
   }
}

size_t Location::Hash() const
{
   return mX ^ (mY << 3);
}


#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <algorithm>

enum class Direction
{
   Up,
   Right,
   Down,
   Left,
   Null
};

class Location
{
public:
   Location() = default;
   Location(const int aX, const int aY) : mX{ aX }, mY{ aY } {};

   static const Location Up;
   static const Location Right;
   static const Location Down;
   static const Location Left;

   int GetX() const
   {
      return mX;
   }

   int GetY() const
   {
      return mY;
   }

   void SetX(const int aX)
   {
      mX = aX;
   }

   void SetY(const int aY)
   {
      mY = aY;
   }

   Location operator+(const Location& aRhs) const;
   Location operator-(const Location& aRhs) const;
   Location& operator+=(const Location& aRhs);
   Location& operator-=(const Location& aRhs);
   
   bool operator==(const Location& aRhs) const;
   bool operator!=(const Location& aRhs) const;

   Location Nudge(const Direction aDir) const;
   size_t Hash() const;

private:
   int mX;
   int mY;
};

namespace std
{
template<>
struct hash<Location>
{
   size_t operator()(Location const& aLoc) const noexcept
   {
      return aLoc.Hash();
   }
};
}

#endif

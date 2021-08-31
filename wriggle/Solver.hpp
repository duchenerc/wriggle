
#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <chrono>
#include <iostream>
#include <list>
#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "Board.hpp"

class Solver
{
public:

   using wall_time = std::chrono::nanoseconds;

   Solver() = delete;
   Solver(const Board& aInitial)
      : mInitialPtr{ std::make_unique<Board>(aInitial) }
   {}

   wall_time GetWallTime() const
   {
      return mWallTime;
   }

   const std::list<Board::Move>* GetSolutionMoves() const
   {
      return &mMoves;
   }

   const Board* GetSolutionBoard() const
   {
      return mSolvedPtr.get();
   }

   bool IsSolved() const
   {
      return mSolved;
   }

   void Exec();
   void PrintToStream(std::ostream& aOut) const;

   virtual void Solve() = 0;

protected:
   
   class SearchNode
   {
   public:
      SearchNode(SearchNode* aParentPtr, const Board::Move& aMove, Board& aBoard)
         : mParentPtr{ aParentPtr }
         , mParentMove{ aMove }
         , mBoardPtr{ std::make_unique<Board>(aBoard) }
         , mDepth{ mParentPtr ? mParentPtr->mDepth + 1 : 0 }
      {}

      SearchNode* mParentPtr;
      Board::Move mParentMove;
      std::unique_ptr<Board> mBoardPtr;
      int mDepth;
      std::unordered_map<Board::Move, std::unique_ptr<SearchNode>> mChildren;
   };

   std::unique_ptr<Board> mInitialPtr;
   std::list<Board::Move> mMoves;
   std::unique_ptr<Board> mSolvedPtr;
   bool mSolved = false;

private:
   wall_time mWallTime;
};

class BreadthFirstTreeSearchSolver : public Solver
{
public:
   BreadthFirstTreeSearchSolver() = delete;
   BreadthFirstTreeSearchSolver(const Board& aInitial)
      : Solver{ aInitial }
   {}

   virtual ~BreadthFirstTreeSearchSolver() = default;

   void Solve() override;

private:
   std::unique_ptr<SearchNode> mInitialNodePtr;
   std::list<SearchNode*> mFrontier;
   std::unordered_set<Board> mExplored;
};

class IterativeDeepeningDepthFirstTreeSearchSolver : public Solver
{
public:
   IterativeDeepeningDepthFirstTreeSearchSolver() = delete;
   IterativeDeepeningDepthFirstTreeSearchSolver(const Board& aInitial)
      : Solver{ aInitial }
   {}

   virtual ~IterativeDeepeningDepthFirstTreeSearchSolver() = default;

   void Solve() override;
   
private:
   SearchNode* SolveToDepth(const int aMaxDepth);

   std::unique_ptr<SearchNode> mInitialNodePtr;
   std::stack<SearchNode*> mFrontier;
   std::unordered_set<Board> mExplored;
};

#endif

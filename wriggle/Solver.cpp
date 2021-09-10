
#include "Solver.hpp"

#include <array>
#include <iostream>

void Solver::Exec()
{
   using std::chrono::system_clock;
   system_clock::time_point start = system_clock::now();
   this->Solve();
   system_clock::time_point end = system_clock::now();
   mWallTime = std::chrono::duration_cast<wall_time>(end - start);
}

void Solver::PrintToStream(std::ostream& aOut) const
{
   Board board = *mInitialPtr;
   Location moveLoc;
   int numMoves = 0;

   // print moves
   for (const auto& move : mMoves)
   {
      ++numMoves;
      board.MakeMove(move);
      moveLoc = board.GetSnakePartLocation(move.mSnakeIdx, move.mSnakePart);
      aOut << move.mSnakeIdx << " "
         << static_cast<int>(move.mSnakePart) << " "
         << moveLoc.GetX() << " " << moveLoc.GetY() << std::endl;
   }

   // print solved board
   board.PrintToStream(aOut);

   // print wall time
   aOut << mWallTime.count() << std::endl;

   // print number moves
   aOut << numMoves << std::endl;
}

void BreadthFirstTreeSearchSolver::Solve()
{
   mInitialNodePtr = std::make_unique<SearchNode>(nullptr, Board::Move{}, *mInitialPtr);
   mFrontier.push_back(mInitialNodePtr.get());

   SearchNode* currentPtr = nullptr;
   while (!mFrontier.empty())
   {
      currentPtr = mFrontier.front();
      mFrontier.pop_front();

      if (currentPtr->mBoardPtr->IsSolved())
      {
         mSolved = true;
         break;
      }
      else if (mExplored.count(*currentPtr->mBoardPtr))
      {
         continue;
      }

      mExplored.insert(*currentPtr->mBoardPtr);
      std::vector<Board::Move> movesFromCurrent = currentPtr->mBoardPtr->LegalMoves();
      for (const auto& move : movesFromCurrent)
      {
         auto nextNode = std::make_unique<SearchNode>(currentPtr, move, *currentPtr->mBoardPtr);
         nextNode->mBoardPtr->MakeMove(move);
         mFrontier.push_back(nextNode.get());
         currentPtr->mChildren[move] = std::move(nextNode);
      }
   }

   if (mSolved && currentPtr)
   {
      mSolvedPtr = std::make_unique<Board>(*currentPtr->mBoardPtr);
      while (currentPtr->mParentPtr)
      {
         mMoves.push_front(currentPtr->mParentMove);
         currentPtr = currentPtr->mParentPtr;
      }
   }
}

void IterativeDeepeningDepthFirstTreeSearchSolver::Solve()
{
   int maxDepth = 0;
   SearchNode* solutionPtr = nullptr;
   while (!mSolved)
   {
      solutionPtr = SolveToDepth(maxDepth);
      ++maxDepth;
      mExplored.clear();
   }

   if (mSolved && solutionPtr)
   {
      mSolvedPtr = std::make_unique<Board>(*solutionPtr->mBoardPtr);
      while (solutionPtr->mParentPtr)
      {
         mMoves.push_front(solutionPtr->mParentMove);
         solutionPtr = solutionPtr->mParentPtr;
      }
   }
}

Solver::SearchNode* IterativeDeepeningDepthFirstTreeSearchSolver::SolveToDepth(const int aMaxDepth)
{
   mInitialNodePtr = std::make_unique<SearchNode>(nullptr, Board::Move{}, *mInitialPtr);
   mFrontier.push(mInitialNodePtr.get());

   SearchNode* currentPtr = nullptr;
   while (!mFrontier.empty())
   {
      currentPtr = mFrontier.top();
      mFrontier.pop();

      if (currentPtr->mBoardPtr->IsSolved())
      {
         mSolved = true;
         return currentPtr;
      }
      else if (mExplored.count(*currentPtr->mBoardPtr))
      {
         // this node has already been generated
         continue;
      }

      mExplored.insert(*currentPtr->mBoardPtr);

      if (currentPtr->mDepth >= aMaxDepth)
      {
         // this node is at the depth limit, don't generate children
         continue;
      }

      std::vector<Board::Move> movesFromCurrent = currentPtr->mBoardPtr->LegalMoves();
      for (const auto& move : movesFromCurrent)
      {
         auto nextNode = std::make_unique<SearchNode>(currentPtr, move, *currentPtr->mBoardPtr);
         nextNode->mBoardPtr->MakeMove(move);
         mFrontier.push(nextNode.get());
         currentPtr->mChildren[move] = std::move(nextNode);
      }
   }

   return nullptr;
}

void GreedyBestFirstGraphSearchSolver::Solve()
{
   mInitialNodePtr = std::make_unique<SearchNode>(nullptr, Board::Move{}, *mInitialPtr);
   mFrontier.push(mInitialNodePtr.get());

   SearchNode* currentPtr = nullptr;
   while (!mFrontier.empty())
   {
      currentPtr = mFrontier.top();
      mFrontier.pop();

      if (currentPtr->mBoardPtr->IsSolved())
      {
         mSolved = true;
         break;
      }
      else if (mExplored.count(*currentPtr->mBoardPtr))
      {
         continue;
      }

      mExplored.insert(*currentPtr->mBoardPtr);
      std::vector<Board::Move> movesFromCurrent = currentPtr->mBoardPtr->LegalMoves();
      for (const auto& move : movesFromCurrent)
      {
         auto nextNode = std::make_unique<SearchNode>(currentPtr, move, *currentPtr->mBoardPtr);
         nextNode->mBoardPtr->MakeMove(move);
         mFrontier.push(nextNode.get());
         currentPtr->mChildren[move] = std::move(nextNode);
      }
   }

   if (mSolved && currentPtr)
   {
      mSolvedPtr = std::make_unique<Board>(*currentPtr->mBoardPtr);
      while (currentPtr->mParentPtr)
      {
         mMoves.push_front(currentPtr->mParentMove);
         currentPtr = dynamic_cast<GreedyBestFirstGraphSearchSolver::SearchNode*>(currentPtr->mParentPtr);
      }
   }
}

int GreedyBestFirstGraphSearchSolver::Heuristic(const GreedyBestFirstGraphSearchSolver::SearchNode* aSearchNodePtr)
{
   // simple heuristic:
   // taxicab distance to exit for 0-snake
   const Location& exitLoc = aSearchNodePtr->mBoardPtr->GetExitLocation();
   Location diffHead = exitLoc - aSearchNodePtr->mBoardPtr->GetSnakePartLocation(0, Snake::SnakePart::Head);
   Location diffTail = exitLoc - aSearchNodePtr->mBoardPtr->GetSnakePartLocation(0, Snake::SnakePart::Tail);

   return std::min(diffHead.Taxicab(), diffTail.Taxicab());
}

void AStarSolver::Solve()
{
   mInitialNodePtr = std::make_unique<SearchNode>(nullptr, Board::Move{}, *mInitialPtr);
   mFrontier.push(mInitialNodePtr.get());

   SearchNode* currentPtr = nullptr;
   while (!mFrontier.empty())
   {
      currentPtr = mFrontier.top();
      mFrontier.pop();

      if (currentPtr->mBoardPtr->IsSolved())
      {
         mSolved = true;
         break;
      }
      else if (mExplored.count(*currentPtr->mBoardPtr))
      {
         continue;
      }

      mExplored.insert(*currentPtr->mBoardPtr);
      std::vector<Board::Move> movesFromCurrent = currentPtr->mBoardPtr->LegalMoves();
      for (const auto& move : movesFromCurrent)
      {
         auto nextNode = std::make_unique<SearchNode>(currentPtr, move, *currentPtr->mBoardPtr);
         nextNode->mBoardPtr->MakeMove(move);
         mFrontier.push(nextNode.get());
         currentPtr->mChildren[move] = std::move(nextNode);
      }
   }

   if (mSolved && currentPtr)
   {
      mSolvedPtr = std::make_unique<Board>(*currentPtr->mBoardPtr);
      while (currentPtr->mParentPtr)
      {
         mMoves.push_front(currentPtr->mParentMove);
         currentPtr = dynamic_cast<AStarSolver::SearchNode*>(currentPtr->mParentPtr);
      }
   }
}

int AStarSolver::Heuristic(const AStarSolver::SearchNode* aSearchNodePtr)
{
   // slightly more advanced heuristic:
   // taxicab distance to exit for 0-snake + terrain penalty for adjacent spaces (1 for other snakes, 2 for walls)
   const Location& exitLoc = aSearchNodePtr->mBoardPtr->GetExitLocation();
   const Location& headLoc = aSearchNodePtr->mBoardPtr->GetSnakePartLocation(0, Snake::SnakePart::Head);
   const Location& tailLoc = aSearchNodePtr->mBoardPtr->GetSnakePartLocation(0, Snake::SnakePart::Tail);

   auto SnakePartPenalty = [aSearchNodePtr](const Location& aLoc) -> int
   {
      const std::array<Location, 2> downright = { aLoc.Nudge(Direction::Down), aLoc.Nudge(Direction::Right) };
      int penalty = 2;

      for (const auto& next : downright)
      {
         if (!aSearchNodePtr->mBoardPtr->IsLocationInside(next))
         {
            continue;
         }
         else if (aSearchNodePtr->mBoardPtr->IsLocationOccupiedBySnake(next))
         {
            penalty = std::min(penalty, 1);
         }
         else if (aSearchNodePtr->mBoardPtr->IsLocationEmpty(next))
         {
            penalty = 0;
         }
      }
      return penalty;
   };

   int costHead = (exitLoc - headLoc).Taxicab() + SnakePartPenalty(headLoc);
   int costTail = (exitLoc - tailLoc).Taxicab() + SnakePartPenalty(tailLoc);
   int subtotalCost = std::min(costHead, costTail);
   
   const Location& closerPart = subtotalCost == costHead ? headLoc : tailLoc;

   // adds penalty of 1 for each layer of blockage in front of the exit
   auto BlockedExitPenalty = [aSearchNodePtr, closerPart, exitLoc]() -> int
   {
      int totalPenalty = 0;
      int sweepPenalty = 0;
      int nudge = 0;
      Location checkLoc;

      while (nudge < exitLoc.Taxicab())
      {
         sweepPenalty = 1;
         nudge++;
         checkLoc = exitLoc - Location{ nudge, 0 };

         while (checkLoc.GetX() <= exitLoc.GetX())
         {
            if (checkLoc == closerPart)
            {
               // we reached the closer part of the goal snake, immediately return result
               return totalPenalty;
            }
            if (aSearchNodePtr->mBoardPtr->IsLocationEmpty(checkLoc) && aSearchNodePtr->mBoardPtr->IsLocationInside(checkLoc))
            {
               // this location is empty, sweep penalty is zero
               sweepPenalty = 0;
            }

            checkLoc += Location::Up + Location::Right;
         }

         totalPenalty += sweepPenalty;
         if (sweepPenalty == 0)
         {
            break;
         }
      }

      return totalPenalty;
   };

   return subtotalCost + BlockedExitPenalty();
}

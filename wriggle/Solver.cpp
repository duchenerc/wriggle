
#include "Solver.hpp"

#include <array>

void Solver::Exec()
{
   using std::chrono::system_clock;
   system_clock::time_point start = system_clock::now();
   this->Solve();
   system_clock::time_point end = system_clock::now();
   auto elapsed = std::chrono::duration_cast<wall_time>(end - start);

   mWallTime = elapsed;
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
         currentPtr = static_cast<GreedyBestFirstGraphSearchSolver::SearchNode*>(currentPtr->mParentPtr);
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
   mFrontier.insert({ mInitialNodePtr->mHeuristicScore, mInitialNodePtr.get() });

   SearchNode* currentPtr = nullptr;
   std::multimap<int, SearchNode*, std::less<int>, std::allocator<FrontierItem>>::iterator currentIt;
   while (!mFrontier.empty())
   {
      currentIt = mFrontier.begin();
      currentPtr = currentIt->second;
      mFrontier.erase(currentIt);

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
         mFrontier.insert({ nextNode->mHeuristicScore, nextNode.get() });
         currentPtr->mChildren[move] = std::move(nextNode);
      }
   }

   if (mSolved && currentPtr)
   {
      mSolvedPtr = std::make_unique<Board>(*currentPtr->mBoardPtr);
      while (currentPtr->mParentPtr)
      {
         mMoves.push_front(currentPtr->mParentMove);
         currentPtr = static_cast<AStarSolver::SearchNode*>(currentPtr->mParentPtr);
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

   auto FindExtraCost = [aSearchNodePtr](const Location& aLoc) -> int
   {
      const std::array<Location, 2> downright = { aLoc.Nudge(Direction::Down), aLoc.Nudge(Direction::Right) };
      int extra = 2;

      for (const auto& next : downright)
      {
         if (!aSearchNodePtr->mBoardPtr->IsLocationInside(next))
         {
            continue;
         }
         else if (aSearchNodePtr->mBoardPtr->IsLocationOccupiedBySnake(next))
         {
            extra = std::min(extra, 1);
         }
         else if (aSearchNodePtr->mBoardPtr->IsLocationEmpty(next))
         {
            extra = 0;
         }
      }
      return extra;
   };

   int costHead = (exitLoc - headLoc).Taxicab() + FindExtraCost(headLoc);
   int costTail = (exitLoc - tailLoc).Taxicab() + FindExtraCost(tailLoc);

   return std::min(costHead, costTail) + aSearchNodePtr->mDepth;
}

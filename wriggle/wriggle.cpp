// wriggle.cpp : Defines the entry point for the application.
//

#include "wriggle.hpp"

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      std::cout << "usage: wriggle <filename> {[b]fts|[i]ddfts|[g]bfgs|[a]star}" << std::endl;
      return 0;
   }
   
   std::string solverChoice = "";
   if (argc < 3)
   {
      bool valid = false;
      do
      {
         std::cout << "wriggle: choose solver ([b]fts|[i]ddfts|[g]bfgs|[a]star): ";
         std::cin >> solverChoice;
         if (solverChoice.empty())
         {
            continue;
         }

         valid = solverChoice[0] == 'b' ||
            solverChoice[0] == 'i' ||
            solverChoice[0] == 'g' ||
            solverChoice[0] == 'a';

      } while (!valid);
   }
   else
   {
      solverChoice = argv[2];
   }

   std::string filename = argv[1];
   std::ifstream fin{ filename };
   
   Board::Builder builder;
   builder.FromStream(fin);
   fin.close();

   Board initial = builder.Build();
   std::unique_ptr<Solver> solver;

   switch (solverChoice[0])
   {
   case 'b':
      solver = std::make_unique<BreadthFirstTreeSearchSolver>(initial);
      break;
   case 'i':
      solver = std::make_unique<IterativeDeepeningDepthFirstTreeSearchSolver>(initial);
      break;
   case 'g':
   case 'a':
   default:
      std::cout << "wriggle: solver not implemented yet, exiting" << std::endl;
      return 0;
      break;
   }
      
   solver->Exec();
   solver->PrintToStream(std::cout);

   return 0;
}

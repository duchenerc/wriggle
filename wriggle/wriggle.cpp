// wriggle.cpp : Defines the entry point for the application.
//

#include "wriggle.hpp"

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      std::cout << "usage: wriggle <filename> {bfts|iddfts|gbfgs|astar}" << std::endl;
      return 0;
   }

   std::string filename = argv[1];
   std::ifstream fin{ filename };
   
   Board::Builder builder;
   builder.FromStream(fin);
   fin.close();

   Board initial = builder.Build();
   std::unique_ptr<Solver> solver = std::make_unique<BreadthFirstTreeSearchSolver>(initial);

   solver->Exec();
   solver->PrintToStream(std::cout);

   return 0;
}

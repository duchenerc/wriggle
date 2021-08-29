// wriggle.cpp : Defines the entry point for the application.
//

#include "wriggle.hpp"

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      std::cout << "usage: wriggle <filename> {1|2|3|4}" << std::endl;
      return 0;
   }

   std::string filename = argv[1];
   std::ifstream fin{ filename };
   
   Board::Builder builder;
   builder.FromStream(fin);
   fin.close();

   std::unique_ptr<Board> initial = builder.Build();

   return 0;
}

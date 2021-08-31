# CMakeList.txt : CMake project for wriggle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (wriggle "wriggle.cpp" "Board.cpp" "Snake.cpp" "Location.cpp" "Solver.cpp")

# TODO: Add tests and install targets if needed.

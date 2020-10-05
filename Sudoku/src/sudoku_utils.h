
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

//Here we create a simple namespace to hold all of the related suDoKu functions together.
namespace sudoku
{
    std::vector<std::vector<int>> get_sudoku_grid(std::string file_path)
    {
        //Just setting an 9x9 grid for storing the sudoku puzzle.
        std::vector<std::vector<int>> puzzle(9, std::vector<int>(9));

        //Let us read the puzzle into a stringstream
        std::ifstream infile(file_path);

        std::string puzzle_data;

        if (infile.is_open())
        {
        std::getline(infile, puzzle_data);
            if (puzzle_data.length() != 81)      //The puzzle should have 81 characters
            {
                std::cerr << "Please check the puzzle file for inconsistencies"
                          << "\n";
                exit(1);
            }
        }

        int idx = 0; //This variable will be used to access the numbers in the puzzle string

        for (int i = 0; i < 9; ++i)
        {
            for (int j = 0; j < 9; ++j)
            {
                if ((puzzle_data.substr(idx, 1) != ".") and (puzzle_data.substr(idx, 1) != "0"))  // We will only convert the numeric charcater to an integer if it is not '.' or '0'.
                {
                    puzzle[i][j] = std::stoi(puzzle_data.substr(idx, 1));
                }
                else
                {
                    puzzle[i][j] = -1;    // If we are currently reading a '.' or '0' make it -1.
                }
                idx++;
            }
        }

        return puzzle;
    }

    void print_sudoku(const std::vector<std::vector<int>> &sudoku_puzzle)
    {
        std::cout << "+----------+-----------+-----------+"
                  << "\n";
        for (auto i = 0; i < 9; ++i)
        {
            std::cout << "|";
            for (auto j = 0; j < 9; ++j)
            {
                if (sudoku_puzzle[i][j] > 0)
                {

                    if (j == 2 or j == 5 or j == 8)
                    {
                        std::cout << sudoku_puzzle[i][j] << " | ";
                    }
                    else
                    {
                        std::cout << sudoku_puzzle[i][j] << "   ";
                    }
                }
                else
                {
                    if (j == 2 or j == 5 or j == 8)
                    {
                        std::cout << "."
                                  << " | ";
                    }
                    else
                    {
                        std::cout << "."
                                  << "   ";
                    }
                }
            }
            std::cout << "\n";

            if (i == 2 or i == 5 or i == 8)
            {
                std::cout << "+----------+-----------+-----------+"
                          << "\n";
            }
        }
    }
} // namespace sudoku
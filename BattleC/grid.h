#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <random>
#include <vector>

class Grid
{
private:
    int x_coord = 0;
    int y_coord = 0;
    char grid[10][10]{};
    char module = ' ';
public:
    Grid(const int& x, const int& y, const char& m) : x_coord(x), y_coord(y), module(m) {}

    //Initializes grid as a 10x10 array, all filled with empty slots.
    void initializeGrid()
    {
        for (int i = 0; i < 10; ++i)
        {
            for (int j = 0; j < 10; ++j)
            {
                grid[i][j] = ' ';
            }
        }
    }

    //Prints out a specific module. -1 is used because the user input is 1 space behind due to arrays iterating from 0.
    char getModule(const int& x, const int& y)
    {
        return grid[x - 1][y - 1];
    }

    //Sets a module as a specific desired char. Used when ships are set down, and shots are taken.
    void setModule(const int& x, const int& y, const char& m)
    {
        grid[x - 1][y - 1] = m;
    }

    //Prints out the whole grid, with all slots (both empty and taken). Also prints out borders between the individual modules.
    void getGrid() const
    {
        std::cout << "    ";
        for (char col = 'A'; col <= 'J'; ++col)
        {
            std::cout << "  " << col << " ";
        }
        std::cout << std::endl;

        std::cout << "   ";
        for (int j = 0; j < 10; ++j)
        {
            std::cout << "+---";
        }
        std::cout << "+" << std::endl;

        for (int i = 0; i < 10; ++i)
        {
            std::cout << (i + 1) << ((i + 1 < 10) ? "  |" : " |");
            for (int j = 0; j < 10; ++j)
            {
                std::cout << " " << grid[i][j] << " |";
            }
            std::cout << std::endl;

            std::cout << "   ";
            for (int j = 0; j < 10; ++j)
            {
                std::cout << "+---";
            }
            std::cout << "+" << std::endl;
        }
    }
};

#endif

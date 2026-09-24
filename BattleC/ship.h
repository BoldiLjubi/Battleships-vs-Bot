#ifndef SHIP_H
#define SHIP_H

#include <iostream>
#include <random>
#include <vector>
#include "grid.h"
#include "extra_functions.h"

class Ship
{
private:
    std::string name;
    int size;
    char identifier;
    Grid* grid;
    int hits = 0;
    std::vector<std::pair<int, int>> coordinates;

public:
    Ship(const std::string& n, const int& s, const char& i, Grid& g) : name(n), size(s), identifier(i), grid(&g) {}

    //Returns the char that identifies the specific type of ship.
    char getIdentifier()
    {
        return identifier;
    }

    //Returns the name of the ship (their type). Used when displaying what ships are currently being placed, or what ship has been hit/destroyed.
    std::string getName()
    {
        return name;
    }

    //Function used to place down a ship for the user.
    bool placeShip()
    {
        grid->getGrid();
        int x_coord = 0;
        char y_coord = 'y';
        char direction = 'a';

        while (true)
        {
            //Makes sure that the user stays within the loop in case they provide an invalid input, until they provide a valid input.
            do
            {
                std::cout << "Enter the row you would like to place your " << name << " (size " << size << ") in: ";
                std::cin >> x_coord;
                if (std::cin.fail() || x_coord < 1 || x_coord > 10)
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid row. Please enter a row between 1 and 10" << std::endl;
                    continue;
                }

                std::cout << "Enter the column you would like to place your " << name << " (size " << size << ") in: ";
                std::cin >> y_coord;
                if (std::cin.fail() || y_coord < 'A' || y_coord > 'J')
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid column. Please enter a column between A and J" << std::endl;
                    continue;
                }
                break;

            } while (true);

            //Checks for if the provided combination of inputs place the ship in a valid way (without colliding with other ships or without going off the grid).
            while (true)
            {
                std::cout << "Enter the direction you want your " << name << " (size " << size << ") to face" << std::endl;
                std::cout << "Enter 'V' for vertical (downwards) or 'H' for horizontal (to the right): ";
                std::cin >> direction;

                bool collision = false;

                //Checks if vertical placement is valid.
                if (direction == 'V')
                {
                    //Checks out of bounds.
                    if (x_coord + size - 1 > 10)
                    {
                        std::cout << name << " goes out of bounds vertically. Please enter a new starting position" << std::endl;
                        break;
                    }

                    for (int i = 0; i < size; ++i)
                    {
                        //Checks for collision with other ships.
                        if (grid->getModule(x_coord + i, char_to_int(y_coord)) != ' ')
                        {
                            std::cout << name << " collides with another ship at (" << (x_coord + i) << ", " << y_coord << "). Please try again" << std::endl;
                            collision = true;
                            break;
                        }
                    }
                    if (collision) break;

                    //Sets ship on grid while keeping track of segment coordinates if the placement is valid.
                    for (int i = 0; i < size; ++i)
                    {
                        grid->setModule(x_coord + i, char_to_int(y_coord), identifier);
                        coordinates.push_back(std::make_pair(x_coord + i, char_to_int(y_coord)));
                    }
                }

                //Checks if horizontal placement is valid.
                else if (direction == 'H')
                {
                    //Checks out of bounds.
                    if (char_to_int(y_coord) + size - 1 > 10)
                    {
                        std::cout << name << " goes out of bounds horizontally. Please enter a new starting position" << std::endl;
                        break;
                    }

                    for (int i = 0; i < size; ++i)
                    {
                        //Checks for collision with other ships.
                        if (grid->getModule(x_coord, char_to_int(y_coord) + i) != ' ')
                        {
                            std::cout << name << " collides with another ship at (" << x_coord << ", " << (char)(y_coord + i) << "). Please try again" << std::endl;
                            collision = true;
                            break;
                        }
                    }
                    if (collision) break;

                    //Sets ship on grid while keeping track of segment coordinates if the placement is valid.
                    for (int i = 0; i < size; ++i)
                    {
                        grid->setModule(x_coord, char_to_int(y_coord) + i, identifier);
                        coordinates.push_back(std::make_pair(x_coord, char_to_int(y_coord) + i));
                    }
                }
                else
                {
                    //Error output if an invalid input is provided.
                    std::cout << "Invalid direction. Please enter 'V' for vertical or 'H' for horizontal" << std::endl;
                    continue;
                }

                std::cout << name << " placed successfully!" << std::endl;
                return true;
            }
        }

        grid->getGrid();
        return false;
    }

    //Function used to place down a ship for A Smith.
    bool placeShipAI()
    {
        //Ship placement is a series of randomly generated inputs until A Smith's board is filled with valid ship placements.
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> x(1, 10);
        std::uniform_int_distribution<> y(1, 10);
        std::uniform_int_distribution<> dir(0, 1);

        //While loop constantly re-generates the various inputs.
        while (true)
        {
            int x_coord = x(gen);
            int y_coord = y(gen);
            char direction = dir(gen) == 0 ? 'V' : 'H';

            bool collision = false;

            //Checks if vertical placement is valid
            if (direction == 'V')
            {
                //Checks out of bounds.
                if (x_coord + size - 1 > 10)
                    continue;

                for (int i = 0; i < size; ++i)
                {
                    //Checks for collision with other ships.
                    if (grid->getModule(x_coord + i, y_coord) != ' ')
                    {
                        collision = true;
                        break;
                    }
                }
                if (collision)
                    continue;

                //Sets ship on grid while keeping track of segment coordinates if the placement is valid.
                for (int i = 0; i < size; ++i)
                {
                    grid->setModule(x_coord + i, y_coord, identifier);
                    coordinates.push_back(std::make_pair(x_coord + i, y_coord));
                }
            }

            //Checks if horizontal placement is valid
            else if (direction == 'H')
            {
                //Checks out of bounds.
                if (y_coord + size - 1 > 10)
                    continue;

                for (int i = 0; i < size; ++i)
                {
                    //Checks for collision with other ships.
                    if (grid->getModule(x_coord, y_coord + i) != ' ')
                    {
                        collision = true;
                        break;
                    }
                }
                if (collision)
                    continue;

                //Sets ship on grid while keeping track of segment coordinates if the placement is valid.
                for (int i = 0; i < size; ++i)
                {
                    grid->setModule(x_coord, y_coord + i, identifier);
                    coordinates.push_back(std::make_pair(x_coord, y_coord + i));
                }
            }

            return true;
        }
    }

    //Function used to register the hits of ships. Core feature of battleships, used for keeping track of ship segments hits.
    bool registerHit(Grid& grid)
    {
        for (auto& coord : coordinates)
        {
            if (grid.getModule(coord.first, coord.second) == identifier)
            {
                //Mark "X" on the grid to show a successfuly hit.
                grid.setModule(coord.first, coord.second, 'X');
                hits += 1;
                return true;
            }
        }
        return false;
    }

    //Function used to check for if a shink is sunk or not. If the hits from registerHit() reaches the in number the size of the ship, the ship is considered sunk.
    bool shipSunk()
    {
        if (hits == size)
        {
            return true;
        }
        return false;
    }
};

#endif

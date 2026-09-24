#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <random>
#include <vector>
#include "grid.h"
#include "extra_functions.h"
#include "ship.h"

class Game
{
private:
    Grid player_ship_grid;
    Grid player_shoot_grid;
    Grid a_smith_ship_grid;
    Grid a_smith_shoot_grid;
    std::vector<Ship> player_ships;
    std::vector<Ship> a_smith_ships;
    std::vector<std::pair<int, char>> player_shots;
    std::vector<std::pair<int, char>> a_smith_shots;
    bool end_game = false;
public:
    Game() : player_ship_grid(0, 0, ' '), player_shoot_grid(0, 0, ' '), a_smith_ship_grid(0, 0, ' '), a_smith_shoot_grid(0, 0, ' '), end_game(false) {}

    //Function used to initialize the game. Function sets up the ship and shoot grid for both the player and A Smith, and also sets up their various different ships.
    void initializeGame()
    {
        //Player's grids.
        player_ship_grid.initializeGrid();
        player_shoot_grid.initializeGrid();

        //A Smith's ships.
        a_smith_ship_grid.initializeGrid();
        a_smith_shoot_grid.initializeGrid();

        //Player's ships.
        player_ships =
        {
            Ship("Carrier", 5, 'A', player_ship_grid),
            Ship("Battleship", 4, 'B', player_ship_grid),
            Ship("Cruiser", 3, 'C', player_ship_grid),
            Ship("Submarine", 3, 'S', player_ship_grid),
            Ship("Destroyer", 2, 'D', player_ship_grid)
        };

        //A Smith's ships.
        a_smith_ships =
        {
            Ship("Carrier", 5, 'A', a_smith_ship_grid),
            Ship("Battleship", 4, 'B', a_smith_ship_grid),
            Ship("Cruiser", 3, 'C', a_smith_ship_grid),
            Ship("Submarine", 3, 'S', a_smith_ship_grid),
            Ship("Destroyer", 2, 'D', a_smith_ship_grid)
        };

        //Iterates through all of the player's ships, having them place their ships one by one.
        for (auto& ship : player_ships)
        {
            ship.placeShip();
        }

        //Iterates through all of A Smith's ships, having them place their ships one by one.
        for (auto& ship : a_smith_ships)
        {
            ship.placeShipAI();
        }
    }

    //Function used by the player to shoot at A Smith's grid.
    bool shootAtPlayer(Grid& op_ship_grid, Grid& p_shoot_grid, std::vector<Ship>& op_ships, int& ships_sunk)
    {
        int x = 0;
        char y = 'Z';

        //Boolean initialized, used to make sure the player shoots at an original coordinate.
        bool shoot_original = false;
        std::pair<int, char> shot;

        //Coordinate selection occurs until the player inputs a coordinate they have yet to shoot at.
        while (!shoot_original)
        {
            //Checks for valid X coordinate
            do
            {
                std::cout << "Please enter the X coordinate you would like to shoot at (1 to 10): ";
                std::cin >> x;

                //Checks for invalid inputs.
                if (std::cin.fail())
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max());
                    std::cout << "Invalid input. Please enter a number between 1 and 10." << std::endl;
                    x = 0;
                }
                //Checks for out of bounds.
                else if (x < 1 || x > 10)
                {
                    std::cout << "Invalid X coordinate (must be between 1 and 10)." << std::endl;
                }
            } while (x < 1 || x > 10);

            //Checks for valid Y coordinate
            do
            {
                std::cout << "Please enter the Y coordinate you would like to shoot at (A to J): ";
                std::cin >> y;

                //Checks for invalid inputs.
                if (std::cin.fail())
                {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max());
                    std::cout << "Invalid input. Please enter a letter between A and J." << std::endl;
                    y = 'Z';
                }
                //Checks for out of bounds.
                else if (y < 'A' || y > 'J')
                {
                    std::cout << "Invalid Y coordinate (must be between A and J)." << std::endl;
                }
            } while (y < 'A' || y > 'J');

            //Creates temporary storage for shot coordinate.
            shot = std::make_pair(x, y);

            //Checks if input shot coordinate has already been input previously.
            if (std::find(player_shots.begin(), player_shots.end(), shot) != player_shots.end())
            {
                std::cout << "You already shot at the coordinate (" << x << ", " << y << "). Please pick a new one to shoot at" << std::endl;

                x = 0;
                y = 'Z';
            }
            //Register shot as original shot, making the series of user inputs valid.
            else
            {
                shoot_original = true;
                player_shots.push_back(shot);
            }
        }

        int y_coord = char_to_int(y);

        char module = op_ship_grid.getModule(x, y_coord);

        //Checks if shot is successful or a miss by whether the module is empty or taken.
        if (module != ' ' && module != 'X' && module != '-')
        {
            //Iterates through all of A Smith's ships.
            for (auto& ship : op_ships)
            {
                //Checks what ship the player has hit.
                if (ship.getIdentifier() == module)
                {
                    //Registers the hit for A Smith's given ship.
                    if (ship.registerHit(op_ship_grid))
                    {
                        std::cout << "Opponent's " << ship.getName() << " has been hit!" << std::endl;

                        //Checks if the hit ship has been sunk or not, given the successful hit.
                        if (ship.shipSunk())
                        {
                            std::cout << "Opponent's " << ship.getName() << " has been sunk! Well done!" << std::endl;
                            ships_sunk += 1;
                        }
                    }
                    break;
                }
            }
            //Marks successful hit on player's shoot grid.
            p_shoot_grid.setModule(x, y_coord, 'X');
            return true;
        }
        //Gives feedback on the missed shot, and marking it on the player's shoot grid and A Smith's ship grid.
        else
        {
            std::cout << "That's a miss." << std::endl;
            p_shoot_grid.setModule(x, y_coord, '-');
            op_ship_grid.setModule(x, y_coord, '-');
            return false;
        }
    }

    //Function used by A Smith to shoot at the player's grid.
    bool shootAtAI(Grid& p_ship_grid, Grid& op_shoot_grid, std::vector<Ship>& p_ships, int& ships_sunk, bool& targeting_mode, std::pair<int, int>& first_hit, std::pair<int, int>& last_hit, std::vector<std::pair<int, int>>& directions, int& direction_index)
    {
        int x = 0;
        int y = 0;

        //Shoots in a specific direction if there was a successful hit during the seeking mode.
        if (targeting_mode)
        {
            while (direction_index < directions.size())
            {
                x = last_hit.first + directions[direction_index].first;
                y = last_hit.second + directions[direction_index].second;

                //Checks if there was a successful hit.
                if (x >= 1 && x <= 10 && y >= 1 && y <= 10 && op_shoot_grid.getModule(x, y) == ' ')
                {
                    break;
                }
                //Proceed in the other direction if the shot missed.
                direction_index += 1;
            }

            //If all the directions were unsuccessful, fire in opposite direction.
            if (direction_index >= directions.size())
            {
                direction_index = (direction_index + 2) % directions.size();
                x = first_hit.first + directions[direction_index].first;
                y = first_hit.second + directions[direction_index].second;

                //Check if shot is a successful hit.
                if (x < 1 || x > 10 || y < 1 || y > 10 || op_shoot_grid.getModule(x, y) != ' ')
                {
                    targeting_mode = false;
                }
            }
        }

        //Shoots randomly around the grid until A Smith finds a successful hit coordinate.
        if (!targeting_mode)
        {
            do
            {
                x = rand() % 10 + 1;
                y = rand() % 10 + 1;
            //Makes sure it only shoots are coordiantes that have not been tried previously.
            } while (op_shoot_grid.getModule(x, y) != ' ');
        }

        //Method for registering the hit.
        if (p_ship_grid.getModule(x, y) != ' ')
        {
            for (auto& ship : p_ships)
            {
                if (ship.getIdentifier() == p_ship_grid.getModule(x, y))
                {
                    if (ship.registerHit(p_ship_grid))
                    {
                        std::cout << "Your " << ship.getName() << " has been hit!" << std::endl;

                        if (ship.shipSunk())
                        {
                            std::cout << "Your " << ship.getName() << " has been sunk!" << std::endl;
                            ships_sunk += 1;

                            //Resets the coordinates of the targeting mode, now that a ship has been sunk successfully. Coordinates will continue re-updating upon a successful hit.
                            targeting_mode = false;
                            direction_index = 0;
                            first_hit = { -1, -1 };
                            last_hit = { -1, -1 };
                        }
                        else
                        {
                            //Registers first hit and last hit, initiating targeting mode upon the first successful hit of a given ship.
                            if (!targeting_mode)
                            {                                
                                targeting_mode = true;
                                first_hit = { x, y };
                                last_hit = { x, y };
                                direction_index = 0;
                            }
                            //Registers last hit, proceeds with targeting mode.
                            else
                            {
                                last_hit = { x, y };
                            }
                        }
                    }
                    break;
                }
            }

            //Registers successful hit on A Smith's shoot grid.
            op_shoot_grid.setModule(x, y, 'X');
            return true;
        }
        //Gives feedback on the missed shot, and marking it on the A Smith's shoot grid and the player's ship grid.
        else
        {
            std::cout << "A Smith misses their shot!" << std::endl;
            op_shoot_grid.setModule(x, y, '-');
            p_ship_grid.setModule(x, y, '-');

            //A Smith shoots in the next direction if it is currently in targeting mode.
            if (targeting_mode)
            {
                direction_index += 1;
                if (direction_index >= directions.size())
                {
                    //If A Smith tried all directions and was unsuccessful, revert to randomised shooting again until the next successful hit.
                    targeting_mode = false;
                }
            }
            return false;
        }
    }

    //Function used to play the game.
    void playGame()
    {
        initializeGame();

        int player_ships_sunk = 0;
        int a_smith_ships_sunk = 0;
        int round = 1;

        static bool targeting_mode = false;
        static std::pair<int, int> first_hit = { -1, -1 };
        static std::pair<int, int> last_hit = { -1, -1 };
        static std::vector<std::pair<int, int>> directions = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
        static int direction_index = 0;

        //The game proceeds until all ships of either side are sunk.
        while (!end_game)
        {
            std::cout << "Round " << round << std::endl;

            std::cout << "Your Grid" << std::endl;
            player_ship_grid.getGrid();
            std::cout << "Your Shoot Grid" << std::endl;
            player_shoot_grid.getGrid();
            a_smith_ship_grid.getGrid();
            a_smith_shoot_grid.getGrid();

            std::cout << "It is your turn to shoot!" << std::endl;
            bool player_hit = shootAtPlayer(a_smith_ship_grid, player_shoot_grid, a_smith_ships, a_smith_ships_sunk);
            //While loops makes player shoot constantly until they have an unsuccessful shot or they win the game.
            while (player_hit)
            {
                //Victory message in case the player wins. All grids are fully displayed.
                if (a_smith_ships_sunk == a_smith_ships.size())
                {
                    std::cout << "--== Congratulations! You sunk all of A Smith's ships, and are victorious! ==--" << std::endl;
                    std::cout << "Your Grid" << std::endl;
                    player_ship_grid.getGrid();
                    std::cout << "Your Shoot Grid" << std::endl;
                    player_shoot_grid.getGrid();
                    std::cout << "A Smith Grid" << std::endl;
                    a_smith_ship_grid.getGrid();
                    std::cout << "A Smith's Shoot Grid" << std::endl;
                    a_smith_shoot_grid.getGrid();
                    end_game = true;
                    break;
                }
                //Lets player shoot again upon successful hit.
                else
                {
                    std::cout << "You hit correctly, so you can shoot again!" << std::endl;
                    std::cout << "Your Grid" << std::endl;
                    player_ship_grid.getGrid();
                    std::cout << "Your Shoot Grid" << std::endl;
                    player_shoot_grid.getGrid();
                    player_hit = shootAtPlayer(a_smith_ship_grid, player_shoot_grid, a_smith_ships, a_smith_ships_sunk);
                }
            }
            //Game ends if conditions are met.
            if (end_game)
            {
                break;
            }

            std::cout << "It is A Smith's turn to shoot!" << std::endl;
            bool a_smith_hit = shootAtAI(player_ship_grid, a_smith_shoot_grid, player_ships, player_ships_sunk, targeting_mode, first_hit, last_hit, directions, direction_index);
            //While loops makes A Smith shoot constantly until they have an unsuccessful shot or they win the game.
            while (a_smith_hit)
            {
                //Victory message in case A Smith wins. All grids are fully displayed.
                if (player_ships_sunk == player_ships.size())
                {
                    std::cout << "--== You have been defeated... ==--" << std::endl;
                    std::cout << "Your Grid" << std::endl;
                    player_ship_grid.getGrid();
                    std::cout << "Your Shoot Grid" << std::endl;
                    player_shoot_grid.getGrid();
                    std::cout << "A Smith Grid" << std::endl;
                    a_smith_ship_grid.getGrid();
                    std::cout << "A Smith's Shoot Grid" << std::endl;
                    a_smith_shoot_grid.getGrid();
                    end_game = true;
                    break;
                }

                //Lets A Smith shoot again upon successful hit.
                std::cout << "A Smith hit correctly, so they can shoot again!" << std::endl;
                std::cout << "Your Grid" << std::endl;
                player_ship_grid.getGrid();
                std::cout << "Your Shoot Grid" << std::endl;
                player_shoot_grid.getGrid();
                a_smith_hit = shootAtAI(player_ship_grid, a_smith_shoot_grid, player_ships, player_ships_sunk, targeting_mode, first_hit, last_hit, directions, direction_index);
            }
            //Game ends if conditions are met.
            if (end_game)
            {
                break;
            }
            //1 gets added to round counter, to show the amount of rounds (turns) the game is currently at.
            round += 1;
        }
    }
};

#endif

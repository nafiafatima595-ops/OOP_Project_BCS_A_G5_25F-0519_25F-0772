// main.cpp
// Driver file of The Locker Heist game.
// It creates the score board, connects it to the GameEngine (Singleton) and runs the game.
// All exceptions of the game are caught here, as the project manual requires.

#include <iostream>
#include "heist.h"
using namespace std;

int main()
{
    // The score board is owned by main(), the engine only uses it (association)
    ScoreBoard board("leader/scoreBoard.txt");

    // Singleton: there is only one GameEngine and getInstance() always gives the same one
    GameEngine& engine = GameEngine::getInstance();
    engine.setScoreBoard(&board);

    engine.showWelcome();

    // The game runs step by step until the player chooses Exit.
    // A GameException thrown anywhere in the game is shown here and the game continues.
    while (engine.getState() != GAME_EXIT)
    {
        try
        {
            engine.step();
        }
        catch (GameException& error)
        {
            cout << endl << error.what() << endl;
        }
        catch (...)
        {
            cout << endl << "HEIST ERROR: An unexpected problem happened, the game continues." << endl;
        }
    }

    cout << endl << "Thank you for playing The Locker Heist. Goodbye, Detective!" << endl;
    return 0;
}
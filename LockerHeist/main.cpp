// main.cpp
// Driver file of The Locker Heist game.
// For now it tests the Character, Victim and Detective classes (Module 3, step A).

#include <iostream>
#include <cstdlib>    // Needed for system("pause"), which keeps the console window open after the output
#include "heist.h"
using namespace std;

int main()
{
    cout << "Welcome to The Locker Heist!" << endl;
    cout << "--- Module 3 Test (Step A: Character, Victim, Detective) ---" << endl;

    // Create one victim and one detective with sample data
    Victim leonardo("Mr.Leonardo", "French", 'S', 1, WORD_2);
    Detective agent("Agent_Halim", 100);
    agent.setNationality("Malay");

    // Polymorphism: both objects are used through Character pointers,
    // but the display() and getMinCredibility() of the actual object are called
    Character* people[2] = { &leonardo, &agent };
    for (int i = 0; i < 2; i++)
    {
        people[i]->display();
        cout << "  Minimum credibility to interview: " << people[i]->getMinCredibility() << endl;
    }

    // One wrong turn: the detective loses 5 credibility and uses one turn
    agent.loseCredibility(5);
    agent.useTurn();
    cout << "After one wrong turn: Credibility = " << agent.getCredibility()
        << ", Turns = " << agent.getTurns() << endl;

    // Show the clue stored inside the victim
    cout << "Victim clue: letter = " << leonardo.getLetter()
        << ", position = " << leonardo.getPosition()
        << ", row = " << static_cast<int>(leonardo.getRow()) << endl;

    // Pause the program so that the output stays visible until a key is pressed
    system("pause");
    return 0;
}
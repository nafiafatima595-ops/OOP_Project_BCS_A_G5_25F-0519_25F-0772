// main.cpp
// Driver file of The Locker Heist game.
// For now it tests the Character hierarchy classes (Module 3).

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

    // Step B test: three house help members are used through Character pointers (polymorphism)
    cout << "--- Module 3 Test (Step B: Suspect, HouseHelp, Cook, Guard, Servant) ---" << endl;
    Cook enzo("Mr.Enzo", "Italian", false, 'P', 2, WORD_2);
    Guard lukas("Mr.Lukas", "German", false, 'A', 3, WORD_2);
    Servant toby("Mr.Toby", "British", false, 'G', 4, WORD_2);

    Character* houseHelp[3] = { &enzo, &lukas, &toby };
    for (int i = 0; i < 3; i++)
    {
        houseHelp[i]->display();
        cout << "  Minimum credibility to interview: " << houseHelp[i]->getMinCredibility() << endl;
    }
    cout << "Cook clue: letter = " << enzo.getLetter()
        << ", position = " << enzo.getPosition()
        << ", row = " << static_cast<int>(enzo.getRow()) << endl;

    // Step C test: two partners and the child are used through Character pointers (polymorphism)
    cout << "--- Module 3 Test (Step C: BusinessRelations, Partner, CloseRelations, Child) ---" << endl;
    Partner ruslana("Ms.Ruslana", "Partner 1", "Ukrainian", false, 'E', 6, WORD_2);
    Partner dmitry("Mr.Dmitry", "Partner 2", "Russian", false, 'T', 7, WORD_2);
    Child celine("Ms.Celine", "French", false, 'H', 5, WORD_2);

    Character* relations[3] = { &ruslana, &dmitry, &celine };
    for (int i = 0; i < 3; i++)
    {
        relations[i]->display();
        cout << "  Minimum credibility to interview: " << relations[i]->getMinCredibility() << endl;
    }
    cout << "Partner clue: letter = " << ruslana.getLetter()
        << ", position = " << ruslana.getPosition()
        << ", row = " << static_cast<int>(ruslana.getRow()) << endl;

    // Step D test: the PA is used through a Character pointer
    cout << "--- Module 3 Test (Step D: PA with virtual inheritance) ---" << endl;
    PA jad("Mr.Jad", "Egyptian", false, 'I', 8, WORD_2);
    Character* assistant = &jad;
    assistant->display();
    cout << "  Minimum credibility to interview: " << assistant->getMinCredibility() << endl;
    cout << "PA clue: letter = " << jad.getLetter()
        << ", position = " << jad.getPosition()
        << ", row = " << static_cast<int>(jad.getRow()) << endl;

    // Pause the program so that the output stays visible until a key is pressed
    system("pause");
    return 0;
}
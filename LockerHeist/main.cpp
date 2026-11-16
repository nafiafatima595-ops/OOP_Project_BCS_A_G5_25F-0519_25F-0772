// main.cpp
// Driver file of The Locker Heist game.
// For now it tests the structures and enumerations of Module 1.

#include <iostream>
#include <cstdlib>
#include "heist.h"
using namespace std;

int main()
{
    cout << "Welcome to The Locker Heist!" << endl;
    cout << "--- Module 1 Test ---" << endl;

    // Create one object of every structure with sample values
    Tile sampleTile = { 'A', false };
    Clue sampleClue = { 'S', 1, WORD_2 };
    Location sampleLocation = { "Bedroom_1", 'A', 1, WORD_1, false };

    // Print the values to confirm that the structures store data correctly
    cout << "Tile: letter = " << sampleTile.tileLetter << ", filled = " << sampleTile.filled << endl;
    cout << "Clue: letter = " << sampleClue.clueLetter << ", position = " << sampleClue.cluePosition
        << ", row = " << static_cast<int>(sampleClue.clueRow) << endl;
    cout << "Location: " << sampleLocation.locName << ", letter = " << sampleLocation.letterClueLoc
        << ", position = " << sampleLocation.boardPosLoc
        << ", row = " << static_cast<int>(sampleLocation.locRow)
        << ", searched = " << sampleLocation.locSearchFlag << endl;

    // Test the enumerations by printing their numeric values
    GameState state = GAME_MENU;
    MenuChoice choice = MENU_RULES;
    cout << "GameState (GAME_MENU) = " << static_cast<int>(state) << endl;
    cout << "MenuChoice (MENU_RULES) = " << static_cast<int>(choice) << endl;

    system("pause");
    return 0;
}
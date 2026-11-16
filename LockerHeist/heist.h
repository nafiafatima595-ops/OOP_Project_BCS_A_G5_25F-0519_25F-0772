// heist.h
// Header file of The Locker Heist game.
// It contains the declarations of all enumerations, structures and (in later modules) classes.

#ifndef HEIST_H
#define HEIST_H

// Include guards (above) prevent this header from being included twice in one program.

#include <iostream>
#include <string>
using namespace std;

// ======================= ENUMERATIONS =======================
// Enumerations utilize meaningful names instead of plain numbers,
// which makes the code easier to read and less error-prone.

// WordRow tells us to which of the two target words (rows of the Scrabble board) a letter belongs.
enum WordRow
{
    WORD_1,    // First row: 7-letter word, its letters come from the locations
    WORD_2     // Second row: 8-letter word, its letters come from the characters
};

// GameState stores the current state of the player in the game.
enum GameState
{
    GAME_MENU,       // Player is in the main menu
    GAME_PLAYING,    // Player is investigating the case
    GAME_WON,        // Player accused the correct culprit
    GAME_LOST,       // Credibility is zero or turns are exhausted
    GAME_EXIT        // Player quit the game
};

// MenuChoice represents the options of the Main Menu.
// The values are fixed so that they match the numbers shown to the user.
enum MenuChoice
{
    MENU_NEW_GAME = 1,
    MENU_SCOREBOARD = 2,
    MENU_ABOUT = 3,
    MENU_RULES = 4,
    MENU_EXIT = 5
};

// ======================= STRUCTURES =======================
// Structures utilize one name to group related data together,
// which improves code reusability and keeps the related data in one place.

// Tile represents one square of the Scrabble board.
struct Tile
{
    char tileLetter;    // Letter placed on this tile
    bool filled;        // True if a letter has already been placed on this tile
};

// Clue represents a letter that the detective gets by interviewing a character.
struct Clue
{
    char clueLetter;     // Letter given by the clue
    int cluePosition;    // Position of the letter on the board
    WordRow clueRow;     // Row (word) to which the letter belongs
};

// Location represents one of the 7 places in the cottage that the detective can search.
struct Location
{
    string locName;         // Name of the place, e.g. Bedroom_1
    char letterClueLoc;     // Letter hidden at this location
    int boardPosLoc;        // Position of that letter on the board
    WordRow locRow;         // Row (word) to which the letter belongs
    bool locSearchFlag;     // True if the detective has already searched this location
};

#endif
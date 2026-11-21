// heist.h
// Header file of The Locker Heist game.
// It contains the declarations of all enumerations, structures and classes.

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

// ======================= CLASSES =======================

// Character is the abstract base class of every person in the game.
// It stores the attributes that are common to all people, and it forces every
// derived class to override display() and getMinCredibility() (pure virtual functions).
class Character
{
protected:
    string characterName;    // Name of the person
    string characterRole;    // Role of the person, e.g. Victim, Cook, Detective
    string nationality;      // Nationality of the person
    bool isCulprit;          // True only for the person who committed the theft

    // Prints the information that is common for all characters (helper for display functions)
    void printBasicInfo() const;

public:
    Character(string name = "", string role = "", string nation = "", bool culprit = false);

    // Virtual destructor makes sure that the destructor of the derived class also runs
    // when an object is deleted through a Character pointer.
    virtual ~Character();

    // Pure virtual functions: they make Character abstract and enable runtime polymorphism.
    virtual void display() const = 0;               // Shows the character on the screen
    virtual int getMinCredibility() const = 0;      // Credibility needed to interview this character

    // Getters and setters (encapsulation: data is accessed only through these functions)
    string getName() const;
    string getRole() const;
    string getNationality() const;
    bool getIsCulprit() const;
    void setNationality(string nation);
    void setIsCulprit(bool culprit);
};

// Victim is the French business owner. He is interviewed by the detective but is not a suspect.
// He gives one letter of Word 2 together with its board position.
class Victim : public Character
{
private:
    char letterClueChar;     // Letter given by the victim
    int boardPosChar;        // Board position of that letter
    WordRow clueRowChar;     // Row of that letter (always Word 2)

public:
    Victim(string name = "", string nation = "", char letter = ' ', int pos = 0, WordRow row = WORD_2);

    void display() const override;
    int getMinCredibility() const override;    // Victim can be interviewed only if credibility >= 80

    char getLetter() const;
    int getPosition() const;
    WordRow getRow() const;
};

// Detective is the player. It stores only its own credibility, the turn count and the solved words.
// The turns are incremented by the GameEngine, the detective only stores the count.
class Detective : public Character
{
private:
    int credibility;         // Reduces with every wrong step, game is lost when it reaches zero
    int turns;               // Number of turns used so far
    string solvedWords;      // Words that the detective has formed correctly

public:
    Detective(string name = "", int startCredibility = 100);

    void display() const override;
    int getMinCredibility() const override;    // Detective is never interviewed, so it returns 0

    void loseCredibility(int amount);          // Reduces credibility, it never goes below zero
    void useTurn();                            // Increases the turn count by one
    int getCredibility() const;
    int getTurns() const;
};

// Suspect is the abstract base class of the seven suspects.
// It adds the Word 2 clue (letter, board position and row) that each suspect gives to the detective.
// It does not override display() and getMinCredibility(), so it stays abstract.
class Suspect : public Character
{
protected:
    char letterClueChar;     // Letter given by the suspect
    int boardPosChar;        // Board position of that letter
    WordRow clueRowChar;     // Row of that letter (always Word 2)

public:
    Suspect(string name = "", string role = "", string nation = "", bool culprit = false,
            char letter = ' ', int pos = 0, WordRow row = WORD_2);
    virtual ~Suspect();

    char getLetter() const;
    int getPosition() const;
    WordRow getRow() const;
};

// HouseHelp groups the staff of the cottage. They can be interviewed at any time.
class HouseHelp : public Suspect
{
public:
    HouseHelp(string name = "", string role = "", string nation = "", bool culprit = false,
              char letter = ' ', int pos = 0, WordRow row = WORD_2);

    void display() const override;
    int getMinCredibility() const override;    // No credibility is required for house help
};

// Cook is a member of the house help who works in the kitchen
class Cook : public HouseHelp
{
public:
    Cook(string name = "", string nation = "", bool culprit = false,
         char letter = ' ', int pos = 0, WordRow row = WORD_2);
    void display() const override;
};

// Guard is a member of the house help who guards the cottage
class Guard : public HouseHelp
{
public:
    Guard(string name = "", string nation = "", bool culprit = false,
          char letter = ' ', int pos = 0, WordRow row = WORD_2);
    void display() const override;
};

// Servant is a member of the house help who serves the guests
class Servant : public HouseHelp
{
public:
    Servant(string name = "", string nation = "", bool culprit = false,
            char letter = ' ', int pos = 0, WordRow row = WORD_2);
    void display() const override;
};

// BusinessRelations groups the suspects who are related to the business of the owner.
// It inherits from Suspect with the "virtual" keyword. This is needed for the PA class (Step D):
// PA will be a BusinessRelations and also a CloseRelations, and both of them are Suspects.
// Virtual inheritance makes sure that PA contains only ONE copy of Suspect (it solves the diamond problem).
// It does not override display() and getMinCredibility(), so it stays abstract.
class BusinessRelations : virtual public Suspect
{
public:
    BusinessRelations(string name = "", string role = "", string nation = "", bool culprit = false,
                      char letter = ' ', int pos = 0, WordRow row = WORD_2);
};

// Partner is a business partner of the owner (Partner 1 or Partner 2)
class Partner : public BusinessRelations
{
public:
    Partner(string name = "", string role = "Partner 1", string nation = "", bool culprit = false,
            char letter = ' ', int pos = 0, WordRow row = WORD_2);

    void display() const override;
    int getMinCredibility() const override;    // Partner can be interviewed only if credibility >= 50
};

// CloseRelations groups the suspects who are personally close to the owner.
// It also inherits virtually from Suspect (see the comment of BusinessRelations).
class CloseRelations : virtual public Suspect
{
public:
    CloseRelations(string name = "", string role = "", string nation = "", bool culprit = false,
                   char letter = ' ', int pos = 0, WordRow row = WORD_2);
};

// Child is the child of the owner
class Child : public CloseRelations
{
public:
    Child(string name = "", string nation = "", bool culprit = false,
          char letter = ' ', int pos = 0, WordRow row = WORD_2);

    void display() const override;
    int getMinCredibility() const override;    // Child can be interviewed only if credibility >= 75
};

#endif
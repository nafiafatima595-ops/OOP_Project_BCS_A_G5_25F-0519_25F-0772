// heist.h
// Header file of The Locker Heist game.
// It contains the declarations of all enumerations, structures, exceptions, templates and classes.

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

    // Friend functions let the stream operators print and read a Tile directly (Module 11)
    friend ostream& operator<<(ostream& out, const Tile& tile);
    friend istream& operator>>(istream& in, Tile& tile);
};

// Clue represents a letter that the detective gets by interviewing a character or searching a location.
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

    friend ostream& operator<<(ostream& out, const Location& place);
    friend istream& operator>>(istream& in, Location& place);
};

// GameRules groups the values that are read from rules.txt, so that they travel together
struct GameRules
{
    int maxTurns;               // Maximum number of turns
    int startCredibility;       // Credibility at the start of a game
    int wrongTurnPenalty;       // Credibility lost for every wrong turn
    int wrongWordPenalty;       // Credibility lost for every wrong word
    string winCondition;        // Text of the win condition
    string loseCondition;       // Text of the lose condition
};

// ======================= OPERATORS FOR STRUCTURES (Module 11) =======================

// Two tiles are equal when they hold the same letter (used to compare letters with clue.txt)
bool operator==(const Tile& first, const Tile& second);

// Two clues are equal when letter, position and row are all the same
bool operator==(const Clue& first, const Clue& second);

// ======================= UTILITY FUNCTIONS =======================
// Small helpers that are shared by the classes (text handling and console input).

string trimText(const string& text);                                         // Removes spaces and line endings from both sides
int splitLine(const string& line, char separator, string parts[], int maxParts);   // Splits a line, returns the number of parts
bool parseInteger(const string& text, int& value);                           // Converts text to int, false if it is not a number
string numberToText(int number);                                             // Converts an int to text
string toUpperText(const string& text);                                      // Converts text to capital letters
string padText(const string& text, int width);                               // Adds spaces on the right to align tables
string todayDate();                                                          // Today's date as dd-mm-yy
void clearScreen();                                                          // Clears the console
void pauseScreen();                                                          // Waits for the Enter key
string readLine(const string& prompt);                                       // Reads one full line from the player
int readNumber(const string& prompt, int low, int high);                     // Reads a number inside a range, throws if invalid
bool readYesNo(const string& prompt);                                        // Reads Y or N, throws if invalid

// ======================= EXCEPTIONS =======================

// ===== CLASS BEGIN: GameException =====
// GameException is the base class of all exceptions of the game.
// Every message starts with "HEIST ERROR". Exceptions are thrown by the classes and caught in main().
class GameException
{
protected:
    string message;    // Message that explains what went wrong

public:
    GameException(string msg = "HEIST ERROR: Something went wrong in the game.");
    virtual ~GameException();
    virtual string what() const;    // Returns the message of the exception
};

// RackFullException is thrown when a letter is added to a tile rack that has no free slot
class RackFullException : public GameException
{
public:
    RackFullException();
};

// RackEmptyException is thrown when a letter is removed from a tile rack that has no letters
class RackEmptyException : public GameException
{
public:
    RackEmptyException();
};

// LetterNotFoundException is thrown when the letter that should be removed is not in the rack
class LetterNotFoundException : public GameException
{
public:
    LetterNotFoundException(char letter);
};

// InvalidPositionException is thrown when a board position (or array index) is outside the valid range
class InvalidPositionException : public GameException
{
public:
    InvalidPositionException(int position);
};

// SlotFilledException is thrown when a letter is placed on a tile that already has a letter
class SlotFilledException : public GameException
{
public:
    SlotFilledException(int position);
};

// InvalidWordException is thrown when a word is formed from an incomplete row
class InvalidWordException : public GameException
{
public:
    InvalidWordException(string reason);
};

// InvalidInputException is thrown when the player types something that the menu does not accept
class InvalidInputException : public GameException
{
public:
    InvalidInputException(string reason);
};

// LowCredibilityException is thrown when the credibility is too low to interview a person
class LowCredibilityException : public GameException
{
public:
    LowCredibilityException(string who, int needed, int have);
};

// FileException is thrown when a text file is not found or its content is corrupted
class FileException : public GameException
{
public:
    FileException(string fileName, string problem);
};

// ======================= TEMPLATE: DynamicArray =======================

// ===== CLASS BEGIN: DynamicArray<T> =====
// DynamicArray is a generic (template) array that owns heap memory. Because it is generic,
// the same code is reused for Tile (TileRack, Scrabble) and Location (Cottage), which avoids
// repeating the same new/delete logic in every class. It performs deep copies and checks the index.
// Templates must be written completely in the header file, so the bodies are here and not in heist.cpp.
template <class T>
class DynamicArray
{
private:
    T* data;           // Heap memory that holds the elements
    int capacity;      // Number of elements that fit in the array

public:
    // Constructor allocates the array. A capacity below one is corrected to one
    DynamicArray(int cap = 1)
    {
        if (cap < 1)
        {
            cap = 1;
        }
        capacity = cap;
        data = new T[capacity];
    }

    // Copy constructor: allocates a NEW array and copies every element (deep copy)
    DynamicArray(const DynamicArray& other)
    {
        capacity = other.capacity;
        data = new T[capacity];
        for (int i = 0; i < capacity; i++)
        {
            data[i] = other.data[i];
        }
    }

    // Assignment operator: checks self-assignment, copies into a new array, then frees the old one
    DynamicArray& operator=(const DynamicArray& other)
    {
        if (this != &other)
        {
            T* fresh = new T[other.capacity];
            for (int i = 0; i < other.capacity; i++)
            {
                fresh[i] = other.data[i];
            }
            delete[] data;
            data = fresh;
            capacity = other.capacity;
        }
        return *this;
    }

    // Destructor releases the heap memory, so there is no memory leak
    ~DynamicArray()
    {
        delete[] data;
    }

    // Index operator with range check: a wrong index throws an exception instead of crashing
    T& operator[](int index)
    {
        if (index < 0 || index >= capacity)
        {
            throw InvalidPositionException(index + 1);
        }
        return data[index];
    }

    // Const version of the index operator, used by const member functions
    const T& operator[](int index) const
    {
        if (index < 0 || index >= capacity)
        {
            throw InvalidPositionException(index + 1);
        }
        return data[index];
    }

    int getCapacity() const { return capacity; }
};

// ======================= CLASS: TileRack =======================

// ===== CLASS BEGIN: TileRack =====
// TileRack stores the letters that the detective has collected.
// Its tiles are now stored in a DynamicArray<Tile> (Module 8). The earlier raw DMA version is kept
// as a comment in heist.cpp. Copy constructor and assignment operator still exist (deep copy).
class TileRack
{
private:
    // Tile* tiles;                // Earlier version: raw dynamic array of tiles (heap memory)
    DynamicArray<Tile> tiles;      // Template version: dynamic array of tiles
    int capacity;                  // Maximum number of letters that the rack can store
    int count;                     // Number of letters stored so far

public:
    TileRack(int cap = 15);                      // Allocates the array
    TileRack(const TileRack& other);             // Copy constructor: deep copy
    TileRack& operator=(const TileRack& other);  // Assignment operator: deep copy
    ~TileRack();                                 // Frees the array (done by DynamicArray now)

    void addTile(char letter);       // Stores a letter, throws RackFullException if the rack is full
    void removeTile(char letter);    // Removes a letter, throws RackEmptyException or LetterNotFoundException
    void display() const;            // Shows every letter with its slot number

    // Operator overloading (Module 11): merges two racks into one new rack with a space between them
    TileRack operator+(const TileRack& other) const;

    int getCount() const;
    int getCapacity() const;
    bool isFull() const;
    bool isEmpty() const;
};

// ======================= CLASSES: CHARACTER HIERARCHY =======================

class GameEngine;    // Forward declaration, needed for the friend class below

// ===== CLASS BEGIN: Character =====
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

    // Normal virtual function: people who give a Word 2 letter override it, others return an empty clue
    virtual Clue getClue() const;

    // Getters and setters (encapsulation: data is accessed only through these functions)
    string getName() const;
    string getRole() const;
    string getNationality() const;
    bool getIsCulprit() const;
    void setNationality(string nation);
    void setIsCulprit(bool culprit);

    // Friend class (Module 11): GameEngine can reach the protected data of every character directly
    friend class GameEngine;

    // Friend stream operators (Module 11)
    friend ostream& operator<<(ostream& out, const Character& person);
    friend istream& operator>>(istream& in, Character& person);
};

// ===== CLASS BEGIN: Victim =====
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
    Clue getClue() const override;             // Returns the letter, position and row of the victim

    char getLetter() const;
    int getPosition() const;
    WordRow getRow() const;
};

// ===== CLASS BEGIN: Detective =====
// Detective is the player. It stores only its own credibility, the turn count and the solved words.
// The turns are incremented by the GameEngine, the detective only stores the count.
class Detective : public Character
{
private:
    int credibility;         // Reduces with every wrong step, game is lost when it reaches zero
    int turns;               // Number of turns used so far
    string solvedWords;      // Words that the detective has formed correctly
    TileRack rack;           // Rack in which the detective keeps the collected letters

public:
    Detective(string name = "", int startCredibility = 100);

    void display() const override;
    int getMinCredibility() const override;    // Detective is never interviewed, so it returns 0

    void loseCredibility(int amount);          // Reduces credibility, it never goes below zero
    void useTurn();                            // Increases the turn count by one
    int getCredibility() const;
    int getTurns() const;
    TileRack& getRack();                       // Gives access to the rack of the detective
};

// ===== CLASS BEGIN: Suspect =====
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

    Clue getClue() const override;    // Returns the letter, position and row of the suspect

    char getLetter() const;
    int getPosition() const;
    WordRow getRow() const;
};

// ===== CLASS BEGIN: HouseHelp =====
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

// ===== CLASS BEGIN: BusinessRelations =====
// BusinessRelations groups the suspects who are related to the business of the owner.
// It inherits from Suspect with the "virtual" keyword. This is needed for the PA class:
// PA is a BusinessRelations and also a CloseRelations, and both of them are Suspects.
// Virtual inheritance makes sure that PA contains only ONE copy of Suspect (it solves the diamond problem).
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

// ===== CLASS BEGIN: CloseRelations =====
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

// ===== CLASS BEGIN: PA =====
// PA (Personal Assistant of the owner) is both a business relation and a close relation.
// It inherits from BusinessRelations and CloseRelations. Both of them inherit virtually from Suspect,
// so PA contains only ONE copy of Suspect. Without "virtual", getLetter() would be ambiguous.
class PA : public BusinessRelations, public CloseRelations
{
public:
    PA(string name = "", string nation = "", bool culprit = false,
        char letter = ' ', int pos = 0, WordRow row = WORD_2);

    void display() const override;
    int getMinCredibility() const override;    // PA can be interviewed only if credibility >= 35
};

// ======================= CLASS: Cottage =======================

// ===== CLASS BEGIN: Cottage =====
// Cottage represents the seven locations that the detective can search (Word 1 letters).
// It AGGREGATES a dynamic array of Location (named LocCache). The array is deep copied by the
// copy constructor and the assignment operator, and it is freed automatically (DynamicArray destructor).
class Cottage
{
private:
    DynamicArray<Location> LocCache;    // Dynamic array of all locations
    int locationCount;                  // Number of locations in the array

public:
    Cottage(int count = 7);
    Cottage(const Cottage& other);               // Deep copy
    Cottage& operator=(const Cottage& other);    // Deep copy
    ~Cottage();

    void setLocation(int index, string name, char letter, int pos, WordRow row);   // Fills one location
    void displayLocations() const;               // Shows all locations with their search status
    Clue getClue(int index) const;               // Gives the clue of a location without changing anything
    void markSearched(int index);                // Marks a location as searched
    bool isSearched(int index) const;            // Checks if a location was searched earlier
    string getLocationName(int index) const;
    int getLocationCount() const;
};

// ======================= CLASS: Scrabble =======================

// ===== CLASS BEGIN: Scrabble =====
// Scrabble is the virtual board with two rows (a jagged layout): 7 slots for Word 1 and 8 slots for Word 2.
// Every slot is a Tile and the position of a tile is its index in the array. It also stores the two
// target words that are loaded from clue.txt, so that the words formed by the detective can be checked.
class Scrabble
{
private:
    static const int ROW_ONE_SIZE = 7;    // Number of slots in Word 1
    static const int ROW_TWO_SIZE = 8;    // Number of slots in Word 2

    // Tile* rowOne; Tile* rowTwo;        // Earlier version: raw dynamic rows
    DynamicArray<Tile> rowOne;            // Row 1: letters from locations
    DynamicArray<Tile> rowTwo;            // Row 2: letters from characters
    string targetWordOne;                 // Correct Word 1 from clue.txt
    string targetWordTwo;                 // Correct Word 2 from clue.txt
    bool wordOneSolved;                   // True after the detective typed Word 1 correctly
    bool wordTwoSolved;                   // True after the detective typed Word 2 correctly
    string lastGuess;                     // Last word that was read with the extraction operator

    DynamicArray<Tile>& rowOf(WordRow row);                // Gives the row that matches WordRow
    const DynamicArray<Tile>& rowOf(WordRow row) const;    // Const version of rowOf
    int rowSize(WordRow row) const;                        // Number of slots in a row

public:
    Scrabble();
    Scrabble(const Scrabble& other);              // Deep copy
    Scrabble& operator=(const Scrabble& other);   // Deep copy
    ~Scrabble();

    void loadTargets(string wordOne, string wordTwo);   // Stores the words of the selected culprit
    void placeLetter(const Clue& clue);                 // Places and reveals a letter, throws on bad position or filled slot
    bool isRowComplete(WordRow row) const;              // True if every slot of the row has a letter
    string getRowText(WordRow row) const;               // Letters of the row, '_' for empty slots
    bool verifyWord(WordRow row, string guess);         // Checks a typed word, throws if the row is incomplete
    bool isSolved(WordRow row) const;
    bool bothSolved() const;
    string getTargetWord(WordRow row) const;
    string getLastGuess() const;

    friend ostream& operator<<(ostream& out, const Scrabble& board);   // Draws the board
    friend istream& operator>>(istream& in, Scrabble& board);          // Reads a typed word into lastGuess
};

// ======================= CLASS: ScoreBoard =======================

// ===== CLASS BEGIN: ScoreBoard =====
// ScoreBoard saves every finished game in leader/scoreBoard.txt and displays all previous scores.
// GameEngine only ASSOCIATES with it (uses it through a pointer, but does not own it).
class ScoreBoard
{
private:
    string fileName;    // Path of the score file

public:
    ScoreBoard(string file = "leader/scoreBoard.txt");

    // Appends one result at the end of the file
    void addScore(string detectiveName, string result, int credibility, int turnsLeft, string culprit);
    void displayAll() const;    // Shows all scores, throws FileException if the file is missing or corrupted
};

// ======================= CLASS: GameEngine =======================

// ===== CLASS BEGIN: GameEngine =====
// GameEngine runs the whole game as ONE object (Singleton pattern: the constructor is private and
// getInstance() always returns the same object). It COMPOSES Detective, Cottage, Scrabble and
// the TileRacks (they live and die with the engine) and ASSOCIATES with the ScoreBoard.
// It is a friend of Character, so it can set isCulprit directly.
class GameEngine
{
private:
    static const int LOCATION_COUNT = 7;    // Number of locations
    static const int SUSPECT_COUNT = 7;     // Number of suspects (possible culprits)
    static const int PEOPLE_COUNT = 8;      // Seven suspects plus the victim

    GameState state;                        // Current state of the game
    GameRules rules;                        // Rules read from rules.txt
    Detective detective;                    // Composition: the player
    Cottage cottage;                        // Composition: the 7 locations
    Scrabble scrabble;                      // Composition: the board
    TileRack locationRack;                  // Composition: letters collected from locations
    TileRack characterRack;                 // Composition: letters collected from people
    ScoreBoard* scoreBoard;                 // Association: the score board is owned by main()
    Character* people[PEOPLE_COUNT];        // Polymorphic array: victim + 7 suspects (heap objects)
    bool interviewed[PEOPLE_COUNT];         // True if the person was interviewed already
    string culpritRole;                     // Role of the selected culprit
    int culpritIndex;                       // Index of the culprit inside people[]

    GameEngine();                                       // Private constructor (Singleton)
    GameEngine(const GameEngine& other) = delete;       // Copying a singleton is not allowed
    GameEngine& operator=(const GameEngine& other) = delete;

    // ---- setup and files ----
    void setDefaultRules();                             // Fills the rules with default values
    bool loadRules();                                   // Reads rules.txt, false if missing or corrupt
    void readClueFile(string roles[], string wordOne[], string wordTwo[]);
    void readLocationFile(string names[], int positions[], char letters[][SUSPECT_COUNT]);
    void readCharacterFile(string names[], string roles[], string nations[], int positions[], char letters[][SUSPECT_COUNT]);
    Character* createCharacter(string name, string role, string nation, char letter, int pos);
    string askDetectiveName();                          // Asks and validates the name of the detective
    void freePeople();                                  // Deletes all character objects
    void startNewGame();                                // Module 2: prepares a fresh game

    // ---- menus ----
    void mainMenuStep();                                // One round of the main menu
    void playStep();                                    // One round of the game menu
    void showMainMenu() const;
    void showGameMenu() const;
    void showAbout() const;
    void showRules();
    void showClue(const Clue& clue) const;              // Prints a clue in a readable sentence

    // ---- game actions ----
    void visitLocation();
    void interviewPerson();
    void viewBoard() const;
    void formWord();
    void accuseSuspect();
    void applyWrongTurn();                              // -5 credibility and one turn
    void checkEndConditions();                          // Sets GAME_LOST if credibility or turns are finished
    void finishGame();                                  // Shows result, saves score, returns to main menu

public:
    static GameEngine& getInstance();                   // Gives the only object of the engine
    ~GameEngine();

    void setScoreBoard(ScoreBoard* board);              // Connects the score board (association)
    GameState getState() const;
    void showWelcome() const;                           // Title and short scenario
    void step();                                        // Runs one step of the game, depending on the state
};

#endif
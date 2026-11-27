// heist.cpp
// Implementation file of The Locker Heist game.
// It contains the function bodies of all classes declared in heist.h.

#include "heist.h"
#include <fstream>
#include <cstdlib>    // rand, srand, exit
#include <ctime>      // time, for the random culprit and the date
#include <cctype>     // toupper

// ======================= UTILITY FUNCTIONS =======================

// Removes spaces, tabs and line endings from both sides of a text.
// It is used for every line that is read from a text file.
string trimText(const string& text)
{
    int start = 0;
    int end = (int)text.length() - 1;
    while (start <= end && (text[start] == ' ' || text[start] == '\t' || text[start] == '\r' || text[start] == '\n'))
    {
        start++;
    }
    while (end >= start && (text[end] == ' ' || text[end] == '\t' || text[end] == '\r' || text[end] == '\n'))
    {
        end--;
    }
    return text.substr(start, end - start + 1);
}

// Splits a line at the separator and trims every part. It returns the real number of parts,
// but stores only maxParts of them, so the caller can detect a corrupted line.
int splitLine(const string& line, char separator, string parts[], int maxParts)
{
    int total = 0;
    string current = "";
    for (int i = 0; i <= (int)line.length(); i++)
    {
        if (i == (int)line.length() || line[i] == separator)
        {
            if (total < maxParts)
            {
                parts[total] = trimText(current);
            }
            total++;
            current = "";
        }
        else
        {
            current += line[i];
        }
    }
    return total;
}

// Converts a text like "-5" or "25" to an int. It returns false when the text is not a number.
bool parseInteger(const string& text, int& value)
{
    string clean = trimText(text);
    if (clean.empty())
    {
        return false;
    }
    int index = 0;
    bool negative = false;
    if (clean[0] == '-' || clean[0] == '+')
    {
        negative = (clean[0] == '-');
        index = 1;
    }
    if (index >= (int)clean.length() || clean.length() > 9)
    {
        return false;
    }
    int result = 0;
    for (; index < (int)clean.length(); index++)
    {
        if (clean[index] < '0' || clean[index] > '9')
        {
            return false;
        }
        result = result * 10 + (clean[index] - '0');
    }
    value = negative ? -result : result;
    return true;
}

// Converts an int to text without using library helpers.
// It is needed to build the messages of the exceptions.
string numberToText(int number)
{
    if (number == 0)
    {
        return "0";
    }
    bool negative = number < 0;
    if (negative)
    {
        number = -number;
    }
    string digits = "";
    while (number > 0)
    {
        digits = char('0' + number % 10) + digits;
        number = number / 10;
    }
    return negative ? "-" + digits : digits;
}

// Converts every letter of a text to a capital letter
string toUpperText(const string& text)
{
    string result = text;
    for (int i = 0; i < (int)result.length(); i++)
    {
        result[i] = (char)toupper((unsigned char)result[i]);
    }
    return result;
}

// Adds spaces on the right until the text has the given width (used to align the score table)
string padText(const string& text, int width)
{
    string result = text;
    while ((int)result.length() < width)
    {
        result += ' ';
    }
    return result;
}

// Writes a number with two digits, e.g. 5 becomes 05 (helper for the date and the score file)
static string twoDigits(int number)
{
    return (number < 10 ? "0" : "") + numberToText(number);
}

// Gives today's date in the format dd-mm-yy, as used in leader/scoreBoard.txt
string todayDate()
{
    time_t now = time(0);
    tm local;
#ifdef _MSC_VER
    localtime_s(&local, &now);       // Safe version for Visual Studio
#else
    local = *localtime(&now);
#endif
    return twoDigits(local.tm_mday) + "-" + twoDigits(local.tm_mon + 1) + "-" + twoDigits(local.tm_year % 100);
}

// Clears the console window (command depends on the operating system)
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Waits until the player presses Enter, so that the output stays visible
void pauseScreen()
{
    cout << "\nPress Enter to continue...";
    string ignored;
    getline(cin, ignored);
}

// Reads one full line. If the input stream is closed, the game ends cleanly instead of looping forever.
string readLine(const string& prompt)
{
    cout << prompt;
    string line;
    if (!getline(cin, line))
    {
        cout << "\nInput closed. The game ends here." << endl;
        exit(0);
    }
    return trimText(line);
}

// Reads a number between low and high. Anything else throws InvalidInputException (caught in main)
int readNumber(const string& prompt, int low, int high)
{
    string line = readLine(prompt);
    int value = 0;
    if (!parseInteger(line, value) || value < low || value > high)
    {
        throw InvalidInputException("Please enter a number between " + numberToText(low) + " and " + numberToText(high) + ".");
    }
    return value;
}

// Reads Y or N. Anything else throws InvalidInputException (caught in main)
bool readYesNo(const string& prompt)
{
    string line = toUpperText(readLine(prompt));
    if (line == "Y" || line == "YES")
    {
        return true;
    }
    if (line == "N" || line == "NO")
    {
        return false;
    }
    throw InvalidInputException("Please answer with Y or N.");
}

// ======================= OPERATORS FOR STRUCTURES =======================

// Compares the letters of two tiles
bool operator==(const Tile& first, const Tile& second)
{
    return first.tileLetter == second.tileLetter;
}

// Compares two clues completely
bool operator==(const Clue& first, const Clue& second)
{
    return first.clueLetter == second.clueLetter && first.cluePosition == second.cluePosition
        && first.clueRow == second.clueRow;
}

// Prints a tile as [A], or [_] if it is still empty
ostream& operator<<(ostream& out, const Tile& tile)
{
    out << "[" << (tile.filled ? tile.tileLetter : '_') << "]";
    return out;
}

// Reads one letter into a tile and marks the tile as filled
istream& operator>>(istream& in, Tile& tile)
{
    in >> tile.tileLetter;
    tile.filled = true;
    return in;
}

// Prints the name of a location and whether it was searched (the hidden letter is not revealed)
ostream& operator<<(ostream& out, const Location& place)
{
    out << place.locName << (place.locSearchFlag ? " (Searched)" : " (Not searched)");
    return out;
}

// Reads the name of a location
istream& operator>>(istream& in, Location& place)
{
    in >> place.locName;
    return in;
}

// ======================= EXCEPTIONS =======================

// The base exception stores the message that is shown to the player
GameException::GameException(string msg) : message(msg)
{
}

GameException::~GameException()
{
}

// Returns the message of the exception
string GameException::what() const { return message; }

RackFullException::RackFullException()
    : GameException("HEIST ERROR: The tile rack is full, no more letters can be added.")
{
}

RackEmptyException::RackEmptyException()
    : GameException("HEIST ERROR: The tile rack is empty, there is no letter to remove.")
{
}

LetterNotFoundException::LetterNotFoundException(char letter)
    : GameException(string("HEIST ERROR: The letter ") + letter + " is not in the tile rack.")
{
}

InvalidPositionException::InvalidPositionException(int position)
    : GameException("HEIST ERROR: Position " + numberToText(position) + " is not valid on the board.")
{
}

SlotFilledException::SlotFilledException(int position)
    : GameException("HEIST ERROR: Position " + numberToText(position) + " already has a letter.")
{
}

InvalidWordException::InvalidWordException(string reason)
    : GameException("HEIST ERROR: Invalid word. " + reason)
{
}

InvalidInputException::InvalidInputException(string reason)
    : GameException("HEIST ERROR: Invalid input. " + reason)
{
}

LowCredibilityException::LowCredibilityException(string who, int needed, int have)
    : GameException("HEIST ERROR: " + who + " refuses to talk. Credibility " + numberToText(needed)
        + " is needed, but you have only " + numberToText(have) + ".")
{
}

FileException::FileException(string fileName, string problem)
    : GameException("HEIST ERROR: The file " + fileName + " " + problem + ".")
{
}

// ======================= CLASS: TileRack =======================

// Constructor creates the DynamicArray<Tile> and marks every tile as empty.
// A capacity below one is corrected to one.
TileRack::TileRack(int cap) : tiles(cap < 1 ? 1 : cap)
{
    if (cap < 1)
    {
        cap = 1;
    }
    capacity = cap;
    count = 0;
    for (int i = 0; i < capacity; i++)
    {
        tiles[i].tileLetter = ' ';
        tiles[i].filled = false;
    }
}

// Copy constructor: DynamicArray copies its own memory (deep copy), so two racks never share memory
TileRack::TileRack(const TileRack& other) : tiles(other.tiles), capacity(other.capacity), count(other.count)
{
}

// Assignment operator: self-assignment is checked, then the DynamicArray makes a deep copy
TileRack& TileRack::operator=(const TileRack& other)
{
    if (this != &other)
    {
        tiles = other.tiles;
        capacity = other.capacity;
        count = other.count;
    }
    return *this;
}

// Destructor: the DynamicArray member frees the heap memory itself, so there is no leak
TileRack::~TileRack()
{
}

// Stores a letter in the first free slot. A full rack throws an exception
void TileRack::addTile(char letter)
{
    if (isFull())
    {
        throw RackFullException();
    }
    tiles[count].tileLetter = letter;
    tiles[count].filled = true;
    count++;
}

// Removes the first tile that contains the letter and moves the next tiles one slot to the left.
// An empty rack or a missing letter throws an exception
void TileRack::removeTile(char letter)
{
    if (isEmpty())
    {
        throw RackEmptyException();
    }
    int found = -1;
    for (int i = 0; i < count && found == -1; i++)
    {
        if (tiles[i].tileLetter == letter)
        {
            found = i;
        }
    }
    if (found == -1)
    {
        throw LetterNotFoundException(letter);
    }
    for (int i = found; i < count - 1; i++)
    {
        tiles[i] = tiles[i + 1];
    }
    count--;
    tiles[count].tileLetter = ' ';
    tiles[count].filled = false;
}

// Shows every letter with its slot number, e.g. [1:S] [2:P]
void TileRack::display() const
{
    cout << "Rack: ";
    if (isEmpty())
    {
        cout << "(empty)" << endl;
        return;
    }
    for (int i = 0; i < count; i++)
    {
        cout << "[" << (i + 1) << ":" << tiles[i].tileLetter << "] ";
    }
    cout << endl;
}

// Operator overloading: rack1 + rack2 creates a NEW rack with the letters of the first rack,
// then one space tile, then the letters of the second rack. Both original racks stay unchanged.
TileRack TileRack::operator+(const TileRack& other) const
{
    TileRack merged(capacity + other.capacity + 1);
    for (int i = 0; i < count; i++)
    {
        merged.addTile(tiles[i].tileLetter);
    }
    merged.addTile(' ');
    for (int i = 0; i < other.count; i++)
    {
        merged.addTile(other.tiles[i].tileLetter);
    }
    return merged;
}

int TileRack::getCount() const { return count; }
int TileRack::getCapacity() const { return capacity; }
bool TileRack::isFull() const { return count == capacity; }
bool TileRack::isEmpty() const { return count == 0; }

/* ----- EARLIER VERSION OF TileRack (Module 5, raw DMA). Kept as a comment because Module 8 replaced it
   with DynamicArray<Tile>. The header had "Tile* tiles;" instead of "DynamicArray<Tile> tiles;".

TileRack::TileRack(int cap)
{
    if (cap < 1)
    {
        cap = 1;
    }
    capacity = cap;
    count = 0;
    tiles = new Tile[capacity];
    for (int i = 0; i < capacity; i++)
    {
        tiles[i].tileLetter = ' ';
        tiles[i].filled = false;
    }
}

TileRack::TileRack(const TileRack& other)
{
    capacity = other.capacity;
    count = other.count;
    tiles = new Tile[capacity];
    for (int i = 0; i < capacity; i++)
    {
        tiles[i] = other.tiles[i];
    }
}

TileRack& TileRack::operator=(const TileRack& other)
{
    if (this != &other)
    {
        delete[] tiles;
        capacity = other.capacity;
        count = other.count;
        tiles = new Tile[capacity];
        for (int i = 0; i < capacity; i++)
        {
            tiles[i] = other.tiles[i];
        }
    }
    return *this;
}

TileRack::~TileRack()
{
    delete[] tiles;
}
   ----- END OF EARLIER VERSION ----- */

   // ======================= CLASS: Character =======================

   // Constructor utilizes an initializer list to set all common attributes of a person
Character::Character(string name, string role, string nation, bool culprit)
    : characterName(name), characterRole(role), nationality(nation), isCulprit(culprit)
{
}

// Virtual destructor: derived objects are destroyed correctly even through a base pointer
Character::~Character()
{
}

// Prints the information that is common for all characters (no new line, derived classes may add more)
void Character::printBasicInfo() const
{
    cout << "Name: " << characterName << " | Role: " << characterRole
        << " | Nationality: " << nationality;
}

// Default clue: a person without a Word 2 letter (the detective) gives an empty clue
Clue Character::getClue() const
{
    Clue empty;
    empty.clueLetter = ' ';
    empty.cluePosition = 0;
    empty.clueRow = WORD_2;
    return empty;
}

// Getters return the stored values without allowing direct access to the data
string Character::getName() const { return characterName; }
string Character::getRole() const { return characterRole; }
string Character::getNationality() const { return nationality; }
bool Character::getIsCulprit() const { return isCulprit; }

// Setters change the stored values in a controlled way
void Character::setNationality(string nation) { nationality = nation; }
void Character::setIsCulprit(bool culprit) { isCulprit = culprit; }

// Friend function: prints a character without revealing who the culprit is
ostream& operator<<(ostream& out, const Character& person)
{
    out << person.characterName << " (" << person.characterRole << ", " << person.nationality << ")";
    return out;
}

// Friend function: reads the name of a character
istream& operator>>(istream& in, Character& person)
{
    in >> person.characterName;
    return in;
}

// ======================= CLASS: Victim =======================

// The role of the victim is always "Victim" and he is never the culprit
Victim::Victim(string name, string nation, char letter, int pos, WordRow row)
    : Character(name, "Victim", nation, false),
    letterClueChar(letter), boardPosChar(pos), clueRowChar(row)
{
}

// Overrides Character::display() to show the victim on the screen
void Victim::display() const
{
    printBasicInfo();
    cout << endl;
}

// Rule of the game: the victim can be interviewed only if credibility is 80 or more
int Victim::getMinCredibility() const { return 80; }

// Packs the letter, position and row of the victim into a Clue (the carrier of data)
Clue Victim::getClue() const
{
    Clue clue;
    clue.clueLetter = letterClueChar;
    clue.cluePosition = boardPosChar;
    clue.clueRow = clueRowChar;
    return clue;
}

char Victim::getLetter() const { return letterClueChar; }
int Victim::getPosition() const { return boardPosChar; }
WordRow Victim::getRow() const { return clueRowChar; }

// ======================= CLASS: Detective =======================

// The detective starts with full credibility, zero turns and an empty rack of 15 letters
Detective::Detective(string name, int startCredibility)
    : Character(name, "Detective", "", false),
    credibility(startCredibility), turns(0), solvedWords(""), rack(15)
{
}

// Overrides Character::display() to also show credibility and turns of the detective
void Detective::display() const
{
    printBasicInfo();
    cout << " | Credibility: " << credibility << " | Turns: " << turns << endl;
}

// The detective is the player and is never interviewed, so no credibility is required
int Detective::getMinCredibility() const { return 0; }

// Reduces credibility for a wrong step. It is kept at zero or above
void Detective::loseCredibility(int amount)
{
    credibility = credibility - amount;
    if (credibility < 0)
    {
        credibility = 0;
    }
}

// Called by the GameEngine: the detective only stores the turn count
void Detective::useTurn() { turns++; }

int Detective::getCredibility() const { return credibility; }
int Detective::getTurns() const { return turns; }

// Returns the rack by reference, so that the GameEngine can add letters to the rack of the detective
TileRack& Detective::getRack() { return rack; }

// ======================= CLASS: Suspect =======================

// Suspect passes the common data to Character and stores its own clue data
Suspect::Suspect(string name, string role, string nation, bool culprit, char letter, int pos, WordRow row)
    : Character(name, role, nation, culprit),
    letterClueChar(letter), boardPosChar(pos), clueRowChar(row)
{
}

// Virtual destructor, so that derived suspects are destroyed correctly through a base pointer
Suspect::~Suspect()
{
}

// Packs the letter, position and row of the suspect into a Clue
Clue Suspect::getClue() const
{
    Clue clue;
    clue.clueLetter = letterClueChar;
    clue.cluePosition = boardPosChar;
    clue.clueRow = clueRowChar;
    return clue;
}

char Suspect::getLetter() const { return letterClueChar; }
int Suspect::getPosition() const { return boardPosChar; }
WordRow Suspect::getRow() const { return clueRowChar; }

// ======================= CLASS: HouseHelp =======================

HouseHelp::HouseHelp(string name, string role, string nation, bool culprit, char letter, int pos, WordRow row)
    : Suspect(name, role, nation, culprit, letter, pos, row)
{
}

// Overrides Character::display() and shows that the person belongs to the house help
void HouseHelp::display() const
{
    printBasicInfo();
    cout << " | House Help" << endl;
}

// Rule of the game: house help can be interviewed at any time, so no credibility is required
int HouseHelp::getMinCredibility() const { return 0; }

// ======================= CLASSES: Cook, Guard, Servant =======================

// Each constructor fixes the role of the object and passes the rest of the data to HouseHelp
Cook::Cook(string name, string nation, bool culprit, char letter, int pos, WordRow row)
    : HouseHelp(name, "Cook", nation, culprit, letter, pos, row)
{
}

void Cook::display() const
{
    printBasicInfo();
    cout << " | Works in the kitchen" << endl;
}

Guard::Guard(string name, string nation, bool culprit, char letter, int pos, WordRow row)
    : HouseHelp(name, "Guard", nation, culprit, letter, pos, row)
{
}

void Guard::display() const
{
    printBasicInfo();
    cout << " | Guards the cottage" << endl;
}

Servant::Servant(string name, string nation, bool culprit, char letter, int pos, WordRow row)
    : HouseHelp(name, "Servant", nation, culprit, letter, pos, row)
{
}

void Servant::display() const
{
    printBasicInfo();
    cout << " | Serves the guests" << endl;
}

// ======================= CLASS: BusinessRelations =======================

// BusinessRelations is abstract. The Suspect(...) call below is ignored when a Partner is created,
// because the most derived class (Partner, and PA later) initializes the virtual base class Suspect itself.
BusinessRelations::BusinessRelations(string name, string role, string nation, bool culprit, char letter, int pos, WordRow row)
    : Suspect(name, role, nation, culprit, letter, pos, row)
{
}

// ======================= CLASS: Partner =======================

// Rule of virtual inheritance: Partner is the most derived class, so it must call
// the constructor of the virtual base class Suspect directly.
Partner::Partner(string name, string role, string nation, bool culprit, char letter, int pos, WordRow row)
    : Suspect(name, role, nation, culprit, letter, pos, row),
    BusinessRelations(name, role, nation, culprit, letter, pos, row)
{
}

// Overrides Character::display() to show that the person is a business relation
void Partner::display() const
{
    printBasicInfo();
    cout << " | Business Relation" << endl;
}

// Rule of the game: a partner can be interviewed only if credibility is 50 or more
int Partner::getMinCredibility() const { return 50; }

// ======================= CLASS: CloseRelations =======================

// CloseRelations is abstract. As in BusinessRelations, the Suspect(...) call is ignored
// when a Child is created, because Child initializes the virtual base class itself.
CloseRelations::CloseRelations(string name, string role, string nation, bool culprit, char letter, int pos, WordRow row)
    : Suspect(name, role, nation, culprit, letter, pos, row)
{
}

// ======================= CLASS: Child =======================

// The role of the child is always "Child". It also calls the virtual base class Suspect directly.
Child::Child(string name, string nation, bool culprit, char letter, int pos, WordRow row)
    : Suspect(name, "Child", nation, culprit, letter, pos, row),
    CloseRelations(name, "Child", nation, culprit, letter, pos, row)
{
}

// Overrides Character::display() to show that the person is a close relation
void Child::display() const
{
    printBasicInfo();
    cout << " | Close Relation" << endl;
}

// Rule of the game: the child can be interviewed only if credibility is 75 or more
int Child::getMinCredibility() const { return 75; }

// ======================= CLASS: PA =======================

// PA is the most derived class of the diamond, so it calls the constructor of the
// virtual base class Suspect directly. Then it calls both of its parent classes.
PA::PA(string name, string nation, bool culprit, char letter, int pos, WordRow row)
    : Suspect(name, "PA", nation, culprit, letter, pos, row),
    BusinessRelations(name, "PA", nation, culprit, letter, pos, row),
    CloseRelations(name, "PA", nation, culprit, letter, pos, row)
{
}

// PA must override display() itself, because both parents leave it abstract
void PA::display() const
{
    printBasicInfo();
    cout << " | Business Relation and Close Relation" << endl;
}

// Rule of the game: the PA can be interviewed only if credibility is 35 or more
int PA::getMinCredibility() const { return 35; }

// ======================= CLASS: Cottage =======================

// Constructor creates the DynamicArray of locations and gives every location empty starting values.
// A count below one is corrected to one.
Cottage::Cottage(int count) : LocCache(count < 1 ? 1 : count), locationCount(count < 1 ? 1 : count)
{
    for (int i = 0; i < locationCount; i++)
    {
        LocCache[i].locName = "";
        LocCache[i].letterClueLoc = ' ';
        LocCache[i].boardPosLoc = 0;
        LocCache[i].locRow = WORD_1;
        LocCache[i].locSearchFlag = false;
    }
}

// Copy constructor: the DynamicArray makes a deep copy of all locations
Cottage::Cottage(const Cottage& other) : LocCache(other.LocCache), locationCount(other.locationCount)
{
}

// Assignment operator: checks self-assignment and then makes a deep copy
Cottage& Cottage::operator=(const Cottage& other)
{
    if (this != &other)
    {
        LocCache = other.LocCache;
        locationCount = other.locationCount;
    }
    return *this;
}

// Destructor: the DynamicArray member deletes the array of locations itself
Cottage::~Cottage()
{
}

// Fills one location with the data that is read from locations.txt
void Cottage::setLocation(int index, string name, char letter, int pos, WordRow row)
{
    LocCache[index].locName = name;
    LocCache[index].letterClueLoc = letter;
    LocCache[index].boardPosLoc = pos;
    LocCache[index].locRow = row;
    LocCache[index].locSearchFlag = false;
}

// Shows every location with its number and search status, e.g. [3] Study_Room (Searched)
void Cottage::displayLocations() const
{
    for (int i = 0; i < locationCount; i++)
    {
        cout << "   [" << (i + 1) << "] " << LocCache[i] << endl;
    }
}

// Gives the clue of a location. It does not mark the location as searched
Clue Cottage::getClue(int index) const
{
    Clue clue;
    clue.clueLetter = LocCache[index].letterClueLoc;
    clue.cluePosition = LocCache[index].boardPosLoc;
    clue.clueRow = LocCache[index].locRow;
    return clue;
}

// Marks a location as searched
void Cottage::markSearched(int index) { LocCache[index].locSearchFlag = true; }

// Checks if a location has been searched earlier or not
bool Cottage::isSearched(int index) const { return LocCache[index].locSearchFlag; }

string Cottage::getLocationName(int index) const { return LocCache[index].locName; }
int Cottage::getLocationCount() const { return locationCount; }

// ======================= CLASS: Scrabble =======================

// Constructor creates both rows (7 and 8 slots) and marks every tile as empty
Scrabble::Scrabble()
    : rowOne(ROW_ONE_SIZE), rowTwo(ROW_TWO_SIZE), targetWordOne(""), targetWordTwo(""),
    wordOneSolved(false), wordTwoSolved(false), lastGuess("")
{
    for (int i = 0; i < ROW_ONE_SIZE; i++)
    {
        rowOne[i].tileLetter = ' ';
        rowOne[i].filled = false;
    }
    for (int i = 0; i < ROW_TWO_SIZE; i++)
    {
        rowTwo[i].tileLetter = ' ';
        rowTwo[i].filled = false;
    }
}

// Copy constructor: both DynamicArrays and all words are copied (deep copy)
Scrabble::Scrabble(const Scrabble& other)
    : rowOne(other.rowOne), rowTwo(other.rowTwo), targetWordOne(other.targetWordOne),
    targetWordTwo(other.targetWordTwo), wordOneSolved(other.wordOneSolved),
    wordTwoSolved(other.wordTwoSolved), lastGuess(other.lastGuess)
{
}

// Assignment operator: checks self-assignment and then makes a deep copy of everything
Scrabble& Scrabble::operator=(const Scrabble& other)
{
    if (this != &other)
    {
        rowOne = other.rowOne;
        rowTwo = other.rowTwo;
        targetWordOne = other.targetWordOne;
        targetWordTwo = other.targetWordTwo;
        wordOneSolved = other.wordOneSolved;
        wordTwoSolved = other.wordTwoSolved;
        lastGuess = other.lastGuess;
    }
    return *this;
}

// Destructor: the DynamicArray members free both rows themselves
Scrabble::~Scrabble()
{
}

// Returns the row that belongs to the WordRow enum value
DynamicArray<Tile>& Scrabble::rowOf(WordRow row)
{
    if (row == WORD_1)
    {
        return rowOne;
    }
    return rowTwo;
}

// Const version of rowOf, used by the const functions
const DynamicArray<Tile>& Scrabble::rowOf(WordRow row) const
{
    if (row == WORD_1)
    {
        return rowOne;
    }
    return rowTwo;
}

// Gives the number of slots of a row (7 for Word 1, 8 for Word 2)
int Scrabble::rowSize(WordRow row) const
{
    return (row == WORD_1) ? ROW_ONE_SIZE : ROW_TWO_SIZE;
}

// Stores the two correct words of the selected culprit (loaded from clue.txt)
void Scrabble::loadTargets(string wordOne, string wordTwo)
{
    targetWordOne = toUpperText(wordOne);
    targetWordTwo = toUpperText(wordTwo);
}

// Places the letter of a clue on the board. Positions start at 1, the array index is position - 1.
// A wrong position or an already filled slot throws an exception
void Scrabble::placeLetter(const Clue& clue)
{
    int size = rowSize(clue.clueRow);
    if (clue.cluePosition < 1 || clue.cluePosition > size)
    {
        throw InvalidPositionException(clue.cluePosition);
    }
    Tile& slot = rowOf(clue.clueRow)[clue.cluePosition - 1];
    if (slot.filled)
    {
        throw SlotFilledException(clue.cluePosition);
    }
    slot.tileLetter = clue.clueLetter;
    slot.filled = true;
}

// A row is complete when every one of its slots has a letter
bool Scrabble::isRowComplete(WordRow row) const
{
    for (int i = 0; i < rowSize(row); i++)
    {
        if (!rowOf(row)[i].filled)
        {
            return false;
        }
    }
    return true;
}

// Gives the letters of a row as text. Empty slots are shown as '_'
string Scrabble::getRowText(WordRow row) const
{
    string text = "";
    for (int i = 0; i < rowSize(row); i++)
    {
        text += rowOf(row)[i].filled ? rowOf(row)[i].tileLetter : '_';
    }
    return text;
}

// Compares the typed word with the target word tile by tile (using operator== of Tile).
// It throws if the row is not complete yet, and remembers a correct word as solved.
bool Scrabble::verifyWord(WordRow row, string guess)
{
    if (!isRowComplete(row))
    {
        throw InvalidWordException("The row is not complete yet.");
    }
    string target = getTargetWord(row);
    guess = toUpperText(guess);
    if (guess.length() != target.length())
    {
        return false;
    }
    for (int i = 0; i < (int)target.length(); i++)
    {
        Tile typed;
        typed.tileLetter = guess[i];
        typed.filled = true;
        Tile correct;
        correct.tileLetter = target[i];
        correct.filled = true;
        if (!(typed == correct))
        {
            return false;
        }
    }
    if (row == WORD_1)
    {
        wordOneSolved = true;
    }
    else
    {
        wordTwoSolved = true;
    }
    return true;
}

bool Scrabble::isSolved(WordRow row) const { return (row == WORD_1) ? wordOneSolved : wordTwoSolved; }
bool Scrabble::bothSolved() const { return wordOneSolved && wordTwoSolved; }
string Scrabble::getTargetWord(WordRow row) const { return (row == WORD_1) ? targetWordOne : targetWordTwo; }
string Scrabble::getLastGuess() const { return lastGuess; }

// Friend function: draws both rows with their positions below the tiles
ostream& operator<<(ostream& out, const Scrabble& board)
{
    out << "  Word 1 (7 letters): ";
    for (int i = 0; i < 7; i++)
    {
        out << board.rowOne[i] << " ";
    }
    out << endl << "  Position          : ";
    for (int i = 0; i < 7; i++)
    {
        out << " " << (i + 1) << "  ";
    }
    out << endl << endl << "  Word 2 (8 letters): ";
    for (int i = 0; i < 8; i++)
    {
        out << board.rowTwo[i] << " ";
    }
    out << endl << "  Position          : ";
    for (int i = 0; i < 8; i++)
    {
        out << " " << (i + 1) << "  ";
    }
    out << endl;
    return out;
}

// Friend function: reads a typed word and keeps it inside the board object
istream& operator>>(istream& in, Scrabble& board)
{
    in >> board.lastGuess;
    return in;
}

// ======================= CLASS: ScoreBoard =======================

ScoreBoard::ScoreBoard(string file) : fileName(file)
{
}

// Appends one line to the score file. If the file is new, the heading line is written first.
// If the old last line has no line ending, one is added so that two results never share a line.
void ScoreBoard::addScore(string detectiveName, string result, int credibility, int turnsLeft, string culprit)
{
    bool exists = false;
    bool needNewLine = false;
    {
        ifstream test(fileName.c_str(), ios::binary);
        exists = test.good();
        if (exists)
        {
            test.seekg(0, ios::end);
            if (test.tellg() > 0)
            {
                test.seekg(-1, ios::end);
                char last = ' ';
                test.get(last);
                needNewLine = (last != '\n');
            }
        }
    }

    ofstream out(fileName.c_str(), ios::app);
    if (!out)
    {
        throw FileException(fileName, "could not be opened for writing (does the folder exist?)");
    }
    if (!exists)
    {
        out << "Detective|Result|Credibility|Turns_Left|Culprit|Date" << endl;
    }
    else if (needNewLine)
    {
        out << endl;
    }

    // Spaces inside the culprit role (Partner 1) become underscores, as in the sample file
    for (int i = 0; i < (int)culprit.length(); i++)
    {
        if (culprit[i] == ' ')
        {
            culprit[i] = '_';
        }
    }
    out << detectiveName << "|" << result << "|" << credibility << "|" << twoDigits(turnsLeft)
        << "|" << culprit << "|" << todayDate() << endl;
}

// Reads the file and prints every score as a table. A missing file or a broken line throws FileException
void ScoreBoard::displayAll() const
{
    ifstream in(fileName.c_str());
    if (!in)
    {
        throw FileException(fileName, "was not found");
    }

    string line;
    string parts[6];
    if (!getline(in, line) || splitLine(trimText(line), '|', parts, 6) != 6)
    {
        throw FileException(fileName, "is corrupted (heading line is wrong)");
    }

    cout << "  " << padText("Detective", 16) << padText("Result", 8) << padText("Cred.", 7)
        << padText("Turns Left", 12) << padText("Culprit", 12) << "Date" << endl;
    cout << "  ------------------------------------------------------------------" << endl;

    int rows = 0;
    while (getline(in, line))
    {
        line = trimText(line);
        if (line.empty())
        {
            continue;
        }
        if (splitLine(line, '|', parts, 6) != 6)
        {
            throw FileException(fileName, "is corrupted (a score line is wrong)");
        }
        cout << "  " << padText(parts[0], 16) << padText(parts[1], 8) << padText(parts[2], 7)
            << padText(parts[3], 12) << padText(parts[4], 12) << parts[5] << endl;
        rows++;
    }
    if (rows == 0)
    {
        cout << "  (no games have been played yet)" << endl;
    }
}

// ======================= CLASS: GameEngine =======================

// Reads a data file: the heading line is skipped and empty lines are ignored.
// Outer '|' characters are removed, so "| a | b |" and "a|b" both work.
// A missing file throws FileException. It returns the number of data lines.
static int readDataLines(const string& fileName, string lines[], int maxLines)
{
    ifstream in(fileName.c_str());
    if (!in)
    {
        throw FileException(fileName, "was not found");
    }
    string line;
    int count = 0;
    bool headingSkipped = false;
    while (getline(in, line))
    {
        line = trimText(line);
        if (line.empty())
        {
            continue;
        }
        if (!headingSkipped)
        {
            headingSkipped = true;
            continue;
        }
        if (line[0] == '|')
        {
            line = line.substr(1);
        }
        if (!line.empty() && line[line.length() - 1] == '|')
        {
            line = line.substr(0, line.length() - 1);
        }
        if (count < maxLines)
        {
            lines[count] = line;
        }
        count++;
    }
    return count;
}

// Private constructor (Singleton). It prepares the empty game objects and starts the random generator
GameEngine::GameEngine()
    : state(GAME_MENU), detective("", 100), locationRack(LOCATION_COUNT), characterRack(PEOPLE_COUNT),
    scoreBoard(0), culpritRole(""), culpritIndex(-1)
{
    setDefaultRules();
    for (int i = 0; i < PEOPLE_COUNT; i++)
    {
        people[i] = 0;
        interviewed[i] = false;
    }
    srand((unsigned)time(0));
}

// Destructor deletes every character object through the base pointer (virtual destructor)
GameEngine::~GameEngine()
{
    freePeople();
}

// Singleton access: the only GameEngine object is created the first time and reused afterwards
GameEngine& GameEngine::getInstance()
{
    static GameEngine instance;
    return instance;
}

// Connects the score board that is owned by main() (association, not ownership)
void GameEngine::setScoreBoard(ScoreBoard* board) { scoreBoard = board; }

GameState GameEngine::getState() const { return state; }

// Deletes all people and sets the pointers to zero, so nothing dangles and nothing is deleted twice
void GameEngine::freePeople()
{
    for (int i = 0; i < PEOPLE_COUNT; i++)
    {
        delete people[i];
        people[i] = 0;
        interviewed[i] = false;
    }
}

// Default rules are used when rules.txt is missing or corrupt
void GameEngine::setDefaultRules()
{
    rules.maxTurns = 25;
    rules.startCredibility = 100;
    rules.wrongTurnPenalty = 5;
    rules.wrongWordPenalty = 10;
    rules.winCondition = "Accuse_Correct_Culprit";
    rules.loseCondition = "Credibility_Zero_Or_Turns_Exhausted";
}

// Reads rules.txt (Key=Value lines). It returns false and keeps the defaults when the file
// is missing or any line is corrupt, so the game can always continue.
bool GameEngine::loadRules()
{
    setDefaultRules();
    ifstream in("rules.txt");
    if (!in)
    {
        return false;
    }
    GameRules loaded = rules;
    string line;
    while (getline(in, line))
    {
        line = trimText(line);
        if (line.empty())
        {
            continue;
        }
        int equalSign = (int)line.find('=');
        if (equalSign < 1)
        {
            setDefaultRules();
            return false;
        }
        string key = trimText(line.substr(0, equalSign));
        string value = trimText(line.substr(equalSign + 1));
        int number = 0;
        if (key == "Max_Turns" || key == "Starting_Credibility" || key == "Every_Wrong_Turn" || key == "Every_Wrong_Word")
        {
            if (!parseInteger(value, number))
            {
                setDefaultRules();
                return false;
            }
            if (number < 0)
            {
                number = -number;    // Penalties are written as -5 in the file, they are stored as 5
            }
            if (key == "Max_Turns" && number > 0) loaded.maxTurns = number;
            else if (key == "Starting_Credibility" && number > 0) loaded.startCredibility = number;
            else if (key == "Every_Wrong_Turn") loaded.wrongTurnPenalty = number;
            else if (key == "Every_Wrong_Word") loaded.wrongWordPenalty = number;
            else
            {
                setDefaultRules();
                return false;
            }
        }
        else if (key == "Win_Condition")
        {
            loaded.winCondition = value;
        }
        else if (key == "Lose_Condition")
        {
            loaded.loseCondition = value;
        }
    }
    rules = loaded;
    return true;
}

// Reads clue.txt: 7 lines of Culprit_Role|Word_1|Word_2. Words must have 7 and 8 letters
void GameEngine::readClueFile(string roles[], string wordOne[], string wordTwo[])
{
    string lines[SUSPECT_COUNT];
    int count = readDataLines("clue.txt", lines, SUSPECT_COUNT);
    if (count != SUSPECT_COUNT)
    {
        throw FileException("clue.txt", "is corrupted (it must have 7 data lines)");
    }
    for (int i = 0; i < SUSPECT_COUNT; i++)
    {
        string parts[3];
        if (splitLine(lines[i], '|', parts, 3) != 3 || parts[0].empty()
            || parts[1].length() != 7 || parts[2].length() != 8)
        {
            throw FileException("clue.txt", "is corrupted (line " + numberToText(i + 2) + " is wrong)");
        }
        roles[i] = parts[0];
        wordOne[i] = toUpperText(parts[1]);
        wordTwo[i] = toUpperText(parts[2]);
    }
}

// Reads locations.txt: Place|W1Pos|then one letter column for each of the 7 culprit roles
void GameEngine::readLocationFile(string names[], int positions[], char letters[][SUSPECT_COUNT])
{
    string lines[LOCATION_COUNT];
    int count = readDataLines("locations.txt", lines, LOCATION_COUNT);
    if (count != LOCATION_COUNT)
    {
        throw FileException("locations.txt", "is corrupted (it must have 7 data lines)");
    }
    for (int i = 0; i < LOCATION_COUNT; i++)
    {
        string parts[9];
        int pos = 0;
        bool good = (splitLine(lines[i], '|', parts, 9) == 9) && !parts[0].empty()
            && parseInteger(parts[1], pos) && pos >= 1 && pos <= 7;
        for (int c = 0; c < SUSPECT_COUNT && good; c++)
        {
            good = (parts[2 + c].length() == 1);
        }
        if (!good)
        {
            throw FileException("locations.txt", "is corrupted (line " + numberToText(i + 2) + " is wrong)");
        }
        names[i] = parts[0];
        positions[i] = pos;
        for (int c = 0; c < SUSPECT_COUNT; c++)
        {
            letters[i][c] = (char)toupper((unsigned char)parts[2 + c][0]);
        }
    }
}

// Reads characters.txt: Name|Role|Nationality|W2Pos|then one letter column for each culprit role|isCulprit
void GameEngine::readCharacterFile(string names[], string roles[], string nations[], int positions[], char letters[][SUSPECT_COUNT])
{
    string lines[PEOPLE_COUNT];
    int count = readDataLines("characters.txt", lines, PEOPLE_COUNT);
    if (count != PEOPLE_COUNT)
    {
        throw FileException("characters.txt", "is corrupted (it must have 8 data lines)");
    }
    for (int i = 0; i < PEOPLE_COUNT; i++)
    {
        string parts[12];
        int pos = 0;
        bool good = (splitLine(lines[i], '|', parts, 12) == 12) && !parts[0].empty() && !parts[1].empty()
            && parseInteger(parts[3], pos) && pos >= 1 && pos <= 8;
        for (int c = 0; c < SUSPECT_COUNT && good; c++)
        {
            good = (parts[4 + c].length() == 1);
        }
        if (!good)
        {
            throw FileException("characters.txt", "is corrupted (line " + numberToText(i + 2) + " is wrong)");
        }
        names[i] = parts[0];
        roles[i] = parts[1];
        nations[i] = parts[2];
        positions[i] = pos;
        for (int c = 0; c < SUSPECT_COUNT; c++)
        {
            letters[i][c] = (char)toupper((unsigned char)parts[4 + c][0]);
        }
    }
}

// Factory: creates the correct derived object on the heap and returns it as a Character pointer.
// This is the only place where the role text is checked, everything else uses polymorphism.
Character* GameEngine::createCharacter(string name, string role, string nation, char letter, int pos)
{
    if (role == "Victim")    return new Victim(name, nation, letter, pos, WORD_2);
    if (role == "Cook")      return new Cook(name, nation, false, letter, pos, WORD_2);
    if (role == "Guard")     return new Guard(name, nation, false, letter, pos, WORD_2);
    if (role == "Servant")   return new Servant(name, nation, false, letter, pos, WORD_2);
    if (role == "Partner 1" || role == "Partner 2") return new Partner(name, role, nation, false, letter, pos, WORD_2);
    if (role == "PA")        return new PA(name, nation, false, letter, pos, WORD_2);
    if (role == "Child")     return new Child(name, nation, false, letter, pos, WORD_2);
    throw FileException("characters.txt", "is corrupted (unknown role " + role + ")");
}

// Asks for a name without spaces. If the player types spaces, the player chooses underscore or dot
string GameEngine::askDetectiveName()
{
    string name = readLine("Enter your detective name (no spaces): ");
    if (name.empty())
    {
        throw InvalidInputException("The name cannot be empty.");
    }
    if (name.find(' ') != string::npos)
    {
        cout << "Spaces are not allowed. Replace every space with:  [1] underscore ( _ )   [2] dot ( . )" << endl;
        int choice = readNumber("Your choice [1-2]: ", 1, 2);
        char replacement = (choice == 1) ? '_' : '.';
        for (int i = 0; i < (int)name.length(); i++)
        {
            if (name[i] == ' ')
            {
                name[i] = replacement;
            }
        }
        cout << "Your name is now: " << name << endl;
    }
    return name;
}

// Module 2: reads all files first (so a file error cannot leave a half-started game), asks the name,
// randomly selects the culprit, and builds a fresh Detective, Cottage, Scrabble, racks and people.
void GameEngine::startNewGame()
{
    bool rulesLoaded = loadRules();

    // Local arrays hold the file data until the culprit is known
    string clueRole[SUSPECT_COUNT], clueWordOne[SUSPECT_COUNT], clueWordTwo[SUSPECT_COUNT];
    string locName[LOCATION_COUNT];
    int locPos[LOCATION_COUNT];
    char locLetter[LOCATION_COUNT][SUSPECT_COUNT];
    string pName[PEOPLE_COUNT], pRole[PEOPLE_COUNT], pNation[PEOPLE_COUNT];
    int pPos[PEOPLE_COUNT];
    char pLetter[PEOPLE_COUNT][SUSPECT_COUNT];

    readClueFile(clueRole, clueWordOne, clueWordTwo);
    readLocationFile(locName, locPos, locLetter);
    readCharacterFile(pName, pRole, pNation, pPos, pLetter);

    string name = askDetectiveName();

    // The culprit is one of the seven suspects. Column k of every file belongs to this culprit
    int k = rand() % SUSPECT_COUNT;

    detective = Detective(name, rules.startCredibility);
    scrabble = Scrabble();
    scrabble.loadTargets(clueWordOne[k], clueWordTwo[k]);
    cottage = Cottage(LOCATION_COUNT);
    for (int i = 0; i < LOCATION_COUNT; i++)
    {
        cottage.setLocation(i, locName[i], locLetter[i][k], locPos[i], WORD_1);
    }
    locationRack = TileRack(LOCATION_COUNT);
    characterRack = TileRack(PEOPLE_COUNT);

    freePeople();
    for (int i = 0; i < PEOPLE_COUNT; i++)
    {
        people[i] = createCharacter(pName[i], pRole[i], pNation[i], pLetter[i][k], pPos[i]);
    }

    // Set isCulprit on the matching object. GameEngine is a friend of Character, so it can do it directly
    culpritIndex = -1;
    for (int i = 1; i < PEOPLE_COUNT; i++)
    {
        if (people[i]->characterRole == clueRole[k])
        {
            people[i]->isCulprit = true;
            culpritIndex = i;
        }
    }
    if (culpritIndex == -1)
    {
        freePeople();
        throw FileException("characters.txt", "is corrupted (no person has the role " + clueRole[k] + ")");
    }
    culpritRole = clueRole[k];

    state = GAME_PLAYING;

    clearScreen();
    cout << "==============================================================" << endl;
    cout << "                  THE INVESTIGATION BEGINS" << endl;
    cout << "==============================================================" << endl;
    if (!rulesLoaded)
    {
        cout << "HEIST NOTICE: rules.txt is missing or corrupt, default rules are used." << endl;
    }
    cout << "The PA of the owner has hired you. Find the culprit!" << endl;
    detective.display();
    cout << "You have " << rules.maxTurns << " turns. Every wrong turn costs " << rules.wrongTurnPenalty
        << " credibility, every wrong word costs " << rules.wrongWordPenalty << "." << endl;
}

// Title and a short scenario, shown once when the program starts
void GameEngine::showWelcome() const
{
    cout << "==============================================================" << endl;
    cout << "                     THE LOCKER HEIST" << endl;
    cout << "          A Detective Game backed by OOP Principles" << endl;
    cout << "==============================================================" << endl;
    cout << "A locker hidden behind a bookshelf in the Study Room of a" << endl;
    cout << "wealthy owner's cottage was robbed during a party. Eight people" << endl;
    cout << "were there. One of them is the culprit. Collect letters from" << endl;
    cout << "seven locations and eight people, build two words, and find" << endl;
    cout << "the thief before your credibility runs out." << endl;
}

// Runs one step of the game depending on the state. main() calls it again and again
void GameEngine::step()
{
    switch (state)
    {
    case GAME_MENU:
        mainMenuStep();
        break;
    case GAME_PLAYING:
        playStep();
        break;
    case GAME_WON:
    case GAME_LOST:
        finishGame();
        break;
    case GAME_EXIT:
        break;
    }
}

// Prints the main menu
void GameEngine::showMainMenu() const
{
    cout << endl << "-------- Main Menu --------" << endl;
    cout << "  [" << MENU_NEW_GAME << "] New Game" << endl;
    cout << "  [" << MENU_SCOREBOARD << "] View Leader/Score Board" << endl;
    cout << "  [" << MENU_ABOUT << "] About" << endl;
    cout << "  [" << MENU_RULES << "] Rules" << endl;
    cout << "  [" << MENU_EXIT << "] Exit" << endl;
}

// Shows the main menu once and performs the chosen option
void GameEngine::mainMenuStep()
{
    showMainMenu();
    int choice = readNumber("Choose an option [1-5]: ", 1, 5);
    switch (static_cast<MenuChoice>(choice))
    {
    case MENU_NEW_GAME:
        startNewGame();
        break;
    case MENU_SCOREBOARD:
        cout << endl << "-------- Leader / Score Board --------" << endl;
        if (scoreBoard == 0)
        {
            throw FileException("leader/scoreBoard.txt", "is not connected to the game");
        }
        scoreBoard->displayAll();
        pauseScreen();
        break;
    case MENU_ABOUT:
        showAbout();
        pauseScreen();
        break;
    case MENU_RULES:
        showRules();
        pauseScreen();
        break;
    case MENU_EXIT:
        state = GAME_EXIT;
        break;
    }
}

// Prints about.txt. A missing file throws FileException
void GameEngine::showAbout() const
{
    ifstream in("about.txt");
    if (!in)
    {
        throw FileException("about.txt", "was not found");
    }
    cout << endl;
    string line;
    while (getline(in, line))
    {
        cout << trimText(line) << endl;
    }
}

// Shows the rules that are read from rules.txt (or the default rules if the file is not usable)
void GameEngine::showRules()
{
    bool loaded = loadRules();
    cout << endl << "-------- Rules --------" << endl;
    if (!loaded)
    {
        cout << "HEIST NOTICE: rules.txt is missing or corrupt, default rules are shown." << endl;
    }
    cout << "  Maximum turns         : " << rules.maxTurns << endl;
    cout << "  Starting credibility  : " << rules.startCredibility << endl;
    cout << "  Every wrong turn      : -" << rules.wrongTurnPenalty << " credibility" << endl;
    cout << "  Every wrong word      : -" << rules.wrongWordPenalty << " credibility" << endl;
    cout << "  Win condition         : " << rules.winCondition << endl;
    cout << "  Lose condition        : " << rules.loseCondition << endl;
    cout << "  Viewing the board is free, every other action uses one turn." << endl;
    cout << "  Interview limits      : Victim 80, Child 75, Partners 50, PA 35, House Help none." << endl;
}

// Prints the game menu together with the current status of the detective
void GameEngine::showGameMenu() const
{
    int turnsLeft = rules.maxTurns - detective.getTurns();
    cout << endl << "==============================================================" << endl;
    cout << " Detective: " << detective.getName() << "   Credibility: " << detective.getCredibility()
        << "   Turns used: " << detective.getTurns() << "/" << rules.maxTurns
        << "   Turns left: " << turnsLeft << endl;
    cout << "-------- Game Menu --------" << endl;
    cout << "  [1] Visit a Location" << endl;
    cout << "  [2] Interview a Person" << endl;
    cout << "  [3] View Scrabble Board (free, no turn)" << endl;
    cout << "  [4] Form a Word" << endl;
    cout << "  [5] Accuse a Suspect" << endl;
    cout << "  [6] Quit" << endl;
}

// Prints a clue as a sentence
void GameEngine::showClue(const Clue& clue) const
{
    cout << "  >> Clue: the letter '" << clue.clueLetter << "' belongs at position " << clue.cluePosition
        << " of Word " << (clue.clueRow == WORD_1 ? 1 : 2) << "." << endl;
}

// Gives the detective a wrong turn: credibility is reduced and one turn is used
void GameEngine::applyWrongTurn()
{
    detective.loseCredibility(rules.wrongTurnPenalty);
    detective.useTurn();
}

// The game is lost when credibility is zero or all turns are used
void GameEngine::checkEndConditions()
{
    if (state == GAME_PLAYING && (detective.getCredibility() <= 0 || detective.getTurns() >= rules.maxTurns))
    {
        state = GAME_LOST;
    }
}

// One round of the game menu: checks the end conditions, shows the menu and runs the chosen action
void GameEngine::playStep()
{
    checkEndConditions();
    if (state != GAME_PLAYING)
    {
        return;
    }
    showGameMenu();
    int choice = readNumber("Choose an option [1-6]: ", 1, 6);
    switch (choice)
    {
    case 1: visitLocation(); break;
    case 2: interviewPerson(); break;
    case 3: viewBoard(); break;
    case 4: formWord(); break;
    case 5: accuseSuspect(); break;
    case 6:
        if (readYesNo("Quit this investigation? It will not be saved (Y/N): "))
        {
            cout << "You left the case unsolved. Back to the main menu." << endl;
            state = GAME_MENU;
        }
        break;
    }
    checkEndConditions();
}

// Shows all 7 locations. A new location gives its letter and the letter is placed on the board.
// An old location asks before it is searched again.
void GameEngine::visitLocation()
{
    cout << endl << "-------- Locations --------" << endl;
    cottage.displayLocations();
    int choice = readNumber("Which location? [1-7, 0 = back]: ", 0, cottage.getLocationCount());
    if (choice == 0)
    {
        return;
    }
    int index = choice - 1;
    if (cottage.isSearched(index))
    {
        if (!readYesNo("You have already searched " + cottage.getLocationName(index) + ". Search again? (Y/N): "))
        {
            return;
        }
        detective.useTurn();
        showClue(cottage.getClue(index));
        return;
    }
    Clue clue = cottage.getClue(index);
    scrabble.placeLetter(clue);                      // May throw, so nothing else is changed before it
    locationRack.addTile(clue.clueLetter);
    detective.getRack().addTile(clue.clueLetter);
    cottage.markSearched(index);
    detective.useTurn();
    cout << "You searched " << cottage.getLocationName(index) << " and found a letter!" << endl;
    showClue(clue);
}

// Shows all 8 people with their status. Polymorphism: getMinCredibility() and display() of the real object run.
// Not enough credibility is a wrong turn.
void GameEngine::interviewPerson()
{
    cout << endl << "-------- People --------" << endl;
    for (int i = 0; i < PEOPLE_COUNT; i++)
    {
        cout << "   [" << (i + 1) << "] " << *people[i] << " - "
            << (interviewed[i] ? "Interviewed" : "Not interviewed");
        if (people[i]->getMinCredibility() > 0)
        {
            cout << "  (needs credibility " << people[i]->getMinCredibility() << ")";
        }
        cout << endl;
    }
    int choice = readNumber("Whom do you interview? [1-8, 0 = back]: ", 0, PEOPLE_COUNT);
    if (choice == 0)
    {
        return;
    }
    int index = choice - 1;

    if (detective.getCredibility() < people[index]->getMinCredibility())
    {
        applyWrongTurn();
        throw LowCredibilityException(people[index]->getName(), people[index]->getMinCredibility(),
            detective.getCredibility());
    }

    if (interviewed[index])
    {
        if (!readYesNo(people[index]->getName() + " is already interviewed. Interview again? (Y/N): "))
        {
            return;
        }
        detective.useTurn();
        showClue(people[index]->getClue());
        return;
    }

    Clue clue = people[index]->getClue();
    scrabble.placeLetter(clue);
    characterRack.addTile(clue.clueLetter);
    detective.getRack().addTile(clue.clueLetter);
    interviewed[index] = true;
    detective.useTurn();
    cout << "You interviewed:" << endl << "  ";
    people[index]->display();
    showClue(clue);
}

// Shows the board and the racks. Viewing is free, so no turn is used.
// The two racks are merged with operator+ to show them as one combined clue.
void GameEngine::viewBoard() const
{
    cout << endl << "-------- Scrabble Board --------" << endl;
    cout << scrabble;
    cout << endl;
    cout << "Locations ";
    locationRack.display();
    cout << "People    ";
    characterRack.display();
    TileRack combined = locationRack + characterRack;
    cout << "Combined clue ";
    combined.display();
}

// The detective types the word of a completed row. A wrong word costs credibility.
// The detective may retry until the word is correct, or enter 0 to stop, or turns/credibility end.
void GameEngine::formWord()
{
    cout << endl << "-------- Form a Word --------" << endl;
    cout << "  [1] Word 1 : " << scrabble.getRowText(WORD_1) << (scrabble.isSolved(WORD_1) ? "  (solved)" : "") << endl;
    cout << "  [2] Word 2 : " << scrabble.getRowText(WORD_2) << (scrabble.isSolved(WORD_2) ? "  (solved)" : "") << endl;
    int choice = readNumber("Which word? [1-2, 0 = back]: ", 0, 2);
    if (choice == 0)
    {
        return;
    }
    WordRow row = (choice == 1) ? WORD_1 : WORD_2;

    if (scrabble.isSolved(row))
    {
        cout << "You have already solved this word." << endl;
        return;
    }
    if (!scrabble.isRowComplete(row))
    {
        applyWrongTurn();
        throw InvalidWordException("This row is not complete yet. Collect all its letters first.");
    }

    bool correct = false;
    while (!correct && detective.getCredibility() > 0 && detective.getTurns() < rules.maxTurns)
    {
        cout << "Type the word (or 0 to stop): ";
        cin >> scrabble;                         // Extraction operator of Scrabble
        string leftover;
        getline(cin, leftover);                  // Removes the rest of the line
        if (!cin)
        {
            cout << "\nInput closed. The game ends here." << endl;
            exit(0);
        }
        string guess = scrabble.getLastGuess();
        if (guess == "0")
        {
            return;
        }
        detective.useTurn();
        correct = scrabble.verifyWord(row, guess);
        if (correct)
        {
            cout << "Correct! The word is " << scrabble.getTargetWord(row) << "." << endl;
        }
        else
        {
            detective.loseCredibility(rules.wrongWordPenalty);
            cout << "Wrong word! You lose " << rules.wrongWordPenalty << " credibility. Credibility left: "
                << detective.getCredibility() << endl;
        }
    }

    if (correct && scrabble.bothSolved())
    {
        cout << endl << "Both words are solved: " << scrabble.getTargetWord(WORD_1) << " "
            << scrabble.getTargetWord(WORD_2) << endl;
        cout << "According to clue.txt, these words point to the culprit's role: " << culpritRole << endl;
        cout << "Now accuse that suspect!" << endl;
    }
}

// Shows the 7 suspects (the victim is not a suspect). The right suspect wins the game.
// A wrong accusation is a wrong turn.
void GameEngine::accuseSuspect()
{
    cout << endl << "-------- Accuse a Suspect --------" << endl;
    for (int i = 1; i < PEOPLE_COUNT; i++)
    {
        cout << "   [" << i << "] " << *people[i] << endl;
    }
    int choice = readNumber("Whom do you accuse? [1-7, 0 = back]: ", 0, SUSPECT_COUNT);
    if (choice == 0)
    {
        return;
    }
    if (people[choice]->getIsCulprit())
    {
        detective.useTurn();
        state = GAME_WON;
        cout << "You accused " << people[choice]->getName() << "..." << endl;
    }
    else
    {
        applyWrongTurn();
        cout << people[choice]->getName() << " is innocent! You lose " << rules.wrongTurnPenalty
            << " credibility. Credibility left: " << detective.getCredibility() << endl;
    }
}

// Shows WIN or LOSE with the details, saves the score, clears the screen and returns to the main menu
void GameEngine::finishGame()
{
    bool won = (state == GAME_WON);
    int turnsLeft = rules.maxTurns - detective.getTurns();
    if (turnsLeft < 0)
    {
        turnsLeft = 0;
    }
    state = GAME_MENU;    // Set first, so that a file error below cannot repeat this step forever

    cout << endl << "==============================================================" << endl;
    if (won)
    {
        cout << "                        *** YOU WIN ***" << endl;
        cout << " Brilliant work, Detective! The culprit is caught." << endl;
    }
    else
    {
        cout << "                        *** YOU LOSE ***" << endl;
        cout << (detective.getCredibility() <= 0 ? " Your credibility reached zero." : " You ran out of turns.") << endl;
    }
    cout << "==============================================================" << endl;
    cout << " Culprit      : " << people[culpritIndex]->getName() << " (" << culpritRole << ")" << endl;
    cout << " Credibility  : " << detective.getCredibility() << endl;
    cout << " Turns used   : " << detective.getTurns() << "   Turns left: " << turnsLeft << endl;

    if (scoreBoard != 0)
    {
        scoreBoard->addScore(detective.getName(), won ? "WIN" : "LOSE", detective.getCredibility(),
            turnsLeft, culpritRole);
        cout << " Result saved in the score board." << endl;
    }
    pauseScreen();
    clearScreen();
}
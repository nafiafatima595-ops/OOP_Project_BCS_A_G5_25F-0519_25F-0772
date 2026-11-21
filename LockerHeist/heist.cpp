// heist.cpp
// Implementation file of The Locker Heist game.
// It contains the function bodies of all classes declared in heist.h.

#include "heist.h"

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

// ======================= CLASS: TileRack =======================

// Constructor allocates the dynamic array. A capacity below one is corrected to one
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

// Copy constructor: it allocates a NEW array and copies every tile (deep copy),
// so that two racks never share the same memory
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

// Assignment operator: it first checks self-assignment, then frees the old array
// and makes a deep copy of the other rack
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

// Destructor releases the heap memory, so there is no memory leak
TileRack::~TileRack()
{
    delete[] tiles;
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

int TileRack::getCount() const { return count; }
int TileRack::getCapacity() const { return capacity; }
bool TileRack::isFull() const { return count == capacity; }
bool TileRack::isEmpty() const { return count == 0; }

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

// Getters return the stored values without allowing direct access to the data
string Character::getName() const { return characterName; }
string Character::getRole() const { return characterRole; }
string Character::getNationality() const { return nationality; }
bool Character::getIsCulprit() const { return isCulprit; }

// Setters change the stored values in a controlled way
void Character::setNationality(string nation) { nationality = nation; }
void Character::setIsCulprit(bool culprit) { isCulprit = culprit; }

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
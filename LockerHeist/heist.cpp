// heist.cpp
// Implementation file of The Locker Heist game.
// It contains the function bodies of all classes declared in heist.h.

#include "heist.h"

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

// The detective starts with full credibility and zero turns
Detective::Detective(string name, int startCredibility)
    : Character(name, "Detective", "", false),
      credibility(startCredibility), turns(0), solvedWords("")
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
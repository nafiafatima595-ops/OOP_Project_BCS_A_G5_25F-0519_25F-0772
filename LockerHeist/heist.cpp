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
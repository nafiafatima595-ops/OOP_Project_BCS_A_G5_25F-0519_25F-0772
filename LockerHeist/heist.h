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
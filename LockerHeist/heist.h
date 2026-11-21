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
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
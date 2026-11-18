    // Step B test: three house help members are used through Character pointers (polymorphism)
    cout << "--- Module 3 Test (Step B: Suspect, HouseHelp, Cook, Guard, Servant) ---" << endl;
    Cook enzo("Mr.Enzo", "Italian", false, 'P', 2, WORD_2);
    Guard lukas("Mr.Lukas", "German", false, 'A', 3, WORD_2);
    Servant toby("Mr.Toby", "British", false, 'G', 4, WORD_2);

    Character* houseHelp[3] = { &enzo, &lukas, &toby };
    for (int i = 0; i < 3; i++)
    {
        houseHelp[i]->display();
        cout << "  Minimum credibility to interview: " << houseHelp[i]->getMinCredibility() << endl;
    }
    cout << "Cook clue: letter = " << enzo.getLetter()
         << ", position = " << enzo.getPosition()
         << ", row = " << static_cast<int>(enzo.getRow()) << endl;
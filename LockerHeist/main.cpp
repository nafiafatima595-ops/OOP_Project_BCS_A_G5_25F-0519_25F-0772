    // Step C test: two partners and the child are used through Character pointers (polymorphism)
    cout << "--- Module 3 Test (Step C: BusinessRelations, Partner, CloseRelations, Child) ---" << endl;
    Partner ruslana("Ms.Ruslana", "Partner 1", "Ukrainian", false, 'E', 6, WORD_2);
    Partner dmitry("Mr.Dmitry", "Partner 2", "Russian", false, 'T', 7, WORD_2);
    Child celine("Ms.Celine", "French", false, 'H', 5, WORD_2);

    Character* relations[3] = { &ruslana, &dmitry, &celine };
    for (int i = 0; i < 3; i++)
    {
        relations[i]->display();
        cout << "  Minimum credibility to interview: " << relations[i]->getMinCredibility() << endl;
    }
    cout << "Partner clue: letter = " << ruslana.getLetter()
         << ", position = " << ruslana.getPosition()
         << ", row = " << static_cast<int>(ruslana.getRow()) << endl;
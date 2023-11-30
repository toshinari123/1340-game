// hangman.cpp
#include "hangman.h"
#include <ncurses.h>
#include <iostream>
#include <ncurses.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
using namespace std;

vector<WordInfo> currentThemeData;
string previousTheme;
const int THEME_LINE = 4;
const int NOTE_LINE = 5;
const int HINT_LINE = 7;
    const int DIFFICULTY_LINE = 9;
    const int BLUNDERS_SCORE_LINE = 11;
    const int PROGRESS_LINE = 13;
    const int MESSAGE_LINE = 24;
    const int GUESS_PROMPT_LINE = 22;
    const int HANGING_LINE = 26;

//what it does: initializes ncurses
void initialize_ncurses() {
    initscr(); // Initialize the ncurses environment
    raw();     // Disable line buffering
    keypad(stdscr, TRUE); // Enable special keys
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK); 
    init_pair(2, COLOR_GREEN, COLOR_BLACK); 
}

//what it does: prints welcome message
void welcome() {
    printw("***********************************\n");
    printw("*  Welcome to Hangman Adventure!  *\n");
    printw("***********************************\n");
    printw("\n");
  
}

//what it does: prints instructions
//inputs: no input
//outputs: true if player chooses to play, fales if no
bool display_instructions(){
    printw("Can you save your friend from a dire fate?\n");
    printw("\n");
    printw("Guess the letters in the secret codeword and keep them from the gallows!\n");
    printw("\n");

    printw("How to Play:\n\n");
    printw("1. Theme Selection:\n");
    printw("   - At the beginning of the game, a random theme is selected from a set of 10.\n\n");

    printw("2. Word and Hint:\n");
    printw("   - A hint is provided to guide you. Pay attention; it might be the key to guessing the word!\n\n");

    printw("3. Difficulty Level:\n");
    printw("   - Each word comes with a difficulty level (easy, medium, hard).\n\n");

    printw("4. Game Loop:\n");
    printw("   - Guess the letters in the secret word within 6 attempts.\n");
    printw("   - More than 6 attempts result in a part of the person being hanged.\n\n");

    printw("5. Scoring:\n");
    printw("   - Earn points for each successfully guessed word.\n\n");

    printw("6. Exiting the Game:\n");
    printw("   - At any time, you can exit the game by entering '!'.\n\n");
;

    printw("Are you ready to embark on the Hangman Adventure? (Y/N)\n\n");

    refresh(); // Refresh the screen to display the prompt

    char response;
    while (true) {
        response = getch();
        if (response == 'y' || response == 'Y') {
            return true;  // Player is ready
        } else if (response == 'n' || response == 'N') {
            return false;  // Player is not ready
        }
    }
}


//Functions from this point are referenced by the start game function
//what it does: display handman state based on number of errors
//inputs: int errors
//outputs: void
void hang(int errors) {
    if (errors == 0) {
        printw("  +-------------+ \n");
        printw("  |             | \n");
        printw("                | \n");
        printw("                | \n");
        printw("                | \n");
        printw("                | \n");
        printw(" ========================= \n");
    } else if (errors == 1) {
        printw("  +-------------+ \n");
        printw("  |             | \n");
        printw("  O             | \n");
        printw("                | \n");
        printw("                | \n");
        printw("                | \n");
        printw(" ========================= \n");
    } else if (errors == 2) {
        printw("  +-------------+ \n");
        printw("  |             | \n");
        printw("  O             | \n");
        printw("  |             | \n");
        printw("                | \n");
        printw("                | \n");
        printw(" ========================= \n");
    } else if (errors == 3) {
        printw("  +-------------+ \n");
        printw("  |             | \n");
        printw("  O             | \n");
        printw(" /|             | \n");
        printw("                | \n");
        printw("                | \n");
        printw(" ========================= \n");
    } else if (errors == 4) {
        printw("  +-------------+ \n");
        printw("  |             | \n");
        printw("  O             | \n");
        printw(" /|\\            | \n");
        printw("                | \n");
        printw("                | \n");
        printw(" ========================= \n");
    } else if (errors == 5) {
        printw("  +-------------+ \n");
        printw("  |             | \n");
        printw("  O             | \n");
        printw(" /|\\            | \n");
        printw(" /              | \n");
        printw("                | \n");
        printw(" ========================= \n");
    } else if (errors == 6) {
        printw("  +-------------+ \n");
        printw("  |             | \n");
        printw("  O             | \n");
        printw(" /|\\            | \n");
        printw(" / \\            | \n");
        printw("                | \n");
        printw(" ========================= \n");
    }
}

//what it does: displays the progress (wrong guesses and secretword)
//inputs: wrong gueses (vector of chars), and the solution string
//outputs: void
void show_progress(vector<char> wrong_guesses, string solution) {
    printw("\nWrong Guesses: \n");
    for (int i = 0; i < wrong_guesses.size(); i++) {
    attron(A_BOLD | COLOR_PAIR(1));
    printw("%c ", wrong_guesses[i]);
    attroff(A_BOLD | COLOR_PAIR(1));
    }
    printw("\n");
    printw("\nSecretword:\n");

    for (int i = 0; i < solution.length(); i++) {
    char currentChar = solution[i];
    if (isalpha(currentChar)) {
        attron(A_BOLD|COLOR_PAIR(2));
        printw("%c ", solution[i]);
        attroff(A_BOLD|COLOR_PAIR(2));
        }
    else{
        printw("%c ", solution[i]);
        }
    }
    printw("\n");
    refresh();  
}

//what it does: shows the word after failure
//inputs: string secret word
//outputs: void
void show_word_after_wrong_guess(string secretword){
    move(GUESS_PROMPT_LINE, 0);
    clrtoeol();
    printw("OOPS! Correct word was ");
    attron(A_BOLD  | COLOR_PAIR(2));
    printw("%s", secretword.c_str());
    attroff(A_BOLD  | COLOR_PAIR(2));
    move(MESSAGE_LINE, 0);
    clrtoeol();
    move(36, 0);
    printw("Press ENTER to continue: ");
    refresh();

    if (getch()) {
        move(MESSAGE_LINE, 0);
        clrtoeol();
        move(36, 0);
        clrtoeol();
        move(34, 0);
        clrtoeol();
        move(17, 0);
        clrtoeol();
        return;
    }
}

//what it does: shows the word after correct guess
//inputs: string secret word
//outputs: void
void show_word_after_correct_guess(string secretword){
    move(GUESS_PROMPT_LINE, 0);
    clrtoeol();
    printw("Congratulations! You guessed the word: ");
    attron(A_BOLD  | COLOR_PAIR(2));
    printw("%s", secretword.c_str());
    attroff(A_BOLD  | COLOR_PAIR(2));
    move(MESSAGE_LINE, 0);
    clrtoeol();
    move(36, 0);
    printw("Press ENTER to continue: ");
    refresh();

    if (getch()) {
        move(MESSAGE_LINE, 0);
        clrtoeol();
        move(36, 0);
        clrtoeol();
        move(34, 0);
        clrtoeol();
        move(17, 0);
        clrtoeol();
        return;
    }
}

//what it does: prints the score
//inputs: int score
//outputs: void
void show_score(int score) {
    printw("Score: %d\n", score);
    return;
}

//what it does: prints the warning that hyphens maybe used
//inputs: no input
//outputs: boid
void print_attention_grabbing_note() {
    attron(A_BOLD | A_UNDERLINE | COLOR_PAIR(1));
    printw("Note: Some names may consist of two or more words connected by hyphens.\n");
    attroff(A_BOLD | A_UNDERLINE | COLOR_PAIR(1));
}

//what it does: called to clean up ncurses
void cleanup_ncurses() {
    endwin(); // Clean up and close the ncurses environment
}

//what it does: called to display ending of the game
//inputs: score, number of errors, total number of words
//outputs: void
void end_game(int score, int errors, int totalWords) {
    if (errors == 6) {
        move(34,0);
        printw("GAME OVER! The man has been hanged :(\n");
        printw("FINAL SCORE: ");
        attron(A_BOLD);
        printw("%d", score);
        attroff(A_BOLD);
        printw(" out of ");
        attron(A_BOLD);
        printw("%d", totalWords);
        attroff(A_BOLD);
        printw(" words\n");
        } 

    else {
        move(34,0);
        printw("Congratulations! You've successfully rescued the person from the brink of hanging, and you've earned a prestigious Medal of Honor!\n");
        printw("FINAL SCORE: ");
        attron(A_BOLD);
        printw("%d", score);
        attroff(A_BOLD);
        printw(" out of ");
        attron(A_BOLD);
        printw("%d", totalWords);
        attroff(A_BOLD);
        printw(" words\n");
        attroff(A_BOLD);    
        }

    refresh();  // Update the physical screen
    printw("Do you want to play again? (y/n): ");
    char choice = getch();
    if (choice == 'y' || choice == 'Y') {
    char userSelection;
    bool validInput = false;

    while (!validInput) {
        move(36,0);
        clrtoeol();
        move(37,0);
        clrtoeol();
        printw("Do you want to:\n");
        printw("1. Play the same theme\n");
        printw("2. Generate a new random theme\n");
        printw("!. Exit\n");
        printw("Enter the corresponding character (1 or 2 or !): ");

    // Get user input
    userSelection = getch();

    // Check the user's choice
    switch (userSelection) {
        case '1':
            clear();
            welcome();
            start_game();
            validInput = true; // Exit the loop, valid input received
            break;

        case '2':
            clear();
            welcome();
            choose_theme();
            start_game();
            validInput = true; // Exit the loop, valid input received
            break;
        
        case '!':
        cleanup_ncurses();
        exit(1);


    
    }
}

    }
}


//what it does: randomly chooses a theme (the text file to get words out of)
//inputs: no input
//outputs: void
void choose_theme(){
    string themes[10] = {"Animals", "Movies", "Countries", "Sports", "Food", "Landmarks", "Science", "Music", "Occupations", "Legends"};

    srand(time(NULL));
    int index = rand() % 10;
    string current_theme = themes[index];
       if (current_theme != previousTheme) {
        previousTheme = current_theme;  // Update the previous theme
    }

    ifstream fin;
    string filename = current_theme + ".txt";
    fin.open(filename);
    if (fin.fail()) {
        exit(1);
    }
    currentThemeData.clear();  // Clear the current theme data before loading a new theme
    WordInfo currentInfo;
    while (fin >> currentInfo.word) {
        getline(fin >> ws, currentInfo.hint1, '#');
        fin >> currentInfo.difficulty;
        currentThemeData.push_back(currentInfo);
    }
    fin.close();
}

//what it does: called to start the game
//inputs: no input
//outputs: void
void start_game() {
    // Check if there is a current theme, otherwise choose a new one
    if (currentThemeData.empty()) {
        choose_theme();
    }

    move(THEME_LINE,0);
    clrtoeol();
    printw("Current Game Theme: ");
     attron(A_BOLD);
    printw("%s!\n", previousTheme.c_str());
    attroff(A_BOLD);
    vector<WordInfo> themeData = currentThemeData;
    std::random_device rd;
    std::default_random_engine rng(rd());
    std::shuffle(themeData.begin(), themeData.end(), rng);
    int errors = 0;
    int score = 0;
    int currentIndex = 0;

    while (errors < 6 && currentIndex < themeData.size()) {
        string secretword = themeData[currentIndex].word;
        int length_of_secretword = themeData[currentIndex].word.length();
        string solution = "";
        for (int z = 0; z < length_of_secretword; z++) {
            solution += "_";
        }
        int blunders = 0;
        vector<char> wrong_guesses;
        bool guess = false;
        char character;


while (solution != secretword && blunders < 6) {
    guess = false;

    // Move the cursor to the position of the "Hint:" line and update its content
    move(NOTE_LINE, 0);
    clrtoeol();
    print_attention_grabbing_note();


    // Move the cursor to the position of the "Hint:" line and update its content
    move(HINT_LINE, 0);
    clrtoeol();
    move(8, 0);
    clrtoeol();
    move(HINT_LINE, 0);
    printw("Hint: %s", themeData[currentIndex].hint1.c_str());


    // Move the cursor to the position of the "Difficulty:" line and update its content
    move(DIFFICULTY_LINE, 0);
    clrtoeol();
    printw("Difficulty: %s", themeData[currentIndex].difficulty.c_str());


    // Move the cursor to the position of the "Blunders: 0 | Score: 0" line and update its content
    move(BLUNDERS_SCORE_LINE, 0);
    clrtoeol();
    printw("Blunders: %d | Score: %d", blunders, score);


    // Move the cursor to the position of the "Incorrect Guesses:" line and update its content
    move(PROGRESS_LINE, 0);
    show_progress(wrong_guesses, solution);


    // Move the cursor to the position where the user enters the guess and update the prompt
    move(GUESS_PROMPT_LINE, 0);
    clrtoeol();
    printw("Please enter your guess: ");
    refresh();

    character = getch();
    if (character == '!') {
        show_score(score);
        cleanup_ncurses();
        exit(1);
    }

    // Check if the entered character is not a newline character
    if (character != '\n') {
        for (int i = 0; i < secretword.length(); i++) {
            if (character == secretword[i] || character == toupper(secretword[i])) {
                solution[i] = tolower(character);
                guess = true;
            }
        }

    // Move the cursor to the position of the "Correct!" or "Incorrect!" message and update it
    move(MESSAGE_LINE, 0);

    if (guess) {
        attron(A_BOLD | COLOR_PAIR(2));
        printw("Correct!");
        attroff(A_BOLD | COLOR_PAIR(2));
        } 
        else 
        {
        attron(A_BOLD | COLOR_PAIR(1));
        printw("Incorrect!");
        attroff(A_BOLD | COLOR_PAIR(1));
        wrong_guesses.push_back(character);
        blunders++;
        }
    clrtoeol();
    refresh();
    } else 
    {
    // Treat newline character as an incorrect guess
    move(MESSAGE_LINE, 0);
    attron(A_BOLD | COLOR_PAIR(1));
    printw("Invalid!");
    attroff(A_BOLD | COLOR_PAIR(1));
    clrtoeol();
    refresh();
    }
    }
        if (blunders == 6) {
            errors++;
            move(HANGING_LINE, 0);
            hang(errors);
            printw("\nA portion of this person has been hanged!!!\n");
            show_word_after_wrong_guess(secretword);

        } else {
            score++;
            show_word_after_correct_guess(secretword);
        }
        currentIndex++;
    }
    end_game(score, errors, themeData.size());
    cleanup_ncurses(); // Call cleanup_ncurses before returning
}


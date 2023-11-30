#include <iostream>
using namespace std;
#include <vector>
struct WordInfo {
    string word;
    string hint1;
    string difficulty;
};
//Declare functions
//Header files contain the declarations of classes, functions, and variables that are intended to be used in a C++ program. These declarations provide information about the names, types, and parameters of these entities.
void initialize_ncurses();
void welcome();
bool display_instructions();
void start_game();
void hang(int blunders);
void show_progress(vector<char> wrong_guesses, string solution);
void end_game(int score, int errors);
void show_score(int score);
void cleanup_ncurses();
void choose_theme();



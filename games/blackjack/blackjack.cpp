#include <iostream>
#include <locale.h>
#include <algorithm>
#include <random>
#include <string>
#include <vector>
#include <numeric>
#include <ncurses.h>
#include <cstring>
#include <string>
using namespace std;

const bool isfacecard(const int &);
string getcard(const int &);
int deal(vector<int> &);
void printhands(const vector<int> &, const vector<vector<int>> &, const vector<int> &);
int sumcards(const vector<int> &);
int numaces(const vector<int> &);
void takeinsurance(const vector<int> &, const vector<int> &, int &);

const bool isfacecard(const int &card){
    int val = card % 13;
    return (val >= 10 || val == 0);
}


string getcard(const int &card){
    string card_string;
    setlocale(LC_ALL, "");
    string uc_spade = "♤";
    string uc_club = "♧";
    string uc_heart = "♥";
    string uc_diaomond = "♦";

    //Different cases for face value cards (J, Q, K, A) [when drawing card]
    switch(card % 13) {
        case 1:
            card_string = "A";
            break;
        case 10:
            card_string = "10";
            break;
        case 11:
            card_string = "J";
            break;
        case 12:
            card_string = "Q";
            break;
        case 0:
            card_string = "K";
            break;
        default:
            card_string = card % 13 + '0';
            break;
    }

    //Different cases for suit of the card [when drawing card]
    /*
    switch (card % 4) {
        case 0:
            card_string += uc_spade;
            break;
        case 1:
            card_string += uc_club;
            break;
        case 2:
            card_string += uc_heart;
            break;
        case 3:
            card_string += uc_diaomond;
            break;
        default:
            break;
    }
    */
    return card_string;
}


// 
int deal(vector<int> &deck){
    int res = deck.back();
    deck.pop_back();
    return res;
}



void printhands(const vector<int> &dealer, const vector<vector<int>> &hands, const vector<int> &bets){
    int currhand = 1, sum;
    mvprintw(1, 0, "Dealer: %s\nYour hand(s):\n\n", getcard(dealer[0]).c_str());
    for (auto hand : hands) {
        printw("(%d): ", currhand);
        for (auto card : hand)
            printw("%s ", getcard(card).c_str());
        sum = sumcards(hand);
        printw("(");
        if (numaces(hand) == 1 && sum < 21)
            printw("%d/%d", sum, sum-10);
        else if (numaces(hand) > 0 && sum > 21)
            printw("%d", sum - (10 * numaces(hand)));
        else
            printw("%d", sum);
        printw(") $%d\n", bets[currhand-1]);
        ++currhand;
    }
}

int numaces(const vector<int> &hand)
{
    int aces = 0;
    for (auto card : hand) {
        if (card % 13 == 1)
            ++aces;
    }
    return aces;
}

int sumcards(const vector<int> &hand)
{
    int val;
    int sum = 0;
    for (int card : hand) {
        val = card % 13;
        if (val == 1)
            sum += 11;
        else if (isfacecard(card))
            sum += 10;
        else
            sum += val;
    }
    return sum;
}


void takeinsurance(const vector<int> &dealer, const vector<int> &bets, int &chips)
{
    mvprintw(1, 0, "Dealer: %s %s \n", getcard(dealer[0]).c_str(), getcard(dealer[1]).c_str());
    int dealer_sum = sumcards(dealer);
    int sum = accumulate(bets.begin(), bets.end(), 0);
    if (dealer_sum == 21) {
        printw("You won %d chips!\n", sum * 3 / 2);
        chips += sum * 3;
    } 
    else {
        printw("You lost the bet.\n");
        chips -= sum;
    }
}

int main()
{
    initscr();
    nodelay(stdscr, FALSE);
    raw();
    setlocale(LC_ALL, "");
    random_device rd;
    const int &DECK_SIZE = 52 * 4;
    int card, numhands, bet, chips = 1000;
    char format[] = "%d";
    char formatstring[] = "%s";
    string c;

    printw("Welcome to Blackjack. Blackjack pays 3:2. Insurance pays " "2:1.\nDealer stands on 17.\n");

    while (chips<=2500) {
        vector<int> deck, dealer, bets;
        vector<vector<int>> hands;
        printw("\n\nChips: %d", chips);
        printw("\nEnter number of hands (10 min. each): ");
        refresh();
        scanw(format, &numhands);
        if (numhands * 10 > chips) {
            printw("You do not have enough chips. Here are 500 chips. Try Again!\n");
            chips += 500;
            continue;
        }
        if (numhands < 1 || chips < 10)
            continue;
        for (int i = 1; i <= numhands; ++i) {
            printw("Enter bet amount for hand %d: ", i);
            refresh();
            scanw(format, &bet);
            if (chips - bet < (numhands - i) * 10) {
                printw("The max bet is %d. Try again.\n", chips - ((numhands - i) * 10));
                --i;
                continue;
            }
            bets.push_back(bet);
            chips -= bet;

        }


        for (int i = 0; i < DECK_SIZE; ++i)
            deck.push_back(i % 52);
        shuffle(begin(deck), end(deck), rd);

        // burn
        deck.pop_back();

        // deal two cards to all hands
        for (int i = 0; i < numhands; ++i)
            hands.push_back(vector<int>());
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < numhands; ++j)
                hands[j].push_back(deal(deck));
            dealer.push_back(deal(deck));
        }

        printhands(dealer, hands, bets);
        for (int i = 0; i < numhands; ++i) {
            if (sumcards(hands[i]) == 21) {
                printw("Hand %d has Blackjack!\nYou won %d chips!\n", i, bets[i] * 5 / 2);
                chips += bets[i] * 5 / 2;
            }
        }
        if (dealer[0] % 13 == 1) {
            printw("\nTake insurance? [y/N]: ");
            refresh();
            scanw(formatstring, &c);
            if (c[0] == 'y' || c[0] == 'Y') {
                printw("You chose to take insurance.\n");
                takeinsurance(dealer, bets, chips);
                continue;
            } 
            else
                printw("You chose not to take insurance.\n");
        }

        for (int i = 0; i < numhands; ++i) {
            if (sumcards(hands[i]) == 21)
                continue;
            printw("Hand %d:\n", i+1);
            if ((hands[i][0] % 13 == hands[i][1] % 13) && chips >= bets[i]) {
                printw("Split hand %d? (cost: %d) [y/N:]", i+1, bets[i]);
                refresh();
                scanw(formatstring, &c);
                if (c[0] == 'y' || c[0] == 'Y') {
                    chips -= bets[i];
                    ++numhands;
                    hands.insert(hands.cbegin() + i, hands[i]);
                    hands[i].pop_back();
                    hands[i+1].pop_back();
                    hands[i].push_back(deal(deck));
                    hands[i+1].push_back(deal(deck));
                    bets.insert(bets.cbegin() + i, bets[i]);
                } else {
                    printw("You chose not to split hand %d.\n", i+1);
                }
            }
            while (sumcards(hands[i]) < 21) {
                printhands(dealer, hands, bets);
                if (hands[i].size() == 2 && chips >= bets[i]) {
                    printw("Hit, stand or double? [h/S/d]: ");
                    refresh();
                } else {
                    printw("Hit or stand? [h/S]: ");
                    refresh();
                }
                scanw(formatstring, &c);
                if ((c[0] == 'd' || c[0] == 'D') && hands[i].size() == 2
                     && chips >= bets[i]) {
                    chips -= bets[i];
                    bets[i] *= 2;
                    hands[i].push_back(deal(deck));
                } else if (c[0] == 'h' || c[0] == 'H') {
                    hands[i].push_back(deal(deck));
                } else {
                    break;
                }
            }
            if (sumcards(hands[i]) > 21)
                printw("Hand %d busted!\n", i+1);
        }
        while (sumcards(dealer) < 17 || (sumcards(dealer) == 17 && numaces(dealer) != 0))
            dealer.push_back(deal(deck));
        printhands(dealer, hands, bets);
        clear();
        printw("Dealer's full hand: ");
        for (auto card : dealer)
            printw("%s ", getcard(card).c_str());
        printw("(%d)\n", sumcards(dealer));
        for (int i = 0; i < hands.size(); ++i) {
            if (sumcards(hands[i]) > 21)
                continue;
            printw("Hand %d", i+1);
            if (sumcards(dealer) > 21 || sumcards(hands[i]) > sumcards(dealer)) {
                printw(" won %d chips!\n", bets[i]);
                chips += bets[i] * 2;
            } 
            else if (sumcards(dealer) < 21 && sumcards(hands[i]) < sumcards(dealer)) {
                printw(" lost!\n");
            } 
            else if (sumcards(dealer) < 21 && sumcards(hands[i]) == sumcards(dealer)) {
                printw(" pushes.\n");
                chips += bets[i];
            }
        }

    }

    printw("Thank you for playing!\nTotal score: %d\nCash return: $%.2f\n", chips, (double)chips/100);
    endwin();

    return 0;
}

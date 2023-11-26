#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <vector>
#include <numeric>
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

string getcard(const int &card)
{
    string card_string;
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
    switch (card % 4) {
        case 0:
            card_string += u8"♤";
            break;
        case 1:
            card_string += u8"♧";
            break;
        case 2:
            card_string += u8"\033[31m♥\033[0m";
            break;
        case 3:
            card_string += u8"\033[31m♦\033[0m";
            break;
        default:
            break;
    }
    return card_string;
}

int deal(vector<int> &deck)
{
    int res = deck.back();
    deck.pop_back();
    return res;
}

void printhands(const vector<int> &dealer,
                const vector<vector<int>> &hands,
                const vector<int> &bets)
{
    int currhand = 1, sum;
    cout << "Dealer: " << getcard(dealer[0])
              << "\nYour hand(s):\n" << endl;
    for (auto hand : hands) {
        cout << "(" << currhand << "): ";
        for (auto card : hand)
            cout << getcard(card) << " ";
        sum = sumcards(hand);
        cout << "(";
        if (numaces(hand) == 1 && sum < 21)
            cout << sum << "/" << sum-10;
        else if (numaces(hand) > 0 && sum > 21)
            cout << sum - (10 * numaces(hand));
        else
            cout << sum;
        cout << ") $" << bets[currhand-1] << endl;
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
    cout << "Dealer: " << getcard(dealer[0]) << " "
              << getcard(dealer[1]) << std::endl;
    int dealer_sum = sumcards(dealer);
    int sum = accumulate(bets.begin(), bets.end(), 0);
    if (dealer_sum == 21) {
        cout << "You won " << sum * 3 / 2 << " chips!" << endl;
        chips += sum * 3;
    } else {
        cout << "You lost the bet." << endl;
        chips -= sum;
    }
}

int main()
{
    random_device rd;
    const int &DECK_SIZE = 52 * 4;
    int card, numhands, bet, chips = 1000;
    string c;

    cout << "Welcome to Blackjack. Blackjack pays 3:2. Insurance pays "
                 "2:1.\nDealer stands on 17." << endl;

    while (true) {
        vector<int> deck, dealer, bets;
        vector<vector<int>> hands;
        cout << "\n\nChips: " << chips
                  << "\nEnter number of hands (10 min. each): " << flush;
        cin >> numhands;
        if (numhands * 10 > chips) {
            cout << "You do not have enough chips." << endl;
            continue;
        }
        if (numhands < 1 || chips < 10)
            continue;
        for (int i = 1; i <= numhands; ++i) {
            cout << "Enter bet amount for hand " << i << ": " << flush;
            cin >> bet;
            if (chips - bet < (numhands - i) * 10) {
                cout << "The max bet is " << chips - ((numhands - i) * 10) << ". Try again." << endl;
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
                cout << "Hand " << i << " has Blackjack!\n"
                             "You won " << bets[i] * 5 / 2 << " chips!"
                          << endl;
                chips += bets[i] * 5 / 2;
            }
        }
        if (dealer[0] % 13 == 1) {
            cout << "\nTake insurance? [y/N]: " << flush;
            cin >> c;
            if (c[0] == 'y' || c[0] == 'Y') {
                cout << "You chose to take insurance." << endl;
                takeinsurance(dealer, bets, chips);
                continue;
            } else
                cout << "You chose not to take insurance." << endl;
        }

        for (int i = 0; i < numhands; ++i) {
            if (sumcards(hands[i]) == 21)
                continue;
            cout << "Hand " << i+1 << ":" << endl;
            if ((hands[i][0] % 13 == hands[i][1] % 13) && chips >= bets[i]) {
                cout << "Split hand " << i+1 << "? (cost: "
                          << bets[i] << ") [y/N]: " << flush;
                cin >> c;
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
                    cout << "You chose not to split hand " << i+1 << "." << endl;
                }
            }
            while (sumcards(hands[i]) < 21) {
                printhands(dealer, hands, bets);
                if (hands[i].size() == 2 && chips >= bets[i]) {
                    cout << "Hit, stand or double? [h/S/d]: "
                              << flush;
                } else {
                    cout << "Hit or stand? [h/S]: " << flush;
                }
                cin >> c;
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
                cout << "Hand " << i+1 << " busted!" << endl;
        }
        while (sumcards(dealer) < 17 ||
               (sumcards(dealer) == 17 && numaces(dealer) != 0))
            dealer.push_back(deal(deck));
        printhands(dealer, hands, bets);
        cout << "Dealer's full hand: ";
        for (auto card : dealer)
            cout << getcard(card) << " ";
        cout << "(" << sumcards(dealer) << ")" << endl;
        for (int i = 0; i < hands.size(); ++i) {
            if (sumcards(hands[i]) > 21)
                continue;
            cout << "Hand " << i+1;
            if (sumcards(dealer) > 21 ||
                     sumcards(hands[i]) > sumcards(dealer)) {
                cout << " won " << bets[i] << " chips!" << endl;
                chips += bets[i] * 2;
            } else if (sumcards(dealer) < 21 &&
                       sumcards(hands[i]) < sumcards(dealer)) {
                cout << " lost!" << std::endl;
            } else if (sumcards(dealer) < 21 &&
                       sumcards(hands[i]) == sumcards(dealer)) {
                cout << " pushes." << endl;
                chips += bets[i];
            }
        }
    }
    cout << "Thank you for playing!\nTotal score: " << chips
              << "Cash return: $" << (double)chips/100 << endl;

    return 0;
}
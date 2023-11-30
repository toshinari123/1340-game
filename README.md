# ENGG1340/COMP2113 Group Project: MAZE OF MINIGAMES

## Group Number: 26

## Members:
Maposa Ngonidzashe, Pingali Divya, Shah Rishi Kalpesh, Tong Chun Shing, Wu Zinuo

# Tasks:
Maposa Ngonidzashe- Hangman game 
Pingali Divya- Gun Game
Wu Zinuo- Blackjack game
Shah Rishi Kalpesh- Maze game
Tong Chun Shing- Overall Architecture

NOTE: WHILST THESE WERE THE SPECIALISED TASKS ALLOCATED, EACH OF US HAS WORKED EQUALLY ON THE PROJECT. WE HAVE ALL BEEIN INVOLVED IN THE DEBUGGING, TESTING AND IMPLEMENATION PROCESSES.


## How to run the game:

1. In your terminal run the command "make menu"
2. Type "./menu"
3. Before playing again we reconmmend running the command "make clean" 

## Overall Gameplay:
Maze of minigames is a simple game in which the player needs to reach from one end of the board to the other (marked by an 'X). However, if the player runs into a hidden trap while playing the maze, they will be forced to play a random minigame and can only return to the maze once it's completed. The player will be at the begining and will restart the maze, however they will now know where the traps they have already stepped on now.

### Controls:
1. UP :   W key
2. DOWN:  S key
3. LEFT:  A key
4. RIGHT: D key
5. QUIT:  Q key
## Minigames

### Hangman

Guess the letters in the secret codeword and keep them from the gallows!

How to Play:

1. Theme Selection:
   - At the beginning of the game, a random theme is selected from a set of 10.

2. Word and Hint:
   - A hint is provided to guide you. Pay attention; it might be the key to guessing the word!

3. Difficulty Level:
   - Each word comes with a difficulty level (easy, medium, hard).

4. Game Loop:
   - Guess the letters in the secret word within 6 attempts.
   - More than 6 attempts result in a part of the person being hanged.

5. Scoring:
   - Earn points for each successfully guessed word.

6. Exiting the Game:
   - At any time, you can exit the game by entering '!'.

### Gun Game

Controls:
Start : G
Move Pistol up : W
Move Pistol down : S
Shoot : SpaceBar
Quit : Q

HOW TO PLAY:
Shoot the monsters with the pistol. Your score increments by 3 everytime you hit a monster. You initially have 10 lives and you will lose on life everytime you miss the target.

Score 30 points to win!

## Blackjack

1. The objective of this game is to have a hand value as close to 21 as possible without exceeding 21. A hand's value is the sum of the card values. Get 2500 chips to win! (You start with 1000 chips)

2. Cards from 2 to 10 are worth their face value. Kings (K), Queens (Q), and Jacks (J) are each worth 10. Aces can be worth either 1 or 11, depending on which value benefits the player or dealer.

3. At the start of each round, the dealer gives two cards to each player and two to themselves.

4. If a player's first two cards are an Ace and a 10-value card, this is a Blackjack and typically pays out at 3:2, unless the dealer also has a Blackjack which results in a push (tie).

5. After receiving their first two cards, each player has a turn to improve their hands. Options are:

Hit: Ask for another card. Players can hit repeatedly until they either bust (go over 21) or decide to stand.
Stand: Stop taking more cards, keeping the current total.
Double Down: Double the initial bet in exchange for committing to stand after receiving exactly one more card.

6. If the player's first two cards are of the same value, they can split them into two separate hands. The bet is also doubled, and an additional card is dealt to each new hand. Rules about re-splitting or hitting on split Aces vary depending on the casino.

7. After all players have made their decisions, the dealer reveals their hole card and plays their hand. Dealer's decisions are automatic and follow a simple rule: they must hit if their hand total is less than or equal to 16, and stand if it's 17 or higher.8. If the dealer busts, all remaining players win an amount equal to their bet. If the dealer doesn't bust, they pay any player with a higher hand total an amount equal to their bet, and collect the bet of any player with a lower hand total. If a player and the dealer have the same hand total, it is a push and the player's bet is returned.


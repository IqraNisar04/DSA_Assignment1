#ifndef UNO_H
#define UNO_H

#include <string>
#include <vector>

struct Card {
    enum Color { RED, GREEN, BLUE, YELLOW };
    enum Type { NUMBER, SKIP, REVERSE, DRAW_TWO };
    
    Color color;
    Type type;
    int value; // 0-9 for number cards, -1 for action cards
    
    Card(Color c, Type t, int v) : color(c), type(t), value(v) {}
    
    std::string getColorString() const;
    std::string getValueString() const;
};

class UNOGame {
public:
    // Initialize game with given number of players
    UNOGame(int numPlayers);

    // Shuffle and deal cards to players
    virtual void initialize();

    // Play one turn of the game
    virtual void playTurn();

    // Return true if game has a winner
    virtual bool isGameOver() const;

    // Return index of winner
    virtual int getWinner() const;

    // Return current state as string
    virtual std::string getState() const;
};

#endif
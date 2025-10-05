#include "uno.h"
#include <iostream>

int main() {
    UNOGame game(2);
    game.initialize(); // Use fixed seed 1234
    
    std::cout << game.getState() << std::endl;
    
    game.playTurn();
    std::cout << game.getState() << std::endl;
    
    game.playTurn();
    std::cout << game.getState() << std::endl;
    
    game.playTurn();
    std::cout << game.getState() << std::endl;
    
    return 0;
}
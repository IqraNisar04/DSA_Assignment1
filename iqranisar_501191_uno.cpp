#include "uno.h"
#include <map>
#include <random>
#include <algorithm>
#include <sstream>

// Card member function implementations
std::string Card::getColorString() const {
    switch(color) {
        case RED: return "Red";
        case GREEN: return "Green";
        case BLUE: return "Blue";
        case YELLOW: return "Yellow";
        default: return "Unknown";
    }
}

std::string Card::getValueString() const {
    if (type == NUMBER) {
        return std::to_string(value);
    } else if (type == SKIP) {
        return "Skip";
    } else if (type == REVERSE) {
        return "Reverse";
    } else if (type == DRAW_TWO) {
        return "Draw Two";
    }
    return "Unknown";
}

// Node for card linked list
struct CardNode {
    Card card;
    CardNode* next;
    
    CardNode(Card c) : card(c), next(nullptr) {}
};

// Player structure
struct Player {
    CardNode* hand;
    int cardCount;
    
    Player() : hand(nullptr), cardCount(0) {}
    
    ~Player() {
        while (hand) {
            CardNode* temp = hand;
            hand = hand->next;
            delete temp;
        }
    }
    
    void addCard(Card card) {
        CardNode* newNode = new CardNode(card);
        newNode->next = hand;
        hand = newNode;
        cardCount++;
    }
    
    bool removeCard(CardNode* prev, CardNode* node) {
        if (prev) {
            prev->next = node->next;
        } else {
            hand = node->next;
        }
        delete node;
        cardCount--;
        return true;
    }
};

// Game data structure
class GameData {
public:
    int numPlayers;
    std::vector<Player*> players;
    CardNode* deck;
    CardNode* discardPile;
    int currentPlayer;
    bool clockwise;
    int deckSize;
    
    GameData(int n) : numPlayers(n), deck(nullptr), discardPile(nullptr), 
                      currentPlayer(0), clockwise(true), deckSize(0) {
        for (int i = 0; i < numPlayers; i++) {
            players.push_back(new Player());
        }
    }
    
    ~GameData() {
        for (Player* p : players) {
            delete p;
        }
        while (deck) {
            CardNode* temp = deck;
            deck = deck->next;
            delete temp;
        }
        while (discardPile) {
            CardNode* temp = discardPile;
            discardPile = discardPile->next;
            delete temp;
        }
    }
    
    void addCardToDeck(Card card) {
        CardNode* newNode = new CardNode(card);
        newNode->next = deck;
        deck = newNode;
        deckSize++;
    }
    
    Card drawFromDeck() {
        if (!deck) {
            // If deck is empty, return a dummy card
            return Card(Card::RED, Card::NUMBER, 0);
        }
        CardNode* temp = deck;
        Card card = deck->card;
        deck = deck->next;
        delete temp;
        deckSize--;
        return card;
    }
    
    void addToDiscard(Card card) {
        CardNode* newNode = new CardNode(card);
        newNode->next = discardPile;
        discardPile = newNode;
    }
    
    Card getTopCard() const {
        return discardPile->card;
    }
};

// Global storage
static std::map<size_t, GameData*> gameStorage;
static size_t nextGameId = 1;

static size_t& getGameId(const UNOGame* game) {
    return *((size_t*)game);
}

static GameData* getGameData(const UNOGame* game) {
    size_t id = getGameId(game);
    if (id == 0 || gameStorage.find(id) == gameStorage.end()) {
        return nullptr;
    }
    return gameStorage[id];
}

UNOGame::UNOGame(int numPlayers) {
    size_t id = nextGameId++;
    getGameId(this) = id;
    gameStorage[id] = new GameData(numPlayers);
}

void UNOGame::initialize() {
    GameData* data = getGameData(this);
    
    // Create deck (76 cards total)
    for (int color = 0; color < 4; color++) {
        Card::Color c = static_cast<Card::Color>(color);
        
        // One 0 per color
        data->addCardToDeck(Card(c, Card::NUMBER, 0));
        
        // Two of each 1-9
        for (int num = 1; num <= 9; num++) {
            data->addCardToDeck(Card(c, Card::NUMBER, num));
            data->addCardToDeck(Card(c, Card::NUMBER, num));
        }
        
        // Two Skip per color
        data->addCardToDeck(Card(c, Card::SKIP, -1));
        data->addCardToDeck(Card(c, Card::SKIP, -1));
        
        // Two Reverse per color
        data->addCardToDeck(Card(c, Card::REVERSE, -1));
        data->addCardToDeck(Card(c, Card::REVERSE, -1));
        
        // Two Draw Two per color
        data->addCardToDeck(Card(c, Card::DRAW_TWO, -1));
        data->addCardToDeck(Card(c, Card::DRAW_TWO, -1));
    }
    
    // Shuffle deck with seed 1234
    std::vector<Card> tempDeck;
    CardNode* current = data->deck;
    while (current) {
        tempDeck.push_back(current->card);
        current = current->next;
    }
    
    std::mt19937 rng(1234);
    std::shuffle(tempDeck.begin(), tempDeck.end(), rng);
    
    // Clear old deck
    while (data->deck) {
        CardNode* temp = data->deck;
        data->deck = data->deck->next;
        delete temp;
    }
    data->deck = nullptr;
    data->deckSize = 0;
    
    // Rebuild deck from shuffled cards
    for (int i = tempDeck.size() - 1; i >= 0; i--) {
        data->addCardToDeck(tempDeck[i]);
    }
    
    // Deal 7 cards to each player
    for (int i = 0; i < 7; i++) {
        for (int p = 0; p < data->numPlayers; p++) {
            Card card = data->drawFromDeck();
            data->players[p]->addCard(card);
        }
    }
    
    // Place top card on discard pile
    Card topCard = data->drawFromDeck();
    data->addToDiscard(topCard);
}

void UNOGame::playTurn() {
    GameData* data = getGameData(this);
    Player* player = data->players[data->currentPlayer];
    Card topCard = data->getTopCard();
    
    // Find a playable card
    CardNode* current = player->hand;
    CardNode* prev = nullptr;
    CardNode* playableCard = nullptr;
    CardNode* playablePrev = nullptr;
    
    // Priority: color match, value match, action card
    while (current) {
        bool canPlay = false;
        
        // Color match
        if (current->card.color == topCard.color) {
            canPlay = true;
        }
        // Value match (for number cards)
        else if (current->card.type == Card::NUMBER && topCard.type == Card::NUMBER &&
                 current->card.value == topCard.value) {
            canPlay = true;
        }
        
        if (canPlay && !playableCard) {
            playableCard = current;
            playablePrev = prev;
        }
        
        prev = current;
        current = current->next;
    }
    
    if (playableCard) {
        // Play the card
        Card played = playableCard->card;
        player->removeCard(playablePrev, playableCard);
        data->addToDiscard(played);
        
        // Handle action cards
        if (played.type == Card::SKIP) {
            // Skip next player - move twice
            if (data->clockwise) {
                data->currentPlayer = (data->currentPlayer + 2) % data->numPlayers;
            } else {
                data->currentPlayer = (data->currentPlayer - 2 + data->numPlayers) % data->numPlayers;
            }
            return;
        } else if (played.type == Card::REVERSE) {
            data->clockwise = !data->clockwise;
        } else if (played.type == Card::DRAW_TWO) {
            // Next player draws 2 and skips
            int nextPlayer = data->clockwise ? 
                (data->currentPlayer + 1) % data->numPlayers :
                (data->currentPlayer - 1 + data->numPlayers) % data->numPlayers;
            
            if (data->deck) {
                data->players[nextPlayer]->addCard(data->drawFromDeck());
            }
            if (data->deck) {
                data->players[nextPlayer]->addCard(data->drawFromDeck());
            }
            
            // Skip that player
            if (data->clockwise) {
                data->currentPlayer = (data->currentPlayer + 2) % data->numPlayers;
            } else {
                data->currentPlayer = (data->currentPlayer - 2 + data->numPlayers) % data->numPlayers;
            }
            return;
        }
    } else {
        // No playable card - draw one
        if (data->deck) {
            Card drawn = data->drawFromDeck();
            player->addCard(drawn);
            
            // Check if drawn card is playable
            bool canPlayDrawn = false;
            if (drawn.color == topCard.color) {
                canPlayDrawn = true;
            } else if (drawn.type == Card::NUMBER && topCard.type == Card::NUMBER &&
                       drawn.value == topCard.value) {
                canPlayDrawn = true;
            }
            
            // If playable, play it immediately
            if (canPlayDrawn) {
                // Find and remove the card we just added
                current = player->hand;
                prev = nullptr;
                while (current) {
                    if (current->card.color == drawn.color && 
                        current->card.type == drawn.type && 
                        current->card.value == drawn.value) {
                        player->removeCard(prev, current);
                        data->addToDiscard(drawn);
                        break;
                    }
                    prev = current;
                    current = current->next;
                }
            }
        }
    }
    
    // Move to next player
    if (data->clockwise) {
        data->currentPlayer = (data->currentPlayer + 1) % data->numPlayers;
    } else {
        data->currentPlayer = (data->currentPlayer - 1 + data->numPlayers) % data->numPlayers;
    }
}

bool UNOGame::isGameOver() const {
    GameData* data = getGameData(this);
    for (Player* player : data->players) {
        if (player->cardCount == 0) {
            return true;
        }
    }
    return false;
}

int UNOGame::getWinner() const {
    GameData* data = getGameData(this);
    for (int i = 0; i < data->numPlayers; i++) {
        if (data->players[i]->cardCount == 0) {
            return i;
        }
    }
    return -1;
}

std::string UNOGame::getState() const {
    GameData* data = getGameData(this);
    std::ostringstream oss;
    
    oss << "Player " << data->currentPlayer << "'s turn, ";
    oss << "Direction: " << (data->clockwise ? "Clockwise" : "Counter-clockwise") << ", ";
    
    Card topCard = data->getTopCard();
    oss << "Top: " << topCard.getColorString() << " " << topCard.getValueString() << ", ";
    
    oss << "Players cards: ";
    for (int i = 0; i < data->numPlayers; i++) {
        oss << "P" << i << ":" << data->players[i]->cardCount;
        if (i < data->numPlayers - 1) {
            oss << ", ";
        }
    }
    
    return oss.str();
}
#include <cstring>
#include <iostream>
#include <memory>
#include <random>
#include <string>

using std::string;

#define ACE_VALUE -1
#define DECK_SIZE 52
#define SUIT_SIZE 13

#define DEALER_WIN 0
#define PLAYER_WIN 1
#define NOBODY_WIN 2

enum Suits {
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES
};

struct Card {
    bool IsInUse;     // Is this card currently in someone's deck? If so, don't deal it.
    string Name;    // String representation of the card's value (suit not included).
    int Value;      // Integer value of the card. Ace's value is always taken to be 11
                    // unless that would cause the deck to bust.
    Suits Suit;     // CLUBS || DIAMONDS || HEARTS || SPADES
};

class Hand {
    private:
        int *_heldCardIndices;
        unsigned int _handSize;

    public:
        Hand();
        ~Hand();

        void addCard(int cardIndex);
        void clear();

        int *getCardIndices();
        int getSize();
};

Hand::Hand() {
    _heldCardIndices = nullptr;
    _handSize = 0;
}

Hand::~Hand() {
    this->clear();
}

void Hand::addCard(int cardIndex) {
    int *newArr = new int[_handSize + 1];

    memcpy(newArr, _heldCardIndices, sizeof(int) * _handSize);
    delete[] _heldCardIndices;

    _heldCardIndices = newArr;
    _heldCardIndices[_handSize] = cardIndex;
    _handSize++;
}

void Hand::clear() {
    delete[] _heldCardIndices;
    _handSize = 0;
}

int *Hand::getCardIndices() {
    return _heldCardIndices;
}

int Hand::getSize() {
    return _handSize;
}

const string SUIT_NAME_STRINGS[] = {
    "Clubs",
    "Diamonds",
    "Hearts",
    "Spades"
};

void initializeDeck(Card *deckBuffer) {
    for (int suit = 0; suit < 4; suit++) {
        // Initialize cards 2-10
        for (int i = 2; i < 11; i++) {
            deckBuffer[((i - 1) + (suit * SUIT_SIZE))] = {
                .IsInUse = false,
                .Name = std::to_string(i),
                .Value = i,
                .Suit = (Suits)suit
            };
        };

        // Initialize face cards
        deckBuffer[(suit * SUIT_SIZE)] = { .IsInUse = false, .Name = "Ace", .Value = ACE_VALUE, .Suit = (Suits)suit };
        deckBuffer[(10 + (suit * SUIT_SIZE))] = { .IsInUse = false, .Name = "Jack", .Value = 10, .Suit = (Suits)suit };
        deckBuffer[(11 + (suit * SUIT_SIZE))] = { .IsInUse = false, .Name = "Queen", .Value = 10, .Suit = (Suits)suit };
        deckBuffer[(12 + (suit * SUIT_SIZE))] = { .IsInUse = false, .Name = "King", .Value = 10, .Suit = (Suits)suit }; 
    }
}

string formatCardName(Card card) {
    return card.Name + " of " + SUIT_NAME_STRINGS[card.Suit];
}

int getRandIndex() {
    std::random_device os_seed;
    static const unsigned int seed = os_seed();
    static std::mt19937 randGenerator(seed);
    static std::uniform_int_distribution<unsigned int> distribute(0, DECK_SIZE - 1);

    return distribute(randGenerator);
}

int main() {
    Card *deck = new Card[DECK_SIZE];
    initializeDeck(deck);

    Hand hand;
    hand.addCard(getRandIndex());
    hand.addCard(getRandIndex());

    for (int i = 0; i < hand.getSize(); i++) {
        std::cout << formatCardName(deck[hand.getCardIndices()[i]]) << std::endl; 
    }
}  

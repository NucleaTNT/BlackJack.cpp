#include <cstring>
#include <iostream>
#include <memory>
#include <random>
#include <string>

using std::string;

#define ACE_VALUE (-1)
#define DECK_SIZE 52
#define SUIT_SIZE 13
#define WIN_VALUE 21

#define DEALER_WIN 0
#define PLAYER_WIN 1
#define NOBODY_WIN 2

size_t getRandIndex();

enum Suits {
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES
};

struct Card {
    bool IsInUse{}; // Is this card currently in someone's deck? If so, don't deal it.
    string Name;    // String representation of the card's value (suit not included).
    int Value{};    // Integer value of the card. Ace's value is always taken to be 11
                    // unless that would cause the deck to bust.
    Suits Suit{};   // CLUBS || DIAMONDS || HEARTS || SPADES
};

class Hand {
    private:
        Card *_deck;

        size_t *_heldCardIndices;
        size_t _handSize;

    public:
        explicit Hand(Card *deck);
        ~Hand();

        void addCard(size_t cardIndex);
        bool checkForBust();
        void clear();

        size_t *getCardIndices();
        size_t getSize() const;
        size_t getValue();
};

Hand::Hand(Card *deck) {
    _heldCardIndices = nullptr;
    _handSize = 0;
    _deck = deck;
}

Hand::~Hand() {
    this->clear();
}

void Hand::addCard(size_t cardIndex) {
    size_t *newArr = new size_t[_handSize + 1];

    memcpy(newArr, _heldCardIndices, sizeof(size_t) * _handSize);
    delete[] _heldCardIndices;

    _heldCardIndices = newArr;
    _heldCardIndices[_handSize] = cardIndex;
    _deck[cardIndex].IsInUse = true;
    _handSize++;
}

bool Hand::checkForBust() {
    if (this->getValue() > WIN_VALUE) {
        return true;
    }
    return false;
}

void Hand::clear() {
    for (size_t i = 0; i < _handSize; i++) {
        _deck[_heldCardIndices[i]].IsInUse = false;
    }

    delete[] _heldCardIndices;
    _handSize = 0;
}

size_t *Hand::getCardIndices() {
    return _heldCardIndices;
}

size_t Hand::getSize() const {
    return _handSize;
}

size_t Hand::getValue() {
    int sum = 0;
    for (int i = 0; i < _handSize; i++) {
        int curValue = _deck[_heldCardIndices[i]].Value;

        if (curValue == ACE_VALUE) {
            if ((sum + 11) > WIN_VALUE) {
                curValue = 1;
            } else {
                curValue = 11;
            }
        }

        sum += curValue;
    }

    return sum;
}

const string SUIT_NAME_STRINGS[] = {
    "Clubs",
    "Diamonds",
    "Hearts",
    "Spades"
};

size_t drawAvailableCardIndex(Card* deck) {
    size_t index, tick = 1000;
    do {
        index = getRandIndex();
        if (!deck[index].IsInUse) {
            return index;
        }
    } while (tick-- > 0);
    return -1;
}

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

string formatCardName(const Card& card) {
    return card.Name + " of " + SUIT_NAME_STRINGS[card.Suit];
}

size_t getRandIndex() {
    std::random_device os_seed;
    static const unsigned int seed = os_seed();
    static std::mt19937 randGenerator(seed);
    static std::uniform_int_distribution<unsigned int> distribute(0, DECK_SIZE - 1);

    return distribute(randGenerator);
}

void dealerChoiceLoop(Card *deck, Hand *dealerHand) {
    while (dealerHand->getValue() < 17 && !dealerHand->checkForBust()) {
        dealerHand->addCard(drawAvailableCardIndex(deck));
    }
}

void playerChoiceLoop(Card *deck, Hand *playerHand) {
    while (!playerHand->checkForBust()) {
        std::cout << "Your hand's value is " << playerHand->getValue() << "\n";

        string playerInput;
        do {
            playerInput = "";
            std::cout << "Would you like to [h]it or [s]tick? ";
            std::cin >> playerInput;
            std::cout << "\n\n";
        } while (!(playerInput[0] == 'h' || playerInput[0] == 's'));

        if (playerInput[0] == 'h') {
            playerHand->addCard(drawAvailableCardIndex(deck));
        } else {
            break;
        }
    }
}

int main() {
    Card *deck = new Card[DECK_SIZE];
    initializeDeck(deck);

    Hand playerHand(deck), dealerHand(deck);

    for (int i = 0; i < 2; i++) {
        playerHand.addCard(drawAvailableCardIndex(deck));
        dealerHand.addCard(drawAvailableCardIndex(deck));
    }

    playerChoiceLoop(deck, &playerHand);
    dealerChoiceLoop(deck, &dealerHand);

    size_t gameOutcome = -1;
    if (dealerHand.checkForBust()) {
        if (playerHand.checkForBust()) {
            gameOutcome = NOBODY_WIN;
        } else {
            gameOutcome = PLAYER_WIN;
        }
    } else if (playerHand.checkForBust()) {
        gameOutcome = DEALER_WIN;
    } else {
        if (playerHand.getValue() > dealerHand.getValue()) {
            gameOutcome = PLAYER_WIN;
        } else if (playerHand.getValue() < dealerHand.getValue()) {
            gameOutcome = DEALER_WIN;
        } else {
            gameOutcome = NOBODY_WIN;
        }
    }

    for (int i = 0; i < playerHand.getSize(); i++) {
        std::cout << formatCardName(deck[playerHand.getCardIndices()[i]]) << (i + 1 == playerHand.getSize() ? "\n" : ", ");
    }
    std::cout << "Player Value: " << playerHand.getValue() << (playerHand.checkForBust() ? " | Bust\n\n" : " | Not Bust\n\n");

    for (int i = 0; i < dealerHand.getSize(); i++) {
        std::cout << formatCardName(deck[dealerHand.getCardIndices()[i]]) << (i + 1 == dealerHand.getSize() ? "\n" : ", ");
    }
    std::cout << "Dealer Value: " << dealerHand.getValue() << (dealerHand.checkForBust() ? " | Bust\n" : " | Not Bust\n");

    switch (gameOutcome) {
        case DEALER_WIN: {
            std::cout << "\nThe Dealer has won!\n";
            break;
        }

        case PLAYER_WIN: {
            std::cout << "\nCongratulations you have won!\n";
        }

        case NOBODY_WIN: {
            std::cout << "\nTie! Nobody wins.\n";
            break;
        }

        default: {
            std::cout << "\nThere was an issue calculating the winner of this game!\n";
            break;
        }
    }
}

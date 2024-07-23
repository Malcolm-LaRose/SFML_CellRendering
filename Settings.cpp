#include "Settings.h"

// Random number generation

std::mt19937 gen(std::random_device{}());

int genRandomInt(const int& min, const int& max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

float genRandomFloat(const int& min, const int& max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

bool genRandomBool(const float& pTrue) {
    std::bernoulli_distribution dist(pTrue);
    return dist(gen);
}

// General functions

const int& dists(const sf::Vector2i& pos1, const sf::Vector2i& pos2) {
    return std::sqrt(((pos2.x - pos1.x) * (pos2.x - pos1.x)) + ((pos2.y - pos1.y) * (pos2.y - pos1.y)));
}

// Algorithm for integer to string conversion

static const char digit_pairs[201] = {
  "00010203040506070809"
  "10111213141516171819"
  "20212223242526272829"
  "30313233343536373839"
  "40414243444546474849"
  "50515253545556575859"
  "60616263646566676869"
  "70717273747576777879"
  "80818283848586878889"
  "90919293949596979899"
};

static const int BUFFER_SIZE = 11;
char buf[BUFFER_SIZE]; // My modification

std::string itostr(unsigned int val)
{
    char* it = (char*)&buf[BUFFER_SIZE - 2];
    int div = val / 100;
    while (div) {
        std::memcpy(it, &digit_pairs[2 * (val - div * 100)], 2);
        val = div;
        it -= 2;
        div = val / 100;
    }
    std::memcpy(it, &digit_pairs[2 * val], 2);
    if (val < 10)
        it++;

    return std::string((char*)it, (char*)&buf[BUFFER_SIZE] - (char*)it);
}
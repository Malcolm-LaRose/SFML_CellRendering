#pragma once

#ifndef VECHASH_H
#define VECHASH_H


#include <SFML/Graphics.hpp>
#include <functional>


// Define hash function for sf::Vector2i
namespace std {
	template<>
	struct hash<sf::Vector2i> {
		size_t operator()(const sf::Vector2i& vec) const {
			// Use a simple hash combining x and y values
			return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1);
		}
	};
}

#endif
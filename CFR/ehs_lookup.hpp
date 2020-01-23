#ifndef EHS_LOOKUP
#define EHS_LOOKUP

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

extern "C" {
#include "hand_index.h"
}

class EHSLookup {
	hand_indexer_t indexer[4];
	std::vector<std::vector<float>> lookup;

public:
	explicit EHSLookup(const char* filename) : lookup(4) {
		uint8_t iiii[] = { 2 };
		uint8_t iiiii[] = { 2,3 };
		uint8_t iiiiii[] = { 2,4 };
		uint8_t iiiiiii[] = { 2,5 };
		assert(hand_indexer_init(1, iiii, &indexer[0]));
		assert(hand_indexer_init(2, iiiii, &indexer[1]));
		assert(hand_indexer_init(2, iiiiii, &indexer[2]));
		assert(hand_indexer_init(2, iiiiiii, &indexer[3]));

		if (!load_lookup(filename))
			throw std::runtime_error("EHSLookup file could not be loaded.");
	}

	float raw(int round, size_t idx) { return lookup[round][idx]; }

	size_t size(int round) { return lookup[round].size(); }

	bool load_lookup(const char* filename) {
		std::ifstream file(filename, std::ios::in | std::ios::binary);

		size_t round_size;
		for (size_t i = 0; i < 4; ++i) {
			round_size = indexer[i].round_size[i == 0 ? 0 : 1];
			lookup[i] = std::vector<float>(round_size);
			file.read(reinterpret_cast<char*>(&lookup[i][0]),
				sizeof(float)* round_size);
		}
		file.close();
		return true;
	}

	~EHSLookup() {}
};

#endif
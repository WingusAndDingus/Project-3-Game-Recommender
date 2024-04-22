#include <iostream>
#include <unordered_map>
#include <string>
#include "GameStruct.cpp"

using namespace std;

class HashTable {
private:
	unordered_map<string, GameStruct> games;

public:
    HashTable() = default;

	void insertReview(const string& title, const GameStruct& reviewInfo) {
		games[title] = reviewInfo;
	}

	void findReview(const string& title) const {
		auto it = games.find(title);
		if (it != games.end()) {
            cout << "Game Found!" << endl;
            cout << "Title: " << it->first << endl;
            cout << "Platform: " << it->second.platform << endl;
            cout << "Rating: " << it->second.ignRating << endl;
            cout << "Genre(s): ";
            for (const auto &genre: it->second.genres) {
                cout << genre << " ";
            }
            cout << "\n";
        } else {
            cout << "Game Not Found in Hash Table :(" << endl;
        }
	}

    // Overloaded subscript operator (access-only)
    GameStruct& operator[](const string& title) {
        return games[title];
    }

    // Overloaded subscript operator (for setting values)
    const GameStruct& operator[](const string& title) const {
        auto it = games.find(title);
        if (it != games.end()) {
            return it->second;
        }
        throw std::out_of_range("Game not found in hash table.");
    }
	
};
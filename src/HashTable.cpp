#include <iostream>
#include <unordered_map>
#include <string>
#include "GameStruct.cpp"

using namespace std;

class HashTable {
private:
	// Hash Function
	static size_t hashFunc(const string& title) {
		size_t hash = 0;
		for (char c : title) {
			hash = hash * 31 + static_cast<size_t>(c);
		}
		return hash;
	}

	size_t bucketCount = 12209; // Prime number closest to 12,206 which is the # of IGN reviews

	//Actual unordered map
	unordered_map<string, GameStruct, decltype(&hashFunc)> IGNReviews;

public:
	HashTable() : IGNReviews(bucketCount, &hashFunc){
		// Constructor Empty ?
	}

	void insertReview(const string& title, const GameStruct& reviewInfo) {
		IGNReviews[title] = reviewInfo;
	}

	void findReview(const string& title) {
		auto game = IGNReviews.find(title);
		if (game != IGNReviews.end()) {
			cout << "Game Found!" << endl;
			cout << "Title: " << game->second.title << endl;
			cout << "Platform: " << game->second.platform << endl;
			cout << "Rating: " << game->second.rating << endl;
			cout << "Genre(s): ";
			// Iterate through all the genres
			int i = 1;
			for (const auto& genre : game->second.genre) {
				if (i == 1) {
					cout << genre;
				}
				else {
					cout << ", " << genre;
				}
			}
			cout << endl;
		}
		else {
			cout << "Game Not Found in Hash Table :(" << endl;
		}
	}
	
};
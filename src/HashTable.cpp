#include <iostream>
#include <unordered_map>
#include <string>
#include <random>
#include <algorithm>
#include "GameStruct.cpp"

using namespace std;

//Class for Main Hashtable with all the games
class HashTableIGN {
private:
	// Calculate the similarity score from Genre
	double calculateGenreSimilarity(const vector<string>& userGenres, const vector<string>& gameGenres) {
		double score = 0.0;
		for (const auto& genre : userGenres) {
			if (find(gameGenres.begin(), gameGenres.end(), genre) != gameGenres.end()) {
				score += 1.0;  // Increment for each matching genre
			}
		}
		return score;
	}
	// Hash Function
	static size_t hashFunc(const string& title) {
		size_t hash = 0;
		for (char c : title) {
			hash = hash * 31 + static_cast<size_t>(c);
		}
		return hash;
	}

	size_t bucketCount = 12209; // Prime number closest to 12,206 which is the # of IGN game Reviews

	//Actual unordered map
	unordered_map<string, GameStruct, decltype(&hashFunc)> IGNReviews;

public:
	HashTableIGN() : IGNReviews(bucketCount, &hashFunc){
		// Constructor Empty ?
	}

	void insertReview(const string& title, const GameStruct& reviewInfo) {
		IGNReviews[title] = reviewInfo;
	}
	// Find a specific IGN review
	void findReview(const string& title, bool print) {
		auto game = IGNReviews.find(title);
		if (print) {
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
	}
	// Update bool
	/* void hasSteam(const string& title) {
		if (IGNReviews.find(title) != IGNReviews.end()) {
			IGNReviews.find(title).hasSteam = true;
		}
	}*/
	int Iterate() {
		int count = 0;
		for (const auto& pair : IGNReviews) {
			count++;
		}
		return count;
	}
	// Recommends games based on user game
	void recommendGames(const string& userFavorite, HashTableSteam& SteamReviews) {
		vector<pair<double, GameStruct>> scoredGames;

		if (IGNReviews.find(userFavorite) != IGNReviews.end()) {
			const GameStruct& favorite = IGNReviews[userFavorite];
			for (const auto& pair : IGNReviews) {
				const GameStruct& game = pair.second;
				if (game.title == userFavorite) continue; // Skip the same game
				double score = 0.0;

				// Platform similarity
				if (game.platform == favorite.platform) {
					score += 1.0;
				}

				// Genre similarity
				score += calculateGenreSimilarity(favorite.genre, game.genre) * 10.0; // Weighted more heavily

				// User rating impact (average ratings)
				if (game.hasSteam) {
					score += SteamReviews.avgRev(game.title);
				}
				else {
					score += 1;
				}
				if (scoredGames.size() < 5) {
					//start the 5
					scoredGames.emplace_back(score, game);
					if (scoredGames.size() == 5) {
						//Sort the list in descending order
						sort(scoredGames.begin(), scoredGames.end(), [](const std::pair<double, GameStruct>& a, const std::pair<double, GameStruct>& b) {
							return a.first > b.first;
							});
					}
				}
				else if(scoredGames[0].first < score) {
					scoredGames[0] = make_pair(score,game);
				}
				else if (scoredGames[1].first < score) {
					scoredGames[1] = make_pair(score, game);
				}
				else if (scoredGames[2].first < score) {
					scoredGames[2] = make_pair(score, game);
				}
				else if (scoredGames[3].first < score) {
					scoredGames[3] = make_pair(score, game);
				}
				else if (scoredGames[4].first < score) {
					scoredGames[4] = make_pair(score, game);
				}
				else if (scoredGames[5].first < score) {
					scoredGames[5] = make_pair(score, game);
				}
			}

			// Sorting in descending order of score
			sort(scoredGames.begin(), scoredGames.end(), [](const pair<double, GameStruct>& a, const pair<double, GameStruct>& b) {
				return a.first > b.first;
				});

			//Print out recommendations;
			cout << "Recommended Games: ";
			for (int i = 0; i < scoredGames.size(); i++) {
				cout << scoredGames[i].second.title;
			}
			cout << endl << "More Details:" << endl;
			for (int i = 0; i < scoredGames.size(); i++) {
				cout << i << ". " << scoredGames[i].second.title << ":" << endl;
				cout << "Genre: ";
				// Iterate through all the genres
				int j = 1;
				for (const auto& genre : scoredGames[i].second.genre) {
					if (j == 1) {
						cout << genre;
					}
					else {
						cout << ", " << genre;
					}
				}
				cout << endl; // done with genres line
				cout << "Rating: " << scoredGames[i].second.rating << endl;
				cout << "Platform: " << scoredGames[i].second.platform << endl;
			}
		}

		cout << "Game not Found. (Make sure you didn't misspell the title) " << endl; // if favorite not found
	}

	
};

//Class for Steam Reviews HashTable
class HashTableSteam {
private:
	// Hash Function
	static size_t hashFunc(const string& title) {
		size_t hash = 0;
		for (char c : title) {
			hash = hash * 31 + static_cast<size_t>(c);
		}
		return hash;
	}

	size_t bucketCount = 400003; // Prime number close to 301,789 which is the # of Steam reviews
	mt19937 gen;
	vector<int> userIDs;

	//Actual unordered map
	unordered_map<int, ReviewStruct, decltype(&hashFunc)> SteamReviews;

public:
	HashTableSteam() : gen(69), SteamReviews(bucketCount, &hashFunc) {
		// Constructor used for initializing hash table & seeding random generator
		// the seed is constant because it doesn't need to be different with every run
	}

	void insertReview(ReviewStruct& reviewInfo) {
		//Generate a 6 digit unique user ID for this specific review
		int userID = gen();
		// make sure that this ID isn't already in use
		while (SteamReviews.find(userID) != SteamReviews.end()) {
			//Generate another 6 digit unique user ID
			userID = gen();
		}
		// Add it to the hash table
		SteamReviews[userID] = reviewInfo;
		reviewInfo.UserID = userID;
		userIDs.push_back(userID);
	}

	void findFirst5Reviews(const string& title) {
		vector<int> users;
		int count = 0;
		auto review = SteamReviews.begin();
		// Find the first 5  reviews for this game
		while (review != SteamReviews.end() && count < 5) {
			if (review->second.gameTitle == title) {
				users.push_back(review->first);
				count++;
			}
			++review;
		}
		// Print out the 5 reviews
		for (int i = 0; i < users.size(); i++) {
			cout << "User ID:" << SteamReviews[users[i]].UserID;
			if (SteamReviews[users[i]].recommends < 0) {
				cout << " does NOT recommend this game." << endl;
			}
			else {
				cout << " recommends this game!" << endl;
			}
			cout << "Review: " << SteamReviews[users[i]].reviewText << endl;
			cout << endl;
		}
	}
	//Print a random Review
	void printOne() {
		uniform_int_distribution<size_t> dist(0, userIDs.size() - 1);
		size_t randomId = dist(gen);
		//Search for a random review
		auto review = SteamReviews[randomId];
		//Print it out
		cout << "User ID:" << review.UserID;
		if (review.recommends < 0) {
			cout << " does NOT recommend " << review.gameTitle << endl;
		}
		else {
			cout << " recommends " << review.gameTitle << " !" << endl;
		}
		cout << "Review: " << review.reviewText << endl;
		cout << endl;
	}
	//Find average rating for certain game
	double avgRev(const string& title) {
		double total = 0.0;
		int count = 0;
		for (const auto& pair : SteamReviews) {
			if (pair.second.gameTitle == title) {
				total += pair.second.recommends;
				count++;
			}
		}
		return (total / ((double)count))*10; // *10 because otherwise it'll be too small to matter
	}
};
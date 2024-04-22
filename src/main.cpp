#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include "json.hpp" //only works if you have the library installed
#include "HashTable.cpp" // contains the HashTable class and GameStruct class

using json = nlohmann::json;
using namespace std;

double calculateGenreSimilarity(const vector<string>& userGenres, const vector<string>& gameGenres) {
    double score = 0.0;
    for (const auto& genre : userGenres) {
        if (find(gameGenres.begin(), gameGenres.end(), genre) != gameGenres.end()) {
            score += 1.0;  // Increment for each matching genre
        }
    }
    return score;
}

vector<GameStruct> recommendGames(const string& userFavorite, const map<string, GameStruct>& games) {
    vector<pair<double, GameStruct>> scoredGames;

    if (games.find(userFavorite) != games.end()) {
        GameStruct favorite = games.at(userFavorite);

        for (const auto& pair : games) {
            const GameStruct& game = pair.second;
            if (game.title == favorite.title) continue; // Skip the same game

            double score = 0.0;

            // Platform similarity
            if (game.platform == favorite.platform) {
                score += 1.0;
            }

            // Genre similarity
            score += calculateGenreSimilarity(favorite.genres, game.genres) * 10.0; // Weighted more heavily

            // User rating impact (average ratings)
            if (!game.userRatings.empty()) {
                double avgRating = accumulate(game.userRatings.begin(), game.userRatings.end(), 0.0) / game.userRatings.size();
                score += avgRating;  // Add average user rating to the score
            }

            // Check if it has user reviews
            if (game.hasOthers) {
                score += 2.0;  // Bonus points for having additional user data
            }

            scoredGames.emplace_back(score, game);
        }

        // Sorting in descending order of score
        sort(scoredGames.begin(), scoredGames.end(), [](const pair<double, GameStruct>& a, const pair<double, GameStruct>& b) {
            return a.first > b.first;
        });


        vector<GameStruct> recommendations;
        for (int i = 0; i < min(5, (int)scoredGames.size()); i++) {
            recommendations.push_back(scoredGames[i].second);
        }
        return recommendations;
    }

    return {};  // Return empty if favorite not found
}

int main() {
    map<string, GameStruct> games;
    HashTable gamesHashTable;

    // Open the JSON file
    ifstream jsonFile("game.json");
    json jsonData;
    if (!jsonFile.is_open()) {
        cout << "Error opening IGN Reviews JSON file." << endl;
        return 1;
    }

    // Parse the JSON file data
    jsonFile >> jsonData;

    for (const auto& item : jsonData["data"]) {
        GameStruct game;
        game.title = item["game"];
        game.platform = item["platform"];
        game.ignRating = item["rating"];
        game.genres = item["genre"].get<vector<string>>();
        game.hasOthers = false;
        games[game.title] = game;
        gamesHashTable.insertReview(game.title, game);
    }

    // Close the file
    jsonFile.close();

    //Open tsv file
    ifstream tsvFile("dataset.tsv");
    if (!tsvFile.is_open()) {
        cout << "Error opening Steam Reviews dataset TSV file." << endl;
        return 1;
    }

    // Extract data from Steam tsv file and store it in the map & hash table
    string line;

    if (!getline(tsvFile, line)) {
        cout << "Error: File is empty or cannot read the header." << endl;
        return 1;
    }

    while (getline(tsvFile, line)) {
        stringstream ss(line);
        string cell;
        vector<string> row;

        while (getline(ss, cell, '\t')) {
            row.push_back(cell);
        }

        if (row.size() == 4) {
            string title = row[0];
            string reviewText = row[1];
            double rating;
            try {
                rating = stod(row[2]);
            } catch (const invalid_argument& ia) {
                cerr << "Invalid rating for game " << title << ": " << row[2] << endl;
                continue; // Skip this row due to invalid data
            }
            bool hasOthers = (row[3] == "1.0");

            if (games.find(title) != games.end()) {
                games[title].reviews.push_back(reviewText);
                gamesHashTable[title].reviews.push_back(reviewText);
                games[title].userRatings.push_back(rating);
                gamesHashTable[title].userRatings.push_back(rating);
                games[title].hasOthers = hasOthers;
                gamesHashTable[title].hasOthers = hasOthers;
            } else {
                // Create a new game entry if not found in JSON
                GameStruct newGame;
                newGame.title = title;
                newGame.reviews.push_back(reviewText);
                newGame.userRatings.push_back(rating);
                newGame.hasOthers = hasOthers;
                // Defaults for missing JSON data
                newGame.platform = "Unknown";
                newGame.ignRating = -1;  // Indicate missing IGN rating
                games[title] = newGame;
                gamesHashTable[title] = newGame;
            }
        }
    }
    tsvFile.close();

    // Example of accessing a specific game review by name
    string gameTitle = "Counter-Strike";

    if (games.find(gameTitle) != games.end()) {
        const GameStruct& game = games[gameTitle];
        cout << "Title: " << game.title << "\nPlatform: " << game.platform << "\nIGN Rating: " << game.ignRating << "\nGenres: ";
        for (const string& genre : game.genres) {
            cout << genre << " ";
        }
        cout << "\n3 User Reviews:\n";
        for (int i = 0; i < 3; ++i) {
            cout << "Review: " << game.reviews[i] << endl;
        }
    } else {
        cout << "Game not found." << endl;
    }

    // Prompt user to input favorite game title
    string userFavorite;
    cout << "Enter your favorite game: ";
    getline(cin, userFavorite);

    // Recommend games based ons similarity score
    vector<GameStruct> recommendations = recommendGames(userFavorite, games);
    cout << "Based on your interest in " << userFavorite << ", we recommend:\n";
    for (const auto& rec : recommendations) {
        cout << rec.title << " on " << rec.platform << " rated " << rec.ignRating;
        cout << ", Genres: ";
        for (const auto& genre : rec.genres) {
            cout << genre << " ";
        }
        cout << "\n";
    }
    return 0;
}
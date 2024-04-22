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
        const GameStruct& favorite = games.at(userFavorite);

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

void displayReviews(const string& gameTitle, const map<string, GameStruct>& games) {
    string displayMore = "y";
    GameStruct currentGame = games.at(gameTitle);

    int currentReview = 0;

    while (displayMore != "n") {
        cout << "\nUser Reviews:\n";
        for (int i = 1; i <= 3; ++i) {
            cout << "Review " << i << ". " << currentGame.reviews[currentReview] << endl;
            ++currentReview;
        }
        cout << "Display 3 more reviews? (y/n) ";
        getline(cin, displayMore);
    }
    cout << "\n";
}

int main() {
    cout << "Loading main menu...\n" << endl;

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

    int userSelection = 0;

    while (true) {
        // MAIN MENU
        cout << "Main Menu (enter a selection 1-4)\n"
                "1. Print all games (title, platform, rating, genre)\n"
                "2. Find a specific game in the database\n"
                "3. Get game recommendations based on a specific game\n"
                "4. Exit" << endl;
        cin >> userSelection;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (userSelection == 1) {
            for (const auto& entry : games) {
                cout << "Title: " << entry.second.title << "\nPlatform: " << entry.second.platform << "\nIGN Rating: "
                     << entry.second.ignRating << "\nGenres: ";
                for (size_t i = 0; i < entry.second.genres.size(); ++i) {
                    cout << entry.second.genres[i];
                    if (i != entry.second.genres.size() - 1) cout << ", "; // Add comma only if it's not the last item
                }
                cout << "\n\n"; // Finish with a newline for separation
            }
        }

        if (userSelection == 2) {
            string lookupTitle;
            cout << "Enter the game title: ";
            getline(cin, lookupTitle);

            if (games.find(lookupTitle) != games.end()) {
                const GameStruct &game = games[lookupTitle];
                cout << "Title: " << game.title << "\nPlatform: " << game.platform << "\nIGN Rating: " << game.ignRating
                     << "\nGenres: ";
                for (const string &genre: game.genres) {
                    cout << genre << " ";
                }
                string viewReviews;
                cout << "Would you like to view user reviews for this game? (type in y/n): ";
                getline(cin, viewReviews);
                displayReviews(lookupTitle, games);
            }
            else {
                cout << "Game not found." << endl;
            }
        }

        if (userSelection == 3) {
            // Prompt user to input favorite game title
            string userFavorite;
            cout << "Enter your favorite game: ";
            getline(cin, userFavorite);

            // Recommend games based ons similarity score
            vector<GameStruct> recommendations = recommendGames(userFavorite, games);
            cout << "Based on your interest in " << userFavorite << ", we recommend:\n";
            int recNumber = 1;
            for (const auto &rec: recommendations) {
                cout << recNumber << ". " << rec.title << " on " << rec.platform << " rated " << rec.ignRating;
                cout << ", Genres: ";
                for (const auto &genre: rec.genres) {
                    cout << genre << " ";
                }
                cout << "\n";
                ++recNumber;
            }
            string viewReviews = "n";
            cout << "Would you like to view user reviews for one of these games? (type the index 1-5, or \"n\" to exit): ";
            getline(cin, viewReviews);
            if (viewReviews == "n") {
                continue;
            }
            else {
                try {
                    int lookUpReviewIndex = stoi(viewReviews) - 1;
                    if (lookUpReviewIndex >= 0 && lookUpReviewIndex < recommendations.size()) {
                        displayReviews(recommendations[lookUpReviewIndex].title, games);
                    } else {
                        cout << "Invalid game index.\n";
                    }
                } catch (const invalid_argument& ia) {
                    cout << "Invalid input. Please enter a number or 'n' to skip.\n";
                } catch (const out_of_range& oor) {
                    cout << "Input out of range. Please try again.\n";
                }
            }

        }

        if (userSelection == 4) break;
    }
    cout << "\nThank you for using the Game Recommender!" << endl;
    return 0;
}
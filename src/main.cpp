#include <fstream>
#include <sstream>
#include <map>
#include <chrono>
#include <nlohmann/json.hpp> //only works if you have the libray installed
#include "HashTable.cpp" // contains the HashTable class and GameStruct class

using json = nlohmann::json;
using namespace std;

// Might take out later::
/*
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
            score += calculateGenreSimilarity(favorite.genre, game.genre) * 10.0; // Weighted more heavily

            // User rating impact (average ratings)
            //if (!game.userRatings.empty()) {
              //  double avgRating = accumulate(game.userRatings.begin(), game.userRatings.end(), 0.0) / game.userRatings.size();
               // score += avgRating;  // Add average user rating to the score
            //}

            // Check if it has user reviews
            //if (game.hasOthers) {
                //score += 2.0;  // Bonus points for having additional user data
            //}

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
}*/
// ^^Might take out later
// Helper Functions
void displayReviews(const string& gameTitle, const map<string, GameStruct>& games) {
    string displayMore = "y";
    GameStruct currentGame = games.at(gameTitle);

    int currentReview = 0;

    while (displayMore != "n") {
        std::cout << "\nUser Reviews:\n";
        for (int i = 1; i <= 3; ++i) {
            //std::cout << "Review " << i << ". " << currentGame.reviews[currentReview] << endl; this won't work
            ++currentReview;
        }
        std::cout << "Display 3 more reviews? (y/n) ";
        getline(cin, displayMore);
    }
    std::cout << "\n";
}

int main() {
    std::cout << "Loading main menu...\n" << endl;
    // Create map to store IGN reviews
    map<string, GameStruct> IGNMap;
    // Create a map to store Other Steam Reviews
    map<string, ReviewStruct> SteamMap;

    // Create hashTable store IGN Reviews
    HashTableIGN IGNHashTable;
    // Create a hashTable to store Other Steam Reviews
    HashTableSteam SteamHashTable;

    // Open the JSON file
    ifstream JSONfile("game.json");
    if (!JSONfile.is_open()) {
        std::cout << "Error opening IGN Reviews JSON file." << endl;
        return 1;
    }

    // Parse the JSON file data
    json jsonData;
    JSONfile >> jsonData;

    // Close the file
    JSONfile.close();

    // Extract data from IGN JSON and store it in the map & hash table
    for (const auto& entry : jsonData["data"]) {
        GameStruct info;
        info.title = entry["game"];
        info.platform = entry["platform"];
        info.rating = entry["rating"];
        info.genre = entry["genre"].get<vector<string>>();
        info.hasSteam = false; // For now, it doesn't have other reviews
        //Add it to the map
        IGNMap[info.title] = info;
        //Add it to the hash table
        IGNHashTable.insertReview(entry["game"], info);
    }

    //Open tsv file
    ifstream TSVfile("dataset.tsv");
    if (!TSVfile.is_open()) {
        std::cout << "Error opening Steam Reviews dataset CSV file." << endl;
        return 1;
    }

    //Parse TSV data and store
    // Extract data from Steam tsv file and store it in the map & hash table
    string line;

    if (!getline(TSVfile, line)) {
        std::cout << "Error: File is empty or cannot read the header." << endl;
        return 1;
    }

    while (getline(TSVfile, line)) {
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
            }
            catch (const invalid_argument& ia) {
                cerr << "Invalid rating for game " << title << ": " << row[2] << endl;
                continue; // Skip this row due to invalid data
            }
            bool hasSteam = (row[3] == "1.0");

            if (IGNMap.find(title) != IGNMap.end()) {
                //Create a Review struct
                ReviewStruct tempRev;
                tempRev.gameTitle = title;
                tempRev.recommends = rating;
                tempRev.reviewText = reviewText;
                tempRev.UserID = 0; // should be changed in the insertReview func
                //update the steam reviews hash table
                SteamHashTable.insertReview(tempRev);
                //IGNHashTable.hasSteam(title); // update the bool might not be needed
                // update the steam reviews map
                SteamMap[title] = tempRev;
            }
            else {
                // Game Doesn't Exist Create a new game entry if not found in JSON
                /*GameStruct newGame;
                newGame.title = title;
                newGame.reviews.push_back(reviewText);
                newGame.userRatings.push_back(rating);
                newGame.hasOthers = hasOthers;
                // Defaults for missing JSON data
                newGame.platform = "Unknown";
                newGame.ignRating = -1;  // Indicate missing IGN rating
                games[title] = newGame;
                gamesHashTable[title] = newGame;*/
                std::cout << title << " doesn't exist in JSON" << endl;
            }
        }
    }
    TSVfile.close(); // close file
    
    // Actual Menu
    int userSelection = 0;
    while (true) {
        // MAIN MENU
        std::cout << "Main Menu (enter a selection 1-4)\n"
            "1. Print all games (title, platform, rating, genre)\n"
            "2. Find a specific game in the database\n"
            "3. Get game recommendations based on a specific game\n"
            "4. Exit" << endl;
        cin >> userSelection;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (userSelection == 1) {
            std::cout << "Do you want to just compare the times? (y/n)" << endl;
            string choice;
            cin >> choice;
            if (choice == "y") {
                std::cout << "Map Time: ";
                auto start = std::chrono::high_resolution_clock::now();
                //Iterate through IGNMap
                int count = 0;
                for (const auto& game : IGNMap) {
                    //DO nothing
                    count++;
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> time = end - start;
                std::cout << "Iterating through " << count << " games in the map took " << time.count() << " milliseconds.";
                std::cout << "Hash Table Time: ";
                //Iterate through IGNHashTable
                start = std::chrono::high_resolution_clock::now();
                count = IGNHashTable.Iterate();
                end = std::chrono::high_resolution_clock::now();
                time = end - start;
                std::cout << "Iterating through " << count << " games in the hash table took " << time.count() << " milliseconds.";

            }
            else {
                std::cout << "OK, get ready to be spammed" << endl;
                // Actually print all the reviews;
                //map
                int count = 0;
                auto start = std::chrono::high_resolution_clock::now();
                for (const auto& game : IGNMap) {
                    count++;
                    cout << "Title: " << game.second.title << "\n";
                    cout << "Platform: " << game.second.platform << "\n";
                    cout << "Rating: " << game.second.rating << "\n";
                    cout << "Genre(s): ";
                    for (const auto& genre : game.second.genre) {
                        cout << genre << ", ";
                    }
                    cout << endl;
                    cout << endl;
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> time = end - start;
                std::cout << "Iterating through " << count << " games in the map took " << time.count() << " milliseconds.";
                //hashtable (only print them once, so dont print these
                start = std::chrono::high_resolution_clock::now();
                count = IGNHashTable.Iterate();
                end = std::chrono::high_resolution_clock::now();
                time = end - start;
                std::cout << "Iterating through " << count << " games in the hash table took " << time.count() << " milliseconds.";
            }
            std::cout << "\033[2J\033[1;1H"; // clears screen
        }

        if (userSelection == 2) {
            string lookupTitle;
            std::cout << "Enter the game title: ";
            getline(cin, lookupTitle);
            auto start = std::chrono::high_resolution_clock::now();
            GameStruct found = IGNMap[lookupTitle];
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> time = end - start;
            std::cout << "Look up through Map took " << time.count() << " milliseconds." << endl;
            start = std::chrono::high_resolution_clock::now();
            IGNHashTable.findReview(lookupTitle, false);
            end = std::chrono::high_resolution_clock::now();
            time = end - start;
            std::cout << "Look up through Hash Table took " << time.count() << " milliseconds." << endl;
            if (IGNMap.find(lookupTitle) != IGNMap.end()) {
                const GameStruct& game = IGNMap[lookupTitle];
                std::cout << "Title: " << game.title << "\nPlatform: " << game.platform << "\nIGN Rating: " << game.rating
                    << "\nGenres: ";
                for (const string& genre : game.genre) {
                    std::cout << genre << " ";
                }
                string viewReviews;
                std::cout << "Would you like to view user reviews for this game? (type in y/n): ";
                getline(cin, viewReviews);
                if (viewReviews == "y") {
                    displayReviews(lookupTitle, IGNMap); // This is not going to work
                }
            }
            else {
                std::cout << "Game not found." << endl;
            }
            std::cout << "\033[2J\033[1;1H"; // clears screen
        }

        if (userSelection == 3) {
            // Prompt user to input favorite game title
            string userFavorite;
            std::cout << "Enter your favorite game: ";
            getline(cin, userFavorite);
            IGNHashTable.recommendGames(userFavorite, SteamHashTable);
            /*
            // Recommend games based ons similarity score
            vector<GameStruct> recommendations = recommendGames(userFavorite, games);
            std::cout << "Based on your interest in " << userFavorite << ", we recommend:\n";
            int recNumber = 1;
            for (const auto& rec : recommendations) {
                std::cout << recNumber << ". " << rec.title << " on " << rec.platform << " rated " << rec.ignRating;
                std::cout << ", Genres: ";
                for (const auto& genre : rec.genres) {
                    std::cout << genre << " ";
                }
                std::cout << "\n";
                ++recNumber;
            }
            string viewReviews = "n";
            std::cout << "Would you like to view user reviews for one of these games? (type the index 1-5, or \"n\" to exit): ";
            getline(cin, viewReviews);
            if (viewReviews == "n") {
                continue;
            }
            else {
                try {
                    int lookUpReviewIndex = stoi(viewReviews) - 1;
                    if (lookUpReviewIndex >= 0 && lookUpReviewIndex < recommendations.size()) {
                        displayReviews(recommendations[lookUpReviewIndex].title, games);
                    }
                    else {
                        std::cout << "Invalid game index.\n";
                    }
                }
                catch (const invalid_argument& ia) {
                    std::cout << "Invalid input. Please enter a number or 'n' to skip.\n";
                }
                catch (const out_of_range& oor) {
                    std::cout << "Input out of range. Please try again.\n";
                }
            }*/
            std::cout << "\033[2J\033[1;1H"; // clears screen
        }

        if (userSelection == 4) break;
    }
    std::cout << "\nThank you for using the Game Recommender!" << endl;
    return 0;
}

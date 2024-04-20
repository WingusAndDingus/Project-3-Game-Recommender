#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

// Define the struct to store game information
struct gameInfo {
    string title;
    string platform;
    double rating;
    vector<string> genre;
};

int main() {
    int count = 0;
    // Open the JSON file
    std::ifstream file("game.json");
    if (!file.is_open()) {
        std::cerr << "Error opening file.\n";
        return 1;
    }

    // Parse the JSON data
    json jsonData;
    file >> jsonData;

    // Close the file
    file.close();

    // Create a map to store game reviews
    map<std::string, gameInfo> gameReviews;

    // Extract data from JSON and store it in the map
    for (const auto& entry : jsonData["data"]) {
        gameInfo info;
        info.title = entry["game"];
        info.platform = entry["platform"];
        info.rating = entry["rating"];
        info.genre = entry["genre"].get<vector<string>>();
        gameReviews[info.title] = info;
    }

    // Example: Accessing game review by name
    string gameName = "Wolfenstein: The New Order";
    if (gameReviews.find(gameName) != gameReviews.end()) {
        gameInfo& info = gameReviews[gameName];
        cout << "Title: " << info.title << "\n";
        cout << "Platform: " << info.platform << "\n";
        cout << "Rating: " << info.rating << "\n";
        cout << "Genre(s): ";
        for (const auto& genre : info.genre) {
            cout << genre << ", ";
        }
        cout << "\n";
    }
    else {
        cout << "Game review not found.\n";
    }
    cout << "Now Iterating through all:" << endl;
    for (const auto& game : gameReviews) {
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
    cout << count;
    return 0;
}

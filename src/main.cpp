#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Bridges.h"
#include "DataSource.h"
#include "data_src/Game.h"

using namespace std;
using namespace bridges;

// Define the gameInfo struct
struct gameInfo {
    string title;
    string platformType;
    double rating;
    vector<string> genres;
};

int main(int argc, char **argv) {
    // Create Bridges object
    Bridges bridges(YOUR_ASSSIGNMENT_NUMBER, "YOUR_USER_ID", "YOUR_API_KEY");
    bridges.setTitle("How to access the IGN Game Data");

    // Read the IGN game data
    DataSource ds(&bridges);
    vector<Game> game_list = ds.getGameData();

    // Create a map to store game reviews
    map<string, gameInfo> gameReviews;

    // Iterate through the game list
    for (const auto& game : game_list) {
        // Extract game information
        string title = game.getTitle();
        string platformType = game.getPlatformType();
        double rating = game.getRating();
        vector<string> genres = game.getGameGenre();

        // Create gameInfo struct
        gameInfo info = {title, platformType, rating, genres};

        // Store in the map with the game name as key
        gameReviews[title] = info;
    }

    // Print a single record of the data for illustration
    cout << "Game 0:" << endl;
    cout << "\tTitle: " << gameReviews.begin()->first << endl;
    cout << "\tPlatform Type: " << gameReviews.begin()->second.platformType << endl;
    cout << "\tRating: " << gameReviews.begin()->second.rating << endl;
    cout << "\tGenres: ";
    for (const auto& genre : gameReviews.begin()->second.genres) {
        cout << genre << ", ";
    }
    cout << endl;

    return 0;
}

#include <fstream>
#include <sstream>
#include <map>
#include <fstream>
#include "json.hpp" //only works if you have the libray installed
#include "HashTable.cpp" // contains the HashTable class and GameStruct class

using json = nlohmann::json;
using namespace std;

int main() {
    int count = 0;

    // Open the JSON file
    ifstream file("game.json");
    if (!file.is_open()) {
        cout << "Error opening IGN Reviews JSON file." << endl;
        return 1;
    }

    // Parse the JSON file data
    json jsonData;
    file >> jsonData;

    // Close the file
    file.close();

    //Open csv file
    ifstream file2("dataset.csv");
    if (!file2.is_open()) {
        cout << "Error opening Steam Reviews dataset CSV file." << endl;
        return 1;
    }


    // Create map to store IGN reviews
    map<string, GameStruct> IGNMap;
    // Create a map to store Other Steam Reviews
    map<string, GameStruct> SteamMap;

    // Create hashTable store IGN Reviews
    HashTable IGNHashTable;
    // Create a hashTable to store Other Steam Reviews
    HashTable SteamHashTable;

    // Extract data from IGN JSON and store it in the map & hash table
    for (const auto& entry : jsonData["data"]) {
        GameStruct info;
        info.title = entry["game"];
        info.platform = entry["platform"];
        info.rating = entry["rating"];
        info.genre = entry["genre"].get<vector<string>>();
        info.hasOthers = false; // For now, it doesn't have other reviews
        //Add it to the map
        IGNMap[info.title] = info;
        //Add it to the hash table
        IGNHashTable.insertReview(info.title, info);
    }

    // Extract data from Steam csv file and store it in the map & hash table
    string line, cell;
    getline(file2, line); // Skip the header row
    while (getline(file2, line)) {
        stringstream lineStream(line);
        GameStruct info;
        vector<string> row;
        while (getline(lineStream, cell, ',')) {
            row.push_back(cell);
        }
        if (row.size() >= 4) {
            info.title = row[0];
            info.platform = row[1];
            info.rating = stod(row[2]);
            info.genre.push_back(row[3]); // Assuming only one genre per entry
            info.hasOthers = false;
            SteamMap[info.title] = info;
            SteamHashTable.insertReview(info.title, info);
        }
    }
    file2.close();

    // Example of accessing a specific game review by name
    string gameName = "Wolfenstein: The New Order";
    if (IGNMap.find(gameName) != IGNMap.end()) {
        GameStruct& info = IGNMap[gameName];
        cout << "IGN Review:\n";
        cout << "Title: " << info.title << "\n";
        cout << "Platform: " << info.platform << "\n";
        cout << "Rating: " << info.rating << "\n";
        cout << "Genre(s): ";
        for (const auto& genre : info.genre) {
            cout << genre << ", ";
        }
        cout << "\n";
    } else {
        cout << "Game review not found in IGN data.\n";
    }

    // Testing accessing review in hash table
    IGNHashTable.findReview(gameName);

    // Output how many IGN Reviews were loaded
    cout << "Total IGN Reviews loaded: " << IGNMap.size() << endl;
}
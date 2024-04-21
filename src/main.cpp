#include <iostream>
#include "HashTable.cpp"
#include <map>
#include <fstream>
#include <nlohmann/json.hpp> //only works if you have the libray installed

using json = nlohmann::json;
using namespace std;

int main() {
    int count = 0;

    // Open the JSON file
    std::ifstream file("game.json");
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
        IGNHashTable.insertReview(entry["game"], info);
    }

    // Extract data from Steam csv file and store it in the map & hash table
    for (const auto& entry : idk) {

    }

    // Testing accessing game review by name
    string gameName = "Wolfenstein: The New Order";
    if (IGNMap.find(gameName) != IGNMap.end()) {
        GameStruct& info = IGNMap[gameName];
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
    //Testing accessing review in hash table
    IGNHashTable.findReview(gameName);
    
    //Counting how many IGN Reviews we have in the json file
    cout << "Now Iterating through all:" << endl;
    for (const auto& game : IGNMap) {
        count++;
        /*cout << "Title: " << game.second.title << "\n";
        cout << "Platform: " << game.second.platform << "\n";
        cout << "Rating: " << game.second.rating << "\n";
        cout << "Genre(s): ";
        for (const auto& genre : game.second.genre) {
            cout << genre << ", ";
        }
        cout << endl;
        cout << endl;*/
    }
    cout << count;

    return 0;
}

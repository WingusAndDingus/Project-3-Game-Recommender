#include <string>
#include <vector>

using namespace std;

//struct to store IGN review game information
struct GameStruct {
    string title;
    string platform;
    double rating;
    vector<string> genre;
    bool hasSteam;
};

//struct to store Steam User Reviews info
struct ReviewStruct {
    string reviewText;
    string gameTitle;
    int recommends; // -1 = not recommended, 1 = recommended
    int UserID; //Randomly generated ID to act as key in map/hashtable
};

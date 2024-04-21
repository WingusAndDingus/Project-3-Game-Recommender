#include <string>
#include <vector>

using namespace std;

//struct to store IGN review game information
struct GameStruct {
    string title;
    string platform;
    double rating;
    vector<string> genre;
    bool hasOthers;
};

//struct to store Steam User Reviews info
struct OtherStruct {
    string reviewText;
    int numUsersHelpful;
    int gameID;

};

struct SmallerOtherStruct {
    string gamerID;
    string gameTitle;
    string behavior;
    double hoursPlayed;
};
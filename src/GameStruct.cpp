#include <string>
#include <vector>

using namespace std;

struct GameStruct {
    string title;
    string platform;       // Comes from JSON
    double ignRating;      // Rating out of 10, from JSON
    vector<string> genres; // Comes from JSON
    vector<string> reviews; // User reviews texts from TSV
    vector<double> userRatings; // Ratings from users (1.0 or -1.0) from TSV
    bool hasOthers;        // Additional flag from TSV
};

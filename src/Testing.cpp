#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

struct Review {
    string user;
    int rating;
    string comment;
};

int main() {
    ifstream file("dataset.csv");

    if (!file.is_open()) {
        cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    vector<Review> reviews;

    string line;
    while (std::getline(file, line)) {
        istringstream ss(line);
        string token;

        Review review;

        // Assuming the format is user, rating, comment
        getline(ss, review.user, ',');
        getline(ss, token, ',');
        review.rating = stoi(token);
        getline(ss, review.comment, ',');

        reviews.push_back(review);
    }

    file.close();

    // Process and manipulate data
    for (const auto& review : reviews) {
        std::cout << "User: " << review.user << ", Rating: " << review.rating << ", Comment: " << review.comment << std::endl;
    }

    // Further processing or output

    return 0;
}

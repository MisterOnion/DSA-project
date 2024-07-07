#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <cmath> // Include this for sqrt and pow functions

using namespace std;

struct Star {
    string name;
    int x, y, z;
    int weight, profit;
};

// Function to generate a random number using only the digits in the given vector
int getRandomNumber(const vector<int>& digits) {
    int num = 0;
    int numDigits = rand() % 2 + 1; // Random number of digits between 1 and 3
    for (int i = 0; i < numDigits; ++i) {
        num = num * 10 + digits[rand() % digits.size()];
    }
    return num;
}

// Function to calculate the distance between two stars
int calculateDistance(const Star& star1, const Star& star2) {
    return static_cast<int>(sqrt(pow(star1.x - star2.x, 2) + pow(star1.y - star2.y, 2) + pow(star1.z - star2.z, 2)));
}

int main() {
    // Seed for random number generation
    srand(time(0)); // Use current time as seed
    
    // Create stars
    vector<Star> stars(20);
    char starName = 'A';
    vector<int> digits = {3, 6, 1, 3, 6, 1, 4, 3, 9, 0}; // Digits to be used in the random numbers
    for (int i = 0; i < 20; ++i) {
        stars[i].name = "Star ";
        stars[i].name += starName + i;
        stars[i].x = getRandomNumber(digits); // Random x-coordinate
        stars[i].y = getRandomNumber(digits); // Random y-coordinate
        stars[i].z = getRandomNumber(digits); // Random z-coordinate
        stars[i].weight = getRandomNumber(digits); 
        stars[i].profit = getRandomNumber(digits); 
    }

    // Define the routes (edges) ensuring each star is connected to at least 3 others
    vector<pair<int, int>> routes;
    for (int i = 0; i < 20; ++i) {
        for (int j = i + 1; j < 20; ++j) {
            routes.push_back({i, j});
        }
    }

    // Shuffle the routes to make them random
    random_shuffle(routes.begin(), routes.end());

    // Ensure each star is connected to at least 3 others
    vector<int> connections(20, 0);
    vector<pair<int, int>> finalRoutes;
    for (const auto& route : routes) {
        if (connections[route.first] < 3 && connections[route.second] < 3) {
            finalRoutes.push_back(route);
            connections[route.first]++;
            connections[route.second]++;
        }
    }

    // Add remaining edges randomly until we reach 54 edges
    for (const auto& route : routes) {
        if (finalRoutes.size() >= 54) {
            break;
        }
        if (find(finalRoutes.begin(), finalRoutes.end(), route) == finalRoutes.end()) {
            finalRoutes.push_back(route);
        }
    }

    // Output the dataset
    ofstream outFile("Q1_dataset_2.txt");
    outFile << "Name\tx\ty\tz\tweight\tprofit\n";
    for (const auto& star : stars) {
        outFile << star.name << "\t" << star.x << "\t" << star.y << "\t" 
                << star.z << "\t" << star.weight << "\t" << star.profit << "\n";
    }

    outFile << "\nRoutes (edges):\n";
    for (const auto& route : finalRoutes) {
        outFile << stars[route.first].name << " <--> " << stars[route.second].name 
                << " Distance: " << calculateDistance(stars[route.first], stars[route.second]) << "\n";
    }

    outFile.close();
    cout << "Dataset and routes have been generated and saved to 'Q1_dataset_2.txt'.\n";

    return 0;
}


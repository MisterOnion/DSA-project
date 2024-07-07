#include <iostream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <utility>
#include <queue>
// file reading and writing
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace chrono;

struct Star
{
    string name;
    int x, y, z, weight, profit;
};

struct Edge
{
    string star1;
    string star2;
    int distance;
};

void fileReader(const string &dataSet2, vector<Star> &stars, vector<Edge> &edges)
{
    // error handling
    ifstream file(dataSet2);
    if (!file.is_open())
    {
        cerr << "Error opening txt file" << endl;
        exit(1);
    }
    string line;
    bool readingEdges = false;
    // skip header out of while loop to prevent parsing error
    getline(file, line);

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        // if true, read route data (edges)
        if (line == "Routes (edges):")
        {
            readingEdges = true;
            continue;
        }
        // if false, read star data (vertices)
        if (!readingEdges)
        {
            Star star;
            istringstream iss(line);
            string temp;

            // Parse star data
            getline(iss, star.name, '\t');
            iss >> star.x >> star.y >> star.z >> star.weight >> star.profit;

            stars.push_back(star);
        }
        else
        {
            Edge edge;
            istringstream iss(line);
            string temp;

            // Parse edge data
            // temp to replace symbols
            getline(iss, edge.star1, ' ');
            iss >> edge.star1 >> temp >> temp >> edge.star2 >> temp >> edge.distance;

            edges.push_back(edge);
        }
    }
    file.close();
}

// 0/1 Knapsack
vector<vector<int>> dp(vector<int> &profit, vector<int> &weight, int capacity)
{
    int N = profit.size(), M = capacity;
    // matrix, add 1 more column for weight
    vector<vector<int>> dp(N, vector<int>(M + 1, 0));
    // runtime store vector
    vector<double> runtimes(N, 0.0);

    // fill the first column and row to 0 (reduce computation time)
    for (int i = 0; i < N; i++)
    {
        dp[i][0] = 0;
    }
    for (int c = 0; c <= M; c++)
    {
        if (weight[0] <= c)
        {
            dp[0][c] = profit[0];
        }
    }

    double totalProcessingTimeMs = 0.0;
    for (int i = 1; i < N; i++)
    {
        for (int c = 1; c <= M; c++)
        {
            // store maximum value in cache to eliminate computation
            auto start_knap = chrono::high_resolution_clock::now();
            // cache
            int skip = dp[i - 1][c];
            int include = 0;
            if (c - weight[i] >= 0)
            {
                // access to the previous row
                include = profit[i] + dp[i - 1][c - weight[i]];
            }
            // compare the maximum from cache, then pick the maximum
            dp[i][c] = max(include, skip);

            auto end_knap = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> knap_duration = end_knap - start_knap;
            runtimes[i] = knap_duration.count();
            totalProcessingTimeMs += knap_duration.count();  

        }
    }
    ofstream outputFile("Q4_knap_runtime_record.txt");
    outputFile << "Note: all index 0 stars and weight are coded to 0 profit \n";
    outputFile << endl;
    outputFile <<"Total processing time: " << totalProcessingTimeMs << endl;
    outputFile << "Processing time for each star increment: \n";
    
    for (int i = 1; i < N; i++)
    {
        outputFile << i << " stars: " << runtimes[i] << " ms\n";
    }
    outputFile.close();

    

    // returns the last maximum profit from matrix
    // return dp[N-1][M];
    return dp;
}

// Display the matrix result
vector<int> matrixGenerator(vector<vector<int>> &dp, vector<int> &weight, vector<int> &profit, int capacity)
{
    vector<int> items;
    int N = dp.size();
    int c = capacity;

    for (int i = N - 1; i > 0; i--)
    {
        if (dp[i][c] != dp[i - 1][c])
        {
            items.push_back(i);
            c -= weight[i];
        }
    }

    if (c >= weight[0])
    {
        items.push_back(0);
    }
    return items;
}

int main()
{
    vector<Star> stars;
    vector<Edge> edges;
    fileReader("Q1_dataset_2.txt", stars, edges);

    // we only need weight and profit data for stars
    /*
    cout << "List of Stars: " << endl;
    for (const auto &star : stars)
    {
        cout << star.name << " " << star.x << " " << star.y << " " << star.z << " " << star.weight << " " << star.profit << endl;
    }
    cout << endl;
    */

    int capacity = 800; // 800 kg of stars
    vector<int> profit;
    vector<int> weight;

    for (const auto &star : stars)
    {
        profit.push_back(star.profit);
        weight.push_back(star.weight);
    }

    cout << "Profit: " << endl;
    for (int i = 0; i < profit.size(); i++)
    {
        cout << profit[i] << " ";
    }
    cout << endl;
    cout << "Weight: " << endl;
    for (int i = 0; i < weight.size(); i++)
    {
        cout << weight[i] << " ";
    }
    cout << endl;
    cout << endl;
    

    auto start = chrono::high_resolution_clock::now();
    // call Knapsack function
    vector<vector<int>> knapsack = dp(profit, weight, capacity);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> knap_program_duration = end - start;

    ofstream outputFile("Q4_knap_results.txt");

    // Display table matrix
    cout << "Knapsack DP Table: " << endl;
    cout << "   ";
    outputFile << "Knapsack DP Table: " << endl;
    outputFile << "   ";
    // row 
    for (int c = 0; c <= capacity; c++)
    {
        cout << c << " ";
        outputFile << c << " ";
    }
    cout << endl;
    outputFile << endl;

    // column
    for (int i = 0; i < knapsack.size(); i++)
    {
        cout << i << ": ";
        outputFile << i << ": ";
        for (int c = 0; c <= capacity; c++)
        {
            // values from matrix
            cout << knapsack[i][c] << " ";
            outputFile << knapsack[i][c] << " ";
        }
        cout << endl;
        outputFile << endl;
    }


    cout << "Knapsack Matrix: " << endl;
    // print out knapsack
    /*
    for (const auto& row : knapsack)
    {
        // call helper function for accumulated vertice display
        cout << vertices << endl;

        for (int val : row)
        {
            cout << val << " ";
        }
        cout << endl;

    }
    */

    string vertices = "A"; // Initialize vertices string with 'A'
    for (const auto &row : knapsack)
    {
        cout << "Stars involved: " << endl;
        cout << endl;

        outputFile << "Stars involved: " << endl;
        outputFile << endl;

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            cout << vertices[i] << " (Weight: " << weight[vertices[i] - 'A'] << ", Profit: " << profit[vertices[i] - 'A'] << ") | \n";
            cout << endl;

            outputFile << vertices[i] << " (Weight: " << weight[vertices[i] - 'A'] << ", Profit: " << profit[vertices[i] - 'A'] << ") | \n";
            outputFile<< endl;
        }

        for (int val : row)
        {
            cout << val << " ";
            outputFile << val << " ";
        }
        cout << endl;
        cout << endl;

        outputFile << endl;
        outputFile << endl;

        if (vertices.size() < knapsack.size())
        {
            vertices += char('A' + vertices.size()); // Add next vertex to the string
        }
    }

    vector<int> items = matrixGenerator(knapsack, weight, profit, capacity);
    cout << endl;
    cout << "Stars included: " << endl;
    outputFile << "Stars included: " << endl;
    for (int item : items)
    {
        cout << stars[item].name << " (Weight: " << stars[item].weight << ", Profit: " << stars[item].profit << ")" << endl;
        outputFile << stars[item].name << " (Weight: " << stars[item].weight << ", Profit: " << stars[item].profit << ")" << endl;
    }

    cout << " Maximum benefit: " << knapsack[profit.size() - 1][capacity] << endl;
    cout << "0/1 Knapsack Program Runtime: " << knap_program_duration.count();

    outputFile << " Maximum benefit: " << knapsack[profit.size() - 1][capacity] << endl;
    outputFile << "0/1 Knapsack Program Runtime: " << knap_program_duration.count() << " ms";

    outputFile.close();
    return 0;
}
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

// Union-Find (Disjoint Set)
class UnionFind
{
public:
    unordered_map<string, string> parent;
    unordered_map<string, int> rank;

    UnionFind(const vector<string> &vertices)
    {
        // set vertex to 1, and rank to 0
        for (const auto &vertex : vertices)
        {
            parent[vertex] = vertex;
            rank[vertex] = 0;
        }
    }

    string find(const string &n)
    {
        // Path Compression, connect only to root node for efficient finding
        if (parent[n] != n)
        {
            parent[n] = find(parent[n]);
        }
        return parent[n];
    }

    bool uni0n(const string &n1, const string &n2)
    {
        string p1 = find(n1), p2 = find(n2);
        if (p1 == p2)
        {
            return false;
        }
        // if less than largest tree, connect to it
        if (rank[p1] > rank[p2])
        {
            parent[p2] = p1;
        }
        else if (rank[p1] < rank[p2])
        {
            parent[p1] = p2;
        }
        else
        {
            // if both trees are even, both increase by 1 height
            parent[p1] = p2;
            rank[p2] += 1;
        }
        return true;
    }
};

// Kruskal's algo
vector<Edge> mst(const vector<Edge> &edges, const vector<string> &vertices)
{
    // lamda function to compare 2 edges
    auto comp = [](const Edge &e1, const Edge &e2)
    {
        return e1.distance > e2.distance;
    };

    priority_queue<Edge, vector<Edge>, decltype(comp)> minHeap(comp);
    for (const auto &edge : edges)
    {
        minHeap.push(edge);
    }

    UnionFind unionFind(vertices);
    vector<Edge> mst;
    // record time for processing kruskal
    vector<double> processEdgeTime;
    double totalProcessingTimeMs = 0.0;
    while (mst.size() < vertices.size() - 1 && !minHeap.empty())
    {
        auto start_time = high_resolution_clock::now();

        Edge cur = minHeap.top();
        minHeap.pop();
        if (unionFind.uni0n(cur.star1, cur.star2))
        {
            mst.push_back(cur);
        }

        auto end_time = high_resolution_clock::now();
        duration<double, milli> duration = end_time - start_time;
        processEdgeTime.push_back(duration.count());
        totalProcessingTimeMs += duration.count();
    }

    ofstream outputFile("Q3_krus_runtime_record.txt");
    for (size_t i = 0; i < mst.size(); i++)
    {
        outputFile << "Edge " << mst[i].star1 << " - " << mst[i].star2 << " took: " << processEdgeTime[i] << " ms\n";
    }
    outputFile << "Total processing time: " << totalProcessingTimeMs << endl;

    for (size_t i = 0; i < mst.size(); i++)
    {
        cout << "Edge " << mst[i].star1 << " - " << mst[i].star2 << " took: " << processEdgeTime[i] << " ms\n";
    }

    cout << endl;
    cout << "Total processing time: " << totalProcessingTimeMs << endl;
    cout << endl;
    return mst;
}

// Dijkstra shortest path
unordered_map<string, int> shortestPath(const vector<Edge> &edges, const vector<string> &vertices, const string &src, unordered_map<string, string> &predecessors, unordered_map<string, int>& weights)
{
    // initialize adjacency list
    unordered_map<string, vector<pair<string, int>>> adj;

    // populate the adjacency list with edges object
    for (const auto &edge : edges)
    {
        adj[edge.star1].push_back(make_pair(edge.star2, edge.distance));
    }

    // hashmap to store shortest distance
    unordered_map<string, int> shortest;

    // minheap priority queue to process node's distance in ascending order
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> minHeap;

    // begin with source code
    minHeap.push(make_pair(0, src));

    // record time for processing
    unordered_map<string, double> processingTime;
    double totalProcessingTimeMs = 0.0;

    // basecase for minheap
    while (!minHeap.empty())
    {
        auto start_time = high_resolution_clock::now();
        // distance, star
        auto [dis_1, vertice_1] = minHeap.top();
        // pop because we know we get the previous node
        minHeap.pop(); // 

        // skip already processed node
        if (shortest.count(vertice_1) > 0)
        {
            continue;
        }
        // so do no overwrite the code below with a larger edge value

        // save the shortest distance to the node
        shortest[vertice_1] = dis_1;

        // iterate over the adjacent nodes, neighboring stars
        for (const auto &[vertice_2, dis_2] : adj[vertice_1])
        {
            // if adjacent node is not processed, add it to the minheap (prevent infinite loop between nodes with least edge value)
            if (shortest.count(vertice_2) == 0)
            {
                minHeap.push(make_pair(dis_1 + dis_2, vertice_2));
                // store star into a vector
                predecessors[vertice_2] = vertice_1;
                // store corresponding edge weight into a vector
                weights[vertice_2] = dis_2;
            }
        }
        auto end_time = high_resolution_clock::now();
        duration<double, milli> duration = end_time - start_time;
        double durationMs = duration.count();
        processingTime[vertice_1] = durationMs;
        totalProcessingTimeMs += durationMs;
    }
    // make another record time file for ploting
    ofstream outputFile("Q3_dijk_runtime_record.txt");
    outputFile << "Processing time for each node:\n";
    for (const auto &[node, time] : processingTime)
    {
        outputFile << "Node " << node << " took: " << time << " ms\n";
    }
    outputFile << "Total processing time: " << totalProcessingTimeMs << " ms\n";

    cout << "Processing time for each node:\n";
    for (const auto &[node, time] : processingTime)
    {
        cout << "Node " << node << " took: " << time << " ms\n";
    }
    cout << "Total processing time: " << totalProcessingTimeMs << " ms\n";
    cout << endl;

    return shortest;
}

vector<pair<string, int>> reconstructPath(const unordered_map<string, string> &predecessors, const unordered_map<string, int>& weights, const string &target)
{
    // track stars visited
    vector<pair<string, int>> path;
    // target == starting source star
    // backtracking technique, update the current star and compare again as long vector is not empty
    for (string at = target; !at.empty(); at = predecessors.at(at))
    {
       // path.push_back(at);
        if (predecessors.at(at).empty())
        {
            path.push_back(make_pair(at,0));
        }
        else
        {
            path.push_back(make_pair(at, weights.at(at)));
        }
    }
    // since backtrack, reverse each element to ascending order
    reverse(path.begin(), path.end());
    return path;
}

int main()
{
    int sortingChoice = 0;

    vector<Star> stars;
    vector<Edge> edges;
    fileReader("Q1_dataset_2.txt", stars, edges);

    cout << "1. Dijkstra's Algorithm (Shortest Path)" << endl;
    cout << "2. Kruskals's Algorithm (Minimum Spanning Tree)" << endl;
    cout << "3. Exit" << endl;
    cout << "Enter Option: ";
    cin >> sortingChoice;

    if (sortingChoice != 1 && sortingChoice != 2)
    {
        cout << "Invalid sorting algorithm, please try again." << endl;
        return 1;
    }

    /*Code for verifying if vector is read*/
    /*
    cout << "List of Stars: " << endl;
    for (const auto &star : stars)
    {
        cout << star.name << " " << star.x << " " << star.y << " " << star.z << " " << star.weight << " " << star.profit << endl;
    }
    cout << endl;
    cout << "List of Edges: " << endl;
    for (const auto &edge : edges)
    {
        cout << edge.star1 << " <-> " << edge.star2 << " Distance: " << edge.distance << endl;
    }
    cout << endl;
    */

    // to prevent parsing error, define code here after computation is done
    // define nodes again for Union-Find, ensure size is only 20 nodes/vertices
    if (sortingChoice == 2)
    {
        auto start_krus = chrono::high_resolution_clock::now();
        vector<string> vertices = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S","T"};

        vector<Edge> mstEdges = mst(edges, vertices);
        auto end_krus = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> krus_duration = end_krus - start_krus;

        cout << "Result for Minimum Spanning Tree: " << endl;
        ofstream outputFile("Q3_krus_results.txt");
        for (const auto &edge : mstEdges)
        {
            cout << "[" << edge.star1 << " - " << edge.star2 << "]  Distance: " << edge.distance << endl;
            // Write result to txt file
            outputFile << "[" << edge.star1 << " - " << edge.star2 << "]  Distance: " << edge.distance << endl;
        }
        cout << endl;
        cout << "Kruskal's Algorithm Program Runtime: " << krus_duration.count() << " ms" << endl;
        outputFile << endl;
        outputFile << "Kruskal's Algorithm Program Runtime: " << krus_duration.count() << " ms" << endl;
    }
    else if (sortingChoice == 1)
    {
        auto start_dij = chrono::high_resolution_clock::now();
        vector<string> vertices = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P","Q", "R", "S","T"};
        string src = "A";
        // unordered_map<string, int> result = shortestPath(edges, src);

        // To store predecessors of each vertex
        unordered_map<string, string> predecessors;
        // to store edge weights to make pairing later
        unordered_map<string, int> weights;
        for (const auto &vertex : vertices)
        {
            predecessors[vertex] = "";
        }

        unordered_map<string, int> result = shortestPath(edges, vertices, src, predecessors, weights);
        auto end_dij = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> dij_duration = end_dij - start_dij;

        ofstream outputFile("Q3_dijk_results.txt");
        cout << "Shortest paths from node " << src << ":\n";
        outputFile << "Shortest paths from node " << src << endl;

        for (const auto &[node, distance] : result)
        {
            cout << "To node " << node << " is at distance " << distance << "\n";
            outputFile << "To node " << node << " is at distance " << distance << "\n";

            // call path contructor
            vector<pair<string, int>> path = reconstructPath(predecessors, weights , node);
            cout << "Path: ";
            outputFile << "Path: ";
            for (const auto& [p, w] : path)
            {
                cout << p << "(" << w << ") ";
                outputFile << p << "(" << w << ") ";
            }
            cout << "\n";
            cout << endl;

            outputFile << endl;
            outputFile << endl;
        }

        cout << endl;
        cout << "Dijkstra Algorithm Program Runtime: " << dij_duration.count() << " ms" << endl;
        outputFile << endl;
        outputFile << "Dijkstra Algorithm Program Runtime: " << dij_duration.count() << " ms" << endl;
    }
    else if (sortingChoice == 3)
    {
        return 0;
    }

    return 0;
}

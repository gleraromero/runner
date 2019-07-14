#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include "json.hpp"

using namespace std;
using namespace nlohmann;

int n; // Number of items.
int W; // Knapsack capacity.
vector<int> w; // w[i] = weight of the ith item.
vector<int> v; // v[i] = value of the ith item.

// Solve knapsack with items [0..i) and capacity K.
// Returns: maximum benefit.
int backtracking(int i, int K)
{
	if (K < 0) return -10e8;
	if (i == 0) return 0;
	return max(backtracking(i-1, K), backtracking(i-1, K-w[i-1])+v[i-1]);
}

int main()
{
	// Read input.
	clog << "Reading input." << endl;
	json experiment, instance, solutions;
	cin >> experiment >> instance >> solutions;

	// Parse instance.
	clog << "Parsing instance." << endl;
	n = instance["item_count"];
	W = instance["capacity"];
	for (int i = 0; i < n; ++i)
	{
		w.push_back(instance["item_weights"][i]);
		v.push_back(instance["item_values"][i]);
	}

	// Solve knapsack.
	clog << "Solving knapsack." << endl;
	auto start_time = chrono::steady_clock::now();
	int solution = backtracking(n, W);
	auto end_time = chrono::steady_clock::now();
	double time_ms = chrono::duration<double, milli>(end_time - start_time).count();
	clog << "Solution: " << solution << endl;
	clog << "Time: " << time_ms << "ms." << endl;
	
	// Print output.
	json output = {{"solution", solution}, {"time", time_ms}};
	cout << output << endl;
}
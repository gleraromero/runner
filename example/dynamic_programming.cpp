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
vector<vector<int>> M; // DP memory of size (n+1)x(W+1).

const int UNDEFINED = -1; // special value to denote that M[i][j] is not defined.

// Solve knapsack with items [0..i) and capacity K.
// Returns: maximum benefit.
int top_down(int i, int K)
{
	if (K < 0) return -10e8;
	if (i == 0) return 0;
	if (M[i][K] == UNDEFINED) M[i][K] = max(top_down(i-1, K), top_down(i-1, K-w[i-1])+v[i-1]);
	return M[i][K];
}

// Solve knapsack with a bottom up approach.
// Returns: maximum benefit.
int bottom_up()
{
	for (int K = 0; K <= W; ++K) M[0][K] = 0;
	for (int i = 0; i <= n; ++i) M[i][0] = 0;
	for (int i = 1; i <= n; ++i)
	{
		for (int K = 1; K <= W; ++K)
		{
			M[i][K] = M[i-1][K];
			if (w[i-1] <= K) M[i][K] = max(M[i][K], M[i-1][K-w[i-1]]+v[i-1]);
		}
	}
	return M[n][W];
}

int main()
{
	// Read input.
	clog << "Reading input." << endl;
	json experiment, instance, solutions;
	cin >> experiment >> instance >> solutions;

	// Parse experiment parameters.
	clog << "Reading Experiment." << endl;
	string version = experiment["version"];
	clog << "\tVersion: " << version << endl;

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
	M = vector<vector<int>>(n+1, vector<int>(W+1, UNDEFINED));
	auto start_time = chrono::steady_clock::now();
	int solution = 0;
	if (version == "top_down") solution = top_down(n, W);
	else solution = bottom_up();
	auto end_time = chrono::steady_clock::now();
	double time_ms = chrono::duration<double, milli>(end_time - start_time).count();
	clog << "Solution: " << solution << endl;
	clog << "Time: " << time_ms << "ms." << endl;
	
	// Print output.
	json output = {{"solution", solution}, {"time", time_ms}};
	cout << output << endl;
}
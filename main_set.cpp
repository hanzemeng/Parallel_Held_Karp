#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <set>
#include <map>
#include <algorithm> 
#include <chrono>
using namespace std;

struct Point
{
    double x;
    double y;
};

void setup(string path);
double dist(const Point& a, const Point& b);

vector<int> subset_index;
vector<Point> points;
map<pair<set<int>, int>, double> dp;
int n;
double res;

double solve(set<int> path, int end)
{
    if(dp.find(pair{path, end}) != dp.end())
    {
        return dp[pair{path, end}];
    }

    double current_min = 666666;
    for(int i : path)
    {
        set<int> new_path = path;
        new_path.erase(i);
        double temp = solve(new_path, i) + dist(points[i], points[end]);
        current_min = min(current_min, temp);
    }
    return dp[pair{path, end}] = current_min;
}

int main()
{
    setup("points.txt");
    auto start = std::chrono::high_resolution_clock::now();
    set<int> all_node;
    for(int i=1; i<n; i++)
    {
        dp[pair{set<int>(), i}] = dist(points[0], points[i]);
        all_node.insert(i);
    }

    // Below is the for loop that I want to parallelize
    for(int i : all_node)
    {   
        set<int> new_path = all_node;
        new_path.erase(i);
        double temp = solve(new_path, i) + dist(points[i], points[0]);
        res = min(res, temp);
    } 
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    long long time = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    cout << "Res: " << res << endl;
    cout << "Time(ms): " << time << endl;
    return 0;
}

void setup(string path)
{
    ifstream input;
    input.open(path);
    double x, y;
    while(input >> x >> y)
    {
        points.push_back(Point{x, y});
    }
    input.close();
    n = points.size();
    for(int i=1; i<n-1; i++)
    {
        subset_index.push_back(0);
    }
    subset_index.push_back(1);
    res = 6666666;
}
double dist(const Point& a, const Point& b)
{
    double dif_x = a.x-b.x;
    double dif_y = a.y-b.y;
    return sqrt(dif_x*dif_x+dif_y*dif_y);
}

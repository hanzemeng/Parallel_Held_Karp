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

vector<Point> points;
map<pair<unsigned int, int>, double> dp;
int n;
double res;

double solve(unsigned int path, int end)
{
    if(dp.find(pair{path, end}) != dp.end())
    {
        return dp[pair{path, end}];
    }

    double current_min = 666666;
    unsigned int mask = 0x7fffffff;
    for(int i=1; i<n; i++)
    {
        if((path & (~mask)) != 0)
        {
            unsigned int new_path = path & mask;
            double temp = solve(new_path, i) + dist(points[i], points[end]);
            current_min = min(current_min, temp);
        }
        mask >>= 1;
        mask |= 0x80000000;
    }
    return dp[pair{path, end}] = current_min;
}

int main()
{
    setup("points.txt");
    auto start = std::chrono::high_resolution_clock::now();
    unsigned int all_path = 0;
    for(int i=1; i<n; i++)
    {
        dp[pair{0, i}] = dist(points[0], points[i]);
        all_path |= 0x80000000;
        all_path >>= 1;
    }
    all_path <<= 1;
    unsigned int mask = 0x7fffffff;
    
    for(int i=1; i<n; i++)
    {   
        unsigned int new_path = all_path & mask;
        mask >>= 1;
        mask |= 0x80000000;
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
    res = 6666666;
}
double dist(const Point& a, const Point& b)
{
    double dif_x = a.x-b.x;
    double dif_y = a.y-b.y;
    return sqrt(dif_x*dif_x+dif_y*dif_y);
}
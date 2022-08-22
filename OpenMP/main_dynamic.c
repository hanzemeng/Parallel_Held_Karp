#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

typedef struct
{
    float x;
    float y;
} Point;
typedef struct
{
    int start;
    float* return_cost;
    int* return_prev;
} solve_wrapper_input;
typedef struct
{
    float cost;
    int prev_end;
} dp_node;

float dist(const Point* a, const Point* b);
double elapsed_seconds();

void setup(const char** path); // read in input
void calculate_base_case(); // setup dp and all_path

#define INT_DIGIT 24 // max number of vertices - 1
#define MAX_INT 16777216 // same as 2^INT_DIGIT
#define LARGE_VALUE 666666;

unsigned int all_path = 0;
int n, thread_count;
Point* points;
dp_node dp[MAX_INT][INT_DIGIT];

float solve(unsigned int path, int end)
{
    if(dp[path][end].cost != -1) // if the sub problem is already solved
    {
        return dp[path][end].cost;
    }

    float current_min = LARGE_VALUE;
    int current_prev;
    unsigned int mask = 0x00000001;
    for(int i=1; i<n; i++) // for every bit position
    {
        if((path & mask) != 0) // if the bit position is 1, solve the sub problem
        {
            unsigned int new_path = path & (~mask);
            float temp = solve(new_path, i) + dist(&points[i], &points[end]);
            if(current_min>temp) // for the shortest path
            {
                current_min = temp; // record the smallest cost
                current_prev = i; // record the last visited vertex in the shortest path
            }
        }
        mask <<= 1;
    }
    dp[path][end].prev_end = current_prev; // no protection because
    return dp[path][end].cost = current_min; // threads would write the same value
}

void Usage(const char** prog_name /* in */) {
   fprintf(stderr, "usage: %s ", prog_name[1]); 
   fprintf(stderr, "<thread_num>\n");
   exit(0);
}  /* Usage */

int main(const int agrc, const char** argv)
{
    double ftime, start, end;
    if(agrc != 3) Usage(argv);
    setup(argv);
    calculate_base_case();

    float res_cost = LARGE_VALUE;
    int res_prev;
    int res_path[n];
    int index = n-1;

    start = elapsed_seconds();
    //Dispatch the work to each thread by OpenMP
#   pragma omp parallel num_threads(thread_count)
    {
        // int my_rank = omp_get_thread_num();
#       pragma omp for schedule(dynamic)
        for(int i=1;i<n;i++)
        {
            unsigned int mask = 0x00000001;
            mask <<= (i-1);
            unsigned int new_path = all_path & (~mask);
            float temp = solve(new_path, i) + dist(&points[i], &points[0]);
#           pragma omp critical
            if(res_cost>temp)
            {
                res_cost = temp;
                res_prev = i;
            }   
        }
    }
    while(res_prev != 0)
    {
        res_path[index--] = res_prev;
        unsigned int mask = 0x00000001;
        mask <<= (res_prev-1);
        all_path &= ~mask;
        res_prev = dp[all_path][res_prev].prev_end;
    }
    res_path[index--] = 0;


    end = elapsed_seconds();
    float res_cost_actual = dist(&points[res_path[n-1]], &points[res_path[0]]);
    printf("Path: 0, ");
    for(int i=1; i<n; i++)
    {
        printf("%d, ", res_path[i]);
        res_cost_actual += dist(&points[res_path[i-1]], &points[res_path[i]]);
    }

    ftime = end - start;

    printf("\n");
    printf("Computed Cost: %f\n", res_cost);
    printf("Actual   Cost: %f\n", res_cost_actual);
    printf("Time Taken(s): %f\n", ftime);

    return 0;
}

void setup(const char** input_parameter)
{
    thread_count = strtol(input_parameter[2], NULL, 10);
    FILE *filePointer;
    char input_line[50];
    filePointer = fopen(input_parameter[1], "r");

    fgets(input_line, 50, filePointer);
    n = strtol(input_line, NULL, 10);
    // printf("n is:%s\n",input_parameter[2]);
    if(INT_DIGIT+1<n)
    {
        printf("Too many points.\n");
        exit(1);
    }
    points = malloc(n*sizeof(Point));

    for(int i=0; i<n; i++)
    {
        fgets(input_line, 50, filePointer);
        points[i].x = strtof(strtok(input_line, " "), NULL);
        points[i].y = strtof(strtok(NULL, " "), NULL);
    }
    fclose(filePointer);
}
float dist(const Point* a, const Point* b)
{
    float dif_x = (*a).x-(*b).x;
    float dif_y = (*a).y-(*b).y;
    return sqrt(dif_x*dif_x+dif_y*dif_y);
}
void calculate_base_case()
{
    for(unsigned long long i=0; i<MAX_INT; i++)
    {
        for(unsigned long long j=0; j<INT_DIGIT; j++)
        {
            dp[i][j].cost = -1;
        }
    }

    unsigned int mask = 0x00000001;
    for(int i=1; i<n; i++)
    {
        dp[0][i].cost = dist(&points[0], &points[i]);
        dp[0][i].prev_end = 0;
        all_path |= mask;
        mask <<= 1;
    }
}
double elapsed_seconds()
{
   struct timeval tv;
   struct timezone tz;
   gettimeofday(&tv, &tz);
   return (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;
}

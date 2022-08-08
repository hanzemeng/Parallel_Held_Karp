#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct
{
    float x;
    float y;
} Point;
typedef struct
{
    int start;
    int end;
    float* return_cost;
    int* return_prev;
} solve_input;
typedef struct
{
    float cost;
    int prev_end;
} dp_node;

float dist(const Point* a, const Point* b);
double elapsed_seconds();

void setup(const char** path);
void base_case_calculation();

#define MAX_INT 16777216
#define INT_DIGIT 24
#define LARGE_VALUE 666666;

unsigned int all_path = 0;
int n, thread_count;
Point* points;
dp_node dp[MAX_INT][INT_DIGIT];

float solve(unsigned int path, int end)
{
    if(dp[path][end].cost != -1)
    {
        return dp[path][end].cost;
    }

    float current_min = LARGE_VALUE;
    float current_prev;
    unsigned int mask = 0x00000001;
    for(int i=1; i<n; i++)
    {
        if((path & mask) != 0)
        {
            unsigned int new_path = path & (~mask);
            float temp = solve(new_path, i) + dist(&points[i], &points[end]);
            if(current_min>temp)
            {
                current_min = temp;
                current_prev = i;
            }
        }
        mask <<= 1;
    }
    dp[path][end].prev_end = current_prev;
    return dp[path][end].cost = current_min;
}
void* solve_wrapper(void* parameter)
{
    solve_input* input = (solve_input*) parameter;
    unsigned int mask = 0x00000001;
    mask <<= (input->start-1);

    float thread_min = LARGE_VALUE;
    int thread_prev;
    for(int i=input->start; i<input->end; i++)
    {
        unsigned int new_path = all_path & (~mask);
        mask <<= 1;
        float temp = solve(new_path, i) + dist(&points[i], &points[0]);
        if(thread_min>temp)
        {
            thread_min = temp;
            thread_prev = i;
        }
    }
    *(input->return_cost) = thread_min;
    *(input->return_prev) = thread_prev;
    return NULL;
}

int main(const int agrc, const char** argv)
{
    if(3 != agrc)
    {
        printf("Wrong number of arguments.\n");
        exit(1);
    }
    setup(argv);
    base_case_calculation();

    double start = elapsed_seconds();
    pthread_t thread_handles[thread_count];
    solve_input thread_inputs[thread_count];
    float thread_cost[thread_count];
    int thread_prev[thread_count];

    int k = (n-1)%thread_count;
    int s = (n-1)/thread_count;
    for(int i=0; i<thread_count; i++)
    {   
        thread_inputs[i].return_cost = &thread_cost[i];
        thread_inputs[i].return_prev = &thread_prev[i];
        if(i<k)
        {
            thread_inputs[i].start = i*s+i+1;
            thread_inputs[i].end = thread_inputs[i].start+s+1;
        }
        else
        {
            thread_inputs[i].start = i*s+k+1;
            thread_inputs[i].end = thread_inputs[i].start+s;
        }
        //printf("%d, %d\n", thread_inputs[i].start,thread_inputs[i].end);
        pthread_create(&thread_handles[i], NULL, solve_wrapper, (void*) &thread_inputs[i]);
    }

    for(int i=0; i<thread_count; i++)
    {
        pthread_join(thread_handles[i], NULL);
    }
    float res_cost = LARGE_VALUE;
    int res_prev;
    for(int i=0; i<thread_count; i++)
    {
        if(res_cost>thread_cost[i])
        {
            res_cost = thread_cost[i];
            res_prev = thread_prev[i];
        }
    }
    while(res_prev != 0)
    {
        printf("%d, ", res_prev);
        unsigned int mask = 0x00000001;
        mask <<= (res_prev-1);
        all_path &= ~mask;
        res_prev = dp[all_path][res_prev].prev_end;
    }
    printf("0\n");
    double end = elapsed_seconds();
    printf("Cost: %f\n", res_cost);
    printf("Time: %f\n", end-start);

    free(points);
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
    points = malloc(n*sizeof(Point));

    for(int i=0; i<n; i++)
    {
        fgets(input_line, 50, filePointer);
        points[i].x = strtof(strtok(input_line, " "), NULL);
        points[i].y = strtof(strtok(NULL, " "), NULL);
    }
    fclose(filePointer);
}
void base_case_calculation()
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
float dist(const Point* a, const Point* b)
{
    float dif_x = (*a).x-(*b).x;
    float dif_y = (*a).y-(*b).y;
    return sqrt(dif_x*dif_x+dif_y*dif_y);
}
double elapsed_seconds()
{
   struct timeval tv;
   struct timezone tz;
   gettimeofday(&tv, &tz);
   return (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;
}
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
} dp_setup_input;
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
void base_case_calculation(); // setup dp and all_path
void dispatch_thread_work(float* thread_return_cost, int* thread_return_prev);
// every thread's best path is written to thread_return_cost
// every thread's best path's second to last vertex is written to thread_return_prev
void collect_thread_result(float* thread_return_cost, int* thread_return_prev, float* res_cost, int* res_path);

#define INT_DIGIT 24 // max number of vertices - 1
#define MAX_INT 16777216 // same as 2^INT_DIGIT
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
    solve_wrapper_input* input = (solve_wrapper_input*) parameter;
    float thread_min = LARGE_VALUE;
    int thread_prev;

    for(int i=input->start; i<n; i+=thread_count)
    {
        unsigned int mask = 0x00000001;
        mask <<= (i-1);
        unsigned int new_path = all_path & (~mask);

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
    
    float thread_return_cost[thread_count];
    int thread_return_prev[thread_count];
    float res_cost = LARGE_VALUE;
    int res_path[n];

    double start = elapsed_seconds();
    dispatch_thread_work(thread_return_cost, thread_return_prev);
    collect_thread_result(thread_return_cost, thread_return_prev, &res_cost, res_path);
    double end = elapsed_seconds();

    float res_cost_actual = dist(&points[res_path[n-1]], &points[res_path[0]]);
    printf("Path: 0, ");
    for(int i=1; i<n; i++)
    {
        printf("%d, ", res_path[i]);
        res_cost_actual += dist(&points[res_path[i-1]], &points[res_path[i]]);
    }

    printf("\n");
    printf("Computed Cost: %f\n", res_cost);
    printf("Actual   Cost: %f\n", res_cost_actual);
    printf("Time Taken(s): %f\n", end-start);
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
void* dp_setup(void* input)
{
    unsigned long long start = ((dp_setup_input*)input)->start;
    unsigned long long end = ((dp_setup_input*)input)->end;
    for(unsigned long long i=start; i<end; i++)
    {
        for(unsigned long long j=0; j<INT_DIGIT; j++)
        {
            dp[i][j].cost = -1;
        }
    }
    return NULL;
}
void base_case_calculation()
{
    pthread_t thread_handles[thread_count];
    dp_setup_input thread_inputs[thread_count];
    unsigned long long k = (MAX_INT)%thread_count;
    unsigned long long s = (MAX_INT)/thread_count;
    for(unsigned long long i=0; i<thread_count; i++)
    {   
        if(i<k)
        {
            thread_inputs[i].start = i*s+i;
            thread_inputs[i].end = thread_inputs[i].start+s+1;
        }
        else
        {
            thread_inputs[i].start = i*s+k;
            thread_inputs[i].end = thread_inputs[i].start+s;
        }
        pthread_create(&thread_handles[i], NULL, dp_setup, (void*) &thread_inputs[i]);
    }
    for(int i=0; i<thread_count; i++)
    {
        pthread_join(thread_handles[i], NULL);
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
void dispatch_thread_work(float* thread_return_cost, int* thread_return_prev)
{
    pthread_t thread_handles[thread_count];
    solve_wrapper_input thread_inputs[thread_count];

    for(int i=0; i<thread_count; i++)
    {   
        thread_inputs[i].return_cost = &thread_return_cost[i];
        thread_inputs[i].return_prev = &thread_return_prev[i];
        thread_inputs[i].start = i+1;
        pthread_create(&thread_handles[i], NULL, solve_wrapper, (void*) &thread_inputs[i]);
    }
    for(int i=0; i<thread_count; i++)
    {
        pthread_join(thread_handles[i], NULL);
    }
}
void collect_thread_result(float* thread_return_cost, int* thread_return_prev, float* res_cost, int* res_path)
{
    int res_prev;
    for(int i=0; i<thread_count; i++)
    {
        if(*res_cost>thread_return_cost[i])
        {
            *res_cost = thread_return_cost[i];
            res_prev = thread_return_prev[i];
        }
    }
    
    int index=n-1;
    while(res_prev != 0)
    {
        res_path[index--] = res_prev;
        unsigned int mask = 0x00000001;
        mask <<= (res_prev-1);
        all_path &= ~mask;
        res_prev = dp[all_path][res_prev].prev_end;
    }
    res_path[index--] = 0;
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
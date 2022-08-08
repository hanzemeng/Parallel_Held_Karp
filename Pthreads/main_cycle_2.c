#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct
{
    double x;
    double y;
} Point;
typedef struct
{
    unsigned int path;
    int end;
    double* return_value;
} solve_input;

void setup(const char* path);
double dist(const Point* a, const Point* b);
double elapsed_seconds();

#define MAX_INT 16777216
#define INT_DIGIT 24
#define THREAD_COUNT 2
#define LARGE_VALUE 666666;

int n;
Point* points;
double dp[MAX_INT][INT_DIGIT];

double solve(unsigned int path, int end)
{
    if(dp[path][end] != -1)
    {
        return dp[path][end];
    }

    double current_min = LARGE_VALUE;
    unsigned int mask = 0xfffffffe;
    for(int i=1; i<n; i++)
    {
        if((path & (~mask)) != 0)
        {
            unsigned int new_path = path & mask;
            double temp = solve(new_path, i) + dist(&points[i], &points[end]);
            current_min = current_min<temp ? current_min:temp;
        }
        mask <<= 1;
        mask |= 0x00000001;
    }
    return dp[path][end] = current_min;
}
void* solve_wrapper(void* parameter)
{
    *(((solve_input*)parameter)->return_value) = solve(((solve_input*)parameter)->path, ((solve_input*)parameter)->end);
    return NULL;
}

int main()
{
    setup("points.txt");
    for(unsigned long long i=0; i<MAX_INT; i++)
    {
        for(unsigned long long j=0; j<INT_DIGIT; j++)
        {
            dp[i][j] = -1;
        }
    }

    double start = elapsed_seconds();
    unsigned int all_path = 0;
    for(int i=1; i<n; i++)
    {
        dp[0][i] = dist(&points[0], &points[i]);
        all_path |= 0x00000001;
        all_path <<= 1;
    }
    all_path >>= 1;

    pthread_t thread_handles[THREAD_COUNT];
    solve_input thread_inputs[THREAD_COUNT];

    unsigned int mask = 0xfffffffe;
    double res = LARGE_VALUE;
    for(int i=1; i<n;)
    {   
        unsigned int new_path;
        double temp0, temp1;
        new_path = all_path & mask;
        mask <<= 1;
        mask |= 0x00000001;
        thread_inputs[0].path = new_path;
        thread_inputs[0].end = i++;
        thread_inputs[0].return_value = &temp0;
        pthread_create(&thread_handles[0], NULL, solve_wrapper, (void*) &thread_inputs[0]);

        new_path = all_path & mask;
        mask <<= 1;
        mask |= 0x00000001;
        thread_inputs[1].path = new_path;
        thread_inputs[1].end = i++;
        thread_inputs[1].return_value = &temp1;
        pthread_create(&thread_handles[1], NULL, solve_wrapper, (void*) &thread_inputs[1]);

        pthread_join(thread_handles[0], NULL);
        pthread_join(thread_handles[1], NULL);

        temp0 += dist(&points[i-2], &points[0]);
        temp1 += dist(&points[i-1], &points[0]);
        double temp = temp0<temp1 ? temp0:temp1;
        res = res<temp ? res:temp;
    }
    double end = elapsed_seconds();
    printf("Res: %f\n", res);
    printf("Time: %f\n", end-start);

    free(points);
    return 0;
}

void setup(const char* path)
{
    FILE *filePointer;
    char input_line[50];
    filePointer = fopen(path, "r");

    fgets(input_line, 50, filePointer);
    n = strtol(input_line, NULL, 10);
    points = malloc(n*sizeof(Point));

    int i=0;
    while(fgets(input_line, 50, filePointer) != NULL)
    {
        points[i].x = strtol(strtok(input_line, " "), NULL, 10);
        points[i].y = strtol(strtok(NULL, " "), NULL, 10);
        i++;
    }
    fclose(filePointer);
}
double dist(const Point* a, const Point* b)
{
    double dif_x = (*a).x-(*b).x;
    double dif_y = (*a).y-(*b).y;
    return sqrt(dif_x*dif_x+dif_y*dif_y);
}
double elapsed_seconds()
{
   struct timeval tv;
   struct timezone tz;
   gettimeofday(&tv, &tz);
   return (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;
}
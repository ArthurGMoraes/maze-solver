#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// get adj vertices
int dX[4] = {-1, 1, 0, 0};
int dY[4] = {0, 0, -1, 1};

typedef struct
{
    int row, col;
} Point;

typedef struct
{
    Point pos;
    int f, g; // f(x) = g(x) + h(x) | h is calculated each time
    bool visited;
    Point parent;
} Vertice;

typedef struct
{
    Point pos;
    Point parent;
} VerticeBFS;

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

// Manhattan distance
int heuristic(int row, int col, int height, int width)
{
    return ((height - row) + (width - col));
}

void insert(Vertice queue[], int *rear, Vertice vertice)
{
    queue[(*rear)++] = vertice;
    // sort queue based on f value
    for (int i = *rear - 1; i > 0 && queue[i].f > queue[i - 1].f; i--)
    {
        Vertice temp = queue[i];
        queue[i] = queue[i - 1];
        queue[i - 1] = temp;
    }
}

void reconstruct(Point pos, Point start, char ***maze, Vertice **graph)
{
    while (!(pos.row == start.row && pos.col == start.col))
    {
        (*maze)[pos.row][pos.col] = 'o';
        pos = graph[pos.row][pos.col].parent;
    }
    (*maze)[start.row][start.col] = 'o';
}

// A*
bool solveMaze(char ***maze, int height, int width)
{
    if(*maze[0][0] != '*'){
        return false;
    }

    int rear = 0;
    Vertice *priorityQueue = (Vertice *)malloc((height * width) * sizeof(Vertice));

    Vertice **trackingGraph = (Vertice **)malloc(height * sizeof(Vertice *));
    for (int i = 0; i < height; i++)
    {
        (trackingGraph)[i] = (Vertice *)malloc(width * sizeof(Vertice));
    };

    // Initialize cost and visited arrays
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            trackingGraph[i][j].pos = (Point){i, j};
            trackingGraph[i][j].g = INT_MAX;
            trackingGraph[i][j].visited = false;
        }
    }

    int eX = width - 3; // windows -2 | linux -3

    // Start and end points
    Point start = {0, 0};
    Point end = {height - 1, eX};
    // printf("%d", width);

    // starting Vertice
    trackingGraph[0][0].g = 0;
    trackingGraph[0][0].f = heuristic(0, 0, height - 1, eX);
    insert(priorityQueue, &rear, trackingGraph[0][0]);

    while (rear > 0)
    {
        Vertice current = priorityQueue[--rear]; // change to point queue?
        Point pos = current.pos;
        // printf("(%d %d) ", pos.row, pos.col);

        if (pos.row == end.row && pos.col == end.col)
        {
            reconstruct(pos, start, maze, trackingGraph);
            for (int i = 0; i < height; i++)
            {
                free(trackingGraph[i]);
            }
            free(trackingGraph);
            free(priorityQueue);
            return true;
        }

        // mark as visited
        if (trackingGraph[pos.row][pos.col].visited != true)
        {
            trackingGraph[pos.row][pos.col].visited = true;
        }

        // explore neighbors
        for (int i = 0; i < 4; i++)
        {
            int adjRow = pos.row + dY[i];
            int adjCol = pos.col + dX[i];

            if (!(adjRow < 0 || adjRow >= height || adjCol < 0 || adjCol >= width || (*maze)[adjRow][adjCol] != '*' || trackingGraph[adjRow][adjCol].visited == true))
            {
                int adjG = trackingGraph[pos.row][pos.col].g + 1;
                int adjH = heuristic(adjRow, adjCol, height - 1, eX);
                int adjF = adjG + adjH;

                if (adjF < trackingGraph[adjRow][adjCol].g)
                {
                    trackingGraph[adjRow][adjCol].g = adjG;
                    trackingGraph[adjRow][adjCol].f = adjF;
                    trackingGraph[adjRow][adjCol].parent = pos;

                    insert(priorityQueue, &rear, trackingGraph[adjRow][adjCol]);
                }
            }
        }
    }

    // no path found
    return false;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void reconstructBFS(Point pos, Point start, char ***maze, VerticeBFS **graph)
{
    while (!(pos.row == start.row && pos.col == start.col))
    {
        (*maze)[pos.row][pos.col] = 'o';
        pos = graph[pos.row][pos.col].parent;
    }
    (*maze)[start.row][start.col] = 'o';
}

// BFS function
bool bfsMaze(char ***maze, int height, int width)
{
    VerticeBFS *queue = (VerticeBFS *)malloc((height * width) * sizeof(VerticeBFS));
    int front = 0;
    int rear = 0;

    VerticeBFS **trackingGraph = (VerticeBFS **)malloc(height * sizeof(VerticeBFS *));
    bool **visited = (bool **)malloc(height * sizeof(bool *));
    for (int i = 0; i < height; i++)
    {
        trackingGraph[i] = (VerticeBFS *)malloc(width * sizeof(VerticeBFS));
        visited[i] = (bool *)malloc(width * sizeof(bool));
        for (int j = 0; j < width; j++)
        {
            visited[i][j] = false;
            trackingGraph[i][j].pos = (Point){i, j};
            trackingGraph[i][j].parent = (Point){-1, -1};
        }
    }

    Point start = {0, 0};
    Point end = {height - 1, width - 3};

    queue[rear++] = (VerticeBFS){start, (Point){-1, -1}};
    visited[start.row][start.col] = true;

    while (front < rear)
    {
        VerticeBFS current = queue[front++];
        Point pos = current.pos;

        if (pos.row == end.row && pos.col == end.col)
        {
            reconstructBFS(pos, start, maze, trackingGraph);
            for (int i = 0; i < height; i++)
            {
                free(trackingGraph[i]);
                free(visited[i]);
            }
            free(trackingGraph);
            free(visited);
            free(queue);
            return true;
        }

        for (int i = 0; i < 4; i++)
        {
            int adjRow = pos.row + dX[i];
            int adjCol = pos.col + dY[i];

            if (adjRow >= 0 && adjRow < height &&
                adjCol >= 0 && adjCol < width &&
                (*maze)[adjRow][adjCol] == '*' &&
                !visited[adjRow][adjCol])
            {

                visited[adjRow][adjCol] = true;
                trackingGraph[adjRow][adjCol].parent = pos;
                queue[rear++] = (VerticeBFS){(Point){adjRow, adjCol}, pos};
            }
        }
    }

    // no path found
    for (int i = 0; i < height; i++)
    {
        free(trackingGraph[i]);
        free(visited[i]);
    }
    free(trackingGraph);
    free(visited);
    free(queue);
    return false;
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void printMaze(char **maze, int height, int width)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf("%c", maze[i][j]);
        }
    }
}

bool readMaze(FILE *file, int *width, int *height, char ***maze)
{
    char line[10000];

    while (fgets(line, sizeof(line), file))
    {
        (*height)++;
        if ((int)strlen(line) > (*width))
        {
            *width = strlen(line); // linux different from windows
            // printf("%d ", *width);
        }
    }

    *maze = (char **)malloc((*height) * sizeof(char *));
    for (int i = 0; i < *height; i++)
    {
        (*maze)[i] = (char *)malloc(((*width)) * sizeof(char));
    }

    fseek(file, 0, SEEK_SET);

    for (int i = 0; i < *height; i++)
    {
        fgets((*maze)[i], (*width) + 1, file);
    }

    return true;
} // TWO-PASS BETTER THAN REALLOC?

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 1; // no file
    }

    FILE *file = fopen(argv[1], "r");
    int width = 0, height = 0;
    char **maze = NULL;

    if (file == NULL)
    {
        return 1; // error opening file
    }

    if (readMaze(file, &width, &height, &maze) == true)
    {
        fclose(file);
        // printMaze(maze, height,width);
        solveMaze(&maze, height, width) ? printMaze(maze, height, width) : printf("no solution found");
    }

    for (int i = 0; i < height; i++)
    {
        free(maze[i]);
    }
    free(maze);
}

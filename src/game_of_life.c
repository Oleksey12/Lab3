#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define LEFT "|"
#define UP "="
#define DEADSYMB " "
#define ALIVESYMB "o"
#define WIDTH 25
#define LEN 80
#define ALIVE 1
#define DEAD 0
#define BASEDELAY 50E3
#define MINDELAY 1
#define MAXDELAY 20

// Calculating new generation
int isNeighborAlive(int Table[WIDTH][LEN], int i, int j);
int neighborCount(int Table[WIDTH][LEN], int i, int j);
void newGeneration(int Table[WIDTH][LEN]);
// End of the game
int alive_count(int Table[WIDTH][LEN]);
void copyTable(int Table[WIDTH][LEN], int temp[WIDTH][LEN]);
int compareMatrixes(int Table[WIDTH][LEN], int temp[WIDTH][LEN]);
// Enter data
int enterMatrix(int Table[WIDTH][LEN]);
// Drawing game
void interface(int delay, int generation);
void reDraw(int Table[WIDTH][LEN]);
// Change game parametres
int inputController(char buf, int *delay);

int main() {
    int arr[WIDTH][LEN], temp[WIDTH][LEN];
    int delay = 11, generation = 1;
    char buf[1];

    if (enterMatrix(arr) == 1) {
        printf("Error with reading file");
        return 1;
    }
    // Returning standart input stream
    FILE *fp;
    fp = freopen("/dev/tty", "r", stdin);
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

    do {
        printf("\033[0d\033[2J");
        interface(delay, generation);
        reDraw(arr);
        copyTable(arr, temp);
        newGeneration(arr);

        usleep(BASEDELAY * delay);
        read(0, buf, 1);
        ++generation;
    } while (alive_count(arr) != 0 && compareMatrixes(arr, temp) != 0 &&
             inputController(buf[0], &delay) != 1);

    fclose(fp);
    return 0;
}

int enterMatrix(int Table[WIDTH][LEN]) {
    char ch;
    for (int i = 0; i < WIDTH; ++i) {
        for (int j = 0; j < LEN; ++j) {
            scanf("%c", &ch);
            if (ch == '\n')
                --j;
            else if (ch == 48 || ch == 49)
                Table[i][j] = ch - 48;
            else {
                return 1;
            }
        }
    }

    return 0;
}

void interface(int delay, int generation) {
    printf("Enter \'q\' to exit, \'w\' and \'s\' to change speed\n");
    printf("Iteration %d, current speed %d\n", generation, (MAXDELAY - delay + 1));
}

void reDraw(int Table[WIDTH][LEN]) {
    for (int i = 0; i < WIDTH + 2; i++) {
        for (int j = 0; j < LEN + 2; j++) {
            if (j == 0 || j == LEN + 1) {
                printf(LEFT);
            } else if (i == 0 || i == WIDTH + 1) {
                printf(UP);
            } else if (Table[i - 1][j - 1] == 0) {
                printf(DEADSYMB);
            } else
                printf(ALIVESYMB);
        }
        printf("\n");
    }
}

int isNeighborAlive(int Table[WIDTH][LEN], int i, int j) {
    if (i >= WIDTH) i = i - WIDTH;
    if (j >= LEN) j = j - LEN;

    if (i < 0) i = WIDTH + i;
    if (j < 0) j = LEN + j;

    return Table[i][j];
}

int neighborCount(int Table[WIDTH][LEN], int i, int j) {
    int count = 0;
    count += isNeighborAlive(Table, i - 1, j - 1);
    count += isNeighborAlive(Table, i - 1, j);
    count += isNeighborAlive(Table, i - 1, j + 1);

    count += isNeighborAlive(Table, i, j + 1);
    count += isNeighborAlive(Table, i, j - 1);

    count += isNeighborAlive(Table, i + 1, j - 1);
    count += isNeighborAlive(Table, i + 1, j);
    count += isNeighborAlive(Table, i + 1, j + 1);

    return count;
}

void newGeneration(int Table[WIDTH][LEN]) {
    int TableB[WIDTH][LEN];

    for (int i = 0; i < WIDTH; ++i)
        for (int j = 0; j < LEN; ++j) {
            int curPointNeighbors = neighborCount(Table, i, j);

            if (curPointNeighbors == 2 && Table[i][j] == ALIVE)
                TableB[i][j] = ALIVE;
            else if (curPointNeighbors == 3)
                TableB[i][j] = ALIVE;
            else
                TableB[i][j] = DEAD;
        }

    for (int i = 0; i < WIDTH; ++i)
        for (int j = 0; j < LEN; ++j) Table[i][j] = TableB[i][j];
}

int alive_count(int Table[WIDTH][LEN]) {
    int count = 0;
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < LEN; j++) {
            if (Table[i][j] == 1) {
                count++;
            }
        }
    }
    return count;
}

void copyTable(int Table[WIDTH][LEN], int temp[WIDTH][LEN]) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < LEN; j++) {
            temp[i][j] = Table[i][j];
        }
    }
}

int compareMatrixes(int Table[WIDTH][LEN], int temp[WIDTH][LEN]) {
    int flag = 0;
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < LEN; j++) {
            if (Table[i][j] != temp[i][j]) {
                flag = 1;
            }
        }
    }
    return flag;
}

int inputController(char buf, int *delay) {
    int flag = 0;
    if (buf == 'q')
        flag = 1;
    else if (buf == 'w')
        *delay = *delay > MINDELAY ? *delay - 1 : *delay;
    else if (buf == 's')
        *delay = *delay < MAXDELAY ? *delay + 1 : *delay;
    return flag;
}

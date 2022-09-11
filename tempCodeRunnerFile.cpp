/**
AUTHOR: Rumen
EMAIL: jordakievr@gmail.com
KEYWORDS: A*, Simple AI
COMMENTS:
**/
//#include <iostream>
#include <cstdio>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>
#include <algorithm>>
using namespace std;

int emptySpaceMoves[4][2] = { {-1, 0}, {0, 1}, {0, -1}, {1, 0} }; /// we consider moving the 0 element
char emptySpaceMovesNames[4][7] = {"up", "right", "left", "down"};

int N, M;

struct Table
{
    int table[10][10];
    int heuristic;
    int emptySpaceRow, emptySpaceColumn;
    int distance;
    int emptySpaceMove;

    bool operator<( const Table& t ) const
    {
        return heuristic + distance > t.heuristic + t.distance;
    }
    bool operator==( const Table& t ) const
    {
        for( int i = 0; i < M; i++ )
            for( int j = 0; j < M; j++ )
                if( table[i][j] != t.table[i][j] )
                    return false;
        return true;
    }
};

Table startTable, targetTable;

unordered_map <int, pair<int, int> > numToPos; /// used to retrieve information about the desired position of a number in the table
void processNumberToPositionMap()
{
    int cnt = 1;
    for( int i = 0; i < M-1; i++ )
        for( int j = 0; j < M; j++ )
            numToPos[cnt++] = make_pair(i, j);
    for( int j = 0; j < M-1; j++ )
        numToPos[cnt++] = make_pair(M-1, j);
    numToPos[0] = make_pair(M-1, M-1);
}

int calculateHeuristicForPosition( const int& row, const int& column, const int& number )
{
    unordered_map<int, pair<int, int> >::iterator it = numToPos.find(number);
    return abs(row - it->second.first) + abs(column - it->second.second);
}

void calculateAndSetHeuristicForTable( Table& t )
{
    int tableHeuristic = 0;
    for( int i = 0; i < M; i++ )
        for( int j = 0; j < M; j++ )
            tableHeuristic += calculateHeuristicForPosition(i, j, t.table[i][j]);

    t.heuristic = tableHeuristic;
}

void printTable( const Table& t )
{
    for( int i = 0; i < M; i++ )
    {
        for( int j = 0; j < M; j++ )
            printf("%d ", t.table[i][j]);
        printf("\n");
    }
}

void input()
{
    scanf("%d", &N);
    M = (int)sqrt(N+1);
    processNumberToPositionMap();

    for( int i = 0; i < M; i++ )
        for( int j = 0; j < M; j++ )
        {
            scanf("%d", &startTable.table[i][j]);
            if( startTable.table[i][j] == 0 )
            {
                startTable.emptySpaceRow = i;
                startTable.emptySpaceColumn = j;
            }
        }

    calculateAndSetHeuristicForTable(startTable);
    startTable.distance = 0;
    startTable.emptySpaceMove = -1;

    int cnt = 1;
    for( int i = 0; i < M; i++ )
        for( int j = 0; j < M; j++ )
            targetTable.table[i][j] = cnt++;
    targetTable.table[M-1][M-1] = 0;
    targetTable.heuristic = 0;
    targetTable.emptySpaceRow = M-1;
    targetTable.emptySpaceColumn = M-1;
    targetTable.distance = 0;
    targetTable.emptySpaceMove = -1;
}

void printSolution( vector<Table>& passedTables, int idx, int cnt )
{
    if( passedTables[idx].emptySpaceMove == -1 )
    {
        printf("%d\n", cnt);
        return;
    }

    int currEmptySpaceRow = passedTables[idx].emptySpaceRow;
    int currEmptySpaceColumn = passedTables[idx].emptySpaceColumn;
    swap(passedTables[idx].table[currEmptySpaceRow][currEmptySpaceColumn],
         passedTables[idx].table[currEmptySpaceRow + emptySpaceMoves[passedTables[idx].emptySpaceMove][0]][currEmptySpaceColumn + emptySpaceMoves[passedTables[idx].emptySpaceMove][1]]);

    for( int i = idx-1; i >= 0; i-- )
    {
        if( passedTables[idx] == passedTables[i] )
        {
            printSolution(passedTables, i, cnt+1);
            printf("%s\n", emptySpaceMovesNames[passedTables[idx].emptySpaceMove]);
            return;
        }
    }
}

bool tableAlreadyPassed( const Table& t, const vector<Table>& passedTables )
{
    int sz = passedTables.size();
    for( int i = 0; i < sz; i++ )
        if( t == passedTables[i] )
            return true;
    return false;
}

void AStar()
{
    vector <Table> passedTables;
    priority_queue <Table> pq;
    pq.push(startTable);
    int currEmptySpaceRow, currEmptySpaceColumn, newEmptySpaceRow, newEmptySpaceColumn;
    while( !pq.empty() )
    {
        Table currTable = pq.top();
        pq.pop();

        if( tableAlreadyPassed(currTable, passedTables) )
            continue;

        passedTables.push_back(currTable);

        if( currTable.heuristic == 0 )
        {
            break;
        }

        currEmptySpaceRow = currTable.emptySpaceRow;
        currEmptySpaceColumn = currTable.emptySpaceColumn;

        Table newTable = currTable;
        for( int i = 0; i < 4; i++ )
        {
            newEmptySpaceRow = currEmptySpaceRow + emptySpaceMoves[i][0];
            newEmptySpaceColumn = currEmptySpaceColumn + emptySpaceMoves[i][1];

            if( newEmptySpaceRow < 0 || newEmptySpaceRow >= M ||
                newEmptySpaceColumn < 0 || newEmptySpaceColumn >= M )
                    continue;

            newTable.heuristic = currTable.heuristic -
                                  calculateHeuristicForPosition(newEmptySpaceRow, newEmptySpaceColumn,
                                                                currTable.table[newEmptySpaceRow][newEmptySpaceColumn]) +
                                  calculateHeuristicForPosition(currEmptySpaceRow, currEmptySpaceColumn,
                                                                currTable.table[newEmptySpaceRow][newEmptySpaceColumn]) -
                                  calculateHeuristicForPosition(currEmptySpaceRow, currEmptySpaceColumn,
                                                                currTable.table[currEmptySpaceRow][currEmptySpaceColumn]) +
                                  calculateHeuristicForPosition(newEmptySpaceRow, newEmptySpaceColumn,
                                                                currTable.table[currEmptySpaceRow][currEmptySpaceColumn]);

            swap(newTable.table[currEmptySpaceRow][currEmptySpaceColumn], newTable.table[newEmptySpaceRow][newEmptySpaceColumn]);
            newTable.emptySpaceRow = newEmptySpaceRow;
            newTable.emptySpaceColumn = newEmptySpaceColumn;
            newTable.distance = currTable.distance + 1;
            newTable.emptySpaceMove = 4 - i - 1;

            pq.push(newTable);
            swap(newTable.table[currEmptySpaceRow][currEmptySpaceColumn], newTable.table[newEmptySpaceRow][newEmptySpaceColumn]);
        }
    }

    int len = passedTables.size();
    printSolution(passedTables, len-1, 0);
}

void solve()
{
    AStar();
}

int main()
{
    input();
    solve();

    return 0;
}
/**
Some tests:

8
1 2 3
4 5 6
0 7 8

8
1 0 3
4 2 5
7 8 6

8
4 2 3
5 0 1
7 8 6
**/
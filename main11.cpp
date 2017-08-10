/*
 COMMVAULT HACKATON 2017
 BOTFIGHT
 Leny,Prince
 */

//#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#define PLAYER_1 0
#define PLAYER_2 1
#define MAX_LEVEL 9
#define LEVEL 3



#define RIGHT 1
#define DOWN 2
#define BOTH 3

#define SIZE 10

using namespace std;
FILE* fp;
char tabs[10];

void settab(int level)
{
    int i;
    for (i = 0; i < level; i++)
    {
        tabs[i] = '\t';
    }
    tabs[i]='\0';
}
bool commandis(const string& str, const string& command)
{
    return (str.find(command) == 0);
}

static char board[SIZE][SIZE];
static unsigned char zone[2];

//static char boards[MAX_LEVEL][2][6];
//static char zones[MAX_LEVEL][2];
//static char pitsums[MAX_LEVEL][2];

typedef struct MV
{
    char x;
    char y;
    char move;
}moveT;

void init()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
            board[i][j] = 0;
    }
    zone[0] = 0;
    zone[1] = 0;
}

void print_board(char _board[][10],char* _zone)
{
    fprintf(fp,"\n%s---------------------------------------------\n%s  ",tabs,tabs);
    
    fprintf(fp, "%d %d %d %d %d %d", _board[0][0], _board[0][1], _board[0][2], _board[0][3], _board[0][4], _board[0][5]);
    
    fprintf(fp, "\n%s%d              %d\n%s  ", tabs, _zone[0],_zone[1], tabs);
    
    fprintf(fp, "%d %d %d %d %d %d", _board[1][0], _board[1][1], _board[1][2], _board[1][3], _board[1][4], _board[1][5]);
    
    fprintf(fp, "\n%s--------------------------------------------\n%s", tabs, tabs);
}

bool make_move(char _board[][10], unsigned char *_zone, int x, int y, int move, int player)
{
    _board[x][y] |= move;
    bool didCloseBox = false;
    
    if (move == RIGHT)
    {
        if (x>0)
            if( ((_board[x-1][y] & BOTH) == BOTH) && ((_board[x - 1][y+1]&DOWN) == DOWN) )
            {
                zone[player]++;
                didCloseBox = true;
            }
        if (x<SIZE-1)
            if ( ((_board[x][y]&DOWN) == DOWN) && ((_board[x][y+1]&DOWN) == DOWN) && ((_board[x+1][y]&RIGHT) == RIGHT) )
            {
                zone[player]++;
                didCloseBox = true;
            }
    }
    else
    {
        if (y>0)
            if( ((_board[x][y-1] & BOTH) == BOTH) && ((_board[x + 1][y-1]&RIGHT) == RIGHT) )
            {
                zone[player]++;
                didCloseBox = true;
            }
        if (y<SIZE-1)
            if ( ((_board[x][y]&RIGHT) == RIGHT) && ((_board[x][y+1]&DOWN) == DOWN) && ((_board[x+1][y]&RIGHT) == RIGHT) )
            {
                zone[player]++;
                didCloseBox = true;
            }
    }
    return didCloseBox;
}


moveT get_next_move(const char _board[][SIZE],unsigned char *_zone, int player, int level)
{
    moveT bestmove;
    bestmove.x=0;
    bestmove.y=0;
    bestmove.move=RIGHT;
    if (level < 1)
        return bestmove;
    char _newboard[SIZE][SIZE];
    unsigned char _newzone[2];
    //char _bestboard[10][10];
    unsigned char _bestzone[2];
    bool isBetter = true;//first valid move is better than nothing
    //int zone_dif;
    int otherplayer = player == PLAYER_1 ? PLAYER_2 : PLAYER_1;
    //fprintf(fp, "\n%sFinding move for player %d level %d\n",tabs, player+1, level);
    
    memcpy(_bestzone, _zone, 2 * sizeof(char));
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            for(int move = 1;move<=DOWN;move=move<<1)
            {
                if ( (_board[i][j] & move) == 0)
                {
                    
                    //settab(MAX_LEVEL - level);
                    
                    memcpy(_newboard, _board, 2 * 6 * sizeof(char));//memcpy(boards[level], _board, 2 * 6 * sizeof(char));//
                    memcpy(_newzone, _zone, 2 * sizeof(char));//memcpy(zones[level], _zone, 2 * sizeof(char));//
                    
                    if (make_move(_newboard, _newzone, i , j, move, player))//if (make_move(boards[level], zones[level], j, player))//
                        get_next_move(_newboard, _newzone, player, level - 1);//get_next_move(boards[level], zones[level], player, level - 1);//
                    else
                        get_next_move(_newboard, _newzone, otherplayer, level - 1);//get_next_move(boards[level], zones[level], otherplayer, level - 1);//
                    
                    
                    
                    if (_bestzone[player] - _bestzone[otherplayer] - (_newzone[player] - _newzone[otherplayer]) < 0)
                        isBetter = true;
                    
                    if (isBetter)
                    {
                        memcpy(_bestzone, _newzone, 2 * sizeof(char)); //memcpy(_bestzone, zones[level], 2 * sizeof(char));//memcpy(_bestzone, _newzone, 2 * sizeof(char));
                        //memcpy(_bestboard, _newboard, 2 * 6 * sizeof(char)); //memcpy(_bestboard, boards[level], 2 * 6 * sizeof(char));//memcpy(_bestzone, _newzone, 2 * sizeof(char));
                        bestmove.x = i;bestmove.y = j; bestmove.move = move;
                        isBetter = false;
                    }
                }
            }
            
        }
    }
    memcpy(_zone, _bestzone, 2 * sizeof(char));
    //memcpy((void*)_board, _bestboard, 2 * 6 * sizeof(char));
    //settab(MAX_LEVEL - level);
    //fprintf(fp, "\n%s%d is the Best move for player %d level %d\n", tabs, bestmove + 1, player + 1, level);
    return bestmove;
}

int get_level_dynamic(int available_moves)
{
    int level=0;
    if(available_moves<5)
        return available_moves;
    int max_cycles=10000000;
    while(max_cycles>1 && available_moves>1)
    {
        level++;
        max_cycles=max_cycles/available_moves;
        available_moves--;
    }
    if(max_cycles>available_moves)
        level++;
    return level;
}


int main(int argc, char* argv[])
{
    int playerId = 0;
    int levels;
    unsigned char tempzone[2];
    char tempboard[SIZE][SIZE];
    int available_moves= 180;
    char x1,y1,x2,y2,x,y,move;
    string respstr;
    if (argc >= 2)
        fp = fopen( argv[1], "w");
    else
        fp = fopen( "default_log.txt", "w");
    
    bool done = false;
    while (!done)
    {
        string input;
        cin>>input;
        fprintf(fp,"\nINPUT %s\n",input.c_str());
        fflush(fp);
        //ostringstream os;
        if (commandis(input, "START"))  //START:X
        {
            //init();
            cin>>input;
            playerId = input[0] - '1';  //hack to get player id
            fprintf(fp, "Player Id =%d\n", playerId);
            fflush(fp);
        }
        else if (commandis(input, "STOP"))
        {
            //os << "STOPPED";
            fprintf(fp, "\nSTOPPED\n\n\n");
            fflush(fp);
            done = true;
        }
        else if (commandis(input, "YOUR_MOVE"))
        {
            fprintf(fp,"\nYOUR_MOVE");
            fflush(fp);
            //settab(0);
            //print_board(board, zone);
            memcpy(tempzone, zone, 2 * sizeof(char));
            memcpy(tempboard, board, 2 * 6 * sizeof(char));
            levels=2;//get_level_dynamic(available_moves);
            fprintf(fp, "\nLEVELS:%d\n",levels);
            fflush(fp);
            moveT mymove= get_next_move(tempboard, tempzone, playerId, levels );   //however you implement it
            fprintf(fp, "\nWE_MOVED %d %d %d\n", (int)mymove.x, (int)mymove.y, (int)mymove.move );
            fflush(fp);
            make_move(board, zone, mymove.x, mymove.y, mymove.move, playerId);
            //print_board(board, zone);
            if(mymove.move == RIGHT)
                cout << "("<<(int)mymove.x <<"," <<(int)mymove.y<<"),("<<(int)mymove.x<<","<<(int)mymove.y+1<<")\n";
            if(mymove.move == DOWN)
                cout << "("<<(int)mymove.x <<"," <<(int)mymove.y<<"),("<<(int)mymove.x+1<<","<<(int)mymove.y<<")\n";
            available_moves--;
        }
        else if (commandis(input, "OPPONENT_MOVE")) //update you board state
        { 
            cin >> input;
            fprintf(fp,"\nINPUT OPPONENT %s\n",input.c_str());
            fflush(fp);
            x1=input[1] - '0'; y1= input[3] - '0';
            x2=input[7] - '0'; y1= input[9] - '0';
            if(x1==x2)
            {
                move = RIGHT;
                x=x1;
                y=y1>y2?y2:y1;
            }
            else
            {
                move = DOWN;
                x=x1>x2?x2:x1;
                y=y1;
            }
            fprintf(fp, "\nOTHER_MOVED [%s] %d %d %s ", input.c_str(), x, y, move==RIGHT?"RIGHT":"DOWN" );
            make_move(board, zone, x,y, move, playerId==PLAYER_1?PLAYER_2:PLAYER_1);
            //print_board(board, zone);
            available_moves--;
        }
        fflush(fp);
        cout.flush();
    }
    fclose(fp);
    return 0;
}

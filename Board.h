//
// Created by ken on 2018/5/25.
//

#ifndef MINIALPHAGO_REVERSI_BOARD_H
#define MINIALPHAGO_REVERSI_BOARD_H

#include <cstring>
#include <list>
using namespace std;

//8 possible directions to form  enclosure
static const int direction[8][2]={{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0}};
const int valueOfPos[8][8] = {//价值矩阵 用于快速走子
        30, 2, 20, 15, 15, 20,  2, 30,
        2,  1,  2,  8,  8,  2,  1,  2,
        20, 2,  9,  9,  9,  9,  2, 20,
        15, 8,  9,  2,  2,  9,  8, 15,
        15, 8,  9,  2,  2,  9,  8, 15,
        20, 2,  9,  9,  9,  9,  2, 20,
        2,  1,  2,  8,  8,  2,  1,  2,
        30, 2, 20, 15, 15, 20,  2, 20
};
class Board{

public:
    int chessBoard[8][8];
    int blackPieceCount;
    int whitePieceCount;
    int player;
    bool isEnd;// whether game ends

    Board(){
        memset(chessBoard,0,64* sizeof(int));
        blackPieceCount = 0;
        whitePieceCount = 0;
        player = 1;// black
        isEnd = false;
    }
    Board(int ** n_Board, int n_blackPieceCount, int n_whitePieceCount,int n_player){
        memcpy(chessBoard,n_Board,64* sizeof(int));
        blackPieceCount = n_blackPieceCount;
        whitePieceCount = n_whitePieceCount;
        player = n_player;
        if (checkHasValidMove()) {
            isEnd = true;
        }else{
            player*=-1;
            isEnd = checkHasValidMove();
            player*=-1;

        }
    }
    void initialGame(){
        blackPieceCount = 2;
        whitePieceCount = 2;
        chessBoard[3][4] = chessBoard[4][3] = 1;  //|白|黑|
        chessBoard[3][3] = chessBoard[4][4] = -1; //|黑|白|
        player = 1;// black first
        isEnd = false;
    }
    int showBoard(){
        cout<<"  ";
        for (int k = 0; k < 8; ++k) {
            cout<<k<<" ";
        }
        cout<<endl;
        for (int i = 0; i < 8; ++i) {
            cout<<i<<" ";
            for (int j = 0; j < 8; ++j) {
                if (chessBoard[i][j]==0) {
                    cout<<"  ";
                } else if (chessBoard[i][j]==1) {
                    cout<<"B ";
                }else if (chessBoard[i][j]==-1) {
                    cout<<"W ";
                }

            }
            cout<<endl;
        }
    }
    bool checkHasValidMove()
    {
        int x, y;
        for (y = 0; y < 8; y++)
            for (x = 0; x < 8; x++)
                if (ProcStep(x, y, true)){
                    return true;
                }
        return false;
    }

    list<pair<pair<int, int>,int>> findValidMove(){
        list<pair<pair<int, int>,int>> result;
        int x, y;
        for (y = 0; y < 8; y++)
            for (x = 0; x < 8; x++)
                if (ProcStep(x, y, true)){
                    result.emplace_back(make_pair(x,y),valueOfPos[x][y]);//类似于push_back 不过避免了多余的临时变量
                }
        return result;
    }

    //change x,y to a designated direction for further detection
    inline bool MoveStep(int &x, int &y, int Direction)
    {
        x+=direction[Direction][0];
        y+=direction[Direction][1];
        return !(x < 0 || x > 7 || y < 0 || y > 7);
    }

    //checkOnly = false: place a new piece; else just detect if xy is a legal position
    bool ProcStep(int xPos, int yPos,  bool checkOnly = false)
    {
        int effectivePoints[8][2];
        int dir, x, y, currCount;
        bool isValidMove = false;
        //xy postion has already had a piece
        if (chessBoard[xPos][yPos] == 1|| chessBoard[xPos][yPos] ==-1)
            return false;
        //check each direction
        for (dir = 0; dir < 8; dir++)
        {
            x = xPos;
            y = yPos;
            currCount = 0;
            while (1)
            {
                if (!MoveStep(x, y, dir))//here, we have changed xy in the direction
                {
                    currCount = 0;
                    break;
                }
                //in the direction, there is a piece of enemy
                if (chessBoard[x][y] == -player)
                {
                    currCount++;//we might reverse this piece
                    effectivePoints[currCount][0] = x;
                    effectivePoints[currCount][1] = y;
                }
                    //in the direction, there is a empty piece
                else if (chessBoard[x][y] == 0)
                {
                    currCount = 0;//not a legal move
                    break;
                }
                else
                {
                    break;
                }
            }
            if (currCount != 0)
            {
                isValidMove = true;
                if (checkOnly)
                    return true;
                if (player == 1)
                {
                    blackPieceCount += currCount;
                    whitePieceCount -= currCount;
                }
                else
                {
                    whitePieceCount += currCount;
                    blackPieceCount -= currCount;
                }
                while (currCount > 0)
                {
                    //flip the pieces of enemy
                    x = effectivePoints[currCount][0];
                    y = effectivePoints[currCount][1];
                    chessBoard[x][y] *= -1;
                    currCount--;
                }
            }
        }
        if (isValidMove)
        {
            //add a new piece
            chessBoard[xPos][yPos] = player;
            if (player == 1)
                blackPieceCount++;
            else
                whitePieceCount++;
            return true;
        }
        else
            return false;
    }

    double judge(bool flag){
        if (blackPieceCount>whitePieceCount) {
            if (flag) {
                cout<<"b:"<<blackPieceCount<<" "<<"w:"<<whitePieceCount<<endl;
                cout<<"Black win!"<<endl;
            }
            return 1;
        } else if (blackPieceCount<whitePieceCount) {
            if (flag) {
                cout<<"b:"<<blackPieceCount<<" "<<"w:"<<whitePieceCount<<endl;
                cout<<"White win!"<<endl;
            }

            return 0;
        } else{
            if (flag) {

                cout<<"Draw game huh!"<<endl;
            }
            return 0.5;
        }
    }
    ~Board(){

    }


};
#endif //MINIALPHAGO_REVERSI_BOARD_H

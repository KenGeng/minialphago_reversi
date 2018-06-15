//
// Created by ken on 2018/5/25.
//

#ifndef MINIALPHAGO_REVERSI_MONTECARLO_H
#define MINIALPHAGO_REVERSI_MONTECARLO_H

#include <iostream>
#include "Board.h"
#include <list>
#include <cmath>
#include <random>

using namespace std;

double delta = 0.1;//随机因子

int debug = 0;//打印中间信息


class TreeNode{
public:
    Board tmpBoard;//game state
    int layer;
    TreeNode* father;
    list<TreeNode*> child;

    double timesOfWin;// evaluate this node; win:+1;lose:+0
    double totalVisit;
    double winRate;




    TreeNode(){
        tmpBoard = Board();
        father = nullptr;
        timesOfWin = 0;
        totalVisit = 0;
        winRate = 0;
        layer = 0;

    }
    TreeNode(Board gameBoard){
        tmpBoard = gameBoard;
        father = nullptr;
        timesOfWin = 0;
        totalVisit = 0;
        winRate = 0;
        layer = 0;

    }
    double getGrade() {
        if (totalVisit) {
            winRate = timesOfWin/totalVisit;
        } else winRate = 0;
        return winRate;
    }
    double getTotalVisit(){
        return totalVisit;//-100?
    }
    list<TreeNode*> getChild(){
        return child;
    }
    double genRandom(unsigned int begin, unsigned int end){
        mt19937 rng;
        rng.seed(std::random_device()());
        uniform_int_distribution<mt19937::result_type> dist6(begin,end); // distribution in range [0, 1]

        return dist6(rng) ;
    }
    TreeNode* select(){
        TreeNode* selected = nullptr;
        double bestValue = -10000;

        for (auto iter : child)//遍历子节点列表
        {
            TreeNode temp = *(iter);

            double  randomFactor = genRandom(0,1) ;
                    //根据下棋方的颜色进行相应的调整
            double uctValue = temp.tmpBoard.player==1?
                    temp.timesOfWin / (temp.totalVisit+delta):(temp.totalVisit-temp.timesOfWin) / (temp.totalVisit+delta)+
                    sqrt(2*temp.totalVisit +1) / (temp.totalVisit+ delta)+randomFactor*delta;

            // small random number to break ties randomly in unexpanded node
            if (uctValue >= bestValue) {//找到uctValue最大的子节点

                selected = iter;
                bestValue = uctValue;
            }
            if (debug) {
                cout<<"se.uctvalue:"<<uctValue<<endl;
            }

        }

        return selected;
    }

    bool expand(){
        //reach game end; no expansion

        int posCount = 0, choice;

        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++)
                if (tmpBoard.ProcStep(x,y,true))
                {
                    posCount++;
                    TreeNode* n_child = new TreeNode(tmpBoard);
                    n_child->tmpBoard.ProcStep(x,y, false);//do a move
                    n_child->tmpBoard.player*=-1;//change player
                    n_child->father = this;

                    //this->layer++;
                    n_child->layer = this->layer+1;
//                    cout<<"ex.child"<<n_child<<endl;
//                    cout<<"ex.father"<<n_child->father<<endl;
                    child.push_back(n_child);//memory leak?
                }

        if (posCount==0)// no legal move for this player
        {
            int posCount2 = 0;
            tmpBoard.player*=-1;//change color to check the other player
            for (int y = 0; y < 8; y++)
                for (int x = 0; x < 8; x++)
                    if (tmpBoard.ProcStep(x,y,true))
                    {
                        posCount2++;
                        TreeNode* n_child = new TreeNode(tmpBoard);
                        n_child->tmpBoard.ProcStep(x,y, false);//do a move
                        n_child->tmpBoard.player*=-1;//change player
                        n_child->father = this;

                        //this->layer++;
                        n_child->layer = this->layer+1;

                        child.push_back(n_child);
                    }
            tmpBoard.player*=-1;//recover
            if (posCount2==0) {
                return false;// game over
            }

        }
        return true;

    }

    double simulation(){


        Board rollOut = tmpBoard;
        int r_player = tmpBoard.player;
        while(1){
            if (rollOut.checkHasValidMove()) {
                list<pair<pair<int, int>,int>> temp = rollOut.findValidMove();
//使用价值矩阵
//               auto bestChoice = std::max_element(temp.begin(), temp.end(),compare);
//               rollOut.ProcStep((*bestChoice).first.first,(*bestChoice).first.second, false);
                //随机走子
                auto it = std::next(temp.begin(), genRandom(0,temp.size()));
                rollOut.ProcStep((*it).first.first,(*it).first.second, false);
                rollOut.player*=-1;
            }else{
                rollOut.player*=-1;
                if (!rollOut.checkHasValidMove()){//both have no legal move; game end.
                    break;
                }
            }

        }

        return rollOut.judge(false);//black win,return 1; white win, return 0;draw game, 0.5
    }

    TreeNode* backpropagate(double value){
        TreeNode* tmp = this;

        (tmp->totalVisit)++;
        tmp->timesOfWin+=value;

        while (tmp->father!= nullptr) {
            (tmp->father->totalVisit)++;
            if (tmp->tmpBoard.player == 1) {//tmp is black turn,just add value
                tmp->father->timesOfWin+=value;
            }else tmp->father->timesOfWin+=(1-value);//white,add 1-value

            if (debug) {
                cout<<"bp value:"<<value<<endl;
                cout<<"bp.tmp.father.vis"<<tmp->father->totalVisit<<endl;
                cout<<"bp.tmp.father.win"<<tmp->father->timesOfWin<<endl;
            }


            tmp = tmp->father;
        }

        return tmp;
    }

    void doMove(){

        TreeNode* cur= this;

        if (debug) {
            cout<<cur->child.size()<<endl;
        }
        while (!cur->child.empty()) {
            cur = cur->select();
        }
        if (debug) {
            cout<<"layer:"<<cur->layer<<endl;
        }
        cur->expand();
        if (debug) {
            cout<<"root_child:"<<child.size()<<endl;
            cout<<"cur_child:"<<cur->child.size()<<endl;
        }
        if (cur->child.size()) {
            cur = cur->select();
            double value = cur->simulation();
            cur->backpropagate(value);
        }else {
            return;//no way to go
        }


    }


    static bool compare(const pair<pair<int, int>,int> s1, const pair<pair<int, int>,int> s2){
//    return s1->timesOfWin/s1->totalVisit<s2->timesOfWin/s2->totalVisit;
        return s1.second<s2.second;//据说有奇效？
    };
};



#endif //MINIALPHAGO_REVERSI_MONTECARLO_H

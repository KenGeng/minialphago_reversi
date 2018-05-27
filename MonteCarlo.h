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

    }
    TreeNode(Board gameBoard){
        tmpBoard = gameBoard;
        father = nullptr;
        timesOfWin = 0;
        totalVisit = 0;
        winRate = 0;

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

        list<TreeNode*>::iterator iter;
        for (iter = child.begin(); iter!= child.end(); iter ++)//遍历子节点列表
        {
            TreeNode temp = *(*iter);

            double  randomFactor = genRandom(0,1) ;

            double uctValue =
                    temp.timesOfWin / (temp.totalVisit+delta) +
                    2*sqrt(2*temp.totalVisit +1) / (temp.totalVisit+ delta)+randomFactor*delta;

            // small random number to break ties randomly in unexpanded node ?
            if (uctValue >= bestValue) {//找到uctValue最大的子节点

                selected = *iter;
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
        if (tmpBoard.isEnd)
        {
            return false;
        }
        int posCount = 0, choice;

        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++)
                if (tmpBoard.ProcStep(x,y,true))
                {
                    posCount++;
                    TreeNode* n_child = new TreeNode(tmpBoard);
                    n_child->tmpBoard.ProcStep(x,y, false);
                    n_child->tmpBoard.player*=-1;
                    n_child->father = this;
//                    cout<<"ex.child"<<n_child<<endl;
//                    cout<<"ex.father"<<n_child->father<<endl;
                    child.push_back(n_child);//memory leak?
                }

        if (posCount==0)
        {
            return false;
        }
        return true;

    }

    double simulation(){

        //todo: try to use a roll-out method ;
        double  res = genRandom(0,1) ;
       if (debug) {
           cout<<"simulation:"<<res;
       }
        return res;
    }

    TreeNode* backpropagate(double value){
        TreeNode* tmp = this;

        (tmp->totalVisit)++;
        tmp->timesOfWin+=value;

        while (tmp->father!= nullptr) {
            (tmp->father->totalVisit)++;
            tmp->father->timesOfWin+=value;
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


};



#endif //MINIALPHAGO_REVERSI_MONTECARLO_H

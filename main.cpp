#include <iostream>
//#include "Board.h"
#include "MonteCarlo.h"

#define MAX_ITER 1000

//todo:可以考虑设置每一步的时间限制?
const clock_t startTime = clock();
const clock_t TIMELIMIT = CLOCKS_PER_SEC;

int myState= -1;//1:black -1:white
using namespace std;


int current_player = 1 ;//1:black -1: white

//compare function for max_element
static bool comp(const TreeNode* s1, const TreeNode* s2){
    return s1->timesOfWin/s1->totalVisit<s2->timesOfWin/s2->totalVisit;

};

int main() {
    Board GameBoard;
    //initial
    GameBoard.initialGame();
    GameBoard.showBoard();

    while(1){
        //current player have valid move
        if (GameBoard.checkHasValidMove()) {
            cout<<endl;
            if (GameBoard.player==1) {
                cout<<"It's black turn"<<endl;
            } else cout<<"It's white turn"<<endl;

            int x,y;
            if (GameBoard.player==myState) {//human turn
                cin>>x>>y;
                //place a piece and check if it is legal
                while(!GameBoard.ProcStep(x,y, false)){
                    cout<<"illegal move"<<endl;
                    cin>>x>>y;
                }
            }else{
                cout<<"Monte-Carlo AI begin!"<<endl;
                TreeNode mm= TreeNode(GameBoard);
                for (int i = 0; i < MAX_ITER; ++i) {
//                    cout<<i<<endl;
                    mm.doMove();
                }
//               cout<<"Monte fuck!"<<endl;

                double  best_value = 0;
                auto iter = mm.child.begin();

                auto bestChoice = std::max_element(mm.child.begin(), mm.child.end(),comp);
//                cout<<"child size:"<<mm.child.size()<<endl;
                best_value = (*bestChoice)->getGrade();
                cout<<"best win rate:"<<best_value<<"\n";

                GameBoard = (*bestChoice)->tmpBoard;

                cout<<"Monte Done!"<<endl;

                GameBoard.player*=-1;//extra change of player for that we use monte-carlo to choose a best child,while the player of child has been changed; double change=>no change

            }


            GameBoard.showBoard();
            cout<<"ply:"<<GameBoard.player<<endl;
            GameBoard.player*=-1;//change player uniformly
        } else{


                cout<<"No legal move. Change color."<<endl;
                GameBoard.player*=-1;
            if (!GameBoard.checkHasValidMove()){//both have no legal move; game end.
                break;
            }
        }

    }
    GameBoard.judge();//judge who wins game
    return 0;
}
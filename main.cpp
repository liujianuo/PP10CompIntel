#include <bits/stdc++.h>
#include <fstream>
#include <sys/stat.h>

using namespace std;

const long int GENERATIONCNT = 100;
const long int TESTCNT = 50;
const long int GENERATIONSIZE = 100;
const long int FOODPERCENT = 40;
const long int MOVES = 400;

string dirname;

void createdir(){
    const int dir_err = mkdir(dirname.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_err)
    {
        printf("Error creating directory!n");
        exit(1);
    }
}

struct indivs{
    char algo[243];
};

struct gendata{
    vector<indivs> ind;
    vector<int> score;
};

class board {
    private:
        char grid[20][20]; //0 is empty, 1 is food
    public:
        void setboard(){
            //reset board
            memset(grid, 0, sizeof(grid));
            srand((unsigned) time(NULL));
            for(int i = 0; i < 20; i++){
                for(int j = 0; j < 20; j++){
                    int rng = rand()%100;
                    if(rng > FOODPERCENT) grid[i][j] = 1;
                }
            }
        }
        int get(int r, int c){
            // get data from square (r, c)
            if(r>=0 && r<20 && c>=0 && c<20){
                return grid[r][c];
            }
            else return 2;
        }
        int eat(int r, int c){
            if(grid[r][c] == true){
                grid[r][c] = false;
                return true;
            }
            else return false;
        }
};

class generation {
    public:
        vector<gendata> population;
        int curgen;
        board bd;
        void init(){
            // initialises the generation size=100
            srand((unsigned) time(NULL));   
            gendata initial;
            for(int i = 0; i < GENERATIONSIZE; i++){
                indivs temp;
                for(int j = 0; j < 243; j++){
                    temp.algo[j] = rand()%5;
                }
                initial.ind.push_back(temp);
                initial.score.push_back(0);
            }
            curgen = 0;
        }
        void mutate(){
            // mutates the current population top50 survive
            //get top 50 of the population
            vector<int> bucket[4000];
            gendata cur = population[curgen];
            gendata next;
            vector<int> topidx;
            curgen++;
            int curfound = 0;
            for(int i = 0; i < GENERATIONSIZE; i++){
                bucket[cur.score[i]].push_back(i);
            }
            for(int i = 3999; i >= 0; i--){
                if(curfound>50) break;
                for(vector<int>::iterator it = bucket[i].begin(); it != bucket[i].end(); it++){
                    if(curfound > 50) break;
                    topidx.push_back(*it);
                }
            }
            srand((unsigned) time(NULL));

            //mutate the top 50 indexes with the next partner to form two more algorithms (with genetic mutation rng~2%)
            for(int i = 0; i < 25; i++){
                int a = 2*i, b = 2*i+1;
                indivs new1, new2;
                int ind = rand()%243;
                for(int j = 0; j < ind; j++){
                    new1.algo[j] = cur.ind[a].algo[j];
                    new2.algo[j] = cur.ind[b].algo[j];
                }
                for(int j = ind; j < 243; j++){
                    new1.algo[j] = cur.ind[b].algo[j];
                    new2.algo[j] = cur.ind[a].algo[j];
                }
                //random mutation
                for(int j = 0; j < 4; j++){
                    int temp = rand()%243;
                    new1.algo[temp] = rand()%5;
                }
                for(int j = 0; j < 4; j++){
                    int temp = rand()%243;
                    new2.algo[temp] = rand()%5;
                }

                next.ind.push_back(cur.ind[a]); next.ind.push_back(cur.ind[b]); next.ind.push_back(new1); next.ind.push_back(new2);
                for(int j = 0; j < 4; j++){
                    next.score.push_back(0);
                }
            }
            ofstream myfile;
            myfile.open (dirname + "/generation" + to_string(curgen-1) + ".txt");
            for(int i = 0; i < GENERATIONSIZE; i++){
                cout << next.ind[i].algo << " | " << next.score[i] << "\n";
            }
            population.push_back(next);
        }
        void test(){ //0 = eat 1 = left 2 = top 3 = right 4 = down;
            // runs multiple test on all individual in the population
            srand((unsigned) time(NULL));
            for(int i = 0; i < GENERATIONSIZE; i++){
                int sc = 0;
                for(int j = 0; j < TESTCNT; j++){
                    int curscore;
                    bd.setboard();
                    int c = rand()%20, r = rand()%20;
                    for(int i = 0; i <  MOVES; i++){
                        int cur = bd.get(r, c);
                        int left = bd.get(r,c-1);
                        int right = bd.get(r,c+1);
                        int top = bd.get(r+1,c);
                        int bot = bd.get(r-1,c);
                        int index = 81*top + 27*bot + 9*right + 3*left + cur;
                        int move = population[curgen].ind[i].algo[index];
                        switch(move){
                            case 0:
                                if(bd.eat(r, c)) curscore+=10;
                                else{
                                    curscore -= 1;
                                    int c = rand()%20, r = rand()%20;
                                }
                                break;
                            
                            case 1:
                                if(left != 2) c--;
                                else{
                                    curscore -= 1;
                                    int c = rand()%20, r = rand()%20;
                                }
                                break;
                            
                            case 2:
                                if(top != 2) r++;
                                else{
                                    curscore -= 1;
                                    int c = rand()%20, r = rand()%20;
                                }
                                break;
                            
                            case 3:
                                if(right != 2) c++;
                                else{
                                    curscore -= 1;
                                    int c = rand()%20, r = rand()%20;
                                }
                                break;
                            
                            case 4:
                                if(bot != 2) r--;
                                else{
                                    curscore -= 1;
                                    int c = rand()%20, r = rand()%20;
                                }
                                break;
                        }
                    }
                    sc = (sc*i + curscore)/(i+1);
                }
                population[curgen].score[i] = sc > 0 ? sc : 0;
            }
        }

        int gethighest(){
            int highscore = 0;
            int idx = 0;
            for(int i = 0; i < GENERATIONSIZE; i++){
                if(population[curgen].score[i] > highscore){
                    highscore = population[curgen].score[i];
                    idx = i;
                }
            }
            return idx;
        }
};

int main(){
    generation current;
    current.init();
    dirname = "algorithmdata/" + to_string((unsigned) time(NULL));
    createdir();
    for(int i = 0; i < GENERATIONCNT; i++){
        current.test();
        if(i < GENERATIONCNT - 1){
            current.mutate();
        }
    }
    int high = current.gethighest();
    cout << current.population[current.curgen].score[high];
    return 0;
}
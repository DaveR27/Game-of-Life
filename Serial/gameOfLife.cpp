#include <iostream>
#include <math.h>
#include "Vect2D.cpp"
#include <vector>
#include <fstream>

using namespace std;

class Grid {
    public:
        int width, height;
        Vect2D grid;
        Vect2D neighborhood;
        int deadValue = 0;
        int aliveValue = 1;
        Grid(int x, int y) {
            width = x; // x-axis
            height = y; //y-axis
            grid = Vect2D(x, y);
        }

    /*
    Returns the current state of the grid
    */
    Vect2D getGrid() {
        return grid;
    }

    /*
    Checks to see if what cells are alive directly around a given index.

    @param int i: first index.
    @param int j: second index.
    */
    int aliveNeighbors(int i, int j) {
        int topL = 0; // neighbors[0]
        int topM = 0; // neighbors[1]
        int topR = 0; // neighbors[2]
        int middleL = 0; // neighbors[3]
        int middleR = 0; // neighbors[4]
        int bottomL = 0; // neighbors[5]
        int bottomM = 0; // neighbors[6]
        int bottomR = 0; // neighbors[7]

        int neighbors[8] = {topL, topM, topR, middleL, middleR, bottomL, bottomM, bottomR};
        int neighborsSize = 8;
        int aliveCells = 0;

        //Handling Boundaries
        if ((i == 0) ||  (j == 0) || (i == height) || (j == width)) {


            if (i == 0 && j == 0) {
                //i != height - 1, neighbors topL, topM, topR have to be dead
                //j != width - 1, neighbors topL,middleL,bottomL have to be dead
                neighbors[4] = grid.index(i, j+1);
                neighbors[6] = grid.index(i+1, j);
                neighbors[7] = grid.index(i+1, j+1);
            }

            if ( i == height && j == width) {
                // i != height + 1, neighbors bottomL, bottomM, bottomR are dead
                // j != width + 1, neighbors topR, middleR, bottomR
                neighbors[0] = grid.index(i-1, j-1);
                neighbors[1] = grid.index(i-1, j);
                neighbors[3] = grid.index(i, j-1);
            }   

            if (i == 0 && j != 0) {
                // Top row is dead
                neighbors[3] = grid.index(i, j-1);
                neighbors[4] = grid.index(i, j+1);
                neighbors[5] = grid.index(i+1, j-1);
                neighbors[6] = grid.index(i+1, j);
                neighbors[7] = grid.index(i+1, j+1);
            } 

            if (i == height && j != width) {
                //bottom row is dead
                neighbors[0] = grid.index(i-1, j-1);
                neighbors[1] = grid.index(i-1, j);
                neighbors[2] = grid.index(i-1, j+1);
                neighbors[3] = grid.index(i, j-1);
                neighbors[4] = grid.index(i, j+1);
            }

            if (j == 0 && i != 0) {
                // left is dead
                neighbors[1] = grid.index(i-1, j);
                neighbors[2] = grid.index(i-1, j+1);
                neighbors[4] = grid.index(i, j+1);
                neighbors[6] = grid.index(i+1, j);
                neighbors[7] = grid.index(i+1, j+1);
            }

            if (j == width && i != height) {
                // right is dead
                neighbors[0] = grid.index(i-1, j-1);
                neighbors[1] = grid.index(i-1, j);
                neighbors[3] = grid.index(i, j-1);
                neighbors[5] = grid.index(i+1, j-1);
                neighbors[6] = grid.index(i+1, j);
            }


        } else { //When not a boundary
            neighbors[0] = grid.index(i-1, j-1);
            neighbors[1] = grid.index(i-1, j);
            neighbors[2] = grid.index(i-1, j+1);
            neighbors[3] = grid.index(i, j-1);
            neighbors[4] = grid.index(i, j+1);
            neighbors[5] = grid.index(i+1, j-1);
            neighbors[6] = grid.index(i+1, j);
            neighbors[7] = grid.index(i+1, j+1);
        }

        //Checks all the neighbors
        for (int x=0; x<neighborsSize; x++) {
            if (neighbors[x] == 1) {
                aliveCells++;
            }
        }
        return aliveCells; // amount of alive cells around it
    }

    void updateGrid(vector<pair<int,int>> vect, int deadOrAlive) {
        for (uint i = 0; i < vect.size(); i ++) {
            grid.insert(vect[i].first, vect[i].second, deadOrAlive);
        }
    }

    /*
    Given the state of a cell the GoL rules apply:
    - Any live call with fewer than 2 neighbors dies = underpopulation
    - Any live cell with two or three live neighbors lives on to the next gen
    - Any live cell with more than 3 live neighbors dies = overpopulation
    - Any dead cell with exactly 3 live neighbors becomes living = reproduction

    After these rules have been checked, the grid is then updated.
    */
    void evolve() {
        vector<pair<int,int>> killPos;
        vector<pair<int,int>> alivePos;

        for (int a=0; a<width; a++){
            for(int b=0; b<height; b++) {
               
                int aliveCells = aliveNeighbors(a, b);

                if (aliveCells < 2) {
                    killPos.push_back(make_pair(a, b));
                }
                if (aliveCells > 3) {
                    killPos.push_back(make_pair(a, b));
                }
                if (aliveCells == 3) {
                    alivePos.push_back(make_pair(a, b));
                }
            }
        }

        updateGrid(killPos, deadValue);
        updateGrid(alivePos, aliveValue);
        
    }

    /*
    Automatically places out a simulation on the grid.

    @param int loop: initial starting point.
    @param int generations: how long the generations will last
    */
    void playGOL(int loop, int generations){
        while (loop <= generations) {
            evolve();
            loop++; 
        }
    }

    /*
    Writes the current state of the grid to a file.
    */
    void outputGrid(){
        ofstream file;
        file.open("game_of_life_save.txt");

        for (int i=0; i < width; i++) {
            for (int j=0; j < height; j++) {
                file << grid.index(i, j);
            }
            
            file << endl;
        }
        file << endl;
           
        file.close();
    }


    /*
    Takes a txt file and inserts the pattern into the middle fo the grid

    @param string fileName: String that is the path to a .txt pattern file
    */
    void insertPatternFromFile(string fileName) {
        ifstream file(fileName);
        string line;
        int centering = 0;
        bool centered = false;
        int i = 0;
        while(getline(file, line)) {
            if(centered == false){
                centering += line.size();
                centered = true;
            }
            for (uint j=0; j<line.length(); j++) {
                if (line[j] == 'O') {
                    grid.insert((i+(width/2)) - (centering/2), (j+(height/2)) - (centering/2), aliveValue);
                }
            }
            i++;
        }
    }

};



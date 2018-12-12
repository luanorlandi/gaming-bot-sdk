#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#define EMPTY_CELL '.'
#define WATER 'X'
#define HOLE 'H'
#define HOLE_OCCUPIED 'h'
#define MOVE_UP '^'
#define MOVE_RIGHT '>'
#define MOVE_DOWN 'v'
#define MOVE_LEFT '<'
#define MOVE_FIRST 'F'      /* used to simulate a ball for the first time */

using namespace std;

class Grid;
class Ball;

class Grid {
    public:
        int width;
        int height;
        
        Grid();
        Grid(int, int);
        bool solve(Grid*,  vector<Ball>*, int, char);
        vector<Ball> getBalls();
        void show();
        void showDebug();

    private:
        vector<string> cell;
        bool moveBall(Grid*, Ball*, char);
        void undoMove(Grid*, Ball*, char);
};

class Ball {
    public:
        int x;
        int y;
        int shotCount;
        
        Ball(int, int, int);
};

/*******************************************************************************
* main
*******************************************************************************/

int main() {
    Grid field;
    Grid solution(field.width, field.height);
    vector<Ball> balls;

    // field.showDebug();

    balls = field.getBalls();

    field.solve(&solution, &balls, 0, MOVE_FIRST);

    solution.show();
}

/*******************************************************************************
* Ball methods
*******************************************************************************/

Ball::Ball(int x, int y, int shotCount) {
    this->x = x;
    this->y = y;
    this->shotCount = shotCount;
}

/*******************************************************************************
* Grid methods
*******************************************************************************/

Grid::Grid() {
    cin >> this->width >> this->height;
    cin.ignore();

    for(int i = 0; i < this->height; i++) {
        string row;
        
        cin >> row;
        cin.ignore();

        this->cell.push_back(row);
    }
}

Grid::Grid(int width, int height) {
    this->width = width;
    this->height = height;

    for(int i = 0; i < this->height; i++) {
        string row(width, EMPTY_CELL);

        this->cell.push_back(row);
    }
}

bool Grid::solve(Grid *solution, vector<Ball> *balls, int i, char direction) {
    Ball *b;
    int pX, pY;     /* previous positions */

    /* check if analyzed all balls */
    if(i < balls->size()) {
        b = &balls->at(i);
    } else {
        return true;    /* backtracking finished */
    }

    pX = b->x;
    pY = b->y;

    /* make ball move */
    if(!this->moveBall(solution, b, direction)) {
        /* can't do the move */
        return false;
    }

    if(this->cell.at(b->y).at(b->x) == HOLE) {
        /* reach a hole */
        this->cell.at(b->y).at(b->x) = HOLE_OCCUPIED;

        /* solve next ball */
        if(!solve(solution, balls, i+1, MOVE_FIRST)) {
            /* couldn't solve with current solution grid,
                another ball must have a different solution */
            this->cell.at(b->y).at(b->x) = HOLE;
            this->undoMove(solution, b, direction);

            return false;
        }
    } else if(b->shotCount == 0) {
        /* didn't reach a hole */
        this->undoMove(solution, b, direction);

        return false;

    /* try each possible move */
    } else if(!solve(solution, balls, i, MOVE_UP)) {
        if(!solve(solution, balls, i, MOVE_DOWN)) {
            if(!solve(solution, balls, i, MOVE_RIGHT)) {
                if(!solve(solution, balls, i, MOVE_LEFT)) {
                    /* couldn't solve with the ball in this position,
                        a previous move must be different */
                    this->undoMove(solution, b, direction);
                    
                    return false;
                }
            }
        }
    }

    /* solved grid for ball i */
    return true;
}

vector<Ball> Grid::getBalls() {
    vector<Ball> balls;
    
    for(int i = 0; i < this->height; i++) {
        for(int j = 0; j < this->width; j++) {
            char c = this->cell.at(i).at(j);

            if(c >= '0' && c <= '9') {
                Ball b(j, i, c - '0');
                balls.push_back(b);
            }
        }
    }

    return balls;
}

bool Grid::moveBall(Grid *solution, Ball *b, char direction) {
    int x = b->x;   /* final position x */
    int y = b->y;   /* final position y */
    int iInc = 0;   /* how much to increment, depends on direction */
    int jInc = 0;
    int i, j;
    char f, s;

    /* calculate final position and direction */
    switch(direction) {
        case MOVE_UP:
            y = b->y - b->shotCount;
            iInc = -1;
            break;
        case MOVE_DOWN:
            y = b->y + b->shotCount;
            iInc = 1;
            break;
        case MOVE_RIGHT:
            x = b->x + b->shotCount;
            jInc = 1;
            break;
        case MOVE_LEFT:
            x = b->x - b->shotCount;
            jInc = -1;
            break;
        /* default is MOVE_FIRST */
        default:
            return true;
    }

    if(x < 0 || x >= this->width || y < 0 || y >= this->height) {
        /* ball went out of the map */
        return false;
    }

    /* check the final position */
    f = this->cell.at(y).at(x);
    s = solution->cell.at(y).at(x);

    if(f == WATER || (f >= '0' && f <= '9') || f == HOLE_OCCUPIED ||
        s != EMPTY_CELL) {
        
        return false;
    }

    /* first cell was already checked above */
    i = b->y + iInc;
    j = b->x + jInc;
    
    /* check the path */
    while(i != y || j != x) {
        f = this->cell.at(i).at(j);         /* field char */
        s = solution->cell.at(i).at(j);     /* field solution */

        if(s != EMPTY_CELL || (f >= '0' && f <= '9') ||
            f == HOLE || f == HOLE_OCCUPIED) {
            
            return false;
        }

        i += iInc;
        j += jInc;
    }

    /* make move */
    i = b->y;
    j = b->x;

    while(i != y || j != x) {
        solution->cell.at(i).at(j) = direction;

        i += iInc;
        j += jInc;
    }
    
    b->x = x;
    b->y = y;

    b->shotCount--;

    return true;
}

void Grid::undoMove(Grid *solution, Ball *b, char direction) {
    int x = b->x;   /* final position x */
    int y = b->y;   /* final position y */
    int iInc = 0;   /* how much to increment, depends on direction */
    int jInc = 0;
    int i, j;
    
    b->shotCount++;
    
    /* calculate final position and direction */
    switch(direction) {
        case MOVE_UP:
            y = b->y + b->shotCount;
            iInc = 1;
            break;
        case MOVE_DOWN:
            y = b->y - b->shotCount;
            iInc = -1;
            break;
        case MOVE_RIGHT:
            x = b->x - b->shotCount;
            jInc = -1;
            break;
        case MOVE_LEFT:
            x = b->x + b->shotCount;
            jInc = 1;
            break;
        /* default is MOVE_FIRST */
        default:
            b->shotCount--;     /* no changes to do */
            return;
    }

    /* undo move */
    i = b->y;
    j = b->x;

    /* last cell on solution grid move was not filled by the move */
    do {
        i += iInc;
        j += jInc;

        solution->cell.at(i).at(j) = EMPTY_CELL;

    } while(i != y || j != x);
    
    b->x = x;
    b->y = y;
}

void Grid::show() {
    for(const auto &it: this->cell) {
        cout << it << endl;
    }
}

void Grid::showDebug() {
    // cerr << "The grid (" << this->width << "x"
    // << this->height << "):" << endl;

    for(const auto &it: this->cell) {
        cerr << it << endl;
    }
    
    cerr << "end" << endl;
}
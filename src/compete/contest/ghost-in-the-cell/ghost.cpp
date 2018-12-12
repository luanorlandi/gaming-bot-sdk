#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <list>
#include <ctime>

#define F_TEST -1
#define F_TEST2 -1

using namespace std;

class Link;
class Graph;
class Factory;
class Troop;
class Bomb;

class Link {
    public:
        int destination;
        int distance;

        int shortestDistance;
        list<int> shortestPath; /* array of nodes for
            shortest path to reach destination */
        
        Link(int, int);
};

class Graph {
    public:
        map<int, vector<Link>> node; /* key is source */
        map<int, map<int, int>> dist;/* key is source */
        map<int, Factory> factory;   /* key is source */
        map<int, vector<Troop>> troop;
            /* key is destination */
        map<int, vector<Bomb>> bomb;
        vector<Bomb> enemyBomb;
            /* key is destination */
        
        int bombsAvailable = 2;
        int actionsUsed = 0;
        int mainFactory;        /* 1 or 2 */
        int eMainFactory;       /* 1 or 2 */
        int highestProduction;
        int turn;               /* current turn */
        int turnsSimulated;
        
        Graph();
        void newLink(int, int, int);
        void readInput();
        void sortTroops();
        void sortBombs();
        void simulateTurn(int);
        void simulate(int);
        void resetSimulation(int);
        vector<int> priorityFactory(int);
        void manageCyborgsNeeded(int);
        void sendBomb(int);
        void increaseProduction(int);
        void manageIdleCyborgs(int);
        void manageAllIdleCyborgs(int);
        bool checkBombSent(int, int);
        bool checkTroopSent(int, int);
        bool checkRestrictions(int);
        void bellmanFord(int, vector<int>*, vector<int>*);
        void buildShortestPaths();
        
        /* game actions */
        void wait(int);
        void move(int, int, int, int);
        void bombard(int, int, int);
        void increase(int, int);
        void message(string, int);
        
        void show();
        void showShortestPaths();
};

class Factory {
    public:
        int id;
        int owner;
        int cyborgs;
        int production;
        int turns;
        
        int originalOwner;
        int originalCyborgs;
        int originalProduction;
        int originalTurns;
        int cyborgsAvailable;   /* how much can send away */

        map<int, int> ownerMap;         /* turn is key */
        map<int, int> cyborgsNeeded;    /* turn is key */
        map<int, int> turnsMap;         /* turn is key */
        
        Factory(int, int, int, int, int);
        bool checkIncrease();
        bool checkBombOverload(int);
        void reset(int);
};

class Troop {
    public:
        int id;
        int owner;
        int source;
        int destination;
        int cyborgs;
        int turns;
        
        int originalTurns;
        
        Troop(int, int, int, int, int, int);
        void reset();
};

class Bomb {
    public:
        int id;
        int owner;
        int source;
        int destination;
        int turns;
        
        int originalTurns;
        
        Bomb(int, int, int, int, int);
        void reset();
};

bool compareLink(Link, Link);
bool compareTroop(Troop, Troop);
bool compareBomb(Bomb, Bomb);

/***********************************************************
 main
***********************************************************/

int main() {
    clock_t begin;
    Graph graph;

    while(1) {
        begin = clock();
        graph.readInput();
        
        // graph.simulate(-1);
        // graph.manageCyborgsNeeded(-1);

        graph.simulate(1);
        graph.manageCyborgsNeeded(1);
        graph.simulate(1);
        graph.sendBomb(1);
        graph.simulate(1);
        graph.increaseProduction(1);
        graph.simulate(1);
        graph.manageAllIdleCyborgs(1);
        graph.show();
        graph.showShortestPaths();
        
        graph.message(to_string(
            float(clock() - begin) / CLOCKS_PER_SEC), 1);
        
        cout << endl;
    }
}

/***********************************************************
 Link methods
***********************************************************/

Link::Link(int destination, int distance) {
    this->destination = destination;
    this->distance = distance;
}

/***********************************************************
 Graph methods
***********************************************************/

/**
 * Initialization input
 */
Graph::Graph() {
    int factoryCount;
    int linkCount;
    int factory1;
    int factory2;
    int distance;
    
    cin >> factoryCount;
    cin.ignore();
    
    cin >> linkCount;
    cin.ignore();
    
    for(int i = 0; i < linkCount; i++) {
        cin >> factory1 >> factory2 >> distance;
        cin.ignore();
        
        this->newLink(factory1, factory2, distance);
    }
    
    this->buildShortestPaths();

    for(map<int, vector<Link>>::iterator it =
        this->node.begin();
        it != this->node.end(); it++) {
            
        sort(it->second.begin(),
            it->second.end(), compareLink);
    }

    
    this->turn = 0;
}

void Graph::newLink(int v1, int v2, int d) {
    Link link(v2, d);
    node[v1].push_back(link);
    
    link.destination = v1;
    link.distance = d;
    node[v2].push_back(link);
    
    dist[v1][v2] = d;
    dist[v2][v1] = d;
}

void Graph::readInput() {
    this->factory.clear();
    this->troop.clear();
    this->bomb.clear();
    this->enemyBomb.clear();
    this->actionsUsed = 0;
    this->highestProduction = 0;
    
    int entityCount;
    int entityId;
    int arg1, arg2, arg3, arg4, arg5;
    string entityType;
    
    cin >> entityCount;
    cin.ignore();
    
    for(int i = 0; i < entityCount; i++) {
        
        cin >> entityId >> entityType >>
            arg1 >> arg2 >> arg3 >> arg4 >> arg5;
        cin.ignore();
        
        if(entityType == "FACTORY") {
            
            Factory f = Factory(entityId,
                arg1, arg2, arg3, arg4);
            this->factory.insert(
                pair<int, Factory>(entityId, f));

            this->highestProduction = max(
                this->highestProduction, f.production);
            
        } else if(entityType == "TROOP") {
            
            Troop t = Troop(entityId,
                arg1, arg2, arg3, arg4, arg5-1);
            this->troop[t.destination].push_back(t);
            
        } else if(entityType == "BOMB") {
            
            Bomb b = Bomb(entityId,
                arg1, arg2, arg3, arg4-1);
                
            if(b.owner == 1) {
                this->bomb[b.destination].push_back(b);
            } else {
                this->enemyBomb.push_back(b);
            }
            
        } else {
            cerr << "Unknown entity type ";
            cerr << entityType << endl;
        }
    }
    
    /* update main factory */
    if(this->turn == 0) {
        if(this->factory.at(1).owner == 1) {
            this->mainFactory = 1;
            this->eMainFactory = 2;
        } else {
            this->mainFactory = 2;
            this->eMainFactory = 1;
        }
    }
    
    this->sortTroops();
    this->sortBombs();
    this->turn++;
    this->turnsSimulated = 0;
}

/**
 * Sort all troops by turns remaining to reach destination
*/
void Graph::sortTroops() {
    for(map<int, vector<Troop>>::iterator it =
        this->troop.begin();
        it != this->troop.end(); it++) {
            
        sort(it->second.begin(),
            it->second.end(), compareTroop);
    }
}

/**
 * Sort all bombs by turns remaining to reach destination
*/
void Graph::sortBombs() {
    for(map<int, vector<Bomb>>::iterator it =
        this->bomb.begin();
        it != this->bomb.end(); it++) {
            
        sort(it->second.begin(),
            it->second.end(), compareBomb);
    }
}

void Graph::simulateTurn(int myOwner) {
    int enemyOwner = -myOwner;
    int ts = this->turnsSimulated;
    
    /* Produce new cyborgs in all factories */
    for(auto &it: this->factory) {
        Factory *f = &it.second;

        f->turns = max(0, f->turns-1);

        if(f->turns <= 0) {
            if(f->owner != 0) {
                f->cyborgs += f->production;
                if(F_TEST == f->id) cerr << f->id << "-" << ts << " Simul prod, " << f->cyborgs << "cy" << endl;
            }
        }
        else if(F_TEST == f->id) cerr << f->id << "-" << ts << " Simul prod: do not prod (" << f->turns << "t remaining)" << endl;
    }

    /* Solve battles */
    for(auto &it: this->troop) {
        Factory *f = &this->factory.at(it.first);
        int myCyborgs = 0;
        int enemyCyborgs = 0;

        for(auto &t: it.second) {
            
            if(t.turns == 0) {
                if(t.owner == myOwner) {
                    myCyborgs += t.cyborgs;
                } else {
                    enemyCyborgs += t.cyborgs;
                }
            }
        }

        int total = myCyborgs - enemyCyborgs;

        if(total == 0) {
            continue;       /* tied battle */
        }

        if(f->owner == myOwner) {
            if(total > 0) {
                f->cyborgs += abs(total);
            } else {
                f->cyborgs -= abs(total);

                if(f->cyborgs < 0) {
                    f->owner = enemyOwner;
                    f->cyborgs = abs(f->cyborgs);
                }
            }
        } else if(f->owner == enemyOwner) {
            if(total > 0) {
                f->cyborgs -= abs(total);

                if(f->cyborgs < 0) {
                    f->owner = myOwner;
                    f->cyborgs = abs(f->cyborgs);
                }
            } else {
                f->cyborgs += abs(total);
            }
        } else {
            f->cyborgs -= abs(total);
            
            if(f->cyborgs < 0) {
                f->cyborgs = abs(f->cyborgs);

                if(total > 0) {
                    f->owner = myOwner;
                } else {
                    f->owner = enemyOwner;
                }
            }
        }

        if(F_TEST == f->id) cerr << f->id << "-" << ts << " Simul battle, " << f->cyborgs << "cy" << endl;
    }
    
    /* Calculate cyborgs needed */
    for(auto &it: this->factory) {
        Factory *f = &it.second;
        
        if(f->owner != myOwner) {
            f->cyborgsNeeded[ts] = f->cyborgs;
            // if(F_TEST == f->id) cerr << f->id << "-" << ts << " Simul calc, cN=" << f->cyborgsNeeded[ts] << "cy. ";
            // if(F_TEST == f->id) cerr << "ts-1=" << ts-1 << ", f->oM[t-1]=" << f->ownerMap[ts-1] << ", f->o=" << f->owner << endl;

            if((ts-1 >= 0 && f->ownerMap[ts-1] != myOwner) ||
                (ts-1 < 0 && f->owner != myOwner)) {
                
                f->cyborgsNeeded[ts]++;
                // if(F_TEST == f->id) cerr << f->id << "-" << ts << " Simul calc, ++ cN=" << f->cyborgsNeeded[ts] << "cy" << endl;
            }

            f->cyborgsAvailable = 0;
        } else {
            f->cyborgsNeeded[ts] = -f->cyborgs;
            f->cyborgsAvailable = max(
                f->cyborgsAvailable, -f->cyborgs);
        }

        f->ownerMap[ts] = f->owner;
    }

    /* Make the bombs explode */
    for(auto &it: this->bomb) {
        Factory *f = &this->factory.at(it.first);

        for(auto &b: it.second) {
            if(b.turns == 0) {
                int destroy = max(10, f->cyborgs / 2);
                f->cyborgs = max(0, f->cyborgs - destroy);
                f->turns = 5;
                
            if(F_TEST == f->id) cerr << f->id << "-" << ts << " Simul bomb exp " << b.id << ", now " << f->cyborgs << "cy" << endl;
            }
        }

        f->turnsMap[ts] = f->turns;
    }
    
    /* Move existing troops and bombs */
    for(auto &it: this->troop) {
        for(auto &t: it.second) {
            t.turns--;
            if(F_TEST == t.destination && t.turns >= 0) cerr << t.destination << "-" << ts << " Simul move troop " << t.id << ", now " << t.turns << "t away" << endl;
        }
    }
    
    for(auto &it: this->bomb) {
        for(auto &b: it.second) {
            b.turns--;
            if(F_TEST == b.destination && b.turns >= 0) cerr << b.destination << "-" << ts << " Simul move bomb " << b.id << ", now " << b.turns << "t away" << endl;
        }
    }
}

void Graph::simulate(int myOwner) {
    int enemyOwner = -myOwner;
    this->resetSimulation(myOwner);

    for(int i = 0; i < 20; i++) {
        this->simulateTurn(myOwner);
        this->turnsSimulated++;
    }

    // cerr << "*** SIMULATION ***" << endl;

    // for(auto &it: this->factory) {
    //     Factory *f = &it.second;
    //     cerr << f->id << ": ";

    //     for(auto &c: f->cyborgsNeeded) {
    //         cerr << c.second << "(" << c.first << "), ";
    //     }

    //     cerr << endl;
    // }
}

void Graph::resetSimulation(int myOwner) {
    this->turnsSimulated = 0;
    
    for(auto &it: this->troop) {
        for(auto &t: it.second) {
            t.reset();
        }
    }
    
    for(auto &it: this->bomb) {
        for(auto &b: it.second) {
            b.reset();
        }
    }

    for(auto &it: this->factory) {
        Factory *f = &it.second;
        f->reset(myOwner);
    }
}

/**
 * Calculate the priority list of factories
 * to be analyzed. The priority factor is:
 * production > distance from main factory
 * @return Vector sorted by priority
*/
vector<int> Graph::priorityFactory(int myOwner) {
    vector<int> priority;
    int myMainFactory;

    if(myOwner == 1) {
        myMainFactory = this->mainFactory;
    } else {
        myMainFactory = this->eMainFactory;
    }
    
    for(int i = 0; i < this->factory.size(); i++) {
        
        Factory f1 = this->factory.at(i);
        
        int d1 = this->dist[this->mainFactory][f1.id];
        bool inserted = false;
        
        for(int j = 0; j < priority.size() &&
            !inserted; j++) {
            
            Factory f2 = this->factory.at(priority[j]);
            int d2 = this->dist[this->mainFactory][f2.id];
            
            if(f1.production > f2.production ||
                (f1.production == f2.production &&
                d1 < d2)) {
                
                priority.insert(priority.begin()+j, i);
                inserted = true;
            }
        }
        
        if(!inserted) {
            priority.push_back(i);
        }
    }
    
    return priority;
}

void Graph::manageCyborgsNeeded(int myOwner) {
    int enemyOwner = -myOwner;
    map<int, int> moveList;   /* <source, cyborgs> */
    bool wait = true;

    vector<int> pF = this->priorityFactory(myOwner);

    for(auto it: pF) {
        Factory *receiver = &this->factory.at(it);

        if(F_TEST == receiver->id) cerr << "*** Analyze factory " << receiver->id << " ***" << endl;

        if(this->checkRestrictions(receiver->id)) {
            if(F_TEST == receiver->id) cerr << "\tRestrictions true" << endl;
            continue;
        }

        moveList.clear();
        vector<Link>::iterator it2 =
            this->node[receiver->id].begin();
        int cyborgsSent = 0;
        bool makeMove = false;
        
        for(auto &it3: receiver->cyborgsNeeded) {
            int turnNeeded = it3.first;
            int cyborgsNeeded = it3.second - cyborgsSent;
            
            if(cyborgsNeeded <= 0) {
                continue;
            }

            if(F_TEST == receiver->id) cerr << "\tNeed " << cyborgsNeeded << "cy, " << turnNeeded << "t" << endl;

            while(it2 != this->node[receiver->id].end() &&
                it2->distance <= turnNeeded) {
                
                Factory *sender = &this->factory
                    .at(it2->destination);
                it2++;              /* go to next factory */

                if(F_TEST == receiver->id) cerr << "\t\tFactory " << sender->id << " have available " << sender->cyborgsAvailable << "cy" << endl;

                if(this->checkBombSent(sender->id,
                    receiver->id)) {
                    
                    continue;
                }

                /* negative value means it
                    has cyborgs available */
                int c = min(-sender->cyborgsAvailable,
                    cyborgsNeeded);
                
                if(c > 0) {
                    if(F_TEST == receiver->id) cerr << "\t\tAdd move with " << c << "cy" << endl;
                    moveList[sender->id] = c;
                    cyborgsSent += c;
                    cyborgsNeeded -= c;
                    if(F_TEST == receiver->id) cerr << "\t\tNow need " << cyborgsNeeded << "cy" << endl;

                    if(cyborgsNeeded <= 0) {
                        makeMove = true;
                        break;
                    }
                }
            }
            
            if(makeMove) {
                break;
            }
        }
   
        if(makeMove) {
            wait = false;

            for(auto a: moveList) {
                this->move(a.first, receiver->id,
                    a.second, myOwner);
            }
        }
    }
    
    if(wait) {
        this->wait(myOwner);
    }
}

void Graph::sendBomb(int myOwner) {
    int enemyOwner = -myOwner;
    Factory *receiver, *sender;
    
    if(this->bombsAvailable <= 0) {
        return;
    }

    for(auto &it: this->factory) {
        receiver = &it.second;

        if(receiver->id == F_TEST) cerr << "--- Check send bomb to " << receiver->id << endl;

        if(receiver->production >=
            this->highestProduction) {
            
            /* get closest factory that is mine */
            for(auto &it2: this->node[receiver->id]) {
                sender = &this->factory.at(it2.destination);
                int d = it2.distance;

                if(checkTroopSent(receiver->id,
                    sender->id)) {

                    continue;
                }

                if(receiver->id == F_TEST) cerr << "\tCheck sender " << sender->id << ", bA(" << this->bombsAvailable << "), s->oW(" << sender->originalOwner << "), r->tM[" << d << "](" << receiver->checkBombOverload(d) << ")" << endl;

                if(this->bombsAvailable > 0 &&
                    sender->ownerMap[d] == myOwner &&
                    sender->originalOwner == myOwner &&
                    receiver->ownerMap[d] == enemyOwner &&
                    !receiver->checkBombOverload(d)) {
                    
                    if(receiver->id == F_TEST) cerr << "\tBomb sent!" << endl;
                    bombard(sender->id, receiver->id,
                    myOwner);
                    
                    /* simulate again to avoid
                        sending bomb to same destination
                        at the same turn */
                    this->simulate(myOwner);
                }

                if(sender->ownerMap[d] == myOwner) {
                    break;
                }
            }
        }
        else if(receiver->id == F_TEST) cerr << "\t production is not high:" << receiver->production << endl;
    }
}

void Graph::increaseProduction(int myOwner) {
    for(auto &it: this->factory) {
        Factory *f = &it.second;

        if(f->checkIncrease()) {
            this->increase(f->id, myOwner);
        }
    }
}

void Graph::manageIdleCyborgs(int myOwner) {
    int enemyOwner = -myOwner;
    Factory *receiver =
        &this->factory.at(this->mainFactory);

    if(receiver->production == 3) {
        for(auto it: this->node[receiver->id]) {
            Factory *f = &this->factory.at(it.destination);

            if(f->production <= 3 &&
                f->owner == myOwner)  {
                
                receiver = f;

                if(f->production < 3) {
                    break;
                }
            }
        }
    }
    
    if(F_TEST == receiver->id)
    cerr << "+++ Check send idle cyborgs, factory rec " << receiver->id << " +++" << endl;
    
    int cyborgsSent = 0;
    int cyborgsNeeded = 10 * (3-receiver->production) +
        receiver->cyborgsAvailable;
    
    if(F_TEST == receiver->id)
    cerr << "Need " << cyborgsNeeded << "cy, 10*(3-" << 3-receiver->production << ")+" << receiver->cyborgsAvailable << endl;
    
    for(auto it: this->node[receiver->id]) {
        Factory *sender = &this->factory.at(it.destination);
        
        if(F_TEST2 == sender->id)
        cerr << "\tSender " << receiver->id << ", " << sender->cyborgsAvailable << "cy" << endl;

        if(sender->originalOwner == myOwner &&
            sender->id != receiver->id &&
            sender->cyborgsAvailable < 0) {
            
            if(this->checkBombSent(sender->id,
                receiver->id)) {
                    
                if(F_TEST2 == sender->id)
                cerr << "\tBomb check true" << endl;
                
                continue;
            }
            
            if(receiver->production < 3) {
                cyborgsSent = min(cyborgsNeeded,
                    -sender->cyborgsAvailable);

                if(F_TEST2 == sender->id)
                cerr << "\tCyborgs sent: " << cyborgsSent << "cy, min(" << cyborgsNeeded << "," << -sender->cyborgsAvailable << ")" << endl;
                
                if(cyborgsSent <= 0) {
                    break;
                }
            } else {
                cyborgsSent = -sender->cyborgsAvailable;
                if(F_TEST2 == sender->id)
                cerr << "\tCyborgs sent: " << cyborgsSent << "cy, = -sender->cyborgsAvailable (" << sender->cyborgsAvailable << ")" << endl;
            }

            this->move(sender->id, receiver->id,
                cyborgsSent, myOwner);
            
            cyborgsNeeded -= cyborgsSent;
            if(F_TEST2 == sender->id)
            cerr << "\tUpdate need: " << cyborgsNeeded << "cy" << endl;
        }
    }
}

void Graph::manageAllIdleCyborgs(int myOwner) {
    int enemyOwner = -myOwner;
    Factory *receiver =
        &this->factory.at(this->mainFactory);

    if(receiver->production == 3) {
        for(auto it: this->node[receiver->id]) {
            Factory *f = &this->factory.at(it.destination);

            if(f->production <= 3 &&
                f->owner == myOwner)  {
                
                receiver = f;

                if(f->production < 3) {
                    break;
                }
            }
        }
    }
    
    if(F_TEST == receiver->id)
    cerr << "+++ Check send idle cyborgs, factory rec " << receiver->id << " +++" << endl;
    
    int cyborgsSent = 0;
    int cyborgsNeeded = 10 * (3-receiver->production) +
        receiver->cyborgsAvailable;
    
    if(F_TEST == receiver->id)
    cerr << "Need " << cyborgsNeeded << "cy, 10*(3-" << 3-receiver->production << ")+" << receiver->cyborgsAvailable << endl;
    
    for(auto it: this->node[receiver->id]) {
        Factory *sender = &this->factory.at(it.destination);
        
        if(F_TEST2 == sender->id)
        cerr << "\tSender " << receiver->id << ", " << sender->cyborgsAvailable << "cy" << endl;

        if(sender->originalOwner == myOwner &&
            sender->id != receiver->id &&
            sender->cyborgsAvailable < 0) {
            
            if(this->checkBombSent(sender->id,
                receiver->id)) {
                    
                if(F_TEST2 == sender->id)
                cerr << "\tBomb check true" << endl;
                
                continue;
            }
            
            if(receiver->production < 3) {
                cyborgsSent = -sender->cyborgsAvailable;

                if(F_TEST2 == sender->id)
                cerr << "\tCyborgs sent: " << cyborgsSent << "cy, min(" << cyborgsNeeded << "," << -sender->cyborgsAvailable << ")" << endl;
                
                if(cyborgsSent <= 0) {
                    break;
                }
            } else {
                cyborgsSent = -sender->cyborgsAvailable;
                if(F_TEST2 == sender->id)
                cerr << "\tCyborgs sent: " << cyborgsSent << "cy, = -sender->cyborgsAvailable (" << sender->cyborgsAvailable << ")" << endl;
            }

            this->move(sender->id, receiver->id,
                cyborgsSent, myOwner);
            
            cyborgsNeeded -= cyborgsSent;
            if(F_TEST2 == sender->id)
            cerr << "\tUpdate need: " << cyborgsNeeded << "cy" << endl;
        }
    }
}

bool Graph::checkBombSent(int source, int destination) {
    if(source == F_TEST) cerr << "\tCHECKING FOR BOMB SENT... ";

    for(auto it: this->bomb[destination]) {
        if(source == F_TEST) cerr << "Bomb " << it.id << " (" << it.turns << " < " << this->dist[source][destination] << ") ";
        
        if(it.turns == this->dist[source][destination]) {
            if(source == F_TEST) cerr << "true" << endl;
            
            return true;
        }
    }
    if(source == F_TEST) cerr << "false" << endl;
    
    return false;
}

bool Graph::checkTroopSent(int source, int destination) {
    if(source == F_TEST) cerr << "\tCHECKING FOR TROOP SENT... ";

    for(auto it: this->troop[destination]) {
        if(source == F_TEST) cerr << "Ytoop " << it.id << " (" << it.turns << " < " << this->dist[source][destination] << ") ";
        
        if(it.turns == this->dist[source][destination]) {
            if(source == F_TEST) cerr << "true" << endl;
            
            return true;
        }
    }
    if(source == F_TEST) cerr << "false" << endl;
    
    return false;
}

bool Graph::checkRestrictions(int factory) {
    // int d1 = this->dist[factory][this->mainFactory];
    // int d2 = this->dist[factory][this->eMainFactory];
    // Factory f = this->factory.at(factory);

    // if(this->turn == 1 && d1 >= d2) {
    //     return true;
    // }

    // if(f.owner == 0 && f.production == 0 &&
    //     this->turn <= 100 && d1 > d2) {
    //     return true;
    // }

    return false;
}

void Graph::bellmanFord(int source,
    vector<int> *d, vector<int> *p) {

    d->clear();
    p->clear();

    d->resize(this->node.size(), 999);
    p->resize(this->node.size(), NULL);

    d->at(source) = 0;

    for(int i = 0; i < this->node.size()-1; i++) {
        bool update = false;

        for(auto it: this->node) {
            int u = it.first;

            for(auto it2: it.second) {
                int v = it2.destination;
                int w = it2.distance;

                if(d->at(v) > d->at(u) + w) {
                    d->at(v) = d->at(u) + w;
                    p->at(v) = u;
                    
                    update = true;
                }
            }
        }

        if(!update) {
            return;     /* already finished */
        }
    }
}

void Graph::buildShortestPaths() {
    int source;
    vector<int> d;
    vector<int> p;

    for(auto &it: this->node) {
        int source = it.first;

        this->bellmanFord(source, &d, &p);

        for(auto &it2: it.second) {
            Link *l = &it2;

            l->shortestDistance = d[l->destination];
            l->shortestPath.clear();

            /* search the previous path */
            int previous = p[l->destination];

            while(previous != source) {
                l->shortestPath.push_front(previous);
                previous = p[previous];
            }

            l->shortestPath.push_back(l->destination);
        }
    }
}

void Graph::wait(int myOwner) {
    if(myOwner != 1) {
        return;
    }

    if(this->actionsUsed > 0) {
        cout << ";";
    }
    
    cout << "WAIT";
    
    this->actionsUsed++;
}

void Graph::move(int source, int destination,
    int cyborgs, int myOwner) {

    int originalDestination;
    int shortestDistance = this->dist[source][destination];
    
    if(myOwner == 1 && this->actionsUsed > 0) {
        cout << ";";
    }

    for(auto it: this->node[source]) {
        list<int> path = it.shortestPath;
        
        if(it.destination == destination) {
            int distance =
                dist[source][path.front()];
            
            if(this->factory.at(path.front())
                .ownerMap[distance] == myOwner) {
                
                destination = path.front();
                shortestDistance = distance;
            }

            break;
        }
    }
    
    if(myOwner == 1) {
        cout << "MOVE " << source << " ";
        cout << destination << " " << cyborgs;
        this->actionsUsed++;
    }

    this->factory.at(source).cyborgs -= cyborgs;
    this->factory.at(source).originalCyborgs -= cyborgs;
    this->factory.at(source).cyborgsAvailable += cyborgs;
    
    /* create troop */
    /* the troop will be sent to the original destination
        so that the simulations knows where it's his final
        destination, with the shortest distance */
    Troop t = Troop(-1, myOwner, source,
        originalDestination, cyborgs, shortestDistance);
    this->troop[t.destination].push_back(t);
}

void Graph::bombard(int source, int destination,
    int myOwner) {
    
    if(myOwner == 1 && this->actionsUsed > 0) {
        cout << ";";
    }
    
    if(myOwner == 1) {
        cout << "BOMB " << source << " ";
        cout << destination;
        this->actionsUsed++;
    }
    
    this->bombsAvailable--;
    
    /* create bomb */
    Bomb b = Bomb(-1, myOwner, source, destination,
        this->dist[source][destination]);
    this->bomb[b.destination].push_back(b);
    
}

void Graph::increase(int source, int myOwner) {
    if(myOwner == 1 && this->actionsUsed > 0) {
        cout << ";";
    }
    
    if(myOwner == 1) {
        cout << "INC " << source;
        this->actionsUsed++;
    }

    this->factory.at(source).cyborgs -= 10;
    this->factory.at(source).originalCyborgs -= 10;
    this->factory.at(source).cyborgsAvailable += 10;
    this->factory.at(source).production++;
    this->factory.at(source).originalProduction++;
}

void Graph::message(string message, int myOwner) {
    if(myOwner != 1) {
        return;
    }
    
    if(this->actionsUsed > 0) {
        cout << ";";
    }
    
    cout << "MSG " << message;
    
    this->actionsUsed++;
}

/**
 * Function for debug
*/
void Graph::show() {
    cerr << "Graph:" << endl;
    cerr << node.size() << " vertices" << endl;
    
    for(map<int, vector<Link>>::iterator it =
        node.begin(); it != node.end(); it++) {
        
        /* first = key(vertex), second = value(vector) */
        cerr << it->first << ": ";
        
        for(vector<Link>::iterator j = it->second.begin();
            j != it->second.end(); j++) {
                
            cerr << j->destination;
            cerr << "(" << j->distance << "), ";
        }
        
        cerr << endl;
    }
    
    cerr << endl;
}

/**
 * Function for debug
*/
void Graph::showShortestPaths() {
    cerr << "Graph of shortest paths:" << endl;
    cerr << node.size() << " vertices" << endl;
    
    for(map<int, vector<Link>>::iterator it =
        node.begin(); it != node.end(); it++) {
        
        for(vector<Link>::iterator j = it->second.begin();
            j != it->second.end(); j++) {
            
            cerr << it->first << "-" << j->destination;
            cerr << "(" << j->shortestDistance << ") = ";

            for(auto it3: j->shortestPath) {
                cerr << it3 << ", ";
            }
            
            cerr << endl;
        }
        
        cerr << endl;
    }
    
    cerr << endl;
}

/***********************************************************
 Factory methods
***********************************************************/

Factory::Factory(int id, int owner,
    int cyborgs, int production, int turns) {
    
    this->id = id;
    this->owner = owner;
    this->cyborgs = cyborgs;
    this->production = production;
    this->turns = turns;
    
    this->originalOwner = owner;
    this->originalCyborgs = cyborgs;
    this->originalProduction = production;
    this->originalTurns = turns;
}

bool Factory::checkIncrease() {
    if(F_TEST == this->id) cerr << "Check increase production for "
        << this->id << ", " << this->cyborgsAvailable << "cyborgs available" << endl;
    return this->cyborgsAvailable <= -10 &&
        this->production < 3;
}

bool Factory::checkBombOverload(int turn) {
    if(F_TEST == this->id) cerr << "\tCheck bomb overload, turn " << turn << ": ";

    for(int i = turn; i < turn + 5; i++) {
        if(F_TEST == this->id) cerr << this->turnsMap[i] << "(" << i << "), ";
        if(this->turnsMap[i] > 0) {
            if(F_TEST == this->id) cerr << endl;
            return true;
        }
    }
    if(F_TEST == this->id) cerr << endl;
    return false;
}

void Factory::reset(int myOwner) {
    this->owner = this->originalOwner;
    this->cyborgs = this->originalCyborgs;
    this->production = this->originalProduction;
    this->turns = this->originalTurns;

    if(this->owner == myOwner) {
        this->cyborgsAvailable = -cyborgs;
    } else {
        this->cyborgsAvailable = 0;
    }
}

/***********************************************************
 Troop methods
***********************************************************/

Troop::Troop(int id, int owner, int source,
    int destination, int cyborgs, int turns) {
    
    this->id = id;
    this->owner = owner;
    this->source = source;
    this->destination = destination;
    this->cyborgs = cyborgs;
    this->turns = turns;

    this->originalTurns = turns;
}

void Troop::reset() {
    this->turns = this->originalTurns;
}

/***********************************************************
 Bomb methods
***********************************************************/

Bomb::Bomb(int id, int owner,
    int source, int destination, int turns) {
    
    this->id = id;
    this->owner = owner;
    this->source = source;
    this->destination = destination;
    this->turns = turns;

    this->originalTurns = turns;
}

void Bomb::reset() {
    this->turns = this->originalTurns;
}

/***********************************************************
 functions
***********************************************************/

bool compareLink(Link l1, Link l2) {
    return l1.shortestDistance < l2.shortestDistance;
}

bool compareTroop(Troop t1, Troop t2) {
    return t1.turns < t2.turns;
}

bool compareBomb(Bomb t1, Bomb t2) {
    return t1.turns < t2.turns;
}
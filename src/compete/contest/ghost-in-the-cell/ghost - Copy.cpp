#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#define F_TEST 7

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
        int turn;               /* current turn */
        
        Graph();
        void newLink(int, int, int);
        void readInput();
        void sortTroops();
        void sortBombs();
        void simulateCyborgs(int);
        void moveTroops();
        void sendBomb();
        void increaseProduction();
        void manageIdleCyborgs();
        vector<int> priorityFactory();
        bool checkBombSent(int, int);
        bool checkRestrictions(int);
        
        void wait();
        void attack(int, int, int);
        void bombard(int, int);
        void increase(int);
        void message(string);
        void show();
};

class Factory {
    public:
        int id;
        int owner;
        int cyborgs;
        int production;
        int turns;
        
        int turnsSimulated;
        int originalOwner;
        int originalCyborgs;
        int cyborgsAvailable;   /* how much can send away */
        int extraCyborgsNeeded;
        
        Factory(int, int, int, int, int);
        int produce(int);
        void receiveTroop(Troop);
        void updateExtra();
        void bombed();
        bool checkIncrease();
        bool checkOverbombard(int);
};

class Troop {
    public:
        int id;
        int owner;
        int source;
        int destination;
        int cyborgs;
        int turns;
        
        Troop(int, int, int, int, int, int);
};

class Bomb {
    public:
        int id;
        int owner;
        int source;
        int destination;
        int turns;
        
        Bomb(int, int, int, int, int);
};

bool compareLink(Link, Link);
bool compareTroop(Troop, Troop);
bool compareBomb(Bomb, Bomb);

/***********************************************************
 main
***********************************************************/

int main() {
    Graph graph;
    
    while(1) {
        graph.readInput();

        graph.sendBomb();
        graph.moveTroops();
        graph.increaseProduction();
        graph.manageIdleCyborgs();
        graph.show();
        
        cout << endl;
    }
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
            
        } else if(entityType == "TROOP") {
            
            Troop t = Troop(entityId,
                arg1, arg2, arg3, arg4, arg5);
            this->troop[t.destination].push_back(t);
            
        } else if(entityType == "BOMB") {
            
            Bomb b = Bomb(entityId,
                arg1, arg2, arg3, arg4);
                
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

/**
 * Calculate how many cyborgs are are needed to
 * capture a factory
 * @param v Vertex of the factory to be analyzed
*/
void Graph::simulateCyborgs(int v) {
    int available;
    bool troopFirst;

    if(v == F_TEST) cerr << "*** ANALYZE FACTORY " << v << " ***" << endl;

    /* calculate in factory */
    if(this->factory.at(v).owner != 1) {
        /* the cyborgs are not mine */
        this->factory.at(v).cyborgs *= -1;
    }
    
    available = this->factory.at(v).cyborgs;

    if(v == F_TEST) cerr << "\tAvailable = " << available << " cyborgs" << endl;
    
    int i = 0, j = 0;
    
    while(i < this->troop[v].size() ||
        j < this->bomb[v].size()) {
        
        /* determinate if should first
            analyze the bomb or troop */
        if(i < this->troop[v].size() &&
            j < this->bomb[v].size()) {
            
            /* there still bombs and troops left */
            if(this->troop[v].at(i).turns <=
                this->bomb[v].at(j).turns) {
                
                troopFirst = true;
            } else {
                troopFirst = false;
            }
        } else if(i < this->troop[v].size()) {
            /* only troops left */
            troopFirst = true;
        } else {
            /* only bombs left */
            troopFirst = false;
        }

        if(troopFirst) {
            Troop t = this->troop[v].at(i);
            this->factory.at(v).produce(t.turns);
            this->factory.at(v).receiveTroop(t);
            i++;
        } else {
            Bomb b = this->bomb[v].at(j);
            this->factory.at(v).produce(b.turns);
            if(v == F_TEST) cerr << "\tAnalyze bomb " << b.id << ": " << b.destination << " destination, " << b.turns << " turns" << endl;
            this->factory.at(v).bombed();
            j++;
        }
        
        available = min(available, this->factory.at(v).cyborgs);

        if(v == F_TEST) cerr << "\tUpdate available = " << available << " cyborgs" << endl;
    }

    this->factory.at(v):updateExtra();
    
    if(this->factory.at(v).owner != 1) {
        this->factory.at(v).cyborgs--;
        if(v == F_TEST) cerr << "\tDecrease 1 for not been my factory: " << this->factory.at(v).cyborgs << " cyborgs" << endl;
    }

    this->factory.at(v).cyborgsAvailable = max(0, available);
    if(v == F_TEST) cerr << "\tFinal available = " << available << " cyborgs, ";
    if(v == F_TEST) cerr << "Final cyborgs = " << this->factory.at(v).cyborgs << " cyborgs, ";
    if(v == F_TEST) cerr << "Final turnsSimulated = " << this->factory.at(v).turnsSimulated << " turns" << endl;
}

void Graph::moveTroops() {
    bool wait = true;
    int cyborgsNeeded;
    map<int, int> attackList;   /* <source, cyborgs> */
    
    vector<int> pF = this->priorityFactory();

    cerr << endl;
    cerr << "MOVE TROOPS" << endl;
    cerr << "Priority Factory: ";
    for(auto it: pF) {
        cerr << it << " ";
    }
    cerr << endl;
    
    for(int i = 0; i < pF.size(); i++) {
        int p = pF[i];

        if(this->checkRestrictions(p)) {
            continue;
        }

        attackList.clear();
        cyborgsNeeded = -this->factory.at(p).cyborgs;
        
        if(p == F_TEST) cerr << "*** ANALYZE FACTORY " << p << " ***" << endl;
        if(p == F_TEST) cerr << "\tTurns simulated = " << this->factory.at(p).turnsSimulated << endl;
        if(p == F_TEST) cerr << "\tCyborgs Needed = " << cyborgsNeeded << endl;
        
        /* check if the factory need ally cyborgs */
        if(cyborgsNeeded < 0 || (cyborgsNeeded == 0 &&
            this->factory.at(p).owner == 1)) {
            if(p == F_TEST) cerr << "\tNo need for help..." << endl;
            continue;
        }
        
        for(vector<Link>::iterator it =
            this->node[p].begin();
            it != this->node[p].end() &&
            cyborgsNeeded > 0; it++) {
            
            /* it->destination is the factory
                that will send cyborgs (source) */
            Factory f = this->factory.at(it->destination);
            
            if(f.originalOwner != 1) {
                continue;
            }

            if(this->checkBombSent(it->destination, p)) {
                if(p == F_TEST) cerr << "\tA bomb is being sent from here, can send troops..." << endl;
                continue;
            }

            if(p == F_TEST) cerr << "\tFactory " << it->destination << ", distance of " << it->distance << endl;
            if(p == F_TEST) cerr << "\tShould simulate production? " << it->distance << " >? " << this->factory.at(p).turnsSimulated << endl;
            
            /* simulate production */
            if(it->distance > this->factory.at(p).turnsSimulated) {
                cyborgsNeeded += this->factory.at(p)
                    .produce(it->distance);
                if(p == F_TEST) cerr << "\tUpdate cyborgs needed: " << cyborgsNeeded << " cy" << endl;
            }

            /* get how much cyborgs the factory can send */
            if(f.originalCyborgs > 0 && f.cyborgs >= 0) {
                int c = min(f.cyborgsAvailable, cyborgsNeeded);

                if(p == F_TEST) cerr << "\tCan send " << c << " cyborgs" << endl;

                if(c > 0) {
                    /* add attack and update cyborgs */
                    cyborgsNeeded -= c;
                    attackList[it->destination] = c;
                    if(p == F_TEST) cerr << "\tAttack added. Cyborgs needed is now " << cyborgsNeeded << " cyborgs" << endl;
                }
            }
        }
        
        /* check if can capture the factory */
        if(cyborgsNeeded <= 0) {
            if(p == F_TEST) cerr << "\tCan capture factory " << p << endl;

            for(map<int, int>::iterator it2 =
                attackList.begin();
                it2 != attackList.end(); it2++) {
                
                this->attack(it2->first,
                    p, it2->second);
                
                if(p == F_TEST) cerr << "\tMOVE " << it2->first << " " << p << " " << it2->second << endl;
                
                /* update cyborgs simulated and available */
                this->factory.at(it2->first)
                    .cyborgs -= it2->second;
                this->factory.at(it2->first)
                    .cyborgsAvailable -= it2->second;
                wait = false;
            }
        }
        else {if(p == F_TEST) cerr << "\tCan NOT capture factory " << p << endl;}
    }
    
    if(wait) {
        cerr << "No moves to do, send WAIT";
        this->wait();
    }
}

void Graph::sendBomb() {
    /* simulate movements of troops */
    for(int i = 0; i < this->factory.size(); i++) {
        this->simulateCyborgs(i);
    }

    if(this->bombsAvailable > 0) {
        int d = this->dist[this->mainFactory][this->eMainFactory];
        Factory eFac = this->factory.at(this->eMainFactory);

        if(eFac.production >= 1 && eFac.checkOverbombard(d)) {
            bombard(this->mainFactory,
                this->eMainFactory);
        }
        
        for(vector<Link>::iterator it =
            this->node[this->eMainFactory].begin();
            it != this->node[this->eMainFactory].end();
            it++) {
            
            d = this->dist[this->mainFactory][it->destination];
            Factory f = this->factory.at(it->destination);

            if(this->bombsAvailable > 0 &&
                f.production >= 2 &&
                f.checkOverbombard(d)) {
                
                /* will not send a bomb to a factory more
                close to me than for the enemy */
                if(this->dist[this->mainFactory][f.id] >=
                    this->dist[this->eMainFactory][f.id]) {
                    
                    bombard(this->mainFactory, f.id);
                }
            }
        }
    }
}

void Graph::increaseProduction() {
    for(int i = 0; i < this->factory.size(); i++) {
        if(this->factory.at(i).checkIncrease()) {
            
            this->increase(i);
        }
    }
}

void Graph::manageIdleCyborgs() {
    int destination = this->mainFactory;

    if(this->factory.at(destination).production == 3) {
        for(auto it: this->node[destination]) {
            int v = it.destination;

            if(this->factory.at(v).production < 3 &&
                this->factory.at(v).owner == 1)  {
                
                destination = v;
                break;
            }   
        }
    }

    for(auto it: this->factory) {
        if(it.second.owner == 1 &&
            it.second.id != destination &&
            it.second.cyborgsAvailable > 0) {
            
            this->attack(it.second.id, destination,
                it.second.cyborgsAvailable);
        }
    }

    // if(this->factory.at(destination).production < 3) {
    //     for(vector<Link>::iterator it =
    //         this->node.at(destination).begin(); it !=
    //         this->node.at(destination).end(); it++) {
            
    //         int v = it->destination;

    //         if(this->factory.at(v).production < 3 &&
    //             this->factory.at(v).owner == 1)  {
                
    //             destination = v;
    //             break;
    //         }   
    //     }
    // }

    // for(int i = 0; i < this->factory.size(); i++) {
    //     Factory f = this->factory.at(i);

    //     if(f.cyborgsAvailable > 0) {
    //         this->attack(f.id, destination, f.cyborgsAvailable);
    //     }
    // }
}

/**
 * Calculate the priority list of factories
 * to be analyzed. The priority factor is:
 * production > distance from main factory
 * @return Vector sorted by priority
*/
vector<int> Graph::priorityFactory() {
    vector<int> priority;
    
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

bool Graph::checkRestrictions(int factory) {
    int d1 = this->dist[factory][this->mainFactory];
    int d2 = this->dist[factory][this->eMainFactory];
    Factory f = this->factory.at(factory);

    if(this->turn == 1 && d1 >= d2) {
        return true;
    }

    if(f.owner == 0 && f.production == 0 &&
        this->turn <= 100 && d1 > d2) {
        return true;
    }

    return false;
}

void Graph::wait() {
    if(this->actionsUsed > 0) {
        cout << ";";
    }
    
    cout << "WAIT";
    
    this->actionsUsed++;
}

void Graph::attack(int source,
    int destination, int cyborgs) {
    
    if(this->actionsUsed > 0) {
        cout << ";";
    }
    
    cout << "MOVE " << source << " ";
    cout << destination << " " << cyborgs;
    
    this->factory.at(source).originalCyborgs -= cyborgs;
    
    this->actionsUsed++;
}

void Graph::bombard(int source, int destination) {
    if(this->actionsUsed > 0) {
        cout << ";";
    }
    
    cout << "BOMB " << source << " ";
    cout << destination;
    
    this->bombsAvailable--;
    
    this->actionsUsed++;
    
    /* create bomb */
    /* add distance + 1, so next readInput
        will the correct distance */
    Bomb b = Bomb(-1, 1, source, destination,
        this->dist[source][destination] + 1);
    this->bomb[b.destination].push_back(b);
}

void Graph::increase(int factory) {
    if(this->actionsUsed > 0) {
        cout << ";";
    }
    
    cout << "INC " << factory;
    
    this->factory.at(factory).cyborgs -= 10;
    this->factory.at(factory).cyborgsAvailable -= 10;
    this->factory.at(factory).production++;
    
    this->actionsUsed++;
}

void Graph::message(string message) {
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

/***********************************************************
 Link methods
***********************************************************/

Link::Link(int destination, int distance) {
    this->destination = destination;
    this->distance = distance;
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
    
    this->turnsSimulated = 0;
    this->originalOwner = owner;
    this->originalCyborgs = cyborgs;
}

/**
 * Calculate production until troop reach the factory
 * @param turns How many turns to simulate
 * @return How many cyborgs was produced
*/
int Factory::produce(int turns) {
    if(this->id == F_TEST) cerr << "\tProduction ow" << this->owner << " of turn " << turns << ", with " << this->production << " cy rate";
    if(this->id == F_TEST) cerr << ". Have " << this->cyborgs << " cy, " << this->turns << " idle t";

    int startCyborgs = this->cyborgs; 
    int t = turns - this->turnsSimulated;
    this->turnsSimulated = turns;
    
    if(this->owner != 0 && this->production != 0 &&
        t > this->turns) {
        
        if(this->owner == 1) {
            this->cyborgs += this->production
                * (t - this->turns);
        } else {
            this->cyborgs -= this->production
                * (t - this->turns);
        }
    }
    
    this->turns = max(0, this->turns - t);
    if(this->id == F_TEST) cerr << ". Result " << this->cyborgs << " cy, " << this->turns << " idle t" << endl;
    return startCyborgs - this->cyborgs;
}

void Factory::receiveTroop(Troop t) {
    if(this->id == F_TEST) cerr << "\tReceive troop " << t.id << ": " << t.cyborgs << " cy, " << t.turns << " t";
    if(this->id == F_TEST) cerr << ". Have " << this->cyborgs << " cy, " << this->turns << " idle t";

    if(t.owner == 1 || this->owner == 0) {
        this->cyborgs += t.cyborgs;
    } else {
        this->cyborgs -= t.cyborgs;
    }
    
    if(this->owner == 0) {
        if(this->cyborgs > 0) {
            if(t.owner == 1) {
                this->owner = 1;
            } else {
                this->owner = -1;
            }
        }
    } else {
        if(this->cyborgs > 0) {
            this->owner = 1;
        } else if(this->cyborgs < 0) {
            this->owner = -1;
        }
    }

    if(this->id == F_TEST) cerr << ". Result " << this->cyborgs << " cy, " << this->turns << " idle t" << endl;
}

void Factory::updateExtra() {
    if(v == F_TEST) cerr << "\tUpdate extra cyborgs, was " << this->factory.at(v).cyborgs << ", now";

    if(this->factory.at(v).owner == 1) {
        this->factory.at(v).extraCyborgsNeeded = -this->factory.at(v).production;
    } else {
        this->factory.at(v).extraCyborgsNeeded = 1;

        if(this->factory.at(v).owner == -1) {
            this->factory.at(v).extraCyborgsNeeded += this->factory.at(v).production;
        }
    }

    if(v == F_TEST) cerr << this->factory.at(v).cyborgs << " cy" << endl;
}

/**
 * Suffer results of receiving a bomb
*/
void Factory::bombed() {
    if(this->id == F_TEST) cerr << "\tReceive bomb. Have " << this->cyborgs << " cy, " << this->turns << " idle t";
    bool invert = false;
    
    if(this->cyborgs < 0) {
        this->cyborgs *= -1;
        invert = true;
    }
    
    int destroy = max(10, this->cyborgs / 2);
    this->cyborgs = max(0, this->cyborgs - destroy);
    this->turns = 4;
    
    if(invert) {
        this->cyborgs *= -1;
    }

    if(this->id == F_TEST) cerr << ". Result " << this->cyborgs << " cy, " << this->turns << " idle t" << endl;
}

bool Factory::checkIncrease() {
    if(F_TEST == this->id) cerr << "Check increase production for "
        << this->id << ", " << this->cyborgsAvailable << "cyborgs available" << endl;
    return this->cyborgsAvailable >= 10 &&
        this->production < 3;
}

bool Factory::checkOverbombard(int distance) {
    if(this->id == F_TEST) cerr << this->id << "f CHECK BOMB OVERBOMBARD " << distance << "d - " << this->turnsSimulated << "tS >=? " << this->turns << "t" << endl;
    return distance - this->turnsSimulated >= this->turns;
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
}

/***********************************************************
 functions
***********************************************************/

bool compareLink(Link l1, Link l2) {
    return l1.distance < l2.distance;
}

bool compareTroop(Troop t1, Troop t2) {
    return t1.turns < t2.turns;
}

bool compareBomb(Bomb t1, Bomb t2) {
    return t1.turns < t2.turns;
}
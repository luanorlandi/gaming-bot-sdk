#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
//#include <math>

using namespace std;

/*******************************************************************************
	CLASSES
*******************************************************************************/

class Ship;
class Barrel;

class Ship {
	private:
		int id;
		int x, y;		/* position */
		int rot;		/* rotation */
		int spd;		/* speed */
		int rum;		/* rum amount */
		int own;		/* owner */

		float dist;		/* distance to my ship */
		bool target;	/* is being target by a ship */
	public:
		Ship(int, int, int, int, int, int, int);
		void findBarrel(vector<Barrel>*, vector<Ship>*);
		int findEnemy(vector<Ship>*);

		int getX() {return this->x;};
		int getY() {return this->y;};
		int getRum() {return this->rum;};
		int getDist() {return this->dist;};
		int getTarget() {return this->target;};

		void setX(int x) {this->x = x;};
		void setY(int y) {this->y = y;};
		void setRum(int rum) {this->rum = rum;};
		void setDist(int dist) {this->dist = dist;};
		void setTarget(int target) {this->target = target;};

		void show();
};

class Barrel {
	private:
		int id;
		int x, y;		/* position */
		int rum;		/* rum amount */

		float dist;		/* distance to ship */
		bool target;	/* is being target by a ship */
	public:
		Barrel(int, int, int, int);

		int getX() {return this->x;};
		int getY() {return this->y;};
		int getRum() {return this->rum;};
		int getDist() {return this->dist;};
		int getTarget() {return this->target;};

		void setX(int x) {this->x = x;};
		void setY(int y) {this->y = y;};
		void setRum(int rum) {this->rum = rum;};
		void setDist(int dist) {this->dist = dist;};
		void setTarget(int target) {this->target = target;};

		void show();
};

/*******************************************************************************
	FUNCTIONS
*******************************************************************************/

float euclidianDistance(int, int, int, int);
bool compareShip(Ship, Ship);
bool compareBarrel(Barrel, Barrel);
void showShips(vector<Ship>);
void showBarrels(vector<Barrel>);

/*******************************************************************************
	MAIN
*******************************************************************************/

int main() {
	vector<Ship> myShip;
	vector<Ship> enemyShip;
	vector<Barrel> barrel;
	
	while(1) {
		int myShipCount;
		cin >> myShipCount;
		cin.ignore();

		int entityCount;
		cin >> entityCount;
		cin.ignore();

		myShip.clear();
		enemyShip.clear();
		barrel.clear();

		for(int i = 0; i < entityCount; i++) {
			int entityId;
			string entityType;
			int x, y;
			int arg1, arg2, arg3, arg4;

			cin >> entityId >> entityType >> x >> y
				>> arg1 >> arg2 >> arg3 >> arg4; cin.ignore();

			if(entityType == "SHIP") {
				Ship s(entityId, x, y, arg1, arg2, arg3, arg4);
				
				if(arg4 == 1) {
					myShip.push_back(s);
				} else {
					enemyShip.push_back(s);
				}
			} else if(entityType == "BARREL") {
				Barrel b(entityId, x, y, arg1);
				barrel.push_back(b);
			} else {
				cerr << "Unknown entity type " << entityType << endl;
			}
		}
		
		for(int i = 0; i < myShip.size(); i++) {
			myShip[i].findBarrel(&barrel, &enemyShip);
		}
	}
}

/*******************************************************************************
	SHIP
*******************************************************************************/

Ship::Ship(int id, int x, int y, int rot, int spd, int rum, int own) {
	this->id = id;
	this->x = x;
	this->y = y;
	this->rot = rot;
	this->spd = spd;
	this->rum = rum;
	this->own = own;
}

void Ship::findBarrel(vector<Barrel> *barrel, vector<Ship> *enemy) {
	for(int i = 0; i < enemy->size(); i++) {
		float d = euclidianDistance(this->x, this->y,
			enemy->at(i).getX(), enemy->at(i).getY());
		
		enemy->at(i).setDist(d);
	}

	//showShips(*enemy);
	sort(enemy->begin(), enemy->end(), compareShip);
	showShips(*enemy);
	
	if(barrel->size() > 0) {
		for(int i = 0; i < barrel->size(); i++) {
			float d = euclidianDistance(this->x, this->y,
				barrel->at(i).getX(), barrel->at(i).getY());
			
			barrel->at(i).setDist(d);
		}

		//showBarrels(*barrel);
		sort(barrel->begin(), barrel->end(), compareBarrel);
		//showBarrels(*barrel);
	} else {
		cout << "FIRE " << enemy->at(0).getX()
			<< " " << enemy->at(0).getY() << endl;
		
		return;
	}
	
	if(enemy->at(0).getDist() <= barrel->at(0).getDist()) {
		cout << "FIRE " << enemy->at(0).getX()
			<< " " << enemy->at(0).getY() << endl;
	} else {
		for(int i = 0; i < barrel->size(); i++) {
			if(!barrel->at(i).getTarget()) {
				cout << "MOVE " << barrel->at(i).getX()
					<< " " << barrel->at(i).getY() << endl;
				
				barrel->at(i).setTarget(true);

				return;
			}
		}
	}
}

/* return the closest enemy index not targeted */
int Ship::findEnemy(vector<Ship> *enemy) {
	for(int i = 0; i < enemy->size(); i++) {
		if(!enemy->at(i).getTarget()) {
			enemy->at(i).setTarget(true);

			return i;
		}
	}
}

void Ship::show() {
	cerr << "Ship (" << this->id << ") -> x"
		<< this->x << "\ty" << this->y << "\tr" << this->rum
		<< "\td" << this->dist << "\tt" << this->target << endl; 
}

/*******************************************************************************
	BARREL
*******************************************************************************/

Barrel::Barrel(int id, int x, int y, int rum) {
	this->id = id;
	this->x = x;
	this->y = y;
	this->rum = rum;
}

void Barrel::show() {
	cerr << "Barrel (" << this->id << ") -> x"
		<< this->x << "\ty" << this->y << "\tr" << this->rum
		<< "\td" << this->dist << "\tt" << this->target << endl; 
}

/*******************************************************************************
	FUNCTIONS
*******************************************************************************/

float euclidianDistance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

bool compareBarrel(Barrel b1, Barrel b2) {
	return b1.getDist() < b2.getDist();
}

bool compareShip(Ship s1, Ship s2) {
	return s1.getDist() < s2.getDist();
}

void showShips(vector<Ship> ship) {
	for(int i = 0; i < ship.size(); i++) {
		ship.at(i).show();
	}
}

void showBarrels(vector<Barrel> barrel) {
	for(int i = 0; i < barrel.size(); i++) {
		barrel.at(i).show();
	}
}
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#define SAMPLES 0
#define DIAGNOSIS 1
#define MOLECULES 2
#define LABORATORY 3
#define CARRY_MINE 0
#define CARRY_OTHER 1
#define CARRY_CLOUD -1
#define SAMPLE_LIMIT 3
#define MOLECULE_LIMIT 10

using namespace std;

class Robot;
class Sample;
class Project;

class Robot {
		int target;
		int eta;
		int score;
		map<char, int> storage;
		map<char, int> expertise;
		
		vector<Sample> sample;		/* carried samples */
		int totalMolecule;
		int totalDiagSample;		/* total diagnosed samples */
		int totalUndSample;			/* total undiagnosed samples */
        int rank;                   /* robot level, based on expertise count */

	public:
		Robot();
		void readUpdate();
		void addSample(Sample);
		void makeAction(vector<Project>, map<char, int>, vector<Sample>*);
		void samplesState(vector<Project>, vector<Sample>*);
		void diagnosisState(vector<Project>, map<char, int>, vector<Sample>*);
		void moleculesState(vector<Project>, map<char, int>, vector<Sample>*);
		void laboratoryState(vector<Project>, map<char, int>);
        bool canCompleteSample(map<char, int>, Sample);
		char selectedBestMolecule(vector<Project>, map<char, int>);
};

class Sample {
		int id;
		int carriedBy;
		int rank;
		string gain;
		int health;
		int totalCost;
		map<char, int> cost;

	public:
		void readUpdate();
		void show();

		int getId() {return this->id;};
		int getCarriedBy() {return this->carriedBy;};
		int getRank() {return this->rank;};
		string getGain() {return this->gain;};
		int getHealth() {return this->health;};
		int getTotalCost() {return this->totalCost;};
		map<char, int> getCost() {return this->cost;};
		void setCost(char m, int c) {this->cost.at(m) = c;};
};

class Project {
        map<char, int> cost;
    
    public:
        void readUpdate();
        void show();

        map<char, int> getCost() {return this->cost;};
};

bool cmpSample(Sample, Sample);

/**
 * Bring data on patient samples from the diagnosis machine to the laboratory
 * with enough molecules to produce medicine!
 **/
int main() {
	int projectCount;
	vector<Robot> robot;
	map<char, int> available;
	vector<Sample> sample;
	vector<Project> project;

	Robot robot1;
	Robot robot2;

	robot.push_back(robot1);
	robot.push_back(robot2);

    /* read projects */
	cin >> projectCount; cin.ignore();

	for(int i = 0; i < projectCount; i++) {
		Project p;
        p.readUpdate();
        p.show();

        project.push_back(p);
	}

	/* game loop */
	while(1) {
		robot.at(0).readUpdate();
		robot.at(1).readUpdate();

		sample.clear();

		cin >> available['A'] >> available['B'] >> available['C'] >>
			available['D'] >> available['E']; cin.ignore();

		int sampleCount;
		cin >> sampleCount; cin.ignore();

		for(int i = 0; i < sampleCount; i++) {
			Sample s;
			s.readUpdate();

			sample.push_back(s);

			if(s.getCarriedBy() == 0) {
				/* my sample */
				robot.at(0).addSample(s);
			} else if(s.getCarriedBy() == 1) {
				/* enemy sample */
				robot.at(1).addSample(s);
			}
		}

		sort(sample.begin(), sample.end(), cmpSample);

		robot.at(0).makeAction(project, available, &sample);
	}
}

/*******************************************************************************
	function
*******************************************************************************/

bool cmpSample(Sample s1, Sample s2) {
	if(s1.getCarriedBy() == CARRY_CLOUD && s2.getCarriedBy() == CARRY_CLOUD) {
		return s1.getHealth() > s2.getHealth();
	} else if(s1.getCarriedBy() == CARRY_CLOUD) {
		return true;
	} else {
		return false;
	}
}

/*******************************************************************************
	Robot
*******************************************************************************/

Robot::Robot() {
	this->totalMolecule = 0;
	this->totalDiagSample = 0;
	this->totalUndSample = 0;
    this->rank = 0;
}

void Robot::readUpdate() {
	string t;

	cin >> t >> this->eta >> this->score >>
		this->storage['A'] >> this->storage['B'] >>
		this->storage['C'] >> this->storage['D'] >>
		this->storage['E'] >> this->expertise['A'] >>
		this->expertise['B'] >> this->expertise['C'] >>
		this->expertise['D'] >> this->expertise['E']; cin.ignore();

	if(t.compare("SAMPLES") == 0) {
		this->target = SAMPLES;
	} else if(t.compare("DIAGNOSIS") == 0) {
		this->target = DIAGNOSIS;
	} else if(t.compare("MOLECULES") == 0) {
		this->target = MOLECULES;
	} else if(t.compare("LABORATORY") == 0) {
		this->target = LABORATORY;
	} else {
		this->target = -1;
	}

	/* reset samples */
	this->sample.clear();

	this->totalMolecule = this->storage['A'] + this->storage['B'] +
        this->storage['C'] + this->storage['D'] + this->storage['E'];

	this->totalUndSample = 0;
	this->totalDiagSample = 0;

    /* update level */
    int totalExpertise = this->expertise['A'] + this->expertise['B'] +
        this->expertise['C'] + this->expertise['D'] + this->expertise['E'];
    
    if(totalExpertise <= 5) {
        this->rank = 1;
    } else if(totalExpertise >= 6 && totalExpertise <= 10) {
        this->rank = 2;
    } else {
        this->rank = 3;
    }
}

void Robot::addSample(Sample sample) {
	this->sample.push_back(sample);

	if(sample.getHealth() < 0) {
		this->totalUndSample++;
	} else {
		this->totalDiagSample++;
	}
}

void Robot::makeAction(vector<Project> project, map<char, int> available,
    vector<Sample> *sample) {

	for(int i = 0; i < sample->size(); i++) {
		sample->at(i).show();
	}

	if(this->eta > 0) {
        cout << endl;
		return;
	}

	switch(this->target) {
		case SAMPLES:
			samplesState(project, sample);
			break;
		case DIAGNOSIS:
			diagnosisState(project, available, sample);
			break;
		case MOLECULES:
			moleculesState(project, available, sample);
			break;
		case LABORATORY:
			laboratoryState(project, available);
			break;
		default:
			cout << "GOTO SAMPLES" << endl;
			break;
	}
}

void Robot::samplesState(vector<Project> project, vector<Sample> *sample) {
	if(this->sample.size() < SAMPLE_LIMIT) {
		cout << "CONNECT " << this->rank << endl;

		return;
	}

	cout << "GOTO DIAGNOSIS" << endl;
}

void Robot::diagnosisState(vector<Project> project, map<char, int> available,
    vector<Sample> *sample) {
    
	/* must have a sample somewhere, if not go to samples state get one */
	if(sample->size() == 0) {
		cout << "GOTO SAMPLES" << endl;
		return;
	}

	/* upload an undiagnosed sample */
	for(auto &i : this->sample) {
		if(i.getHealth() < 0) {
			cout << "CONNECT " << i.getId() << endl;
			return;
		}
	}

    /* download a diagnosed sample */
	if(this->sample.size() < SAMPLE_LIMIT) {
		for(auto &s : *sample) {
			if(s.getCarriedBy() == CARRY_CLOUD &&
                canCompleteSample(available, s)) {
                
				cout << "CONNECT " << s.getId() << endl;
				return;
			}
		}
	}

    /* upload a diagnosed sample */
	if(this->totalDiagSample > 0 ) {
		for(auto &s : this->sample) {
            /* can't take enough molecules for this sample, discard */
			if(s.getHealth() >= 0 && !canCompleteSample(available, s)) {
				cout << "CONNECT " << s.getId() << endl;
				return;
			}
		}
	}

    /* must be carrying at least 2 sample, or go take more samples */
    if(this->sample.size() >= 2) {
        cout << "GOTO MOLECULES" << endl;
    } else {
        cout << "GOTO SAMPLES" << endl;
    }
}

void Robot::moleculesState(vector<Project> project, map<char, int> available,
    vector<Sample> *sample) {

	if(this->totalMolecule == MOLECULE_LIMIT) {
		cout << "GOTO LABORATORY" << endl;
		return;
	}

	char molecule = this->selectedBestMolecule(project, available);

	if(molecule > 0) {
		cout << "CONNECT " << molecule << endl;
	} else {
		cout << "GOTO LABORATORY" << endl;
	}
}

void Robot::laboratoryState(vector<Project> project, map<char, int> available) {
	for(int i = 0; i < this->sample.size(); i++) {
		bool completed = true;
		
		/* check if can produce the medicine */
		for(const auto &m : this->sample.at(i).getCost()) {
            /* required molelucules for sample minus the expertise */
            int need = m.second - this->expertise.at(m.first);

			if(need > this->storage.at(m.first)) {
				completed = false;
				break;
			}
		}

		if(completed) {
			cout << "CONNECT " << this->sample.at(i).getId() << endl;
			return;
		}
	}

    /* check if the robot can complete at least one of his samples */
    bool complete = false;

    //for(int i = 0; i < this->sample.size(); i++) {
    for(auto &s : this->sample) {
        if(this->canCompleteSample(available, s)) {
            complete = true;
            break;
        }
    }

	if(this->totalDiagSample >= 1 && complete) {
		cout << "GOTO MOLECULES" << endl;
	} else {
		cout << "GOTO SAMPLES" << endl;
	}
}

bool Robot::canCompleteSample(map<char, int> available, Sample s) {
    /* check if it is diagnosed */
    if(s.getHealth() < 0) {
        return false;
    }

    /* check if can get each price */
    for(const auto &m : s.getCost()) {
        int need = m.second - this->storage.at(m.first)
            - this->expertise.at(m.first);
        
        if(need > available.at(m.first)) {
            return false;
        }
    }

    return true;
}

char Robot::selectedBestMolecule(vector<Project> project,
	map<char, int> available) {
	
	sort(this->sample.begin(), this->sample.end(), cmpSample);

	map<char, int> storageSave = this->storage;
	char bestMolecule = 0;
	int bestValue = 999;

	/* select the most needed molecule for the a sample that can be completed */
	for(auto &s : this->sample) {
		if(!this->canCompleteSample(available, s)) {
			continue;
		}

		for(const auto &m : s.getCost()) {
            /* required molelucules for sample minus the expertise */
            int need = m.second - this->expertise.at(m.first);
			int have = this->storage.at(m.first);
			int spare = available.at(m.first);

            /* check if need more molecules for this sample,
                considering how much is available and expertise */
			if(have < need && spare > 0) {
				if(spare - need < bestValue) {
					bestValue = spare - need;
					bestMolecule = m.first;
				}
			} else {
				/* save molecules for this sample */
				this->storage.at(m.first) -= m.second;
			}
		}
	}

	if(bestMolecule > 0) {
		return bestMolecule;
	}

	this->storage = storageSave;

	/* select the most needed molecule, completing or not the sample */
	for(auto &s : this->sample) {
		for(const auto &m : s.getCost()) {
            /* required molelucules for sample minus the expertise */
            int need = m.second - this->expertise.at(m.first);
			int have = this->storage.at(m.first);
			int spare = available.at(m.first);

            /* check if need more molecules for this sample,
                considering how much is available and expertise */
			if(have < need && spare > 0) {
				if(spare - need < bestValue) {
					bestValue = spare - need;
					bestMolecule = m.first;
				}
			} else {
				/* save molecules for this sample */
				this->storage.at(m.first) -= m.second;
			}
		}
	}

	return bestMolecule;
}

/*******************************************************************************
	Sample
*******************************************************************************/

void Sample::readUpdate() {
	cin >> this->id >> this->carriedBy >> this->rank >> this->gain >>
		this->health >> this->cost['A'] >> this->cost['B'] >>
		this->cost['C'] >> this->cost['D'] >> this->cost['E']; cin.ignore();

	this->totalCost = this->cost['A'] + this->cost['B'] + this->cost['C'] +
		this->cost['D'] + this->cost['E'];
}

void Sample::show() {
	cerr << "(id" << this->id << " , c" << this->carriedBy << ") r" <<
		this->rank << ", g" << this->gain << ", h" << this->health << " = a" <<
		this->cost['A'] << ", b" << this->cost['B'] << ", c" <<
		this->cost['C'] << ", d" << this->cost['D'] << ", e" <<
		this->cost['E'] << endl;
}

/*******************************************************************************
	Project
*******************************************************************************/

void Project::readUpdate() {
    cin >> this->cost['A'] >> this->cost['B'] >> this->cost['C'] >>
        this->cost['D'] >> this->cost['E']; cin.ignore();
}

void Project::show() {
	cerr << "Project cost: a" <<
        this->cost['A'] << ", b" << this->cost['B'] << ", c" <<
		this->cost['C'] << ", d" << this->cost['D'] << ", e" <<
		this->cost['E'] << endl;
}
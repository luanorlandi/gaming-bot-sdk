#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

enum Morse {
    dot,
    dash,
    root
};

class Trie {
        Trie *pred;
        Trie *childDot;
        Trie *childDash;
        Morse morse;
        long combinations;
    public:
        Trie(Trie*, Morse);
        void newChild(Morse);
        bool hasChild(Morse);
        void newWord(string);
        long calculateCombinations(string);
        
        void showWords(string);
        void showCombinations(string word);
};

string stringToMorse(string);
Morse getMorse(char);

int main() {
    string message;
    int N;
    
    cin >> message; cin.ignore();
    
    /* read dictionary */
    Trie *trieRoot = new Trie(NULL, root);
    
    cin >> N; cin.ignore();
    
    for(int i = 0; i < N; i++) {
        string W;
        cin >> W; cin.ignore();
        
        W = stringToMorse(W);
        trieRoot->newWord(W);
    }
    
trieRoot->showCombinations("");

const clock_t begin_time = clock();
    long c = trieRoot->calculateCombinations(message);
cerr << "Time: " << float(clock() - begin_time) / CLOCKS_PER_SEC << endl;

    cerr << "Combinations: " << c << endl;
    cout << c << endl;
    
    //trieRoot->showCombinations("");
    
    delete trieRoot;

    return 0;
}

Trie::Trie(Trie *pred, Morse morse) {
    this->pred = pred;
    this->childDot = NULL;
    this->childDash = NULL;
    this->morse = morse;
    this->combinations = 0;    /* no combinations found yet */
}

/* allocate a new child for a dot or dash of current node,
it will already be assigned in the attribute */
void Trie::newChild(Morse morse) {
    Trie *child = new Trie(this, morse);
    
    switch(morse) {
        case dot: {
            this->childDot = child;
            break;
        } case dash: {
            this->childDash = child;
            break;
        } default:
            cerr << "Unknown morse character (" << morse << ")" << endl;
            exit(1);
    }
}

/* check if the node has a dot or dash child */
bool Trie::hasChild(Morse morse) {
    switch(morse) {
        case dot: {
            return this->childDot != NULL;
            break;
        } case dash: {
            return this->childDash != NULL;
            break;
        } default:
            cerr << "Unknown morse character (" << morse << ")" << endl;
            exit(1);
    }
}

/* search in the trie for the word, creating new childs to achieve it */
void Trie::newWord(string word) {
    Trie *current = this;
    
    for(int i = 0; word[i] != '\0'; i++) {
        Morse morse = getMorse(word[i]);
        
        if(!current->hasChild(morse)) {
            current->newChild(morse);
        }
        
        switch(morse) {
            case dot: {
                current = current->childDot;
                break;
            } case dash: {
                current = current->childDash;
                break;
            } default:
                cerr << "Unknown morse character (" << morse << ")" << endl;
                exit(1);
        }
    }
    
    /* increase combinations */
    current->combinations++;
}

long Trie::calculateCombinations(string message) {
    Trie *current = this;
    
//int n = 40;
//if(message.length() < n) {cerr << message;} else {cerr << message.substr(0, n);}
//cerr << " > start" << endl;

    /* check if already have the message in the trie */
    for(int i = 0; message[i] != '\0'; i++) {
        Morse morse = getMorse(message[i]);
        
        if(current->hasChild(morse)) {
            switch(morse) {
                case dot: {
                    current = current->childDot;
                    break;
                } case dash: {
                    current = current->childDash;
                    break;
                } default:
                    cerr << "Unknown morse character (" << morse << ")" << endl;
                    exit(1);
            }
            
            if(message[i+1] == '\0') {
                /* reach the end of the message, the trie has the message */
//if(message.length() < n) {cerr << message;} else {cerr << message.substr(0, n);}
//cerr << " > trie has the message, return " << current->combinations << endl << endl;
                return current->combinations;
            }
        } else {
            if(current->combinations == 0) {
                /* invalid message, the dictionary doesn't have a word for this */
//if(message.length() < n) {cerr << message;} else {cerr << message.substr(0, n);}
//cerr << " > invalid, return 0" << endl << endl;
                return 0;
            }
            
            break;
        }
    }
    
    /* calculate all possibilities, starting from the smallest possible word */
    current = this;
    long combinations = 0;
    
    for(int i = 0; message[i] != '\0'; i++) {
        Morse morse = getMorse(message[i]);
        
        if(current->combinations > 0) {
            /* found a possible word */
//if(message.length() < n) {cerr << message;} else {cerr << message.substr(0, n);}
//cerr << " > found possible word(";
//string newWord = message.substr(0, i);
//if(newWord.length() < n) {cerr << newWord;} else {cerr << newWord.substr(0, n);}
//cerr << "), combination += " << current->combinations << " * " << endl;

            combinations += current->combinations *
                calculateCombinations(message.substr(i, message.length() - i));

//if(message.length() < n) {cerr << message;} else {cerr << message.substr(0, n);}
//cerr << " > word calculated, combination = " << combinations << endl;
        }
        
        if(current->hasChild(morse)) {
            switch(morse) {
                case dot: {
                    current = current->childDot;
                    break;
                } case dash: {
                    current = current->childDash;
                    break;
                } default:
                    cerr << "Unknown morse character (" << morse << ")" << endl;
                    exit(1);
            }
        } else {
//if(message.length() < n) {cerr << message;} else {cerr << message.substr(0, 10);}
//cerr << " > break" << endl;
            break;
        }
    }
    
//if(message.length() < n) {cerr << message;} else {cerr << message.substr(0, n);}
//cerr << " > finished calculations, return " << combinations << endl << endl;

    /* memoize the calculation */
    current = this;
    
    for(int i = 0; message[i] != '\0'; i++) {
        Morse morse = getMorse(message[i]);
        
        if(!current->hasChild(morse)) {
            current->newChild(morse);
        }
        
        switch(morse) {
            case dot: {
                current = current->childDot;
                break;
            } case dash: {
                current = current->childDash;
                break;
            } default:
                cerr << "Unknown morse character (" << morse << ")" << endl;
                exit(1);
        }
        
        if(message[i+1] == '\0') {
            /* reach the end of the message, the trie has the message */
            current->combinations = combinations;
    	}
    }

    return combinations;
}

/* show the words in the trie, must pass a empty string
the words to be show will only be the longest, no initial substring */
void Trie::showWords(string word) {
    bool childDot = this->hasChild(dot);
    bool childDash = this->hasChild(dash);
    string newWord;
    
    if(childDot) {
        newWord = word.substr(0, word.length());
        newWord += '.';
        this->childDot->showWords(newWord);
    }
    
    if(childDash) {
        newWord = word.substr(0, word.length());
        newWord += '-';
        this->childDash->showWords(newWord);
    }
    
    if(!(childDot or childDash)) {
        cerr << word << endl;
    }
}

/* show the words in the trie with the combinations, must pass a empty string
all known words in the dictionary will be show */
void Trie::showCombinations(string word) {
    bool childDot = this->hasChild(dot);
    bool childDash = this->hasChild(dash);
    string newWord;
    
    if(childDot) {
        newWord = word.substr(0, word.length());
        newWord += '.';
        this->childDot->showCombinations(newWord);
    }
    
    if(childDash) {
        newWord = word.substr(0, word.length());
        newWord += '-';
        this->childDash->showCombinations(newWord);
    }
    
    if(this->combinations > 0) {
        cerr << this->combinations << " = " << word << endl;
    }
}

string stringToMorse(string word) {
    string morse = "";
    
    for(string::iterator it = word.begin(); it != word.end(); it++) {
        switch(*it) {
            case 'A': morse += ".-"; break;
            case 'B': morse += "-..."; break;
            case 'C': morse += "-.-."; break;
            case 'D': morse += "-.."; break;
            case 'E': morse += "."; break;
            case 'F': morse += "..-."; break;
            case 'G': morse += "--."; break;
            case 'H': morse += "...."; break;
            case 'I': morse += ".."; break;
            case 'J': morse += ".---"; break;
            case 'K': morse += "-.-"; break;
            case 'L': morse += ".-.."; break;
            case 'M': morse += "--"; break;
            case 'N': morse += "-."; break;
            case 'O': morse += "---"; break;
            case 'P': morse += ".--."; break;
            case 'Q': morse += "--.-"; break;
            case 'R': morse += ".-."; break;
            case 'S': morse += "..."; break;
            case 'T': morse += "-"; break;
            case 'U': morse += "..-"; break;
            case 'V': morse += "...-"; break;
            case 'W': morse += ".--"; break;
            case 'X': morse += "-..-"; break;
            case 'Y': morse += "-.--"; break;
            case 'Z': morse += "--.."; break;
            default: cerr << "unknown letter" << endl;
        }
    }
    
    return morse;
}

Morse getMorse(char letter) {
    switch(letter) {
        case '.':
            return dot;
        case '-':
            return dash;
        default:
            cerr << "Unknown morse character (" << letter << ")" << endl;
            exit(1);
    }
}
#include <bits/stdc++.h>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <utility>
#include <fstream>

using namespace std;

set<int> epsilonClosure(const set<int>& states, const multimap<pair<int, char>, set<int>>& nfaTransitions, char epsilon) {
    set<int> closure;
    queue<int> stateQueue;
    for (int state : states) {
        closure.insert(state);
        stateQueue.push(state);
    }

    while (!stateQueue.empty()) {
        int currentState = stateQueue.front();
        stateQueue.pop();
        auto transitions = nfaTransitions.equal_range({currentState, epsilon});
        for (auto it = transitions.first; it != transitions.second; ++it) {
            const set<int>& nextStates = it->second;
            for (int nextState : nextStates) {
                if (closure.insert(nextState).second) {
                    stateQueue.push(nextState);
                }
            }
        }
    }

    return closure;
}

void transformNFAtoDFA(const multimap<pair<int, char>, set<int>>& nfaTransitions, 
                       const set<char>& alphabet, char epsilon, 
                       const set<int>& nfaAcceptStates, int n) {
    ofstream dotFile("nfa.dot");
    
    dotFile << "digraph DFA {" << endl;
    dotFile << "  rankdir=LR;" << endl;
    dotFile << "  node [shape = circle];" << endl;

    queue<set<int>> statesQueue;
    set<set<int>> dfaStates;
    map<set<int>, map<char, set<int>>> dfaTransitions;
    set<int> initialDFAState = {0};

    statesQueue.push(initialDFAState);
    dfaStates.insert(initialDFAState);

    while (!statesQueue.empty()) {
	    set<int> currentState = statesQueue.front();
	    statesQueue.pop();
	
	 
	    set<int> currentStateClosure = epsilonClosure(currentState, nfaTransitions, epsilon);
	
	    for (char symbol : alphabet) {
	        if (symbol == epsilon) continue;
	
	        set<int> nextState;
	        for (int nfaState : currentStateClosure) {
	            auto transitions = nfaTransitions.equal_range({nfaState, symbol});
	            for (auto it = transitions.first; it != transitions.second; ++it) {
	                nextState.insert(it->second.begin(), it->second.end());
	            }
	        }
	
	
	        set<int> nextStateClosure = epsilonClosure(nextState, nfaTransitions, epsilon);
	
	        // Check if nextStateClosure is not empty before creating a transition
	        if (!nextStateClosure.empty()) {
	            if (dfaStates.find(nextStateClosure) == dfaStates.end()) {
	                statesQueue.push(nextStateClosure);
	                dfaStates.insert(nextStateClosure);
	            }
	
	            dfaTransitions[currentState][symbol] = nextStateClosure;
	        }
	    }
	}


	for (const auto& state : dfaStates) {
	    bool isAcceptState = false;
	    for (int s : state) {
	        if (nfaAcceptStates.find(s) != nfaAcceptStates.end()) {
	            isAcceptState = true;
	            break;
	        }
	    }
	
	    string fromState = "{";
	    for (int s : state) fromState += to_string(s) + " ";
	    fromState += "}";
	

	    if (isAcceptState) {
	        dotFile << "  \"" << fromState << "\" [shape = doublecircle];" << endl;
	    } else {
	        dotFile << "  \"" << fromState << "\";" << endl;
	    }
	
	
	    for (const auto& trans : dfaTransitions[state]) {
	        string toState = "{";
	        for (int s : trans.second) toState += to_string(s) + " ";
	        toState += "}";
	
	        dotFile << "  \"" << fromState << "\" -> \"" << toState 
	                << "\" [label=\"" << trans.first << "\"];" << endl;
	    }
	}

	dotFile << "}" << endl;
	dotFile.close();
	system("dot -Tpng nfa.dot -o nfa.png");
}

int main() {
    multimap<pair<int, char>, set<int>> nfaTransitions = {
        {{0, 'a'}, {1}},
        {{0, 'b'}, {2}},
        {{1, '$'}, {3}},
        {{3, 'a'}, {0}},
        {{3, 'b'}, {2}},
        {{0, '$'}, {2}},
        {{2 , 'a'} , {3}},
        {{2 , 'b'} , {1}},
		
		
    };
    int n = 3;
    set<char> alphabet = {'a', 'b', '$'};
    char epsilon = '$';
    set<int> nfaAcceptStates = {1};

    transformNFAtoDFA(nfaTransitions, alphabet, epsilon, nfaAcceptStates, n);

    return 0;
}

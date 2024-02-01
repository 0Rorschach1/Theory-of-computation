#include <bits/stdc++.h>
#include <utility>
#include <limits>
#include <map>

using namespace std;

bool contains(const vector<int>& vec, int elem) {
    return find(vec.begin(), vec.end(), elem) != vec.end();
}

vector<int> lambdaClosure(int s, const multimap<pair<int, char>, int>& transitions, char lambda) {
    vector<int> closure;
    stack<int> stateStack;
    stateStack.push(s);
    closure.push_back(s);

    while (!stateStack.empty()) {
        int currentState = stateStack.top();
        stateStack.pop();

        auto range = transitions.equal_range(make_pair(currentState, lambda));
        for (auto it = range.first; it != range.second; ++it) {
            if (!contains(closure, it->second)) {
                closure.push_back(it->second);
                stateStack.push(it->second);
            }
        }
    }

    return closure;
}
#include <fstream>

void transformENFAtoNFA(const multimap<pair<int, char>, int>& transitions, 
                        const vector<char>& alphabet, char lambda, int numStates) {
    ofstream dotFile("nfa.dot");
    
    dotFile << "digraph NFA {" << endl;
    dotFile << "  rankdir=LR;" << endl;
    dotFile << "  node [shape = circle];" << endl;

    for (int p = 0; p < numStates; ++p) {
        vector<int> lambdaClosureP = lambdaClosure(p, transitions, lambda);

        for (char symbol : alphabet) {
            if (symbol == lambda) continue;

            vector<int> T;
            for (int q : lambdaClosureP) {
                auto range = transitions.equal_range(make_pair(q, symbol));
                for (auto it = range.first; it != range.second; ++it) {
                    if (!contains(T, it->second)) {
                        T.push_back(it->second);
                    }
                }
            }

            vector<int> K;
            for (int t : T) {
                vector<int> tempClosure = lambdaClosure(t, transitions, lambda);
                for (int k : tempClosure) {
                    if (!contains(K, k)) {
                        K.push_back(k);
                    }
                }
            }

            for (int k : K) {
                dotFile << "  " << p << " -> " << k << " [label=\"" << symbol << "\"];" << endl;
            }
        }
    }

    dotFile << "}" << endl;
    dotFile.close();

    system("dot -Tpng nfa.dot -o nfa.png");
}


int main() {
    char lambda = '$';

    vector<char> alphabet = {'a', 'b', lambda};

    multimap<pair<int, char>, int> transitions = {
        {{0, lambda}, 2},
        {{0, lambda}, 1},
        {{1, 'a'}, 0},
        {{1, 'a'}, 2},
        {{2, 'a'}, 1},
    };

    int maxState = 0;
    for (const auto& transition : transitions) {
        maxState = max(maxState, transition.first.first);
    }
    int numStates = maxState + 1;

    transformENFAtoNFA(transitions, alphabet, lambda, numStates);

    return 0;
}

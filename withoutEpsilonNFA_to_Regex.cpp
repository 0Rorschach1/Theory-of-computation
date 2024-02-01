#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

typedef int State;
typedef pair<State, string> TransitionKey;
typedef multimap<TransitionKey, State> TransitionMap;
typedef set<State> StateSet;

string unionLabels(const string& a, const string& b) {
    if (a.empty()) return b;
    if (b.empty()) return a;
    return a + "|" + b;  
}
void printTransitions(const TransitionMap& transitions) {
    for (const auto& trans : transitions) {
        cout << "Transition from state " << trans.first.first;
        cout << " on '" << trans.first.second << "' to state ";
        cout << trans.second << endl;
    }
}


void eliminateState(TransitionMap& transitions, State state) {
    

  
  string loopLabel = "";
  for (const auto& trans : transitions) {
      if (trans.first.first == state && trans.second == state) {
          loopLabel = "(" + trans.first.second + ")*";
          break;  
      }  
  }  


    for (const auto& inTransition : transitions) {
        if (inTransition.second == state) {
            string string1 = inTransition.first.second;
            for (const auto& outTransition : transitions) {
                if (outTransition.first.first == state && outTransition.second != state) {
                    
                    string string2 = outTransition.first.second;
                    string newLabel = loopLabel.empty() ? string1 + string2 : string1 + loopLabel + string2;

                   
                    State P = inTransition.first.first;
                    State Q = outTransition.second;
                    auto range = transitions.equal_range({P, ""});

                    bool found = false;
                    for (auto it = range.first; it != range.second; ++it) {
                        if (it->first.first == P && it->second == Q) {
                            string existingLabel = it->first.second;
                            string updatedLabel = unionLabels(existingLabel, newLabel);

                            
                            transitions.erase(it);

                            
                            transitions.insert({{P, updatedLabel}, Q});

                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        
                        transitions.insert({{P, newLabel}, Q});
                    }
                }
            }
        }
    }


    auto it = transitions.begin();
    while (it != transitions.end()) {
        if (it->first.first == state || it->second == state) {
            it = transitions.erase(it);
        } else {
            ++it;
        }
    }
     cout << "Transitions after eliminating state " << state << ":" << endl;
    
}

string convertNFAtoRegex(TransitionMap& transitions, State startState, const StateSet& finalStates) {
    StateSet states;

    
    for (const auto& transition : transitions) {
        states.insert(transition.first.first);
        states.insert(transition.second);
    }

    
    for (State state : states) {
        if (state != startState && finalStates.find(state) == finalStates.end()) {
            eliminateState(transitions, state);
        }
    }

    
    string regex = "";
    for (auto& transition : transitions) {
        if (transition.first.first == startState && finalStates.find(transition.second) != finalStates.end()) {
            regex = unionLabels(regex, transition.first.second);
        }
    }
    return regex;
}


int main() {
   
    TransitionMap transitions = {
        {{1, "a"}, 2},
        {{2, "b"}, 2},
        {{2, "c"}, 3},
      
    };
    State startState = 1;
    StateSet finalStates = {3};

    string regex = convertNFAtoRegex(transitions, startState, finalStates);
    cout << "Regular Expression: " << regex << endl;
    printTransitions(transitions) ;

    return 0;
}

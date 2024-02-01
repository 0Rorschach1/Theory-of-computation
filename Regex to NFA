#include <bits/stdc++.h>
#include <fstream>


using namespace std;

bool is_operator(char c) {
    return c == '|' || c == '.' || c == '*';
}

bool is_operand(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int get_precedence(char op) {
    if (op == '|') return 1;
    if (op == '.') return 2;
    if (op == '*') return 3;
    return 0;
}

string regex_to_postfix(const string& regex) {
    string postfix_expr;
    stack<char> operator_stack;

    for (char c : regex) {
        if (is_operand(c)) {
            postfix_expr += c;
        } else if (c == '(') {
            operator_stack.push(c);
        } else if (c == ')') {
            while (!operator_stack.empty() && operator_stack.top() != '(') {
                postfix_expr += operator_stack.top();
                operator_stack.pop();
            }
            if (!operator_stack.empty() && operator_stack.top() == '(') {
                operator_stack.pop();
            }
        } else if (is_operator(c)) {
            while (!operator_stack.empty() && get_precedence(c) <= get_precedence(operator_stack.top())) {
                postfix_expr += operator_stack.top();
                operator_stack.pop();
            }
            operator_stack.push(c);
        }
    }

    while (!operator_stack.empty()) {
        postfix_expr += operator_stack.top();
        operator_stack.pop();
    }

    return postfix_expr;
}

class NFAState {
public:
    int id;
    bool is_final;
    map<char, vector<NFAState*>> transitions;

    NFAState(int id, bool final_state = false) : id(id), is_final(final_state) {}

    void add_transition(char symbol, NFAState* state) {
        transitions[symbol].push_back(state);
    }
};

class NFAFragment {
public:
    NFAState* start_state;
    NFAState* end_state;

    NFAFragment(NFAState* start, NFAState* end) : start_state(start), end_state(end) {}
};

int state_counter = 0;

NFAState* new_state(bool is_final = false) {
    return new NFAState(state_counter++, is_final);
}

NFAFragment create_basic_nfa_fragment(char symbol) {
    auto start_state = new_state();
    auto end_state = new_state(true);
    start_state->add_transition(symbol, end_state);
    return NFAFragment(start_state, end_state);
}

NFAFragment concatenate(NFAFragment &frag1, NFAFragment &frag2) {
    frag1.end_state->is_final = false;
    frag1.end_state->add_transition('$', frag2.start_state);
    return NFAFragment(frag1.start_state, frag2.end_state);
}

NFAFragment union_op(NFAFragment &frag1, NFAFragment &frag2) {
    auto start_state = new_state();
    auto end_state = new_state(true);

    start_state->add_transition('$', frag1.start_state);
    start_state->add_transition('$', frag2.start_state);
    frag1.end_state->is_final = false;
    frag2.end_state->is_final = false;
    frag1.end_state->add_transition('$', end_state);
    frag2.end_state->add_transition('$', end_state);

    return NFAFragment(start_state, end_state);
}

NFAFragment kleene_star(NFAFragment &fragment) {
    auto start_state = new_state();
    auto end_state = new_state(true);

    start_state->add_transition('$', fragment.start_state);
    start_state->add_transition('$', end_state);
    fragment.end_state->is_final = false;
    fragment.end_state->add_transition('$', fragment.start_state);
    fragment.end_state->add_transition('$', end_state);

    return NFAFragment(start_state, end_state);
}

void print_nfa(NFAState* start) {
    set<NFAState*> visited;
    vector<NFAState*> to_visit = {start};

    while (!to_visit.empty()) {
        NFAState* current = to_visit.back();
        to_visit.pop_back();

        if (visited.find(current) != visited.end()) {
            continue;
        }
        visited.insert(current);

        cout << "State " << current->id;
        if (current->is_final) {
            cout << " [shape=doublecircle];";
        } else {
            cout << " [shape=circle];";
        }
        cout << endl;

        for (const auto& transition : current->transitions) {
            for (NFAState* target : transition.second) {
                cout << "State " << current->id;
                cout << " -> " << target->id;
                cout << " [label=\"" << (transition.first != '$' ? transition.first : 'e') << "\"];" << endl;
                if (visited.find(target) == visited.end()) {
                    to_visit.push_back(target);
                }
            }
        }
    }
}

void generate_dot_file(NFAState* start, const string& filename) {
    ofstream dot_file(filename);

    dot_file << "digraph NFA {" << endl;

    set<NFAState*> visited;
    vector<NFAState*> to_visit = {start};

    while (!to_visit.empty()) {
        NFAState* current = to_visit.back();
        to_visit.pop_back();

        if (visited.find(current) != visited.end()) {
            continue;
        }
        visited.insert(current);

        if (current == start) {
            dot_file << "  " << current->id << " [shape=ellipse, color=red, label=start];" << endl;
        } else {
            dot_file << "  " << current->id;
            if (current->is_final) {
                dot_file << " [shape=doublecircle, color=green, label=final];" << endl;
            } else {
                dot_file << " [shape=circle];" << endl;
            }
        }

        for (const auto& transition : current->transitions) {
            for (NFAState* target : transition.second) {
                dot_file << "  " << current->id << " -> " << target->id;
                dot_file << " [label=\"" << (transition.first != '$' ? transition.first : 'e') << "\"];" << endl;
                if (visited.find(target) == visited.end()) {
                    to_visit.push_back(target);
                }
            }
        }
    }

    dot_file << "}" << endl;
    dot_file.close();
    cout << "DOT file generated: " << filename << endl;
}

NFAFragment convert_postfix_to_nfa(const string &postfix) {
    stack<NFAFragment> stack;

    for (char c : postfix) {
        if (isalpha(c)) {
            stack.push(create_basic_nfa_fragment(c));
        } else {
            switch (c) {
                case '.': {
                    NFAFragment frag2 = stack.top(); stack.pop();
                    NFAFragment frag1 = stack.top(); stack.pop();
                    stack.push(concatenate(frag1, frag2));
                    break;
                }
                case '|': {
                    NFAFragment frag2 = stack.top(); stack.pop();
                    NFAFragment frag1 = stack.top(); stack.pop();
                    stack.push(union_op(frag1, frag2));
                    break;
                }
                case '*': {
                    NFAFragment frag = stack.top(); stack.pop();
                    stack.push(kleene_star(frag));
                    break;
                }
                default:
                    cerr << "Invalid character in postfix expression: " << c << endl;
                    exit(1);
            }
        }
    }

    if (stack.size() != 1) {
        cerr << "Invalid postfix expression" << endl;
        exit(1);
    }

    return stack.top();
}

int main() {
    string infix_expr = "(a|b).a*"; 
    string postfix_expr = regex_to_postfix(infix_expr);
    cout << "Infix: " << infix_expr << endl;
    cout << "Postfix: " << postfix_expr << endl;
    NFAFragment nfa = convert_postfix_to_nfa(postfix_expr);
    generate_dot_file(nfa.start_state, "nfa.dot");

    system("dot -Tpng nfa.dot -o nfa.png");

    return 0;
}

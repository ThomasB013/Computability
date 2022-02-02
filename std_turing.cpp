/*
Chapter 8.1 - Exc 1.

*/

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

namespace Turing {
	struct exhausted_tape : std::runtime_error { exhausted_tape() : std::runtime_error{ "Execution fell of the tape." } {} }; //Execution tried to fall off the tape.
	struct exhausted_transitions : std::runtime_error { exhausted_transitions() : std::runtime_error{ "No transitions possible." } {} }; //No transitions possible.


	enum State { q0 = 0, q1, q2, q3, q4, q5, q6, q7, q8, q9 };
	enum Alpha { a, b, c, X, Y, Z, B };
	enum Dir { L = -1, S, R };
	
	std::string to_string(State s) {
		return "q" + std::to_string(static_cast<int>(s));
	}

	Alpha alpha_from_char(char c) {
		switch (c) {
		case 'a': return Alpha::a;
		case 'b': return Alpha::b;
		case 'c': return Alpha::c;
		case 'X': return Alpha::X;
		case 'Y': return Alpha::Y;
		case 'Z': return Alpha::Z;
		case 'B': return Alpha::B;
		}
		throw std::runtime_error{ "alpha_from_char(char c) c not recognized where c = " + c };
	}

	std::string to_string(Alpha a) {
		switch (a) {
		case Alpha::a: return "a";
		case Alpha::b: return "b";
		case Alpha::c: return "c";
		case Alpha::X: return "X";
		case Alpha::Y: return "Y";
		case Alpha::Z: return "Z";
		case Alpha::B: return " ";
		}
        throw std::runtime_error{ "to_string(Alpha a) a not recognized where a = " + std::to_string(static_cast<int>(a)) };
	}

	int index_change(Dir d) {
		return static_cast<int>(d);
	}

	struct Input {
		State from;
		Alpha read;
	};

	struct Output {
		State to;
		Alpha write;
		Dir dir;
	};

	struct Transition {
		Input in;
		Output out;
	};

	bool operator==(Input a, Input b) {
		return a.from == b.from && a.read == b.read;
	}
	 
	struct Tape {
		std::vector<Alpha> vec;

		Alpha get(int i) const {
			if (i < 0)
				throw exhausted_tape{};
			if (i < vec.size())
				return vec[i];
			return Alpha::B;
		}

		void set(int i, Alpha a) {
			if (i < 0)
				throw exhausted_tape{};
			if (i < vec.size())
				vec[i] = a;
			else {
				for (int x = vec.size(); x < i; ++x)
					vec.push_back(Alpha::B);
				vec.push_back(a);
			}
		}
	};

	std::string to_string(const Tape& t) {
		std::string s;
		for (Alpha a : t.vec)
			s.push_back(to_string(a)[0]);
		return s;
	}

	Tape tape_from_string(const std::string& s) {
		Tape t;
		for (int i = 0; i < s.size(); ++i)
			t.vec.push_back(alpha_from_char(s[i]));
		return t;
	}

	Tape operator""_t(const char* str, size_t n) {
		return tape_from_string(std::string(str, n));
	}

	class Turing_Machine {
	public:
		const static int MAX_ITER = 100;
		
        Turing_Machine(std::vector<Transition> transitions_) : transitions(transitions_) { set_start(); }
		
		void execute(Tape tape, bool wait = false) {
			set_start();
			try {
                for (int iter = 0; iter != MAX_ITER; ++iter) {
                    show(tape);
                    if (wait)
                        std::cin.get();
                    exec_transition(tape);
                }
            }
			catch(std::runtime_error e){
                std::cout << e.what() << '\n';
            }
		}

	private:
		void exec_transition(Tape& tape){
            auto x = std::find_if(transitions.begin(), transitions.end(), [&](Transition t) -> bool { return t.in == Input{cur_state, tape.get(pos)}; });   
            if (x == transitions.end())
                throw exhausted_transitions {};
            Output out = x->out; 
            tape.set(pos, out.write);
            cur_state = out.to;
            pos += index_change(out.dir);
        }
        
        void set_start() {
			cur_state = State::q0;
			pos = 0;
		}
		
        void show(const Tape& tape, std::ostream& os = std::cout) {
			os << std::string(tape.vec.size() + 4, '#') << '\n'
				<< to_string(tape) << to_string(Alpha::B) << "...\n"
				<< std::string(pos, '#') << '_' << std::string(tape.vec.size() + 3 - pos, '#') << '\n'
				<< "State: " << to_string(cur_state) << "\n\n";
		}
        
		State cur_state;
		int pos;
		std::vector<Transition> transitions;
	};
}

using namespace Turing;

int main() {
	Turing_Machine M(std::vector<Transition>{
		{{q0, B}, {q1, B, R}},
		{{q1, B}, {q2, B, L}},
		{{q1, a}, {q1, a, R}},
		{{q1, b}, {q1, c, R}},
		{{q1, c}, {q1, c, R}},
		{{q2, a}, {q2, c, L}},
		{{q2, c}, {q2, b, L}}
		});
	
	std::cout << "1a)\n";
	M.execute("Baabca"_t);
	
	std::cout << "\n\n1b)\n";
	M.execute("Bbcbc"_t);
}

/*
Output:

1a)
##########
 aabca ...
_#########
State: q0

##########
 aabca ...
#_########
State: q1

##########
 aabca ...
##_#######
State: q1

##########
 aabca ...
###_######
State: q1

##########
 aacca ...
####_#####
State: q1

##########
 aacca ...
#####_####
State: q1

##########
 aacca ...
######_###
State: q1

###########
 aacca  ...
#####_#####
State: q2

###########
 aaccc  ...
####_######
State: q2

###########
 aacbc  ...
###_#######
State: q2

###########
 aabbc  ...
##_########
State: q2

###########
 acbbc  ...
#_#########
State: q2

###########
 ccbbc  ...
_##########
State: q2

No transitions possible.


1b)
#########
 bcbc ...
_########
State: q0

#########
 bcbc ...
#_#######
State: q1

#########
 ccbc ...
##_######
State: q1

#########
 ccbc ...
###_#####
State: q1

#########
 cccc ...
####_####
State: q1

#########
 cccc ...
#####_###
State: q1

##########
 cccc  ...
####_#####
State: q2

##########
 cccb  ...
###_######
State: q2

##########
 ccbb  ...
##_#######
State: q2

##########
 cbbb  ...
#_########
State: q2

##########
 bbbb  ...
_#########
State: q2

No transitions possible
*/

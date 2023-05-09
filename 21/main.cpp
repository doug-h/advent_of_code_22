#include <algorithm>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <iostream>
#include <vector>
#include <optional>
#include <map>

enum Mode {
	ADD,
	MULTIPLY,
	SUBTRACT,
	DIVIDE,
};
std::map<char, Mode> ops {{'+', ADD}, {'-', SUBTRACT}, {'*', MULTIPLY}, {'/', DIVIDE}};
using my_t  = float;
std::optional<my_t> Operation(std::optional<my_t> a, std::optional<my_t> b, Mode m) {
	if(!a or !b){ return std::nullopt; }
	switch (m) {
	case ADD:
		return a.value() + b.value();
	case SUBTRACT:
		return a.value() - b.value();
	case MULTIPLY:
		return a.value() * b.value();
	case DIVIDE:
		return a.value() / b.value();
	}

	assert(0);
	return 0;
}

struct Node {
	std::string name;
	std::optional<my_t> value;
	Mode m;

	Node* ns[2];

	std::optional<my_t>  Evaluate() {
		if(!value and name!="humn") { value = Operation(ns[0]->Evaluate(), ns[1]->Evaluate(), m); }
		return value;
	}
};

std::map<std::string, Node> Parse(const char* file) {
	std::map<std::string, Node> tree;
	{
		std::ifstream ifs(file);
		while (!ifs.eof() and ifs.good()) {
			std::string name;
			ifs >> name;
			name.pop_back();

			Node n{name, std::nullopt, ADD};
			tree[name] = n;
			//std::cout << name << '\n';

			// Skip rest of line
			for (char c = '\0'; ifs.good() and c != '\n'; ifs.get(c)) {}
		}
	}

	{
		std::ifstream ifs(file);
		while (!ifs.eof() and ifs.good()) {
			std::string name;
			ifs >> name;
			name.pop_back();
			Node& n = tree[name];

			char c;
			ifs.get(c);
			c = ifs.peek();
			if (name == "humn"){

			} else if (c >= '0' and c <= '9') {
				my_t i;
				ifs >> i;
				n.value = i;
			}
			else {
				std::string child_1;
				std::string child_2;
				char op;
				ifs >> child_1 >> op >> child_2;
				n.ns[0] = &tree[child_1];
				n.ns[1] = &tree[child_2];
				n.m = ops[op];
			}


			// Skip rest of line
			for (char c = '\0'; ifs.good() and c != '\n'; ifs.get(c)) {}
		}
	}



	return tree;
}

my_t Backtrace(Node* n, my_t target){
	//if n[0] 'op' none == target, what is n[0]
	if(n->name == "humn") { return target; }
	Node* child1 = (n->ns[0]->value) ? n->ns[0] : n->ns[1];
	Node* child2 = !(n->ns[0]->value) ? n->ns[0] : n->ns[1];

	target = Operation(child1->value.value(), target, Mode((int(n->m) + 2)%4)).value();

	return Backtrace(child2, target);
}
int main() {
	auto tree = Parse("example.txt");

	for (auto [c, v] : tree) {
		std::cout << c << ',' << v.value.value_or(-1) << '\n';
		if (v.ns[0]) {
			std::cout << v.ns[0]->name << ' ' << v.m << ' ' << v.ns[1]->name << '\n';
		}
	}

	auto& root = tree["root"];
	std::cout << root.ns[0]->Evaluate().value_or(-1) << ',' <<  root.ns[1]->Evaluate().value_or(-1) << '\n';;

	my_t target = root.ns[1]->Evaluate().value_or(-1);
	std::cout << Backtrace(&root, target);

	return 0;
}
#include "../utility.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <map>

struct Monkey {
  std::string name;
  std::vector<long long> items;

  long long operation(long long old) {
    long long op1 = (operand_1 == -1) ? old : operand_1;
    long long op2 = (operand_2 == -1) ? old : operand_2;

    return (op == '*') ? op1 * op2 : op1 + op2;
  }

  int test(long long old) {
    return (old % div == 0) ? dest_true : dest_false;
  }


  char op;
  int operand_1;
  int operand_2;

  int div;

  int dest_true;
  int dest_false;

};

std::istream&
operator>>(std::istream& source, Monkey& m) {
  std::getline(source, m.name);
  source.ignore(18);


  int i;
  source >> i;
  m.items.push_back(i);
  while (source.peek() == ',') {
    source.ignore(2);
    source >> i;
    m.items.push_back(i);
  }
  SkipLine(source);

  source.ignore(19);

  if (source.peek() == 'o') {
    m.operand_1 = -1;
    IgnoreTo(source, ' ');
  }
  else {
    source >> m.operand_1;
    source.ignore(1);
  }

  source >> m.op;
  source.ignore(1);

  if (source.peek() == 'o') {
    m.operand_2 = -1;
    source.ignore(4);
  }
  else {
    source >> m.operand_2;
    source.ignore(1);
  }

  source.ignore(21);
  source >> m.div;

  source.ignore(29);
  source >> m.dest_true;
  source.ignore(30);
  source >> m.dest_false;


  return source;
}

int main() {

  std::ifstream ifs("data.txt");

  std::vector<Monkey> monkeys;


  while (not ifs.eof()) {
    Monkey m;
    ifs >> m;
    monkeys.push_back(m);
    SkipLine(ifs);
    SkipLine(ifs);
  }

  std::vector<int> interaction_count (monkeys.size(), 0);

  long mod_base = 1;
  for(auto m : monkeys) {
    mod_base *= m.div;
  }
  std::cout << mod_base << '\n';;

  int round = 1;
  while (round < 10001) {
    for (int i = 0; i < monkeys.size(); ++i) {
      auto& m = monkeys[i];
      for (long long s : m.items) {
        ++interaction_count[i];
        s = m.operation(s);

        s = s % mod_base;
        assert(s >= 0);
        int target_monkey = m.test(s);


        monkeys[target_monkey].items.push_back(s);

      }

      m.items.clear();
    }
    ++round;
  }


  long long max_1 = -1;
  long long max_2 = -1;
  for(long long c : interaction_count) {
    max_2 = std::max(c, max_2);
    long long a = std::max(max_1, max_2);
    long long b = std::min(max_1, max_2);
    max_1 = a;
    max_2 = b;
  }

  std::cout << max_1 << ' ' << max_2 << ' ' << max_1 * max_2 << '\n';

  return 0;
}

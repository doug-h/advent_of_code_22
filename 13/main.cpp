#include "../utility.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <vector>


using Packet = std::string;
using Pair = std::vector<Packet>;



// Returns whether left and right are in correct order
bool Compare(Packet left, Packet right, int idx1_start = 0, int idx2_start = 0) {
  int idx1 = idx1_start; int idx2 = idx2_start;
  /*std::cout << "Left: " << left <<
            "Right: " << right
            << "idx1: " << idx1_start << "idx2: " << idx2_start << '\n';*/

  // We assume that input can always be sorted
  assert( not(idx1_start == left.size() and idx2_start == right.size()) );

  // Left ran out
  if (idx1_start == left.size()) {
    //std::cout << "Left ran out" << '\n';
    //std::cout << "Found result: TRUE." << '\n';
    return true;
  }

  // Right ran out
  else if (idx2_start == right.size()) {
    //std::cout << "Right ran out." << '\n';
    //std::cout << "Found result: FALSE." << '\n';
    return false;
  }

  char c1 = left[idx1];
  char c2 = right[idx2];

  // List has ran out
  if ( (c1 == ']') != (c2 == ']')) {
    //std::cout << "List ran out" << '\n';
    return (c1 == ']');
  }

  bool left_needs_skip = c1 == ']' or c1 == ',';
  bool right_needs_skip = c2 == ']' or  c2 == ',';

  if (left_needs_skip or right_needs_skip) {
    //std::cout << "Skip" << '\n';
    return Compare(left, right, (left_needs_skip) ? idx1 + 1 : idx1, (right_needs_skip) ? idx2 + 1 : idx2);
  }

  // Assumed input was 0-9, NOT TRUE, have to deal with 10s
  bool left_is_int = ( c1 != '[' );
  bool right_is_int = ( c2 != '[' );

  int i_left = c1 - '0';
  if (left[idx1 + 1] == '0') { i_left = 10; }
  int i_right = c2 - '0';
  if (right[idx2 + 1] == '0') { i_right = 10; }

  //std::cout << "Comparing " << i_left << " and " << i_right << '\n';
  if (left_is_int and right_is_int) {
    if (i_left == i_right) {
      //std::cout << "Same so skip" << '\n';
      if (i_left == 10) { ++idx1; }
      if (i_right == 10) { ++idx2; }
      return Compare(left, right, idx1 + 1, idx2 + 1);
    }
    else {
      if (i_left < i_right) {
        //std::cout << "Found result: TRUE." << '\n';
      }
      else {
        //std::cout << "Found result: FALSE." << '\n';
      }
      return (i_left < i_right);
    }
  }
  else if (!left_is_int and !right_is_int) {
    //std::cout << "Both lists so step in" << '\n';
    return Compare(left, right, idx1 + 1, idx2 + 1);
  }
  else {
    //std::cout << "Comparing list and int" << '\n';
    if (left_is_int) {

      std::string s1(left, 0, idx1);
      if (i_left == 10) { ++idx1; }
      std::string s2(left, idx1 + 1);
      if (i_left == 10) { --idx1; }
      return Compare(s1 + "[" + std::to_string(i_left) + "]" + s2, right, idx1 + 1, idx2 + 1);
    }
    else if (right_is_int) {

      std::string s1(right, 0, idx2);
      if (i_right == 10) { ++idx2; }
      std::string s2(right, idx2 + 1);
      if (i_right == 10) { --idx2; }
      return Compare(left, s1 + "[" + std::to_string(i_right) + "]" + s2, idx1 + 1, idx2 + 1);
    }
  }

  assert(0);
  return false;


}

int main() {
  std::ifstream ifs("data.txt");


  std::vector<Pair> pairs;
  while (not ifs.eof()) {
    Packet p1;
    std::getline(ifs, p1);
    Packet p2;
    std::getline(ifs, p2);
    Pair ps = {p1, p2};
    pairs.push_back(ps);

    SkipLine(ifs);
  }

  int ind_count = 0;

  Packet div_1 = "[[2]]";
  Packet div_2 = "[[6]]";
  int smaller_than_div1 = 1;
  int smaller_than_div2 = 2;
  for (int p = 0; p < pairs.size(); ++p) {
    //std::cout << pairs[p][0] << ',' << pairs[p][1] << '\n';
    //std::cout << p << '\n';
    ////std::cout << p << ":" << Compare(pairs[p][0], pairs[p][1]) << '\n';
    if (Compare(pairs[p][0], pairs[p][1])) { ind_count += (p + 1); }
    for (auto i : {0, 1}) {
      if (Compare(pairs[p][i], div_1)) { ++smaller_than_div1; ++smaller_than_div2;}
      else if (Compare(pairs[p][i], div_2)) {++smaller_than_div2; }
    }
  }
  std::cout << ind_count << '\n';
  std::cout << smaller_than_div1 << '\n';
  std::cout << smaller_than_div2 << '\n';
  return 0;
}

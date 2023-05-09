#include "../utility.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <map>


struct File {
  File(size_t s) : size(s) {}
  size_t size = 0;

  virtual void CalcSize() {};
};

std::vector<File*> directory_list{};

struct Directory : File {
  Directory(Directory* parent) : File(0) {
    contents[".."] = static_cast<File*>(parent);
    contents["."] = static_cast<File*>(this);
    directory_list.push_back(this);
  }

  virtual void CalcSize() {
    for (const auto &[k, v] : contents) {
      if (k == "." or k == "..") { continue; }
      v->CalcSize();
      size += v->size;
    }
  }



  File* AddFile(std::string name, size_t size) {
    File* f = new File(size);
    contents[name] = f;

    return f;
  }
  File* AddDirectory(std::string name) {
    Directory* d = new Directory(this);
    contents[name] = static_cast<File*>(d);

    return d;
  }

  std::map<std::string, File*> contents = {};
};



int main() {
  Directory root(nullptr);
  root.AddDirectory("/");

  Directory* current_dir = &root;

  std::ifstream ifs("data.txt");
  if (!ifs) { assert(0); }

  while (not ifs.eof()) {
    std::string s;
    std::getline(ifs, s);
    std::istringstream iss(s);

    // Command
    if (s[0] == '$') {
      // Change directory
      if (s[2] == 'c') {
        std::string target (std::next(s.begin(), 5), s.end());
        current_dir = static_cast<Directory*>(current_dir->contents[target]);

        std::cout << "cd to " << target << '(' << current_dir << ')' << '\n';
      }
      // List
      else {
        // ls line can be discarded
        std::cout << "ls " << '\n';
      }
    }
    // Not command, so must be ls output
    else {
      // Dir
      if (s[0] == 'd') {
        std::string name (std::next(s.begin(), 4), s.end());

        std::cout << "add dir " << name << " to " << current_dir << '\n';
        current_dir->AddDirectory(name);
      }
      // File
      else {
        size_t size;
        std::string name;
        iss >> size >> name;
        std::cout << "add file " << name << " to " << current_dir << '\n';
        current_dir->AddFile(name, size);
      }
    }
  }


  for (const auto &[k, v] : root.contents) {
    std::cout << k << '\n';
  }

  root.CalcSize();
  const size_t filesystem = 70000000;
  const size_t used = root.size;
  const size_t required = 30000000;

  const size_t need_to_free = required - (filesystem - used);

  // Find smallest dir larger than need_to_free

  size_t smallest = filesystem;
  for (auto f : directory_list) {
    auto d = static_cast<Directory*>(f);
    if (d->size >= need_to_free and d->size < smallest) { smallest = d->size; }
  }

  std::cout << smallest;
  return 0;
}

#include <vector>
#include <tuple>
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <ranges>

#include <cassert>

#define IN_RANGE(N) (std::views::iota(0, N))
#define SET_MAX(N) (N = std::numeric_limits<decltype(N)>::max())
#define SET_MIN(N) (N = std::numeric_limits<decltype(N)>::min())


size_t FindNext(const char* s, size_t start=0, char delim=',') {
	size_t idx = start;
	
	for (char c2; c2 = s[idx], c2 and c2 != delim;) { ++idx; }
	return idx;

}

template <typename T, typename... Targs>
void Print(T&& head, Targs&&... body){
	std::cout << std::forward<T>(head);
	((std::cout << ',' << std::forward<Targs>(body)), ... ) << '\n';
}

void IgnoreTo(std::istream& stream, char delim) {
  stream.ignore(std::numeric_limits<std::streamsize>::max(), delim);
}

void SkipLine(std::istream& stream) {
  stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

template <typename T> 
[[nodiscard]] std::vector<T> ParsePerLine(const std::string& file) {
	std::ifstream ifs(file);
	if (!ifs) { assert(0); }

	std::vector<T> rv;
	while (not ifs.eof()) {
		std::string s;
		std::getline(ifs, s);
		std::istringstream iss(s);

		T t;
		iss >> t;
		
		rv.push_back(t);
	}

	return rv;
}

#if 0
/* Constructs an object of type T using
	arguments taken from a string. */
template <typename T, typename... Targs>
[[nodiscard]] T FromString(const std::string& s) {
	std::tuple<Targs...> data;
	std::istringstream iss(s);
	std::apply([&](auto && ... args) { (iss >> ... >> args); }, data);

	return std::make_from_tuple<T>(data);
}


[[nodiscard]] std::vector<std::vector<char>> CharLineInput(const std::string& file)
{
	std::ifstream ifs(file);
	if (!ifs) { assert(0); }

	std::vector<std::vector<char>> rv;
	while (not ifs.eof()) {
		std::string s;
		std::getline(ifs, s);
		std::vector<char> line (s.begin(), s.end());

		rv.push_back(line);
	}

	return rv;
}

template <typename ReturnType, typename... Targs>
[[nodiscard]] std::vector<std::vector<ReturnType>> ChunkInput(const std::string& file) {
	/* Parses input into vectors, seperated by whitespace and splitting on empty lines, e.g.

		1 2
		3

		4

	becomes
	{
		{
			T{1,2},
			T{3}
		},
		{
			T{4}
		}
	}.
	*/

	std::ifstream ifs(file);
	if (!ifs) { assert(0); }

	std::vector<std::vector<ReturnType>> rv;
	while (not ifs.eof()) {
		std::vector<ReturnType> block;

		std::string s;
		while (std::getline(ifs, s) and not s.empty()) {
			block.push_back(FromString<ReturnType, Targs...>(s));
		}
		rv.push_back(block);
	}
	return rv;
}

#endif
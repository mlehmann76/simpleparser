#include <cstring>
#include <iostream>
#include <sstream>
#include <string_view>
#include <tuple>
#include <cassert>

#include "main.h"
#include "simpleparser.h"

void check(const char *key, const char *mnem, KeywordPattern::match_return_type match) {
  auto ret = KeywordPattern{key}.match(mnem);
  std::cout << key << " / " << mnem << " / ";
  //std::cout << ret.equal << ":" << ret.sym << "-" << ret.rest << std::endl;
  assert(ret.equal == match.equal);
  assert(ret.sym == match.sym); 
}

int test() {
  check("*IDN", "*IDN?", {"", '?', 1});
  check("TESTer:HALlo", "Test:Hallo?", {"", '?', 1});
  check("TESTer#:HALlo#", "Test:Hallo?", {"", '?', 1});
  check("TESTer#:HALlo#", "Test2:Hallo3?", {"", '?', 1});
  check("TESTer#:HALlo#", "Test2:Hallo3 1234", {"", ' ', 1});
  check("TESTer#:HALlo#", "Teste2:Hallo3 1234", {"", ':', 0});
  check("TESTer#:HALlo#", "Tester2:Hallo3 1234", {"", ' ', 1});

  KeywordPatternLink<std::string, std::stringstream> link = {
      "TESTer#:HALlo#",
      []() {std::cout << "getter" << std::endl; return "";},
      [](std::string s) { std::cout << "setter : " << s << std::endl; }};

  std::stringstream s;

  link.match("Test2:Hallo3?", s);
  link.match("Tester2:Hallo3 1234", s);

  return 0;
}

int main() {
  test();
}
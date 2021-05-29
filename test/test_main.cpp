#include <cassert>
#include <cstring>
#include <iostream>
#include <string_view>
#include <tuple>

#include "main.h"
#include "simpleparser.h"

void check(const char *key, const char *mnem,
           KeywordPattern<std::string_view, 4>::result_type match) {

  auto keyp = KeywordPattern<std::string_view, 4>{key};
  auto ret = keyp.match(mnem);
  int index = keyp.getSize();

  assert(keyp.result(index).equal == match.equal);
  assert(keyp.result(index > 0 ? index-1 : index).sym == match.sym);
}

int test() {
  check("*IDN", "*IDN?", {"", '?', 1});
  check("TESTer:HALlo", "Test:Hallo?", {"", '?', 1});
  check("TESTer#:HALlo#", "Test:Hallo?", {"", '?', 1});
  check("TESTer#:HALlo#", "Test2:Hallo3?", {"", '?', 1});
  check("TESTer#:HALlo#", "Test2:Hallo3 1234", {"", ' ', 1});
  check("TESTer#:HALlo#", "Teste2:Hallo3 1234", {"", ':', 0});
  check("TESTer#:HALlo#", "Tester2:Hallo3 1234", {"", ' ', 1});

  KeywordPatternLink<std::string_view, 4> link = {
      "TESTer#:HALlo#",
      []() {
        std::cout << "getter" << std::endl;
        return "";
      },
      [](std::string_view s) { std::cout << "setter : " << s << std::endl; }};

  link.match("Test2:Hallo3?");
  link.match("Tester2:Hallo3 1234");

  return 0;
}

int main() { test(); }
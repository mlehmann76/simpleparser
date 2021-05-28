#include <cstring>
#include <iostream>
#include <sstream>
#include <string_view>
#include <tuple>
#include <cassert>

#include "main.h"
#include "simpleparser.h"

int main() {
  KeywordPatternLink<std::string, std::stringstream, 4> link = {
      "TESTer#:HALlo#",
      []() {
        std::cout << "getter" << std::endl;
        return "";
      },
      [](std::string s) { std::cout << "setter : " << s << std::endl; }};

  KeywordPatternLink<std::string, std::stringstream, 4> link2 = {
      "*IDN",
      []() {
        std::cout << "getter" << std::endl;
        return "";
      },
      [](std::string s) { std::cout << "setter : " << s << std::endl; }};

  std::stringstream s;

  link.match("Test2:Hallo3?", s);
  link.match("Tester2:Hallo3 1234", s);
  link2.match("*IDN?", s);
  link2.match("*IDN 1234", s);

  return EXIT_SUCCESS;
}
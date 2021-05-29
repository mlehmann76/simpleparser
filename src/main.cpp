#include <cstring>
#include <iostream>
#include <string_view>
#include <tuple>
#include <cassert>

#include "main.h"
#include "simpleparser.h"

int main() {
  KeywordPatternLink<std::string_view, 4> link = {
      "TESTer#:HALlo#",
      []() {
        std::cout << "getter" << std::endl;
        return "";
      },
      [](std::string_view s) { std::cout << "setter : " << s << std::endl; }};

  KeywordPatternLink<std::string_view, 4> link2 = {
      "*IDN",
      []() {
        std::cout << "getter" << std::endl;
        return "";
      },
      [](std::string_view s) { std::cout << "setter : " << s << std::endl; }};

  link.match("Test2:Hallo3?");
  link.match("Tester2:Hallo3 1234");
  link2.match("*IDN?");
  link2.match("*IDN 1234");

  return EXIT_SUCCESS;
}
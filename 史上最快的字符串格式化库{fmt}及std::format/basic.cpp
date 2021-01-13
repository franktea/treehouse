#include <fmt/core.h>
#include <string>
#include <cassert>
#include <iostream>

using namespace std;

int main() {
    string s1 = fmt::format("this is string {}", 1);
    assert(s1 == "this is string 1");

    string s2 = fmt::format("{} + {} = {}", 1, 2, 3);
    assert(s2 == "1 + 2 = 3");

    string s3 = fmt::format("{1} > {0}, {2} > {0}", 1, 2, 3);
    assert(s3 == "2 > 1, 3 > 1");

    string first_name = "house";
    string second_name = "tree";
    string s4 = fmt::format("hello, {second_name} {first_name}", 
        fmt::arg("second_name", second_name),fmt::arg("first_name", first_name));
    assert(s4 == "hello, tree house");

string s = fmt::format("int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 35);
assert(s == "int: 35;  hex: 23;  oct: 43; bin: 100011");

s = fmt::format("int: {0:d};  hex: {0:#x};  oct: {0:#o};  bin: {0:#b}", 35);
assert(s == "int: 35;  hex: 0x23;  oct: 043;  bin: 0b100011");
}
#include <fmt/color.h>
#include <string>
#include <codecvt>
#include <locale>
#include <iostream>

using namespace std;

size_t DisplayLength(const string& utf8) {
    wstring_convert<codecvt_utf8<char32_t>, char32_t> cvt;
    u32string utf32 = cvt.from_bytes(utf8);
    return utf32.size();
}

int main() {
    string chinese = "你好，世界！";
    string korean = "안녕하세요세계";
    string russian = "Привет, мир";

    fmt::print("┌{0:─^{1}}┬{0:─^{1}}┐\n", "", 20);
    fmt::print("│{0: ^{1}}│{2: ^{3}}│\n", "Hello, world!", 20, chinese, 20-DisplayLength(chinese));
    fmt::print("├{0:─^{1}}┼{0:─^{1}}┤\n", "", 20);
    fmt::print("│{0: ^{1}}│{2: ^{3}}│\n", korean, 20-DisplayLength(korean), russian, 20);
    fmt::print("└{0:─^{1}}┴{0:─^{1}}┘\n", "", 20);
}
#include <fmt/core.h>
#include <string>
#include <cassert>

using namespace std;

int main() {
    string s = fmt::format("{:<30}", "left aligned");
    assert(s == "left aligned                  ");
    s = fmt::format("{:>30}", "right aligned");
    assert(s == "                 right aligned");
    s = fmt::format("{:^30}", "centered");
    assert(s == "           centered           ");
    s = fmt::format("{:*^30}", "centered");
    assert(s == "***********centered***********");
}
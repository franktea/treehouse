#include <vector>
#include <string>
#include <iostream>
#include <cctype> // std::isdigit
#include <variant> // std::variant

using namespace std;

// 数字状态，为了记录数字，需要一个类成员
struct Number { size_t num_begin; };
// 非数字状态，不关心内容，不需要内成员
struct Other {};
// 状态机类型
using State = variant<Number, Other>;

// 为了实现std::visit以lambda为参数定义的辅助类型，详见cpprenference
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

vector<string> FindNumbers(const string& str) {
    vector<string> ret;
    State state = Other {};
    for(size_t i = 0; i < str.length(); ++i) {
        if(isdigit(str[i])) { // 处理数字
            std::visit(overload{
                [&](Other) { state = Number{i}; },
                [](auto){}
            }, state);

            // 字符串以数字结尾时，需要加上这行判断，后面再解释。
            if(i == str.length() - 1) {
                const size_t num_begin = std::get<Number>(state).num_begin;
                ret.emplace_back(str, num_begin, i + 1 - num_begin);
            }
        } else { // 处理非数字
            std::visit(overload{
                [&](Number& num) { 
                    ret.emplace_back(str, num.num_begin, i - num.num_begin); 
                    state = Other {};
                },
                [](auto) {}
            }, state);
        }
    }
    return ret;
}

int main() {
    auto v = FindNumbers("abc123FGX#@AD5LKYO0936sda*xc342s&");
    for(string& s: v) {
        cout<<s<<"\n";
    }
}
#include <vector>
#include <string>
#include <iostream>
#include <cctype> // std::isdigit

using namespace std;

// 找出连续的数字，通过一个vector返回，题目并不要求转换成int类型，直接用string即可。
vector<string> FindNumbers(const string& str) {
    vector<string> ret;
    // 定义状态及初始状态
    enum State {OTHER, NUMBER} state = OTHER;
    size_t num_begin; // 当前解析到的连续数字的起始位置
    // 逐个遍历每个字符
    for(size_t i = 0; i < str.length(); ++i) {
        if(isdigit(str[i])) { // 处理数字
            switch(state) {
            case OTHER:
                state = NUMBER;
                num_begin = i;
                break;
            case NUMBER:
                // 什么也不用做
                break;
            }

            // 字符串以数字结尾时，需要加上这行判断，后面再解释。
            if(i == str.length() - 1) {
                ret.emplace_back(str, num_begin, i + 1 - num_begin);
            }
        } else { // 处理非数字
            switch(state) {
            case OTHER:
                // 什么也不用做
                break;
            case NUMBER:
                state = OTHER;
                ret.emplace_back(str, num_begin, i - num_begin);
                break;
            }
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

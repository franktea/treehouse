#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <map>
#include <vector>
#include <string>
#include <stack>
#include <array>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <QGraphicsLineItem>
#include <QRect>
#include <algorithm>
#include <stdint.h>
#include <QDebug>

struct Point {
    float x;
    float y;
};

struct State {
    Point p;
    float angle;
    int level;
};

struct LineLevel {
    QLineF line;
    int level;
};

class LSystem
{
public:
    LSystem(State s, std::string str, float angle, float length, std::map<char, std::string>&& rules):start_state_(s),
        start_str_(str),
        angle_(angle),
        length_(length),
        rules_(std::move(rules)){}

    std::vector<LineLevel> NthLines(int n, int& max_level) {
        std::string str = start_str_;
        for(int i = 0; i < n; ++i) {
            std::string new_str;
            for(char c: str) {
                //qDebug() << "c: " << c;
                auto it = rules_.find(c);
                if(it == rules_.end()) {
                    new_str.push_back(c);
                } else {
                    new_str.append(it->second);
                }
            }
            str = new_str;
        }

        //qDebug() << "get str:" << str.c_str();

        std::vector<LineLevel> ret;
        std::stack<State> stack;
        max_level = 0;
        State s = start_state_;

        for(char c: str) {
            switch(c) {
            case 'F':
            case 'f':
                {
                    const float x = s.p.x + length_ * std::cos(pi * s.angle / 180);
                    const float y = s.p.y + length_ * std::sin(pi * s.angle / 180);
                    //if(c == 'F') {
                        QLineF l(s.p.x, s.p.y, x, y);
                        ret.push_back({l, s.level});
                    //}
                    s.p.x = x;
                    s.p.y = y;
                    max_level = std::max(max_level, s.level);
                    ++ s.level;
                }
                break;
            case '+':
                s.angle += angle_;
                break;
            case '-':
                s.angle -= angle_;
                break;
            case '[':
                stack.push(s);
                break;
            case ']':
                s = stack.top();
                stack.pop();
                break;
            default:
                break;
            }
        }
        return ret;
    }
private:
    State start_state_;
    std::string start_str_;
    int32_t angle_;
    float length_;
    std::map<char, std::string> rules_;
    const float pi = 3.1415926;
};

#endif // LSYSTEM_H

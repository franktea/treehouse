#ifndef HORSEJUMP_H
#define HORSEJUMP_H

#include <iostream>
#include <array>
#include <bitset>
#include <optional>
#include <vector>
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include "generator.h"

using namespace std;

// 每个点最多可以朝8个方向跳
const int DIRECTIONS = 8;

struct XY {int x; int y;};

// 每个方向的x、y左边的变化
const XY delta[DIRECTIONS] = {
    {1, -2}, {2, -1}, {2, 1}, {1, 2},
    {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}};

template<int WIDTH = 9, int HEIGHT = 10>
class HorseJump
{
private: // 用于其它算法的函数
    inline bool Valid(const XY p) const;
    inline XY Neighbour(const XY p, int direction) const;
    int WayOut(const XY p) const;
private: // 用于分治算法的数据和函数
    // 用左上角和右下角表示一个区域
    struct SmallRect {
        XY p1;
        XY p2;
        bool Contains(XY pos) const {
            return pos.x >= p1.x && pos.x <= p2.x &&
                 pos.y >= p1.y && pos.y <= p2.y;
        }

        int PointCount() const {
            return (p2.x + 1 - p1.x) * (p2.y + 1 - p1.y);
        }
    };
    // 3个小区域
    const SmallRect SmallGrids[3] = {
        {{0, 0}, {5, 4}},
        {{6, 0}, {8, 9}},
        {{0, 5}, {5, 9}}};

    // 起点确定以后，以后每一步所在的格子也就确定，用一个数组记录下来
    vector<const SmallRect*> rect_of_step_;
    int DCWayOut(int depth, const XY pos) const;
public: // 分治算法的公用函数
    // 初始化以pos为起点的格子判断数据
    void DCInitRect(XY pos);
    generator<tuple<int, int, int>> DCStep(int depth, XY pos);
// ==========递归方式，简单明了=========
private:
    void OutPut();
public:
    // 获取以pos为起点的全部路径
    void StepAll(int depth, XY pos);
    void StepAllGreedy(int depth, XY pos); // 贪心算法
public: // 协程方式，绘制动画更方便
    generator<tuple<int, int, int>> CoroStepGreedy(int depth, XY pos);
    generator<tuple<int, int, int>> CoroStep(int depth, XY pos);
    // 绘图的时候需要获取path
    const auto& Path() const { return path_; }
public:
    HorseJump() = default;
private:
    bool done_ = false; // 采用协程时需要用到
    array<XY, WIDTH*HEIGHT> path_ = {};
    array<array<bool, WIDTH>, HEIGHT> visited_ = {};
};

template<int WIDTH, int HEIGHT>
bool HorseJump<WIDTH, HEIGHT>::Valid(const XY p) const
{
    return p.x >=0 && p.x < WIDTH
            && p.y >=0 && p.y < HEIGHT;
}

template<int WIDTH, int HEIGHT>
XY HorseJump<WIDTH, HEIGHT>::Neighbour(const XY p, int direction) const
{
    assert(0 <= direction && direction < DIRECTIONS);
    return {p.x + delta[direction].x, p.y + delta[direction].y};
}

template<int WIDTH, int HEIGHT>
int HorseJump<WIDTH, HEIGHT>::WayOut(const XY p) const
{
    int ret = 0;
    for(int i = 0; i < DIRECTIONS; ++i) {
        const XY n = Neighbour(p, i);
        if(Valid(n) && !visited_[n.y][n.x]) {
            ++ ret;
        }
    }
    return ret;
}

template<int WIDTH, int HEIGHT>
void HorseJump<WIDTH, HEIGHT>::OutPut()
{
    int steps[HEIGHT][WIDTH] = {0};
    for(int i = 0;i < WIDTH*HEIGHT; ++i) {
        const XY& xy = path_[i];
        steps[xy.y][xy.x] = i + 1;
    }
    for(int i = 0; i < HEIGHT; ++i) {
        for(int j = 0; j < WIDTH; ++j) {
            cout.width(2);
            cout<<steps[i][j]<<" ";
        }
        cout<<"\n";
    }
    cout<<"\n";
}

template<int WIDTH, int HEIGHT>
void HorseJump<WIDTH, HEIGHT>::StepAll(int depth, XY pos)
{
    path_[depth] = pos;
    visited_[pos.y][pos.x] = true;

    //cout<<depth<<" : ("<<pos.x<<", "<<pos.y<<")\n";

    if(depth >= WIDTH*HEIGHT- 1) { // 找到了一条完整路径
        OutPut();
        return;
    }

    for(int i = 0; i < DIRECTIONS; ++i) {
        const XY new_pos = Neighbour(pos, i);
        if(!Valid(new_pos) || visited_[new_pos.y][new_pos.x]) {
            continue;
        }

        StepAll(depth + 1, new_pos);
        visited_[new_pos.y][new_pos.x] = false;
    }
}

template<int WIDTH, int HEIGHT>
void HorseJump<WIDTH, HEIGHT>::StepAllGreedy(int depth, XY pos)
{
    path_[depth] = pos;
    visited_[pos.y][pos.x] = true;

    if(depth >= WIDTH*HEIGHT- 1) { // 找到了一条完整路径
        OutPut();
        return;
    }

    vector<int> dirs; // 所有的方向，放这里面排序
    for(int i = 0; i < DIRECTIONS; ++i) {
        XY neighbour = Neighbour(pos, i);
        if(Valid(neighbour) && !visited_[neighbour.y][neighbour.x]) {
            dirs.push_back(i);
        }
    }
    sort(begin(dirs), end(dirs), [&pos, this](const int lhs, const int rhs) {
        return WayOut(Neighbour(pos, lhs)) <
            WayOut(Neighbour(pos, rhs));
    });

    for(int i: dirs) { // 对8个方向按出口从少到多排序以后再访问
        const XY new_pos = Neighbour(pos, i);
        StepAllGreedy(depth + 1, new_pos);
        visited_[new_pos.y][new_pos.x] = false;
    }
}

template<int WIDTH, int HEIGHT>
generator<tuple<int, int, int> > HorseJump<WIDTH, HEIGHT>::CoroStepGreedy(int depth, XY pos)
{
    path_[depth] = pos;
    visited_[pos.y][pos.x] = true;
    co_yield tuple{depth, pos.x, pos.y};

    if(depth >= WIDTH*HEIGHT - 1) { // 找到了一条完整路径
        done_ = true;
        co_return;
    }

    vector<int> dirs;
    for(int i = 0; i < DIRECTIONS; ++i) {
        XY neighbour = Neighbour(pos, i);
        if(Valid(neighbour) && !visited_[neighbour.y][neighbour.x]) {
            dirs.push_back(i);
        }
    }
    sort(begin(dirs), end(dirs), [&pos, this](const int lhs, const int rhs) {
        return WayOut(Neighbour(pos, lhs)) <
            WayOut(Neighbour(pos, rhs));
    });

    for(int i: dirs) {
        const XY new_pos = Neighbour(pos, i);

        co_yield CoroStepGreedy(depth + 1, new_pos);
        if(done_) {
            co_return;
        }
        visited_[new_pos.y][new_pos.x] = false;
    }
}

template<int WIDTH, int HEIGHT>
generator<tuple<int, int, int> > HorseJump<WIDTH, HEIGHT>::CoroStep(int depth, XY pos)
{
    path_[depth] = pos;
    visited_[pos.y][pos.x] = true;

    co_yield tuple{depth, pos.x, pos.y};

    if(depth >= WIDTH*HEIGHT - 1) { // 找到了一条完整路径
        done_ = true;
        co_return;
    }

    for(int i = 0; i < DIRECTIONS; ++i) {
        const XY new_pos = Neighbour(pos, i);
        if(! Valid(new_pos) || visited_[new_pos.y][new_pos.x]) continue;

        co_yield CoroStep(depth + 1, new_pos);
        if(done_) {
            co_return;
        }
        visited_[new_pos.y][new_pos.x] = false;
    }
}


// --------------------------分治算法----------------------------
template<int WIDTH, int HEIGHT>
void HorseJump<WIDTH, HEIGHT>::DCInitRect(XY pos)
{
    assert(pos.x >=0 && pos.x < WIDTH &&
           pos.y >= 0 && pos.y < HEIGHT);
    const int start_rect = [pos, this]() {
        for(int i = 0; i < 3; ++i) {
            if(SmallGrids[i].Contains(pos)) {
                return i;
            }
        }
        assert(0>1); // 不可能运行到这里来
        return -1;
    }();

    // 以pos所在的小方格为第一个方格，顺时针排列3个方格
    const SmallRect* ReArranged[3];
    for(int i = 0; i < 3; ++i) {
        ReArranged[i] = &SmallGrids[(i+start_rect) % 3];
    }

    // 初始化rect_of_step_
    rect_of_step_.reserve(WIDTH*HEIGHT);

    for(int i = 0; i < 3; ++i) {
        const int pc = ReArranged[i]->PointCount();
        for(int j = 0; j < pc; ++j) {
            rect_of_step_.push_back(ReArranged[i]);
        }
    }
    std::cout<<"size:::::"<<rect_of_step_.size();
    assert(rect_of_step_.size() == MAX_STEPS);
}

template<int WIDTH, int HEIGHT>
int HorseJump<WIDTH, HEIGHT>::DCWayOut(int depth, const XY pos) const
{
    int ret = 0;
    for(int i = 0; i < DIRECTIONS; ++i) {
        const XY n = Neighbour(pos, i);
        if(rect_of_step_[depth+1]->Contains(n) && !visited_[n.y][n.x]) {
            ++ ret;
        }
    }
    return ret;
}

template<int WIDTH, int HEIGHT>
generator<tuple<int, int, int> > HorseJump<WIDTH, HEIGHT>::DCStep(int depth, XY pos)
{
    path_[depth] = pos;
    visited_[pos.y][pos.x] = true;

    co_yield tuple{depth, pos.x, pos.y};

    if(depth >= WIDTH*HEIGHT - 1) { // 找到了一条完整路径
        done_ = true;
        co_return;
    }

    for(int i = 0; i < DIRECTIONS; ++i) {
        const XY new_pos = Neighbour(pos, i);
        if(! rect_of_step_[depth+1]->Contains(new_pos) || visited_[new_pos.y][new_pos.x]) continue;

        co_yield DCStep(depth + 1, new_pos);
        if(done_) {
            co_return;
        }
        visited_[new_pos.y][new_pos.x] = false;
    }
}


#endif // HORSEJUMP_H

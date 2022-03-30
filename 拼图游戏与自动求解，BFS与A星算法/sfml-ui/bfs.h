#pragma once

#include "puzzle_data.h"

template<int W=3, int H=3>
class BFSPuzzle: public PuzzleData<W, H> {
public:
    using PuzzleData<W,H>::PuzzleData; // 继承构造函数
    bool LoadImage(std::string path); // 加载图片并初始化数据
    // 自动求解相关的方法
    void Next();
    void OnClick(sf::Vector2i pos) = delete;
private: // 自动解题时播放动画需要的数据和方法
    generator<IndexArray<W,H>> gen_;
    typename generator<IndexArray<W,H>>::iterator it_;
    generator<IndexArray<W,H>> BFS();
};

template<int W, int H>
bool BFSPuzzle<W, H>::LoadImage(std::string path) {
    if(!PuzzleData<W,H>::LoadImage(path)) {
        std::cout<<"load image error\n";
        return false;
    }

    // 下面两句是自动求解的逻辑
    gen_ = BFS();
    it_ = gen_.begin();

    return true;
}

template<int W, int H>
generator<IndexArray<W,H>> BFSPuzzle<W, H>::BFS() {
    const auto x_delta = std::array{0, 1, 0, -1};
    const auto y_delta = std::array{-1, 0, 1, 0};
    std::set<IndexArray<W,H>> set; // set用来去重
    std::queue<IndexArray<W,H>> queue; // bfs的queue
    
    auto status = this->indexes_; // 初始状态，是一个二位数组
    queue.push(status); 
    set.insert(status); 
    
    while(!queue.empty()) {
        auto status = queue.front(); // 当前状态取出来
        queue.pop();
        co_yield status; // 直接将status yield出来，Next()函数中通过iterator获取

        if(set.size() > 100000) {
            // 作为演示，不展示过多
            break;
        }

        // 查找空白的方块所在的位置
        const auto [y, x] = [&status](){
            for(int y = 0; y < H; ++y) {
                for(int x = 0; x < W; ++x) {
                    if(status[y][x] == W*H-1) {
                        return std::pair<int,int>{y, x};
                    }
                }
            }
            std::exit(-1);
        }();

        // 朝四个方向展开
        for(int i = 0; i < x_delta.size(); ++i) {
            const int x2 = x + x_delta[i];
            const int y2 = y + y_delta[i];

            if(x2 < 0 || x2 >= W || y2 < 0 || y2 >= H) {
                continue;
            }

            auto status2 = status;
            std::swap(status2[y2][x2], status2[y][x]);
            if(set.find(status2) == set.end()) {
                set.insert(status2);
                queue.push(status2);
            }
        }
    }
}

template<int W, int H>
void BFSPuzzle<W, H>::Next() {
    static int steps = 0;
    static int iter_count = 0;

    if(this->Finished()) {
        return;
    }

    if(it_ == gen_.end()) {
        //std::cout<<"end of iterator\n";
        return;
    }

    if(steps >= 1000) {
        auto status = *it_;
        this->indexes_ = status;
        ++it_;
        ++ iter_count;
        std::cout<<"try for the "<<iter_count<<" times\n";
        steps = 0;
    }

    ++steps;
}

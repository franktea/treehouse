#pragma once

#include "puzzle_data.h"

template<int W, int H>
struct AstarNode { // A*算法需保存的状态，出来记录每个位置的二维数组以外，还需要记录F=G+H中的F和G。
    AstarNode(const IndexArray<W,H>& arr, int g) : data_(arr), G_(g) {
        F_ = G_ + H_();
    }
    IndexArray<W,H> data_;
    int F_;
    int G_;
    int H_() {
        int ret = 0;
        for(int y = 0; y < H; ++y) {
            for(int x = 0; x < W; ++x) {
                if(data_[y][x] == W * H - 1) {
                    continue;
                }
                const int y2 = data_[y][x] / W;
                const int x2 = data_[y][x] % W;
                ret += std::abs(x2 - x) + std::abs(y2 - y);
            }
        }
        return ret;
    }

    // 放入优先队列中的比较运算符，F值越小越优先
    bool operator<(const AstarNode& other) const {
        return F_ > other.F_;
    }
};

template<int W=3, int H=3>
class AstarPuzzle: public PuzzleData<W, H> {
public:
    using PuzzleData<W,H>::PuzzleData; // 继承构造函数
    bool LoadImage(std::string path); // 加载图片并初始化数据
    // 自动求解相关的方法
    void Next();
    void OnClick(sf::Vector2i pos) = delete;
private: // F = G + H
private: // 自动解题时播放动画需要的数据和方法
    generator<IndexArray<W,H>> gen_;
    typename generator<IndexArray<W,H>>::iterator it_;
    generator<IndexArray<W,H>> ASTAR();
};

template<int W, int H>
bool AstarPuzzle<W, H>::LoadImage(std::string path) {
    if(!PuzzleData<W,H>::LoadImage(path)) {
        std::cout<<"load image error\n";
        return false;
    }

    // 下面两句是自动求解的逻辑
    gen_ = ASTAR();
    it_ = gen_.begin();

    return true;
}

template<int W, int H>
generator<IndexArray<W,H>> AstarPuzzle<W, H>::ASTAR() {
    const auto x_delta = std::array{0, 1, 0, -1};
    const auto y_delta = std::array{-1, 0, 1, 0};
    std::set<IndexArray<W,H>> set;
    std::priority_queue<AstarNode<W,H>> queue;

    auto status = this->indexes_;
    set.insert(status);
    queue.push(AstarNode<W,H>(status, 1));

    while(!queue.empty()) {
        auto node = queue.top();
        queue.pop();
        auto& status = node.data_;
        co_yield status;

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

        // 展开
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
                queue.push(AstarNode<W,H>(status2, node.G_ + 1));
            }
        }
    }
}

template<int W, int H>
void AstarPuzzle<W, H>::Next() {
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

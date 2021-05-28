#pragma once
#include <array>
#include <iostream>
#include <utility>
#include <random>

template<int WIDTH, int HEIGHT>
class World {
    World(World const&) = delete;
    World& operator=(World const&) = delete;
public:
    World() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, 1);

        for(auto& row: cells_) {
            for(auto& col: row) {
                col = distrib(gen);
            }
        }
    }

    // 更新全部细胞的下一个状态
    void Next() {
        std::array<std::array<bool, WIDTH>, HEIGHT> tmp;
        for(int row = 0; row < HEIGHT; ++row) {
            for(int col = 0; col < WIDTH; ++col) {
                const int nb = LiveNeighbours(col, row);
                if(nb > 3 || nb < 2) {
                    tmp[row][col] = false;
                } else if(nb == 2) {
                    tmp[row][col] = cells_[row][col];
                } else { // nb ==3
                    tmp[row][col] = true;
                }
            }
        }
        cells_ = tmp;
    }

    // 查询某个格子的细胞是否活着
    bool Alive(int x, int y) const {
        return cells_[y][x];
    }

    void OutPut() const {
        for(const auto& row: cells_) {
            for(const auto& col: row) {
                std::cout<<col;
            }
            std::cout<<"\n";
        }
        std::cout<<"====================================\n";
    }
private:
    // 计算八个相邻方向活着的细胞数量
    int LiveNeighbours(int x, int y) const {
        const int deltax[] = { 0,  1, 1, 1, 0, -1, -1, -1};
        const int deltay[] = {-1, -1, 0, 1, 1,  1,  0, -1};
        int ret = 0;
        for(int i = 0; i < 8; ++i) {
            int x2 = x + deltax[i];
            int y2 = y + deltay[i];
            if(x2 >= WIDTH || x2 < 0 || y2 >= HEIGHT || y2 < 0) {
                continue;
            }
            ret += cells_[y2][x2];
        }
        return ret;
    }
private:
    // 每个细胞的状态，只用两种状态，用bool类型
    std::array<std::array<bool, WIDTH>, HEIGHT> cells_ = {};
};

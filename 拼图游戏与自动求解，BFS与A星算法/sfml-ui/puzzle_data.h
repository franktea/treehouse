#pragma once

#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <random>
#include <queue>
#include <tuple>
#include <set>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include "generator.h"

template<int W, int H>
using IndexArray = std::array<std::array<int, W>, H>;

template<int W=3, int H=3>
class PuzzleData {
public:
    PuzzleData(sf::RenderWindow& window): window_(window) {}
    PuzzleData(const PuzzleData&) = delete;

    bool LoadImage(std::string path); // 加载图片并初始化数据
    void Draw(); // 绘制
    void OnClick(sf::Vector2i pos); // 响应鼠标点击事件
protected:
    int piece_width_; // 每个小方块的长和宽
    int piece_height_;
    sf::Sprite sprites_[W * H]; // 每个小方块的图片，对应texture_上面的一个小的矩形区域
    IndexArray<W,H> indexes_; // index_[y][x]表示当前在[y][x]位置的原始方块编号
    sf::RenderWindow& window_; // 绘图的窗口
    sf::Texture texture_; // 加载的整张图片，用这个图片来切分
    void Shuffle(); // 打乱顺序
    bool Finished(); // 判断是否拼好了
};

template<int W, int H>
bool PuzzleData<W, H>::LoadImage(std::string path) {
    if(!texture_.loadFromFile(path)) { // 加载图片
        return false;
    }

    // 计算每个小方块的长和宽
    piece_width_ = texture_.getSize().x / W;
    piece_height_ = texture_.getSize().y / H;
    std::cout<<"pic width="<<piece_width_<<", pic height="<<piece_height_<<"\n";

    // 将图片的最后一块清除掉
    sf::Image image;
    image.create(piece_width_, piece_height_, sf::Color::White);
    texture_.update(image, piece_width_ * (W-1), piece_height_ * (H-1));

    // 将每个小方块的图形在大图中的坐标点读取出来
    for(int y = 0; y < H; ++y) {
        for(int x = 0; x < W; ++x) {
            auto& sp = sprites_[y * W + x];
            sp.setTexture(texture_);
            sp.setTextureRect(sf::IntRect(
                sf::Vector2i(x*piece_width_, y*piece_height_), 
                sf::Vector2i(piece_width_, piece_height_)));
            indexes_[y][x] = W*y +x;
        }
    }

    Shuffle(); // 打乱顺序

    // 窗口resize要像这样：
    window_.create(sf::VideoMode(piece_width_*W, piece_height_*H), "15 puzzle");

    return true;
}

template<int W, int H>
void PuzzleData<W, H>::Draw() {
    for(int y = 0; y < H; ++y) {
        for(int x = 0; x < W; ++x) {
            auto& sp = sprites_[indexes_[y][x]];
            sp.setPosition(sf::Vector2f(x*piece_width_, y*piece_height_));
            window_.draw(sp);
        }
    }
    for(int y = 1; y < H; ++y) { // 画横线
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(0, y * piece_height_)),
            sf::Vertex(sf::Vector2f(piece_width_ * W, y * piece_height_)),
        };
        window_.draw(line, 2, sf::Lines);
    }
    for(int x = 1; x < W; ++x) { // 画竖线
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(x * piece_width_, 0)),
            sf::Vertex(sf::Vector2f(x * piece_width_, H * piece_height_)),
        };
        window_.draw(line, 2, sf::Lines);
    }    
}

template<int W, int H>
bool PuzzleData<W, H>::Finished() {
    for(int y = 0; y < H; ++y) {
        for(int x = 0; x < W; ++x) {
            if(indexes_[y][x] != W*y +x) {
                return false;
            }
        }
    }

    return true;
}

template<int W, int H>
void PuzzleData<W, H>::OnClick(sf::Vector2i pos) {
    const int x = pos.x / piece_width_;
    const int y = pos.y / piece_height_;
    const auto x_delta = std::array{0, 1, 0, -1};
    const auto y_delta = std::array{-1, 0, 1, 0};
    for(int i = 0; i < x_delta.size(); ++i) {
        int x2 = x + x_delta[i];
        int y2 = y + y_delta[i];
        
        if(x2 < 0 || x2 >= W || y2 < 0 || y2 >= H) {
            continue;
        }

        if(indexes_[y2][x2] == W * H - 1) {
            std::swap(indexes_[y][x], indexes_[y2][x2]);
            std::cout<<"swap index with "<<indexes_[y][x]<<" to "<<indexes_[y2][x2]<<"\n";
            break;
        }
    }
}

template<int W, int H>
void PuzzleData<W, H>::Shuffle() {
    auto last_pos = [&]()->std::pair<int, int> {
        for(int y = 0; y < H; ++y) {
            for(int x = 0; x < W; ++x) {
                if(indexes_[y][x] == W*H - 1) {
                    return std::pair<int, int>{y, x};
                }
            }
        }
        std::exit(-1);
        return std::pair<int, int>(-1, -1);
    };
    
    std::random_device rd;
    std::mt19937 g(rd());

    for(int times = 0; times < 20; ++times) {
        auto [y, x] = last_pos();
        const int x_delta[] = {0, 1, 0, -1};
        const int y_delta[] = {-1, 0, 1, 0};
        std::vector<std::pair<int, int>> temp;
        for(int i = 0; i < 4; ++i) {
            const int y2 = y + y_delta[i];
            const int x2 = x + x_delta[i];
            if(x2 < 0 || x2 >= W || y2 < 0 || y2 >= H) {
                continue;
            }
            temp.push_back(std::pair<int, int>{y2, x2});
        }

        std::uniform_int_distribution<int> dist(0, temp.size()-1);
        const int next = dist(g);
        auto [y2, x2] = temp[next];
        std::swap(indexes_[y][x], indexes_[y2][x2]);
    }
}

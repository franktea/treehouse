#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include "generator.h"

class Items;

class Item {
    friend class Items;
public:
    Item() = default;

    Item(size_t index, int height) : index_(index), height_(height) {
        rect_.setSize({10, (float)height});
        rect_.setOrigin(sf::Vector2f(0, height));
        rect_.setPosition(sf::Vector2f(5 + index * 12, 550));
    }

    const sf::RectangleShape& Rect() {
        return rect_;
    }

    int Height() const {
        return height_;
    }

    void Swap(Item& other) {
        std::swap(index_, other.index_);
        std::swap(height_, other.height_);
        auto pos = other.rect_.getPosition();
        other.rect_.setPosition(rect_.getPosition());
        rect_.setPosition(pos);
        std::swap(rect_, other.rect_);
    }
private:
    size_t index_;
    int height_;
    sf::RectangleShape rect_;
};

inline bool operator<(const Item& lhs, const Item& rhs) {
    return lhs.Height() < rhs.Height();
}

struct SwapInfo {
    size_t from_index;
    size_t to_index;
};

class Items {
public:
    explicit Items(size_t count) {
        array_.resize(count);

        std::random_device r;
        std::default_random_engine e1(r());
        std::uniform_int_distribution<int> uniform_dist(1, 500);
        for(size_t i = 0; i < count; ++i) {
            array_[i] = Item(i, uniform_dist(e1));
        }
    }

    sf::RectangleShape& operator[](size_t i) {
        return array_[i].rect_;
    }

    size_t Size() {
        return array_.size();
    }

    generator<SwapInfo> BubbleSort() {
        if(array_.empty()) {
            co_return;
        }

        for (size_t i = 0; i < array_.size() - 1; ++i) {
            for (size_t j = i + 1; j < array_.size(); ++j) {
                array_[i].rect_.setFillColor(sf::Color::Red);
                array_[j].rect_.setFillColor(sf::Color::Blue);               
                co_yield SwapInfo {i, j};
                if (array_[j] < array_[i]) {
                    array_[i].Swap(array_[j]);
                    co_yield SwapInfo {i, j};
                }
                array_[i].rect_.setFillColor(sf::Color::White);
                array_[j].rect_.setFillColor(sf::Color::White);
            }
        }
    }
private:
    std::vector<Item> array_;
};

int main() {
    using namespace std::chrono_literals;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Empty Window");

    sf::Clock clock; // 启动timer

    Items items(66);

    auto gen = items.BubbleSort();
    auto it = gen.begin();

    // 每次循环就是游戏的一帧
    while(window.isOpen()) {
        // 处理事件
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // 绘制内容，分三步：
        // 1、清除窗口内容
        window.clear(sf::Color::Black);

        // 2、绘制全部的对象
        for(size_t i = 0; i < items.Size(); ++i) {
            window.draw(items[i]);
        }

        // 3、显示绘制的内容
        window.display();

        if(it != gen.end()) {
            ++it;
        }

        std::this_thread::sleep_for(5ms);
    }
}

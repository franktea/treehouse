#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <array>
#include <iostream>
#include <string>
#include <assert.h>

// sf::FloatRect
// sf::Vector2f

static const size_t THRESHOLD = 4;
static const int MAX_DEPTH = 7;

class QuadTreeNode {
public:
    explicit QuadTreeNode(const sf::FloatRect& bound) : bound_(bound) {}
public:
    void Add(const sf::FloatRect& rect) {
        Add(0, rect);
    }

    void Draw(sf::RenderWindow& window) {
        { // 边框
            sf::RectangleShape rs;
            rs.setPosition(bound_.getPosition());
            rs.setSize(bound_.getSize());
            rs.setOutlineColor(sf::Color::White);
            rs.setOutlineThickness(1);
            rs.setFillColor(sf::Color::Transparent);
            window.draw(rs);
        }

        for(const auto& r: values_) {
            sf::RectangleShape rs;
            rs.setPosition(r.getPosition());
            rs.setSize(r.getSize());
            rs.setOutlineColor(sf::Color::Red);
            rs.setOutlineThickness(1);
            rs.setFillColor(sf::Color::Transparent);
            window.draw(rs);
        }

        if(! IsLeaf()) {
            for(int i = 0; i < 4; ++i) {
                children_[i]->Draw(window);
            }
        }
    }

    void FindAllIntersections(std::vector<std::pair<sf::FloatRect, sf::FloatRect>>& v) const {
        // 判断当前节点存储的对象彼此之间是否会碰撞
        for(int i = 0; i < values_.size(); ++i) {
            for(int j = 0; j < i; ++j) {
                if(values_[i].intersects(values_[j])) {
                    v.emplace_back(std::make_pair(values_[i], values_[j]));
                }
            }
        }

        if(! IsLeaf()) {
            for(const auto& child: children_) {
                // 判断每个子树与当前节点中存储的每个对象是否会碰撞
                for(const auto& value: values_) {
                    child->FindAllIntersectionWithRect(value, v);
                }
            }

            for(const auto& child: children_) {
                // 递归判断每个子树
                child->FindAllIntersections(v);
            }
        }
    }
private:
    bool IsLeaf() const {
        return !children_[0];
    }

    sf::FloatRect ComputChildBound(int i) {
        const float origin = bound_.top;
        const sf::Vector2f half_size {bound_.width / 2.0f, bound_.height / 2.0f};
        switch(i) {
            case 0: // north west
                return sf::FloatRect(sf::Vector2f{bound_.left,               bound_.top}, half_size);
            case 1: // north east
                return sf::FloatRect(sf::Vector2f{bound_.left + half_size.x, bound_.top}, half_size);
            case 2: // south west
                return sf::FloatRect(sf::Vector2f{bound_.left,               bound_.top + half_size.y}, half_size);
            case 3: // south east
                return sf::FloatRect(sf::Vector2f{bound_.left + half_size.x, bound_.top + half_size.y}, half_size);
            default:
                assert(0 > 1);
                return sf::FloatRect{};
        } 
    }

    int GetQuadrant(const sf::FloatRect& rect) {
        const float center_x = bound_.left + bound_.width / 2.0;
        const float center_y = bound_.top + bound_.height / 2.0;

        // West
        if(rect.left + rect.width < center_x) {
            // North West
            if(rect.top + rect.height < center_y) {
                return 0;
            } else if(rect.top >= center_y) { // South West
                return 2;
            } else {
                return -1;
            }
        } else if(rect.left >= center_x) { // East
            // North East
            if(rect.top + rect.height < center_y) {
                return 1;
            } else if(rect.top >= center_y) { // South East
                return 3;
            } else {
                return -1;
            }
        }

        return -1;
    }

    // 划分成四块，每块创建一个子树
    void Split(const sf::FloatRect& rect) {
        assert(IsLeaf());
        for(int i = 0; i < 4; ++i) {
            children_[i] = std::make_unique<QuadTreeNode>(ComputChildBound(i));
        }

        std::vector<sf::FloatRect> new_values;
        for(const auto& v: values_) {
            int i = GetQuadrant(v);
            if(i != -1) {
                children_[i]->values_.push_back(v);
            } else {
                new_values.push_back(v);
            }
        }
        values_ = std::move(new_values);
    }

    void Add(int depth, const sf::FloatRect& rect) {
        assert(bound_.left <= rect.left);
        assert(bound_.top <= rect.top);
        assert(bound_.left + bound_.width >= rect.left + rect.width);
        assert(bound_.top + bound_.height >= rect.top + rect.height);
        if(IsLeaf()) {
            // 判断是否需要划分成四块
            if(depth >= MAX_DEPTH || values_.size() < THRESHOLD) {
                values_.push_back(rect);
            } else { // 划分
                Split(rect);
                Add(depth, rect);
            }
        } else {
            int i = GetQuadrant(rect);
            if(i != -1) {
                children_[i]->Add(depth + 1, rect);
            } else {
                values_.push_back(rect);
            }
        }
    }

    void FindAllIntersectionWithRect(const sf::FloatRect& rect, 
        std::vector<std::pair<sf::FloatRect, sf::FloatRect>>& v) const {
        for(const auto& other: values_) {
            if(rect.intersects(other)) {
                v.emplace_back(std::make_pair(rect, other));
            }
        }

        if(!IsLeaf()) {
            for(const auto& child: children_) {
                child->FindAllIntersectionWithRect(rect, v);
            }
        }
    }
private:
    std::array<std::unique_ptr<QuadTreeNode>, 4> children_ {};
    std::vector<sf::FloatRect> values_;
    sf::FloatRect bound_;
};
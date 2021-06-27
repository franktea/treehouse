#include <random>
#include "quadtree.h"

const unsigned int WINDOW_W = 2000;
const unsigned int WINDOW_H = 1200;

// 矩形和移动速度
struct Entity {
    sf::FloatRect rect;
    sf::Vector2f velocity;
};

// 根据速度和逝去的时间dt来计算新位置
void Move(std::vector<Entity>& v, float dt) {
    for(auto& entity: v) {
        entity.rect.left += entity.velocity.x * dt;
        entity.rect.top += entity.velocity.y * dt;

        if(entity.rect.left <= 0.0f) {
            entity.velocity.x *= -1.0f;
            entity.rect.left = 0.0f;
        }
        if(entity.rect.left >= 1.0f*WINDOW_W - entity.rect.width) {
            entity.rect.left = 1.0f*WINDOW_W - entity.rect.width;
            entity.velocity.x *= -1.0f;
        }
        if(entity.rect.top <= 0.0f) {
            entity.rect.top = 0.0f;
            entity.velocity.y *= -1.0f;
        }
        if(entity.rect.top >= 1.0f*WINDOW_H - entity.rect.height) {
            entity.rect.top = 1.0f*WINDOW_H - entity.rect.height;
            entity.velocity.y *= -1.0f;
        }
    }
}

// 随机生成N个矩形，以及移动速度
std::vector<Entity> RandomEntities(size_t count) {
    std::vector<Entity> ret;
    ret.reserve(count);

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> random_x(1, WINDOW_W-300);
    std::uniform_int_distribution<int> random_y(1, WINDOW_H-100);
    std::uniform_int_distribution<int> random_width(3, 100);
    std::uniform_int_distribution<int> random_height(3, 100);
    std::uniform_int_distribution<int> random_vx(5, 200);
    std::uniform_int_distribution<int> random_vy(5, 200);

    for(size_t i = 0; i < count; ++i) {
        float x = random_x(e1);
        float y = random_y(e1);
        float w = random_width(e1);
        float h = random_height(e1);
        float vx = random_vx(e1);
        float vy = random_vy(e1);
        ret.emplace_back(Entity{sf::FloatRect(x, y, w, h), sf::Vector2f(vx, vy)});
    }

    return ret;
}

// 用不同的颜色绘制有碰撞的矩形
void DrawIntersections(sf::RenderWindow& window, std::vector<std::pair<sf::FloatRect, sf::FloatRect>>& v) {
    auto f = [&window](const sf::FloatRect& r) {
        sf::RectangleShape rs;
        rs.setPosition(r.getPosition());
        rs.setSize(r.getSize());
        rs.setOutlineColor(sf::Color::Blue);
        rs.setOutlineThickness(2);
        rs.setFillColor(sf::Color::Red);
        window.draw(rs);
    };
    for(auto&& [r1, r2]: v) {
        f(r1);
        f(r2);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "quadtree rectanges");
    
    // 随机创建n个移动的矩形
    std::vector<Entity> enties = RandomEntities(100);

    sf::Clock clock{};
    sf::Time dt{};
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        dt = clock.restart();
        Move(enties, dt.asSeconds()); // 移动位置

        // 用quadtree来查找碰撞的矩形
        QuadTreeNode root{sf::FloatRect(0, 0, window.getSize().x, window.getSize().y)};
        for(const auto& e: enties) {
            root.Add(e.rect);
        }
        std::vector<std::pair<sf::FloatRect, sf::FloatRect>> intersections;
        root.FindAllIntersections(intersections);

        // 重绘所有矩形
        window.clear(sf::Color::Black);
        root.Draw(window);
        DrawIntersections(window, intersections); // 发生碰撞的高亮
        window.display();
    }
}
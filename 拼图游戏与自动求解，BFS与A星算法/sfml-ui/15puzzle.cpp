#include <SFML/Graphics.hpp>
#include <iostream>
#include "puzzle_data.h"

using namespace sf;

int main(int argc, const char* argv[]) {
    RenderWindow window(VideoMode(1080, 720), "15 puzzle");
    window.setFramerateLimit(60);

    std::string file_name = argc > 1 ? argv[1] : "15.png";

    PuzzleData<4,4> pd(window); // 创建一个PuzzleData对象
    bool ret = pd.LoadImage(file_name); // 加载图片并随机打乱
    if(!ret) {
        std::cout<<"can not load image file: "<<file_name<<"\n";
        return 0;
    }

    while (window.isOpen()) { 
        sf::Event event;
        while (window.pollEvent(event)) { // 处理事件
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Button::Left) {
                Vector2i pos = Mouse::getPosition(window);
                pd.OnClick(pos);
            }
        }

        window.clear(sf::Color::Black);
        pd.Draw(); // 绘制到窗口
        window.display();
    }

    return 0;
}
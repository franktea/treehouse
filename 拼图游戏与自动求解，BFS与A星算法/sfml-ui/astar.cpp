#include <SFML/Graphics.hpp>
#include <iostream>
#include "astar.h"

using namespace sf;

int main(int argc, const char* argv[]) {
    RenderWindow window(VideoMode(1080, 720), "15 puzzle");
    window.setFramerateLimit(60);

    std::string file_name = argc > 1 ? argv[1] : "15.png";

    AstarPuzzle<4,4> pd(window);
    bool ret = pd.LoadImage(file_name);
    if(!ret) {
        std::cout<<"can not load image file: "<<file_name<<"\n";
        return 0;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        pd.Draw();
        pd.Next();
        window.display();
    }

    return 0;
}

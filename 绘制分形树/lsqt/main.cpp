#include "lspanel.h"

#include <QApplication>
#include <QGridLayout>
 #include <QWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget w;

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setSpacing(1);

    LsPanel* ls = new LsPanel([](QGraphicsScene& scene) {
        static float angle = 90;
        LSystem ls(State{{0, 0}, 270, 0}, "X", angle, 2.0,
                   std::map<char, std::string>{{'X', "F-[[X]+X]+F[+FX]-X"}, {'F', "FF"}});

        scene.clear();
        int max_level;
        auto v = ls.NthLines(6, max_level);
        for(auto&& [l, level]: v) {
            scene.addLine(l,  QPen(QColor(0, 255.0 * level / max_level, 0)));
        }

        angle -= 5.0;
        if(angle <= 10.0) {
            angle = 90.0;
        }
    }, &w);
    gridLayout->addWidget(ls, 0, 0, 1, 1);

    ls = new LsPanel([](QGraphicsScene& scene){
        static float angle = 0;
        LSystem ls(State{{0, 0}, 270, 0}, "F", angle, 5.4,
                   std::map<char, std::string>{{'F', "FF-[-F+F+F]+[+F-F-F]"}});

        scene.clear();
        int max_level;
        auto v = ls.NthLines(4, max_level);
        for(auto&& [l, level]: v) {
            scene.addLine(l, QPen(QColor(0, 255.0 * level / max_level, 0)));
        }

        angle += 10;
        if(angle >= 100) {
            angle = 0;
        }
    }, &w);
    gridLayout->addWidget(ls, 0, 1, 1, 1);

    ls = new LsPanel([](QGraphicsScene& scene){
        static float angle = 0;
        LSystem ls(State{{0, 0}, 270, 0}, "X", angle, 1.3,
                   std::map<char, std::string>{{'X', "F[+X][-X]FX"}, {'F', "FF"}});

        scene.clear();
        int max_level;
        auto v = ls.NthLines(7, max_level);
        for(auto&& [l, level]: v) {
            scene.addLine(l, QPen(QColor(0, 255.0 * level / max_level, 0)));
        }

        angle += 10;
        if(angle >= 100) {
            angle = 0;
        }
    }, &w);
    gridLayout->addWidget(ls, 1, 0, 1, 1);

    ls = new LsPanel([](QGraphicsScene& scene){
        static float angle = 0;
        LSystem ls(State{{0, 0}, 270, 0}, "F", angle, 5.2,
                   std::map<char, std::string>{{'F', "F[+F]F[-F][F]"}});

        scene.clear();
        int max_level;
        auto v = ls.NthLines(5, max_level);
        for(auto&& [l, level]: v) {
            scene.addLine(l, QPen(QColor(0, 255.0 * level / max_level, 0)));
        }

        angle += 10;
        if(angle >= 100) {
            angle = 0;
        }
    }, &w);
    gridLayout->addWidget(ls, 1, 1, 1, 1);

    w.setLayout(gridLayout);
    w.show();

    return a.exec();
}

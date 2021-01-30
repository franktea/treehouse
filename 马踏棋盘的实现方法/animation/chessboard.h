#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <memory>
#include <QPoint>
#include <QGraphicsLineItem>
#include <QGraphicsPixmapItem>
#include <vector>
#include "horsejump.h"

namespace Ui {
class ChessBoard;
}

class ChessBoard : public QGraphicsView
{
    Q_OBJECT
    static const int WIDTH = 9;
    static const int HEIGHT = 10;
    static const int GRID_SIZE = 100; // 每个格子的边长
public:
    explicit ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard() override;
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
private:
    void CalcPoints();
private slots:
    void timeout();
private:
    Ui::ChessBoard *ui;
    QGraphicsScene scene_;
    QTimer* timer_;
    QPoint points_[HEIGHT][WIDTH];
    QGraphicsLineItem grid_meridian_[10];
    QGraphicsLineItem grid_latitude_[16];
    QGraphicsLineItem grid_slash_[4]; // 米字型的四条斜线
private:
    HorseJump<>* horse_;
    QGraphicsPixmapItem* horse_image_;
    std::vector<QGraphicsLineItem*> pathes_;
    QPoint last_pos_;
    generator<tuple<int, int, int>> gen_;
    generator<tuple<int, int, int>>::iterator it_;
};

#endif // CHESSBOARD_H

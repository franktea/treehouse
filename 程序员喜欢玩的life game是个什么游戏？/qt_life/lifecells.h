#ifndef LIFECELLS_H
#define LIFECELLS_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QTimer>
#include "life.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LifeCells; }
QT_END_NAMESPACE

class LifeCells : public QGraphicsView
{
    Q_OBJECT
    // 每个小矩形（其实是正方形）的宽度与高度
    static const int CELL_SIZE = 20;
    // 矩形的行数
    static const int CELL_ROWS = 30;
    // 矩形的列数
    static const int CELL_COLS = 40;
public:
    LifeCells(QWidget *parent = nullptr);
    ~LifeCells() override;
private slots:
    void timeout();
private:
    QGraphicsScene scene_;
    QTimer* timer_; // 定时器，用来调用world的Next函数
    // 表示每个细胞状态的矩形
    QGraphicsRectItem* world_[CELL_ROWS][CELL_COLS];
    // World对象，保存所有细胞的数据
    World<CELL_COLS, CELL_ROWS> data_;
private:
    Ui::LifeCells *ui;
};
#endif // LIFECELLS_H

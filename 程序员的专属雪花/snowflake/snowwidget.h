#ifndef SNOWWIDGET_H
#define SNOWWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <random>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsEllipseItem>
#include <vector>
#include <QKeyEvent>
#include <optional>
#include "particle.h"

class SnowWidget : public QGraphicsView
{
    Q_OBJECT
public:
    SnowWidget(QWidget *parent = nullptr);
    ~SnowWidget();
private slots:
    void timeout();
protected:
    // 按下R键时，重新生成一个雪花
    void keyPressEvent(QKeyEvent *event) override;
private:
    QGraphicsScene scene_;
    QTimer* timer_; // 用timer_显示绘制过程，每次生成一个点
    std::vector<Particle> points_; // 生成好的点保存在这里
    std::optional<Particle> current_; // 下一个随机开始运动的点的起始位置
    bool finished_ = false; // 是否绘制完整个雪花
};
#endif // SNOWWIDGET_H

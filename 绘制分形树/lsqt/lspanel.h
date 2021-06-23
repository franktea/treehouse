#ifndef LSPANEL_H
#define LSPANEL_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <functional>
#include "lsystem.h"

using OnTimer = std::function<void(QGraphicsScene& scene)>;

class LsPanel : public QGraphicsView
{
    Q_OBJECT

public:
    LsPanel(OnTimer&& f, QWidget *parent = nullptr);
    ~LsPanel();
private:
    QGraphicsScene scene_;
    QTimer timer_;
    OnTimer f_;

};
#endif // LSPANEL_H

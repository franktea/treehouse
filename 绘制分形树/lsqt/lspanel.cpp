#include <QDebug>
#include "lspanel.h"

LsPanel::LsPanel(OnTimer&& f, QWidget *parent)
    : QGraphicsView(parent),
      timer_(this),
      f_(std::move(f))
{
    this->setScene(&scene_);
    qDebug() << "self size: (" << this->width() << ", " << this->height() <<")";

    QObject::connect(&timer_, &QTimer::timeout, [this] {
        this->f_(this->scene_);
    });

    timer_.start(50);
}

LsPanel::~LsPanel()
{
}


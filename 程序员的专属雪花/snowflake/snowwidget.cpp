#include <QtGlobal>
#include "snowwidget.h"

const int WIDTH = 800;
const int HEIGHT = 800;

SnowWidget::SnowWidget(QWidget *parent)
    : QGraphicsView(parent)
{
    this->setScene(&scene_);
    scene_.setParent(this);
    this->resize(WIDTH + 40, HEIGHT);
    this->setSceneRect(QRect(-size().width()/2, -size().height()/2, size().width(), size().height()));
    setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));

    // 初始位置，总是从雪花花瓣的最顶端开始
    current_.emplace(Particle(WIDTH / 2, 0));

    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(timeout()));
    timer_->start(1);
}

SnowWidget::~SnowWidget()
{
}

void SnowWidget::timeout()
{
    if(finished_) return;

    int count = 0;
    while(current_.has_value() && !current_->finished() && !current_->Intersects(points_)) {
        current_->RandMove();
        ++ count;
    }

    if(count == 0) {
        finished_ = true;
        return;
    }

    // 得到旋转后的新坐标
    for(int i = 0; i < 6; ++i) {
        QPointF p = transform().rotate(60.0 * i)
                .map(QPointF(current_->vec.x(), current_->vec.y()));
        scene_.addEllipse(p.x(), p.y(), R, R, QPen(Qt::white), QBrush(Qt::white));
        // 当前点关于y轴的对称点
        p = transform().rotate(60.0 * i)
                .map(QPointF(current_->vec.x(), -current_->vec.y()));
        scene_.addEllipse(p.x(), p.y(), R, R, QPen(Qt::white), QBrush(Qt::white));
    }

    points_.push_back(current_.value());
    current_.emplace(Particle(WIDTH/2, 0));
}

void SnowWidget::keyPressEvent(QKeyEvent *event)
{
    // 重新生成一个雪花
    if(event->key() != Qt::Key_R) return;
    qDebug() << "clicked";
    if(!finished_) return;
    current_.emplace(Particle(WIDTH / 2, 0));
    points_.clear();
    scene_.clear();
    finished_ = false;
}


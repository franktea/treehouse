#include "lifecells.h"
#include "./ui_lifecells.h"

LifeCells::LifeCells(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::LifeCells)
{
    ui->setupUi(this);
    this->setScene(&scene_);

    // 按照矩形的数量计算窗口的大小
    this->resize(CELL_SIZE * CELL_COLS, CELL_SIZE * CELL_ROWS);

    // 创建每个矩形对象
    for(int row = 0; row < CELL_ROWS; ++row) {
        for(int col = 0; col < CELL_COLS; ++col) {
            auto p = new QGraphicsRectItem(
                        CELL_SIZE * col + 1,
                        CELL_SIZE * row + 1,
                        CELL_SIZE - 2,
                        CELL_SIZE - 2);
            world_[row][col] = p;
            scene_.addItem(p);
            p->setBrush(data_.Alive(col, row) ? Qt::black : Qt::white);
        }
    }

    // 创建定时器对象，每隔一定时间更新一次数据
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(timeout()));
    timer_->start(200);
}

LifeCells::~LifeCells()
{
    for(int row = 0; row < CELL_ROWS; ++row) {
        for(int col = 0; col < CELL_COLS; ++col) {
            scene_.removeItem(world_[row][col]);
            delete world_[row][col];
        }
    }

    delete ui;
}

void LifeCells::timeout()
{
    data_.Next();
    for(int row = 0; row < CELL_ROWS; ++row) {
        for(int col = 0; col < CELL_COLS; ++col) {
            world_[row][col]->setBrush(data_.Alive(col, row) ? Qt::black : Qt::white);
        }
    }
}


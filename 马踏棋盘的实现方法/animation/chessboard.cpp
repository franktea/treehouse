#include <QDebug>
#include <QGraphicsRectItem>
#include "chessboard.h"
#include "ui_chessboard.h"

ChessBoard::ChessBoard(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::ChessBoard)
{
    ui->setupUi(this);
    timer_ = new QTimer(this);
    //scene_.setParent(this);
    this->setScene(&scene_);
    this->resize(9*GRID_SIZE + 20, 10 * GRID_SIZE + 20);
    this->setScene(&scene_);
    connect(timer_, SIGNAL(timeout()), this, SLOT(timeout()));
    CalcPoints();
    for(QGraphicsLineItem& l: grid_meridian_)
    {
        scene_.addItem(&l);
    }
    for(QGraphicsLineItem& l: grid_latitude_)
    {
        scene_.addItem(&l);
    }
    for(QGraphicsLineItem& l: grid_slash_)
    {
        scene_.addItem(&l);
    }

    // 2021-01-27, 外面加个框框，使得所有的元素一开始就能全部居中显示
    QGraphicsRectItem* rect = new QGraphicsRectItem(points_[0][0].x() - 50, points_[0][0].y() - 50, 900, 1000);
    QPen pen;
    pen.setColor(Qt::white);
    rect->setPen(pen);
    scene_.addItem(rect);

    QImage image(":/img/horse.png");
    //QImage small = image.scaled(50, 50, Qt::KeepAspectRatio);
    horse_image_ = new QGraphicsPixmapItem(QPixmap::fromImage(image));

    XY start_point {3, 2};
    horse_image_->setPos(points_[start_point.y][start_point.x]);
    horse_image_->setOffset(-25, -25);
    scene_.addItem(horse_image_);

    horse_ = new HorseJump<>;
    //gen_ = horse_->CoroStepGreedy(0, start_point);
    //gen_ = horse_->CoroStep(0, start_point);
    horse_->DCInitRect(start_point);
    gen_ = horse_->DCStep(0, start_point);
    it_ = gen_.begin();
}

ChessBoard::~ChessBoard()
{
    delete ui;
}

void ChessBoard::mouseDoubleClickEvent(QMouseEvent *)
{
    qDebug()<<"double clicked";
    timer_->start(0);
}

void ChessBoard::CalcPoints()
{
    for(int y = 0; y < HEIGHT; ++y)
    {
        for(int x = 0; x < WIDTH; ++x)
        {
            int xvalue = 10 + x * GRID_SIZE;
            int yvalue = 10 + y * GRID_SIZE;
            points_[y][x] = QPoint{xvalue, yvalue};
        }
    }

    for(int y = 0; y < 10; ++y)
    {
        grid_meridian_[y].setLine(QLine(points_[y][0], points_[y][8]));
    }

    for(int x = 0; x < 7; ++x)
    {
        grid_latitude_[x].setLine(QLine(points_[0][x+1], points_[4][x+1]));
    }

    for(int x = 7; x < 14; ++x)
    {
        grid_latitude_[x].setLine(QLine(points_[5][x+1-7], points_[9][x+1-7]));
    }
    grid_latitude_[14].setLine(QLine(points_[0][0], points_[9][0]));
    grid_latitude_[15].setLine(QLine(points_[0][8], points_[9][8]));

    grid_slash_[0].setLine(QLine(points_[0][3], points_[2][5]));
    grid_slash_[1].setLine(QLine(points_[0][5], points_[2][3]));
    grid_slash_[2].setLine(QLine(points_[9][3], points_[7][5]));
    grid_slash_[3].setLine(QLine(points_[7][3], points_[9][5]));
}

void ChessBoard::timeout()
{
    if(it_ != gen_.end()) {
       /*auto&& [depth, x, y] = *it_;

        for(QGraphicsLineItem* l: pathes_) {
            scene_.removeItem(l);
            delete l;
        }
        pathes_.clear();
        for(int i = 1; i <= depth; ++i) {
            QPoint p2(horse_->Path()[i].x, horse_->Path()[i].y);
            pathes_.emplace_back(new QGraphicsLineItem(QLine(
                   points_[horse_->Path()[i].y][horse_->Path()[i].x],
                   points_[horse_->Path()[i-1].y][horse_->Path()[i-1].x])));
            QPen pen;
            pen.setColor(Qt::red);
            pen.setWidth(3);
            pathes_.back()->setPen(pen);
            scene_.addItem(pathes_.back());
        }

        horse_image_->setPos(points_[y][x]);*/

        //qDebug()<<depth<<": ("<<x<<", "<<y<<")";
        ++it_;
    } else {
        static bool showed = false;
        if(! showed) {
            showed = true;
            for(int i = 0; i < horse_->Path().size(); ++i) {
                qDebug()<<i+1<<" : ("<<horse_->Path()[i].x<<", "<<horse_->Path()[i].y<<")";
            }

            for(QGraphicsLineItem* l: pathes_) {
                scene_.removeItem(l);
                delete l;
            }
            pathes_.clear();
            for(int i = 1; i < horse_->Path().size(); ++i) {
                QPoint p2(horse_->Path()[i].x, horse_->Path()[i].y);
                pathes_.emplace_back(new QGraphicsLineItem(QLine(
                       points_[horse_->Path()[i].y][horse_->Path()[i].x],
                       points_[horse_->Path()[i-1].y][horse_->Path()[i-1].x])));
                QPen pen;
                pen.setColor(Qt::red);
                pen.setWidth(3);
                pathes_.back()->setPen(pen);
                scene_.addItem(pathes_.back());
            }

            horse_image_->setPos(points_[horse_->Path().back().y][horse_->Path().back().x]);
        }
    }
}

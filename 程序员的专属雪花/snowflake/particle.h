#ifndef PARTICLE_H
#define PARTICLE_H

#include <random>
#include <cmath>
#include <vector>
#include <QVector2D>

inline const float R = 5.0;
inline const float PI = 3.1415926;

struct Particle
{
    QVector2D vec;
    Particle(float radius, float angle);
    // 运动，即更新坐标到附近的随机位置
    void RandMove();

    // 判断是否与其它的任意一个点重合
    bool Intersects(const std::vector<Particle>& v) const;

    // 停止运动的条件
    bool finished() const { return vec.x() < 1.0; }
};

#endif // PARTICLE_H

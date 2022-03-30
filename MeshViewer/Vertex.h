#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include <QColor>

struct Vertex
{
    Vertex(){};
    Vertex(const QVector3D &coords, const QColor &col) :
        x(float(coords.x())),
        y(float(coords.y())),
        z(float(coords.z())),
        r(float(col.redF())),
        g(float(col.greenF())),
        b(float(col.blueF()))
    {}

    float x,y,z;
    float r,g,b;
};

#endif // VERTEX_H

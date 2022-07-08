#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include <QColor>

#include "Halfedge.h"

struct Vertex
{
    Vertex(){};
    /*Vertex(const QVector3D &coords, const QColor &col) :
        x(float(coords.x())),
        y(float(coords.y())),
        z(float(coords.z())),
        r(float(col.redF())),
        g(float(col.greenF())),
        b(float(col.blueF()))
    {}*/
    Vertex(const QVector3D &coords, const QColor &col, const QVector3D &normals = QVector3D(0,0,0)) :
        x(float(coords.x())),
        y(float(coords.y())),
        z(float(coords.z())),
        r(float(col.redF())),
        g(float(col.greenF())),
        b(float(col.blueF())),
        nx(float(normals.x())),
        ny(float(normals.y())),
        nz(float(normals.z()))
    {}

    float x,y,z;
    float r,g,b;
    //頂点法線
    float nx,ny,nz;
};

#endif // VERTEX_H

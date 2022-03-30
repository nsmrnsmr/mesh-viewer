#ifndef MYMESH_H
#define MYMESH_H

#include<QtGui/QOpenGLFunctions>

#include <QColor>
#include <QMatrix4x4>
#include <vector>

#include "Transform3d.h"
#include "Vertex.h"
#include "Face.h"

//メッシュの読み込み：obj形式 off形式
//メッシュの読み込みに自分のライブラリを利用する "input.h"
class MyMesh
{
public:
    MyMesh(const std::string &fileName, const std::string &suffix, QColor meshColor=Qt::white);

    //全ての面の色を一括で変更
    void setColor(QColor c){
        m_colors = std::vector<QColor>(1,c);
    }
    //各面に対して色を設定できるように変更する
    //c.size() == 6 は六面体に対するassert
    void setFaceColors(const std::vector<QColor> &c){
        Q_ASSERT(c.size() == FaceSize);
        m_colors = c;
    }

    void transform(const QMatrix4x4 &transform);

    unsigned int getFaceSize(){
        return FaceSize;
    }

    unsigned int getFaceSize() const{
        return FaceSize;
    }

    void copy2Buffer(Vertex *&vertexBuffer, GLuint *&elementBuffer, unsigned int &elementStartIndex) const;

    unsigned int VertexCount;
    unsigned int IndexCount;
    unsigned int FaceSize;
    unsigned int scale = 10;

    /*! Tests if line in space, defined through starting point p1 and distance/direction d intersects the plane
            with index planeIdx.
    */
    bool intersects(unsigned int planeIdx, const QVector3D &p1, const QVector3D &d, float &dist) const;

private:
    struct Triangle{
        Triangle(){}
        Triangle(QVector3D a, QVector3D b, QVector3D c);
        QVector3D m_normal;
        QVector3D m_offset;
        QVector3D m_a;
        QVector3D m_b;
    };

    std::vector<QVector3D> m_vertices;
    std::vector<Face> m_faces;
    std::vector<Triangle> m_planeInfo; //populated in copy2Buffer
    std::vector<QColor> m_colors; //size l = uniform color, size FaceSize = face colors
};

#endif // MYMESH_H

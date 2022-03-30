#ifndef BOXMESH_H
#define BOXMESH_H

#include<QtGui/QOpenGLFunctions>

#include <QColor>
#include <QMatrix4x4>
#include <vector>

#include "Transform3d.h"
#include "Vertex.h"

class BoxMesh
{
public:
    BoxMesh(float width=1, float height=1, float depth=1, QColor boxColor=Qt::blue);

    void setColor(QColor c){
        m_colors = std::vector<QColor>(1,c);
    }
    void setFaceColors(const std::vector<QColor> &c){
        Q_ASSERT(c.size() == 6);
        m_colors = c;
    }

    void transform(const QMatrix4x4 &transform);

/*!     Fills in vertex data in a buffer, provided by the caller.
        The vertex data is stored interleaved, "coordinates(vec3)-color(vec3)-coordinates(vec3)-...".
        \param vertexBuffer Pointer to vertex memory array to write into. Will be moved forward to point to the next
        position after the inserted vertices.
        \param elementBuffer Pointer to element memory array to write into. Will be moved forward to point to the next
        index position after the inserted vertices.
        elementStartIndex is the start index, that we should start indexing our newly added vertexes with.
*/
    void copy2Buffer(Vertex *&vertexBuffer, GLuint *&elementBuffer, unsigned int &elementStartIndex) const;

    static const unsigned int VertexCount = 6*4;
    static const unsigned int IndexCount = 6*2*3;

    /*! Tests if line in space, defined through starting point p1 and distance/direction d intersects the plane
            with index planeIdx.
    */
    bool intersects(unsigned int planeIdx, const QVector3D &p1, const QVector3D &d, float &dist) const;

private:
    struct Rect{
        Rect(){}
        Rect(QVector3D a, QVector3D b, QVector3D d);
        QVector3D m_normal; //normalized
        QVector3D m_offset;
        QVector3D m_a;
        QVector3D m_b;
    };
    std::vector<QVector3D> m_vertices;
    std::vector<Rect> m_planeInfo; //populated in copy2Buffer
    std::vector<QColor> m_colors; //size l = uniform color, size 6 = face colors
};

#endif // BOXMESH_H

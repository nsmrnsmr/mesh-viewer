#include "MyMesh.h"

#include <QDebug>

#include "PickObject.h"
#include "Input.h"

//六面体用のコードになっているため変更する
void copyPlane2Buffer(Vertex *&vertexBuffer, GLuint *&elementBuffer, unsigned int & elementStartIndex, const Vertex &a, const Vertex &b, const Vertex &c);
MyMesh::MyMesh(const std::string &fileName, const std::string &suffix, QColor meshColor)
{
    setColor(meshColor);
    if(suffix == "obj"){
        readOBJ(fileName, m_vertices, m_faces);
    }
    else if(suffix == "off"){
        readOFF(fileName, m_vertices, m_faces);
    }
    FaceSize = m_faces.size();
    VertexCount = FaceSize * 3;
    IndexCount = FaceSize * 3;
}

void MyMesh::transform(const QMatrix4x4 &transform)
{
    for(QVector3D &v : m_vertices){
        v = transform * v;
    }
}

void MyMesh::copy2Buffer(Vertex *&vertexBuffer, GLuint *&elementBuffer, unsigned int &elementStartIndex) const
{
    std::vector<QColor> cols;
    Q_ASSERT(!m_colors.empty());
    //three ways to store vertex colors
    if(m_colors.size() == 1){
        cols = std::vector<QColor>(FaceSize, m_colors[0]);
    }
    else{
        Q_ASSERT(m_colors.size() == FaceSize);
        cols = m_colors;
    }

    // now we populate the vertex buffer for all planes
    // and compute all face normals
    int cnt = 0;
    std::vector<Triangle> &planeInfo = const_cast<std::vector<Triangle>&>(m_planeInfo);
    planeInfo.resize(FaceSize);
    for(const Face &f : m_faces){
        QVector3D a, b, c;
        a = m_vertices[f.v0] * scale;
        b = m_vertices[f.v1] * scale;
        c = m_vertices[f.v2] * scale;

        copyPlane2Buffer(vertexBuffer, elementBuffer, elementStartIndex,
                         Vertex(a, cols[cnt]),
                         Vertex(b, cols[cnt]),
                         Vertex(c, cols[cnt])
                        );

        planeInfo[cnt] = Triangle(a, b, c);
        cnt++;
    }
}

bool MyMesh::intersects(unsigned int planeIdx, const QVector3D &p1, const QVector3D &d, float &dist) const
{
    const Triangle &p = m_planeInfo[planeIdx];
    return intersectsTriangle(p.m_a, p.m_b, p.m_normal, p.m_offset, p1, d, dist);
}

//三角形メッシュ用に改造する
void copyPlane2Buffer(Vertex *&vertexBuffer, GLuint *&elementBuffer, unsigned int & elementStartIndex, const Vertex &a, const Vertex &b, const Vertex &c)
{
    //first store the vertex data (a,b,c in counter-clockwise order)
    vertexBuffer[0] = a;
    vertexBuffer[1] = b;
    vertexBuffer[2] = c;

    //advance vertexBuffer
    vertexBuffer += 3;

    //we generate data for a triangle: a, b, c
    elementBuffer[0] = elementStartIndex;
    elementBuffer[1] = elementStartIndex+1;
    elementBuffer[2] = elementStartIndex+2;

    //advance elementBuffer
    elementBuffer += 3;
    //3 vertices have been added, so increase start number for next plane
    elementStartIndex += 3;

}

MyMesh::Triangle::Triangle(QVector3D a, QVector3D b, QVector3D c)
{
    m_a = b - a;
    m_b = c - a;
    m_normal = QVector3D::crossProduct(m_a, m_b);
    Q_ASSERT(m_normal.length() != 0.f);
    m_normal.normalize();
    m_offset = a;
}

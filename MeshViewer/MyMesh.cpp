#include "MyMesh.h"

#include <QDebug>
#include <map>
#include <utility>

#include "PickObject.h"
#include "Input.h"

//六面体用のコードになっているため変更する（済）
void copyPlane2Buffer(Vertex *&vertexBuffer, GLuint *&elementBuffer, unsigned int & elementStartIndex, const Vertex &a, const Vertex &b, const Vertex &c);
inline void compare(float &large, const QVector3D &b);

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

    //Halfedgeの初期化
    //outgoing halfedge(頂点ごとのhalfedgeの初期化m_vertexToHalfedge)
    std::map<std::pair<int, int>, int> eToh;
    std::vector<std::pair<int, int> > edges;
    m_halfedges.resize(FaceSize*3);
    m_vertexToHalfedge.resize(m_vertices.size());
    for(int i=0; i<(int)FaceSize; ++i){
        Face f = m_faces[i];
        for(int j=0; j<3; ++j){
            m_vertexToHalfedge[f[j]] = 3*i+((j+2)%3);
            m_vertexToHalfedge[f[j]] = 3*i+j;
            int hId = 3*i + j;
            m_halfedges[hId].setId(hId);
            m_halfedges[hId].setMeshPointer(this);
            std::pair<int,int> edge = std::make_pair(f[j], f[(j+1)%3]);
            edges.push_back(edge);
            eToh.insert(std::make_pair(edge, hId));
        }
    }

    //opposite halfedgeの初期化
    int cnt = 0;
    for(const auto & e : edges){
        std::pair<int, int> oppEdge = std::make_pair(e.second, e.first);
        if(eToh.find(oppEdge) != eToh.end()){
            int hId = eToh.at(e);
            m_halfedges[hId].setOppHalfedge(eToh.at(oppEdge));
        }else{
            cnt++;
        }
    }
    qDebug() << "boundary: " << cnt;

    //大きさと位置（高さのみ）の調整
    float min = 9999;
    float max = -9999;
    for(const auto & v : m_vertices){
        compare(max, v);
        if(min > v.y()) min = v.y();
    }
    for(auto & v : m_vertices){
        v.setY(v.y() - min);
        v /= max;
        v *= scale;
    }
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
        a = m_vertices[f.v0];
        b = m_vertices[f.v1];
        c = m_vertices[f.v2];

        //Vertex(a, cols[cnt], normal[cnt])ってやりたい直下のcopyPlane2Bufferはそのうち消す
        //copyPlane2Buffer(vertexBuffer, elementBuffer, elementStartIndex,
        //                 Vertex(a, cols[cnt]),
        //                 Vertex(b, cols[cnt]),
        //                 Vertex(c, cols[cnt])
        //                );

        planeInfo[cnt] = Triangle(a, b, c);
        cnt++;
    }

    //halfedge data structureを組み込めたらコメントアウト外せる:完了
    //頂点法線を計算する
    //例外処理を実装する（境界処理）
    //calc vertex normal if m_planeInfo is not empty.
    Q_ASSERT(!planeInfo.empty());
    //halfedge data structureが必要
    std::vector<QVector3D> &vertexNormals = const_cast<std::vector<QVector3D>&>(m_vertexNormals);
    vertexNormals.resize(m_vertices.size());
    for(int i=0; i<(int)m_vertices.size(); ++i){
        int h = m_vertexToHalfedge[i], h_end = h;
        cnt = 0;
        QVector3D normal;
        do{
            normal += planeInfo[h/3].m_normal;
            cnt++;
            h = m_halfedges[h].preHalfedge();
            h = m_halfedges[h].oppHalfedge();
            //qDebug() << m_halfedges[h].srcVertex();
            //Q_ASSERT(m_halfedges[h].srcVertex() == i);
        }while(h != h_end && h != -1);

        if(h == -1){
            h = h_end;
            h = m_halfedges[h].oppHalfedge();
            while(h != -1){
                h = m_halfedges[h].nextHalfedge();
                normal += planeInfo[h/3].m_normal;
                cnt++;
                h = m_halfedges[h].oppHalfedge();
            }
        }
        vertexNormals[i] = normal/cnt;
    }

    cnt = 0;
    for(const Face &f : m_faces){
        QVector3D a, b, c;
        a = m_vertices[f.v0];
        b = m_vertices[f.v1];
        c = m_vertices[f.v2];

        QVector3D na, nb, nc;
        na = m_vertexNormals[f.v0];
        nb = m_vertexNormals[f.v1];
        nc = m_vertexNormals[f.v2];

        copyPlane2Buffer(vertexBuffer, elementBuffer, elementStartIndex,
                         Vertex(a, cols[cnt], na),
                         Vertex(b, cols[cnt], nb),
                         Vertex(c, cols[cnt], nc)
                        );
        cnt++;
    }
}

bool MyMesh::intersects(unsigned int planeIdx, const QVector3D &p1, const QVector3D &d, float &dist) const
{
    const Triangle &p = m_planeInfo[planeIdx];
    return intersectsTriangle(p.m_a, p.m_b, p.m_normal, p.m_offset, p1, d, dist);
}

//三角形メッシュ用に改造する（済）
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

inline void compare(float &large, const QVector3D &b){
    if(large < b.x()){
        large = b.x();
    }
    if(large < b.y()){
        large= b.y();
    }
    if(large < b.z()){
        large = b.z();
    }
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

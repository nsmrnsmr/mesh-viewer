#ifndef HALFEDGE_H
#define HALFEDGE_H

//#include "MyMesh.h"
class MyMesh;
class Halfedge
{
public:
    Halfedge(MyMesh *m=nullptr, int id=-1, int opp_h=-1):
        m_mesh(m),
        id(id),
        opp_h(opp_h)
    {}

    void setMeshPointer(MyMesh *mesh);
    void setId(int hId);
    void setOppHalfedge(int opp);

    int tgtVertex() const;
    int srcVertex() const;
    int nextHalfedge() const;
    int preHalfedge() const;
    int oppHalfedge() const;

private:
    MyMesh *m_mesh;
    int id;
    int opp_h;
};

#endif // HALFEDGE_H

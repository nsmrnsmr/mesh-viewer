#include "Halfedge.h"
#include "MyMesh.h"

void Halfedge::setMeshPointer(MyMesh *mesh)
{
    m_mesh = mesh;
}

void Halfedge::setId(int hId)
{
    id = hId;
}

void Halfedge::setOppHalfedge(int opp)
{
    opp_h = opp;
}

int Halfedge::tgtVertex() const
{
    Face f = m_mesh->getFace(id/3);
    int i = (id + 1) % 3;
    if(i == 0) return f.x();
    else if(i == 1) return f.y();
    else return f.z();
    return -1;
}

int Halfedge::srcVertex() const
{
    Face f = m_mesh->getFace(id/3);
    int i = id % 3;
    if(i == 0) return f.x();
    else if(i == 1) return f.y();
    else return f.z();
    return -1;
}

int Halfedge::nextHalfedge() const
{
    int i = id % 3;
    if(i != 2) return id + 1;
    else return id - 2;
    return -1;
}

int Halfedge::preHalfedge() const
{
    int i = id % 3;
    if(i == 0) return id + 2;
    else return id - 1;
    return -1;
}

int Halfedge::oppHalfedge() const
{
    return opp_h;
}

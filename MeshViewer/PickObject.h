#ifndef PICKOBJECT_H
#define PICKOBJECT_H

#include <QVector3D>

//An Object to hold information on the clicked-on object.
struct PickObject
{
public:
    PickObject(float dist, unsigned int id) :
        m_dist(dist), m_objectId(id), m_faceId(0)
    {}
    PickObject(float dist, unsigned int id, unsigned int faceId) :
        m_dist(dist), m_objectId(id), m_faceId(faceId)
    {}

    float m_dist;
    unsigned int m_objectId;
    unsigned int m_faceId;
};

/*! Tests if a line (with equation p = p1 + t * d) hits a plane, defined by
    p = x * a  +  y * b. Returns true if intersection is found, and returns
    the normalized distance (t) between intersection point and point p1.
*/
bool intersectsRect(const QVector3D &a, const QVector3D &b, const QVector3D &normal, const QVector3D &offset, const QVector3D &p1, const QVector3D &d, float &dist);
bool intersectsTriangle(const QVector3D &a, const QVector3D &b, const QVector3D &normal, const QVector3D &offset, const QVector3D &p1, const QVector3D &d, float &dist);
#endif // PICKOBJECT_H

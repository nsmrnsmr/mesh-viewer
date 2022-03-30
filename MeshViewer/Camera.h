#ifndef CAMERA_H
#define CAMERA_H

#include "Transform3d.h"

class Camera : public Transform3D
{
public:

    QVector3D forward() const{
        const QVector3D LocalForward(0.0f, 0.0f, -1.0f);
        return m_rotation.rotatedVector(LocalForward);
    }

    QVector3D up() const{
        const QVector3D LocalUp(0.0f, 1.0f, 0.0f);
        return m_rotation.rotatedVector(LocalUp);
    }

    QVector3D right() const{
        const QVector3D LocalRight(1.0f, 0.0f, 0.0f);
        return m_rotation.rotatedVector(LocalRight);
    }

/*! Transformation matrix to convert from world to view coordinates when left-multiplied with world coords.
            Mind: no scaling applied.
*/
    const QMatrix4x4 &toMatrix() const{
        if(m_dirty){
            m_dirty = false;
            m_world.setToIdentity();
            m_world.rotate(m_rotation.conjugated());
            m_world.translate(-m_translation);
        }
        return m_world;
    }
};

#endif // CAMERA_H

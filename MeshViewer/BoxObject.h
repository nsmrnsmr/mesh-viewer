#ifndef BOXOBJECT_H
#define BOXOBJECT_H

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

QT_BEGIN_NAMESPACE
class QOpenGLShaderProgram;
QT_END_NAMESPACE

#include "BoxMesh.h"

struct PickObject;

class BoxObject
{
public:
    BoxObject();

    //The function is called during OpenGL initialization, where the OpenGL context is current.
    void create(QOpenGLShaderProgram *shaderProgram);
    void destroy();

    void render();

    /*! Thread-save pick function.
            Checks if any of the box object surfaces is hit by the ray defined by "p1 + d [0..1]" and
            stores data in po (pick object).
    */
    void pick(const QVector3D &p1, const QVector3D &d, PickObject &po) const;

    // Changes color of box and face to show that the box was clicked on.
    void highlight(unsigned int boxId, unsigned int faceId);

    std::vector<BoxMesh> m_boxes;

    //The function is called during OpenGL initialization, where the OpenGL context is current.
    std::vector<Vertex> m_vertexBufferData;
    std::vector<GLuint> m_elementBufferData;

    //Wraps an OpenGL VertexArrayObject, that references the vertex coordinates and color buffers.
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_ebo;
};

#endif // BOXOBJECT_H

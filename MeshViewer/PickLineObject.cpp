#include "PickLineObject.h"

#include <QOpenGLShaderProgram>
#include <vector>

void PickLineObject::create(QOpenGLShaderProgram *shaderProgram)
{
    //create a temporary buffer that will contain the x-z coordinates of all grid lines
    //we have 1 line, with two vertices, with 2 x three floats (position and color)
    m_vertexBufferData.resize(2);
    m_vertexBufferData[0] = Vertex(QVector3D(5,5,5), Qt::white);
    m_vertexBufferData[1] = Vertex(QVector3D(-5,-5,-5), Qt::red);

    //Create Vertex Array Object
    m_vao.create();
    m_vao.bind();

    //Create Vertex Buffer Object
    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    int vertexMenSize = m_vertexBufferData.size()*sizeof (Vertex);
    m_vbo.allocate(m_vertexBufferData.data(), vertexMenSize);

    //index 0 = position
    shaderProgram->enableAttributeArray(0);
    shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof (Vertex));
    //index 1 = position
    shaderProgram->enableAttributeArray(1);
    shaderProgram->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, r), 3, sizeof (Vertex));

    m_vao.release();
    m_vbo.release();

}

void PickLineObject::destroy()
{
    m_vao.destroy();
    m_vbo.destroy();
}

void PickLineObject::render()
{
    m_vao.bind();
    glDrawArrays(GL_LINES, 0, m_vertexBufferData.size());
    m_vao.release();
}

void PickLineObject::setPoints(const QVector3D &a, const QVector3D &b)
{
    m_vertexBufferData[0] = Vertex(a, Qt::white);
    m_vertexBufferData[1] = Vertex(b, QColor(64,0,0));
    int vertexMemSize = m_vertexBufferData.size() * sizeof (Vertex);
    m_vbo.bind();
    m_vbo.allocate(m_vertexBufferData.data(), vertexMemSize);
    m_vbo.release();
    m_visible = true;
}

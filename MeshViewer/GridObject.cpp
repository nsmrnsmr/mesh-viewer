#include "GridObject.h"

#include <QOpenGLShaderProgram>
#include <vector>

#include <QDebug>
void GridObject::create(QOpenGLShaderProgram *shaderProgram)
{
    const unsigned int N = 3000;
    float width = 5000;

    std::vector<float> gridVertexBufferData;
    //we have 2*N lines, each line requires two vertices, with two floats (x and z coordinates) each.
    m_bufferSize = 2*N*2*2;
    gridVertexBufferData.resize(m_bufferSize);
    float *gridVertexBufferPtr = gridVertexBufferData.data();
    //compute grid lines with z = const
    float x1 = -width*0.5;
    float x2 = width*0.5;
    for(unsigned int i=0; i<N; ++i, gridVertexBufferPtr +=4){
        float z = width/(N-1)*i-width*0.5;
        //qDebug() << z;
        gridVertexBufferPtr[0] = x1;
        gridVertexBufferPtr[1] = z;
        gridVertexBufferPtr[2] = x2;
        gridVertexBufferPtr[3] = z;
    }
    //compute grid lines with x = const
    float z1 = -width*0.5;
    float z2 = width*0.5;
    for(unsigned int i=0; i<N; ++i, gridVertexBufferPtr +=4){
        float x = width/(N-1)*i-width*0.5;
        //qDebug() << x;
        gridVertexBufferPtr[0] = x;
        gridVertexBufferPtr[1] = z1;
        gridVertexBufferPtr[2] = x;
        gridVertexBufferPtr[3] = z2;
    }

    //Create vertex Array Object
    m_vao.create();
    m_vao.bind();

    //Create Vertex Buffer Object
    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    int vertexMemSize = m_bufferSize*sizeof (float);
    //qDebug() << vertexMemSize;
    qDebug() << "GridObject - VertexBuffer size =" << vertexMemSize/1024.0 << "kByte";
    m_vbo.allocate(gridVertexBufferData.data(), vertexMemSize);

    //layout (location = 0) = vex2 position
    shaderProgram->enableAttributeArray(0);
    shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);
    m_vao.release();
    m_vbo.release();
}

void GridObject::destroy()
{
    m_vao.destroy();
    m_vbo.destroy();
}

void GridObject::render()
{
    m_vao.bind();
    //draw the grid lines, m_NVertices = number of floats in buffer
    //qDebug() << m_bufferSize;
    glDrawArrays(GL_LINES, 0, m_bufferSize);
    m_vao.release();
}


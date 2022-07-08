#include "MyObject.h"

#include <QDebug>
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QElapsedTimer>

#include "PickObject.h"

bool MyObject::readMesh(QString fileName, QString suffix)
{
    qDebug() << "MyObject::readMesh():"<< "file name:" << fileName << ", suffix:" << suffix;
    std::string stdFilename = fileName.toStdString();
    std::string stdSuffix = suffix.toStdString();
    MyMesh m_mesh(stdFilename, stdSuffix);

    m_meshes.push_back(m_mesh);

    unsigned int vertexSize = 0, indexSize = 0;
    for(MyMesh &m : m_meshes){
        vertexSize += m.VertexCount;
        indexSize += m.IndexCount;
    }
    //resize storage arrays
    qDebug() << "all vertexSizes: " << vertexSize;
    qDebug() << "all indexSizes: " << indexSize;
//    std::vector<Vertex> m_vertexBufferData(vertexSize);
//    std::vector<GLuint> m_elementBufferData(indexSize);
    //これまでに読み込んだデータの数を保存しておく
    unsigned int beforeVbdSize = m_vertexBufferData.size();
    unsigned int beforeEbdSize = m_elementBufferData.size();
    m_vertexBufferData.resize(vertexSize);
    m_elementBufferData.resize(indexSize);

    //update the buffers
    Vertex *vertexBuffer = m_vertexBufferData.data() + beforeVbdSize;
    unsigned int vertexCount = beforeVbdSize;
    GLuint *elementBuffer = m_elementBufferData.data() + beforeEbdSize;
    m_meshes[meshCounter].copy2Buffer(vertexBuffer, elementBuffer, vertexCount);
//    m_vertexBufferDatas.push_back(m_vertexBufferData);
//    m_elementBufferDatas.push_back(m_elementBufferData);

    return true;
}

void MyObject::create(QOpenGLShaderProgram *shaderProgram)
{
    qDebug() << "MyObject::create()";

//    QOpenGLVertexArrayObject m_vao;
//    QOpenGLBuffer m_vbo(QOpenGLBuffer::VertexBuffer);
//    QOpenGLBuffer m_ebo(QOpenGLBuffer::IndexBuffer);
//    std::vector<Vertex> &m_vertexBufferData = m_vertexBufferDatas[counter];
//    std::vector<GLuint> &m_elementBufferData = m_elementBufferDatas[counter];

    //create each buffers if already created buffers
    if( !m_vao.isCreated() ){
        //create and bind Vertex Buffer Object
        m_vao.create();
    }
    m_vao.bind();

    //create and bind vertex buffer
    if( !m_vbo.isCreated() ){
        m_vbo.create();
    }
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    int vertexMemSize = m_vertexBufferData.size()*sizeof (Vertex);
    qDebug() << "MyObject - VertexBuffer size =" << vertexMemSize/1024.0 << "kByte";
    m_vbo.allocate(m_vertexBufferData.data(), vertexMemSize);

    //create and bind element buffer
    if( !m_ebo.isCreated() ){
        m_ebo.create();
    }
    m_ebo.bind();
    m_ebo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    int elementMemSize = m_elementBufferData.size()*sizeof (GLuint);
    qDebug() << "MyObject - ElementBuffer size =" << elementMemSize/1024.0 << "kByte";
    m_ebo.allocate(m_elementBufferData.data(), elementMemSize);

    //set shader attributes
    //tell shader program we have two data arrays to be used as input to the shaders

    //index 0 = position
    shaderProgram->enableAttributeArray(0); //array with index/id 0
    shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof (Vertex));
    //index 1 = color
    shaderProgram->enableAttributeArray(1);
    shaderProgram->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, r), 3, sizeof (Vertex));
#if 1 // シェーディングを実装したら使えるようになる
    //index 2 = vertex normal
    shaderProgram->enableAttributeArray(2);
    shaderProgram->setAttributeBuffer(2, GL_FLOAT, offsetof(Vertex, nx), 3, sizeof (Vertex));
#endif
    //Release (unbind) all
    m_vao.release();
    m_vbo.release();
    m_ebo.release();
//    m_vaos.push_back(m_vao);
//    m_vbos.push_back(m_vbo);
//    m_ebos.push_back(m_ebo);
    meshCounter++;
}

void MyObject::destroy()
{
    m_vao.destroy();
    m_vbo.destroy();
    m_ebo.destroy();

//    for(unsigned int meshId=0; meshId<counter; ++meshId){
//        m_vaos[meshId].destroy();
//        m_vbos[meshId].destroy();
//        m_ebos[meshId].destroy();
//    }
}

void MyObject::render()
{
    // set the geometry ("position" and "color" arrays)
    m_vao.bind();

    //now draw the cube by drawing individual triangles
    //- GL_TRIANGLES - draw individual triangles via elements

    glDrawElements(GL_TRIANGLES, m_elementBufferData.size(), GL_UNSIGNED_INT, nullptr);

    //オブジェクトごとの処理に変更
//    for(auto & m_elementBufferData: m_elementBufferDatas){
//        glDrawElements(GL_TRIANGLES, m_elementBufferData.size(), GL_UNSIGNED_INT, nullptr);
//    }

    m_vao.release();

    //vectorに変更してみた
//    for(unsigned int meshId=0; meshId<counter; ++meshId){
//        m_vaos[meshId].bind();
//        glDrawElements(GL_TRIANGLES, m_elementBufferDatas[meshId].size(), GL_UNSIGNED_INT, nullptr);
//        m_vaos[meshId].release();
//    }
}

void MyObject::pick(const QVector3D &p1, const QVector3D &d, PickObject &po) const
{
    //改良の余地あり（面の全探索しているのはもったいない）
    //now prosess all import objects
    for(unsigned int i=0; i<m_meshes.size(); ++i){
        const MyMesh &mm = m_meshes[i];
        for(unsigned int j=0; j<mm.getFaceSize(); ++j){
            float dist;
            //is intersection point closes to viewer than previous intersection points?
            if(mm.intersects(j, p1, d, dist)){
                qDebug() << QString("Plane %1 of box %2 intersects lines at normalized distance = %3").arg(j).arg(i).arg(dist);
                //keep objects this is closer to near plane
                if(dist < po.m_dist){
                    po.m_dist = dist;
                    po.m_objectId = i;
                    po.m_faceId = j;
                }
            }
        }
    }
}

void MyObject::highlight(unsigned int meshId, unsigned int faceId)
{
    qDebug() << "MyObject::highlight()";
    //we change the color of all vertices of the selected mesh to lightgray and the vertex colors of the selsected plane/face to light blue
    unsigned int faceSize = m_meshes[meshId].getFaceSize();
    std::vector<QColor> faceCols(faceSize);
    for(unsigned int i=0; i<faceSize; ++i){
        if(i == faceId){
            faceCols[i] = QColor("#b40808");
        }
        else{
            faceCols[i] = QColor("#f3f3f3");
        }
    }
    m_meshes[meshId].setFaceColors(faceCols);
    unsigned int vbdStart = 0;
    unsigned int ebdStart = 0;
    for(unsigned int i=0; i<meshId; ++i){
        vbdStart += m_meshes[i].getVertexSize();
        ebdStart += m_meshes[i].getIndexSize();
    }

    //advance the pointers and vertex number to the respected box position/numbering
    //Vertex *vertexBuffer = m_vertexBufferData.data() + meshId*faceSize*3; //faceSize planes, with 3 vertexes each
    Vertex *vertexBuffer = m_vertexBufferData.data() + vbdStart;
    //unsigned int vertexCount = meshId*faceSize*3; //start index for target mesh
    unsigned int vertexCount = vbdStart;
    //GLuint *elementBuffer = m_elementBufferData.data() + meshId*faceSize*3; //faceSize planes, with 1 triangles with 3 indexes each
    GLuint *elementBuffer = m_elementBufferData.data() + ebdStart;
    //then we update the respective portion of the vertexbuffer memory
    m_meshes[meshId].copy2Buffer(vertexBuffer, elementBuffer, vertexCount);

    QElapsedTimer t;
    t.start();
    //QOpenGLBuffer &m_vbo = m_vbos[meshId];
    //and now update the entire vertex buffer
    m_vbo.bind();
    //only update the modified portion of the data
    m_vbo.write(vbdStart*sizeof (Vertex), m_vertexBufferData.data() + vbdStart, m_meshes[meshId].getVertexSize()*sizeof (Vertex));
    //alternatively use the call below, witch (re-) copies the entire buffer, witch can be slow
    m_vbo.allocate(m_vertexBufferData.data(), m_vertexBufferData.size()*sizeof(Vertex)); //sizeof(float)*9に変更する必要あり：HalfedgeをVertexに入れた関係でサイズが変わってしまったから
    m_vbo.release();
    qDebug() << "MyObject::hilight(): " << t.elapsed() << "ms";
}

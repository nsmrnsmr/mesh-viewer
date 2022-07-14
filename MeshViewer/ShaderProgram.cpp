#include "ShaderProgram.h"

#include <QOpenGLShaderProgram>
#include <QDebug>

#include"OpenGLException.h"

ShaderProgram::ShaderProgram()
{
}
ShaderProgram::ShaderProgram(const QString &vertexShaderFilePath, const QString &fragmentShaderFilePath) :
    m_vertexShaderFilePath(vertexShaderFilePath),
    m_fragmentShaderFilePath(fragmentShaderFilePath),
    m_program(nullptr)
{
}

void ShaderProgram::create()
{
    FUNCID(ShaderProgram::create);
    Q_ASSERT(m_program == nullptr);

    //build and compile our shader program
    qDebug() << "ShaderProgram::create()";

    m_program = new QOpenGLShaderProgram();

    //read the shader programs from the resource
    if(!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, m_vertexShaderFilePath)){
        qDebug() << "Vertex shader errors :\n" << m_program->log();
        throw OpenGLException(QString("Error compiling vertex shader %1:\n%2").arg(m_vertexShaderFilePath, m_program->log()), FUNC_ID);
    }
    if(!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, m_fragmentShaderFilePath)){
        qDebug() << "Fragment shader errors :\n" << m_program->log();
        throw OpenGLException(QString("Error compiling fragment shader %1:\n%2").arg(m_fragmentShaderFilePath, m_program->log()), FUNC_ID);
    }
    if(!m_program->link()){
        qDebug() << "Shader link errors :\n" << m_program->log();
        throw OpenGLException(QString("Shader linker error:\n%2").arg(m_program->log()), FUNC_ID);
    }

    m_uniformIDs.clear();
    for(const QString &uniformName : qAsConst(m_uniformNames)){
        m_uniformIDs.append(m_program->uniformLocation(uniformName));
        qDebug() << uniformName;
    }
    //qDebug() << m_uniformIDs.size();
    //qDebug() << m_uniformNames.size();
}

void ShaderProgram::destroy()
{
    qDebug() << "ShaderProgram::destroy()";
    delete m_program;
    m_program = nullptr;
}

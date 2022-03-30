#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QString>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QOpenGLShaderProgram;
QT_END_NAMESPACE

/*! A small wrapper class around QOpenGLShaderProgram to encapsulate
    shader code compilation and linking and error handling.
    It is meant to be used with shader programs in files, for example from qrc files.
    The embedded shader programm is not destroyed automatically upon destruction.
    You must call destroy() to end the lifetime of the allocated OpenGL resources.
*/

class ShaderProgram
{
public:
    ShaderProgram();
    ShaderProgram(const QString &vertexShaderFilePath, const QString &fragmentShaderFilePath);

    void create();
    void destroy();

    QOpenGLShaderProgram *shaderProgram(){
        return m_program;
    }

    QString m_vertexShaderFilePath;
    QString m_fragmentShaderFilePath;

    QStringList m_uniformNames;
    QList<int> m_uniformIDs;

private:
    QOpenGLShaderProgram *m_program;
};

#endif // SHADERPROGRAM_H

#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#define SSAO 0
#define MAXSAMPLES 256

#include <QMatrix4x4>
#include <QOpenGLTimeMonitor>
#include <QElapsedTimer>
#include <QStringList>

#include "OpenGLwindow.h"
#include "ShaderProgram.h"
#include "KeyboardMouseHandler.h"
#include "GridObject.h"
#include "BoxObject.h"
#include "MyObject.h"
#include "PickLineObject.h"
#include "Camera.h"

class SceneView : public OpenGLWindow
{
public:
    SceneView();
    virtual ~SceneView() override;

    void inputMyObject(const QString &fileName, const QString &suffix);
    void inputFragmentShader(const QString &fileName, const QString &suffix, const QString &name);
    void changeFragmentShader(const int shaderId);
    QList<QString> getShaderList(){ return m_shaderList; }

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

    //Functions to handle key press and mouse press events, all the work is done in clss KeyboardMouseHandler
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *event) override;

    void pick(const QPoint &globalMousePos);

private:
    //Test, if any relevant input was received and registers a state change.
    void checkInput();
    //This function is called first thing in the paintGL() routine and processes input received so far and updates camera position.
    void processInput();
    //Compines camera matrix and project matrix to from the world2view mateix.
    void updateWorld2ViewMatrix();

    /*! Determine which objects/planes are selected and color them accordingly.
            nearPoint and farPoint define the current ray and are given in model coordinates.
        */
    void selectNearestObject(const QVector3D &nearPoint, const QVector3D &farPoint);

    //The input handler, that encapsulates the event handling code.
    KeyboardMouseHandler m_keyboardMouseHandler;
    //If set to true, an input event was received, which will be evaluated at next repaint.
    bool m_inputEventReceived;

    //The projection matrix, updated whenever the viewport geometry changes (in resizeGL() ).
    QMatrix4x4 m_projection;
    Transform3D m_transform;
    Camera m_camera;
    QMatrix4x4 m_worldToView;

    //light
    QVector3D m_lightColor;
    QVector3D m_lightDirection;

#if SSAO
    QVector4D m_samplePoint[MAXSAMPLES];
    QVector4D m_reflection = QVector4D(0.2, 0.2, 0.2, 0.0);
    float m_refraction = 0.67;
    float m_bgdistance = 0.1;
#endif
    //All shader programs used in the scene.
    QList<ShaderProgram> m_shaderPrograms;
    QList<ShaderProgram> m_gridShaderPrograms;
    //useing m_shaderPrograms index to m_object
    u_int m_shaderId;
    u_int m_gridShaderId;

    //TestDialogクラス内のQComboBoxに利用可能シェーダを表示させるためのリスト
    QStringList m_shaderList;
    QStringList m_gridShaderList;

    BoxObject m_boxObjct;
    GridObject m_gridObject;
    PickLineObject m_pickLineObject;

    QOpenGLTimeMonitor m_gpuTimers;
    QElapsedTimer m_cpuTimer;

    //いずれコメントアウト消すやつ
    MyObject m_object;
};

#endif // SCENEVIEW_H

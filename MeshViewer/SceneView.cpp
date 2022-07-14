#include "SceneView.h"

#include <QExposeEvent>
#include <QOpenGLShaderProgram>
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>

#include "DebugApplication.h"
#include "PickObject.h"

//#define N 0
#define SHADER(x) m_shaderPrograms[x].shaderProgram()
#define GSHADER(x) m_gridShaderPrograms[x].shaderProgram()

SceneView::SceneView() :
    m_inputEventReceived(false),
    m_shaderId(0),
    m_gridShaderId(0)
{
    qDebug() << "ScenecView::SceneView()";
    //tell keyboard handler to monitor certain keys
    m_keyboardMouseHandler.addRecongnizedKey(Qt::Key_W);
    m_keyboardMouseHandler.addRecongnizedKey(Qt::Key_A);
    m_keyboardMouseHandler.addRecongnizedKey(Qt::Key_S);
    m_keyboardMouseHandler.addRecongnizedKey(Qt::Key_D);
    m_keyboardMouseHandler.addRecongnizedKey(Qt::Key_Q);
    m_keyboardMouseHandler.addRecongnizedKey(Qt::Key_E);
    m_keyboardMouseHandler.addRecongnizedKey(Qt::Key_Shift);

    //***create scene (no OpenGL calls are being issued, just the date structures are creared.)

    //Shaderprogram #0 : regular geometry (painting triangules via element index)
    //shaderの名前をそのうち変更する Lambert
    ShaderProgram blocks(":/Shaders/sample.vert",":/Shaders/sample.frag");
    blocks.m_uniformNames.append("worldToView");
    blocks.m_uniformNames.append("lightColor");
    blocks.m_uniformNames.append("lightDirection");
    blocks.m_uniformNames.append("viewToWorld");
    m_shaderPrograms.append(blocks);

    //Shaderprogram #1 : regular geometry Phong Shading(painting triangules via element index)
    ShaderProgram Phong(":/Shaders/sample.vert",":/Shaders/Phong.frag");
    Phong.m_uniformNames.append("worldToView");
    Phong.m_uniformNames.append("lightColor");
    Phong.m_uniformNames.append("lightDirection");
    Phong.m_uniformNames.append("viewToWorld");
    m_shaderPrograms.append(Phong);

    //GridShaderprogram #0 : grid (painting grid lines)
    ShaderProgram grid(":/Shaders/grid.vert",":/Shaders/grid.frag");
    grid.m_uniformNames.append("worldToView");
    grid.m_uniformNames.append("gridColor");
    grid.m_uniformNames.append("backColor");
    m_gridShaderPrograms.append(grid);

    //***Initialize fragment shader file name list
    for(const ShaderProgram &s : m_shaderPrograms){
        QFileInfo fileInfo = QFileInfo(s.m_fragmentShaderFilePath);
        m_shaderList.append(fileInfo.baseName());
    }
    for(const ShaderProgram &s : m_gridShaderPrograms){
        QFileInfo fileInfo = QFileInfo(s.m_fragmentShaderFilePath);
        m_gridShaderList.append(fileInfo.baseName());
    }

    //***Initialize camera placement and model placement in the world

    //move camera a little back and up
    m_camera.translate(-50, 100, 150);
    //look slightly down
    m_camera.rotate(-30, m_camera.right());
    //look slightly right
    m_camera.rotate(-25, QVector3D(0.0f, 1.0f, 0.0f));
    //lighting
    m_lightColor = QVector3D(1,1,1);
    //m_lightDirection = QVector3D(-50, 100, 150);
    m_lightDirection = QVector3D(0,0,1);
}

SceneView::~SceneView()
{
    if(m_context){
        m_context->makeCurrent(this);

        for(ShaderProgram &p : m_shaderPrograms){
            p.destroy();
        }

        for(ShaderProgram &p : m_gridShaderPrograms){
            p.destroy();
        }

        //m_boxObjct.destroy();
        m_object.destroy();
        m_gridObject.destroy();
        m_pickLineObject.destroy();

        m_gpuTimers.destroy();
    }
}

void SceneView::inputMyObject(const QString &fileName, const QString &suffix){
    qDebug() << "SceneView::inputMyObject()";
    m_object.readMesh(fileName, suffix);
    m_object.create(SHADER(m_shaderId)); //指定したシェーダでcreateするようにする
    renderLater();
    return;
}

void SceneView::inputFragmentShader(const QString &fileName, const QString &suffix, const QString &name)
{
    qDebug() << "SceneView::inputFragmentShader()";
    if(suffix == "frag"){
        ShaderProgram newShader(":/Shaders/sample.vert",fileName);
        newShader.m_uniformNames.append("worldToView");
        newShader.m_uniformNames.append("lightColor");
        newShader.m_uniformNames.append("lightDirection");
        newShader.m_uniformNames.append("viewToWorld");
        m_shaderPrograms.append(newShader);
    }
    m_shaderList.append(name);
    //update useing fragment shader id
    m_shaderId = m_shaderPrograms.size() - 1;
    //new shader compile
    m_shaderPrograms[m_shaderId].create();
    //attach m_object data to new shader
    if(m_object.size() != 0) m_object.create(SHADER(m_shaderId));
    renderLater();
    return;
}

void SceneView::changeFragmentShader(const int shaderId)
{
    m_shaderId = shaderId;
    if(m_object.size() != 0) m_object.create(SHADER(m_shaderId));
    renderLater();
    return;
}

void SceneView::initializeGL()
{
    FUNCID(SceneView::initializeGL);
    qDebug() << "SceneView::initializeGL()";
    try{
        //Initialize shader programs
        //指定したシェーダのみコンパイルするようにする。
        //デフォルトは、3つのシェーダを入れておく
        for(ShaderProgram &p : m_shaderPrograms){
            p.create();
        }
        for(ShaderProgram &p : m_gridShaderPrograms){
            p.create();
        }

        // tell OpenGL to show only faces whose normal vector points towards us
        glEnable(GL_CULL_FACE);
        // enable depth testing, important for the grid and for the drawing order of several objects
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        //initialize drawable objects
        //m_boxObjct.create(SHADER(0));
        if(m_object.size() != 0) m_object.create(SHADER(m_shaderId));
        m_gridObject.create(GSHADER(m_gridShaderId));
        m_pickLineObject.create(SHADER(m_shaderId));

        //Timer
        m_gpuTimers.setSampleCount(5);
        m_gpuTimers.create();
    }
    catch(OpenGLException &ex){
        throw OpenGLException(ex, "OpenGL initialization failed.", FUNC_ID);
    }
}

void SceneView::resizeGL(int width, int height)
{
    // the projection matrix need to be updated only for window size changes
    m_projection.setToIdentity();
    // create projection matrix, i.e. camera lens
    m_projection.perspective(45.0f, width/float(height), 0.1f, 10000.0f);

    // Mind: to not use 0.0 for near plane, otherwise depth buffering and depth testing won't work!

    // update cached world2view matrix
    updateWorld2ViewMatrix();
}

void SceneView::paintGL()
{
    m_cpuTimer.start();
    if(((DebugApplication *)qApp)->m_aboutToTerminate) return;

    qDebug() << "SceneView::paintGL()";
    //process input, i.e. check if any keys have been pressed
    if(m_inputEventReceived){
        qDebug() << "input event";
        processInput();
    }

    const qreal retinaScalse = devicePixelRatio();
    glViewport(0, 0, width()*retinaScalse, height()*retinaScalse);
    qDebug() << "SceneView::paintGL(): Rendering to:" << width() << "x" << height();

    //set the background color = clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //set the background color = clear color
    QVector3D backColor(0.1f, 0.15f, 0.3f);
    glClearColor(0.1f, 0.15f, 0.3f, 1.0f);

    QVector3D gridColor(0.5f, 0.5f, 0.7f);

    m_gpuTimers.reset();

    /*
    // *** render boxes ***
    m_gpuTimers.recordSample(); //setup boxes
    SHADER(0)->bind();
    SHADER(0)->setUniformValue(m_shaderPrograms[0].m_uniformIDs[0], m_worldToView);

    m_gpuTimers.recordSample(); //render boxes
    m_boxObjct.render();

    if(m_pickLineObject.m_visible){
        //m_gpuTimers.recordSample(); //render pickline
        m_pickLineObject.render();
    }
    SHADER(0)->release();
    */

    //*** render import objects ***
    qDebug() << "my_object size :" <<m_object.size();
    if(m_object.size() != 0){
        m_gpuTimers.recordSample();//setup meshes
        SHADER(m_shaderId)->bind();

        SHADER(m_shaderId)->setUniformValue(m_shaderPrograms[m_shaderId].m_uniformIDs[0], m_worldToView);
        SHADER(m_shaderId)->setUniformValue(m_shaderPrograms[m_shaderId].m_uniformIDs[1], m_lightColor);
        SHADER(m_shaderId)->setUniformValue(m_shaderPrograms[m_shaderId].m_uniformIDs[2], m_lightDirection);
        SHADER(m_shaderId)->setUniformValue(m_shaderPrograms[m_shaderId].m_uniformIDs[3], m_worldToView.inverted());

        m_gpuTimers.recordSample(); //render meshes
        m_object.render();

        if(m_pickLineObject.m_visible){
            //m_gpuTimers.recordSample(); //render pickline
            m_pickLineObject.render();
        }
        SHADER(m_shaderId)->release();
    }

    //*** render grid afterwards ***

    m_gpuTimers.recordSample(); //setup grid
    GSHADER(m_gridShaderId)->bind();
    GSHADER(m_gridShaderId)->setUniformValue(m_gridShaderPrograms[m_gridShaderId].m_uniformIDs[0], m_worldToView);
    GSHADER(m_gridShaderId)->setUniformValue(m_gridShaderPrograms[m_gridShaderId].m_uniformIDs[1], gridColor);
    GSHADER(m_gridShaderId)->setUniformValue(m_gridShaderPrograms[m_gridShaderId].m_uniformIDs[2], backColor);

    m_gpuTimers.recordSample(); //render grid
    m_gridObject.render();
    GSHADER(m_gridShaderId)->release();

    m_gpuTimers.recordSample(); //done painting

#if 0
    //do some animation stuff
    m_transform.rotate(1.0f, QVector3D(0.0f, 0.1f, 0.0f));
    updateWorld2ViewMatrix();
    renderLater();
#endif

    checkInput();

    QVector<GLuint64> intervals = m_gpuTimers.waitForIntervals();
    for(GLuint64 it : intervals){
        qDebug() << "  " << it*1e-6 << "ms/frame";
    }
    QVector<GLuint64> samples = m_gpuTimers.waitForSamples();
    qDebug() << "Total render time: " << (samples.back() - samples.front())*1e-6 << "ms/frame";

    qint64 elapsedMs = m_cpuTimer.elapsed();
    qDebug() << "Total paintGL time: " << elapsedMs << "ms";
}

void SceneView::keyPressEvent(QKeyEvent *event)
{
    m_keyboardMouseHandler.keyPressEvent(event);
    checkInput();
}

void SceneView::keyReleaseEvent(QKeyEvent *event)
{
    m_keyboardMouseHandler.keyReleaseEvent(event);
    checkInput();
}

void SceneView::mousePressEvent(QMouseEvent *event)
{
    m_keyboardMouseHandler.mousePressEvent(event);
    checkInput();
}

void SceneView::mouseReleaseEvent(QMouseEvent *event)
{
    m_keyboardMouseHandler.mouseReleaseEvent(event);
    checkInput();
}

void SceneView::mouseMoveEvent(QMouseEvent */*event*/)
{
    checkInput();
}

void SceneView::wheelEvent(QWheelEvent *event)
{
    m_keyboardMouseHandler.wheelEvent(event);
    checkInput();
}

void SceneView::pick(const QPoint &globalMousePos)
{
    //local mouse coordinates
    QPoint localMousePos = mapFromGlobal(globalMousePos);
    int my = localMousePos.y();
    int mx = localMousePos.x();

    //viewport dimensions
    const qreal retinaScale = devicePixelRatio(); //needed for Macs with retina display
    qreal halfVpw = width()*retinaScale/2;
    qreal halfVph = height()*retinaScale/2;

    //invert world2view matrix, with m_worldToView = m_projection * m_camera.toMatrix() * m_transform.toMatrix();
    bool invertible;
    QMatrix4x4 projectionMatrixInverted = m_worldToView.inverted(&invertible);
    if(!invertible){
        qWarning() << "Cannot invert projection matrix.";
        return;
    }

    //mouse position in NDC space, one point on near plane on far plane
    QVector4D nearVec((mx - halfVpw)/halfVpw, -1*(my - halfVph)/halfVph, -1, 1.0);
    QVector4D farVec(nearVec.x(), nearVec.y(), 1, 1.0);

    //transform form NDC to model coordinates
    QVector4D nearResult = projectionMatrixInverted * nearVec;
    QVector4D farResult = projectionMatrixInverted * farVec;
    //don't forget normalization;
    nearResult /= nearResult.w();
    farResult /= farResult.w();

    //update pick line vertices (visualize pick line)
    m_context->makeCurrent(this);
    m_pickLineObject.setPoints(nearResult.toVector3D(), farResult.toVector3D());

    //now do the actual picking - for now we implement a selection
    selectNearestObject(nearResult.toVector3D(), farResult.toVector3D());
}

void SceneView::checkInput()
{
    //this function is called whenever _any_key/mouse event was issued

    // we test, if the current state of the key handler requires a scene update
    // (camera movement) and if so, we just set a flag to do that upon next repaint
    // and we schedule a repaint

    //trigger key holds?
    if(m_keyboardMouseHandler.buttonDown(Qt::RightButton)){
        //any of the interesting keys held?
        if(m_keyboardMouseHandler.keyDown(Qt::Key_W) ||
                m_keyboardMouseHandler.keyDown(Qt::Key_A) ||
                m_keyboardMouseHandler.keyDown(Qt::Key_S) ||
                m_keyboardMouseHandler.keyDown(Qt::Key_D) ||
                m_keyboardMouseHandler.keyDown(Qt::Key_Q) ||
                m_keyboardMouseHandler.keyDown(Qt::Key_E)){
            m_inputEventReceived = true;
            //qDebug() << "SceneView::checkInput() inputEventReceived";
            renderLater();
            return;
        }
        //has the mouse been moved?
        if(m_keyboardMouseHandler.mouseDownPos() != QCursor::pos()){
            m_inputEventReceived = true;
           // qDebug() << "SceneView::checkInput() inputEventReceived: " << QCursor::pos() << m_keyboardMouseHandler.mouseDownPos();
            renderLater();
            return;
        }
    }
    //has the left mouse button been release
    if(m_keyboardMouseHandler.buttonReleased(Qt::LeftButton)){
        m_inputEventReceived = true;
        renderLater();
        return;
    }
    //scroll-wheel turend?
    if(m_keyboardMouseHandler.wheelDelta() != 0){
        m_inputEventReceived = true;
        renderLater();
        return;
    }
}

void SceneView::processInput()
{
    //function must only be called if an input event has been received
    Q_ASSERT(m_inputEventReceived);
    m_inputEventReceived = false;
    //qDebug() << "SceneView::prosessInput()";

    //check for trigger key
    if(m_keyboardMouseHandler.buttonDown(Qt::RightButton)){
        //Handle translation
        QVector3D translation;
        if(m_keyboardMouseHandler.keyDown(Qt::Key_W)) translation += m_camera.forward();
        if(m_keyboardMouseHandler.keyDown(Qt::Key_S)) translation -= m_camera.forward();
        if(m_keyboardMouseHandler.keyDown(Qt::Key_A)) translation -= m_camera.right();
        if(m_keyboardMouseHandler.keyDown(Qt::Key_D)) translation += m_camera.right();
        if(m_keyboardMouseHandler.keyDown(Qt::Key_Q)) translation -= m_camera.up();
        if(m_keyboardMouseHandler.keyDown(Qt::Key_E)) translation += m_camera.up();

        float transSpeed = 0.8f;
        if(m_keyboardMouseHandler.keyDown(Qt::Key_Shift)) transSpeed = 0.1f;

        m_camera.translate(transSpeed*translation);

        //Handle rotations
        //get and reset mouse delta (pass current mouse cursor position)
        QPoint mouseDelta = m_keyboardMouseHandler.resetMouseDelta(QCursor::pos()); //resets the internal position
        static const float rotationSpeed = 0.4f;
        const QVector3D LocalUp(0.0f, 1.0f, 0.0f); //same as in Camera::up()
        m_camera.rotate(-rotationSpeed * mouseDelta.x(), LocalUp);
        m_camera.rotate(-rotationSpeed * mouseDelta.y(), m_camera.right());
    }

    int wheelDelta = m_keyboardMouseHandler.resetWheelDelta();
    if(wheelDelta != 0){
        float transSpeed = 8.0f;
        if(m_keyboardMouseHandler.keyDown(Qt::Key_Shift)) transSpeed = 0.8f;
        m_camera.translate(wheelDelta * transSpeed * m_camera.forward());
    }

    //check for picking operation
    if(m_keyboardMouseHandler.buttonReleased(Qt::LeftButton)){
        pick(m_keyboardMouseHandler.mouseReleasePos());
    }

    //finally, reset "WasPressed" key states
    m_keyboardMouseHandler.clearWasPressedKeyStates();
    updateWorld2ViewMatrix();
    //not need to request update here, since we are called from paint anyeay
}

void SceneView::updateWorld2ViewMatrix()
{
    //transformation steps:
    //model space -> transform -> world space
    //world space -> camera view
    //camera view -> projection -> normalized device coordinates (NDC)
    m_worldToView = m_projection * m_camera.toMatrix() * m_transform.toMatrix();
}

void SceneView::selectNearestObject(const QVector3D &nearPoint, const QVector3D &farPoint)
{
    QElapsedTimer pickTimer;
    pickTimer.start();

    //compute view direction
    QVector3D d = farPoint - nearPoint;

    //create pick object, distance is a value between 0 and 1, so initialize with 2 (very far back) to be on the safe side.
    PickObject p(2.f, std::numeric_limits<unsigned int>::max());

    //now process all objects and update p to hold the closest hit
    //m_boxObjct.pick(nearPoint, d, p);
    m_object.pick(nearPoint, d, p);
    // ...other objects

    //any object accepted a pick?
    if(p.m_objectId == std::numeric_limits<unsigned int>::max()){
        return; //nothing selected
    }

    qDebug().nospace() << "Pick successful (Box #" << p.m_objectId << ", Face #" << p.m_faceId << ", t = " << p.m_dist << ") after" << pickTimer.elapsed() << " ms";

    //Mind: OpenGL-context must be current when we call this function!
    //m_boxObjct.highlight(p.m_objectId, p.m_faceId);
    m_object.highlight(p.m_objectId, p.m_faceId);
}


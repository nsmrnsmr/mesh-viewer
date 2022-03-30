#include "OpenGLwindow.h"

#include <QCoreApplication>
#include <QDebug>

#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>

OpenGLWindow::OpenGLWindow(QWindow *parent) :
    QWindow(parent),
    m_context(nullptr),
    m_debugLogger(nullptr)
{
    qDebug() << "OpenGLWindow::OpenGLWindow";
    setSurfaceType(QWindow::OpenGLSurface);
}

void OpenGLWindow::renderLater()
{
    qDebug() << "OpengGLWindow::renderLater()";
    requestUpdate();
}

void OpenGLWindow::renderNow()
{
    if(!isExposed()) return;
    // initialize on first call

    if(m_context == nullptr) initOpenGL();

    m_context->makeCurrent(this);
    // call user code
    paintGL();
    m_context->swapBuffers(this);
}

//*** protected functions ***
bool OpenGLWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void OpenGLWindow::exposeEvent(QExposeEvent */*event*/)
{
    qDebug() << "OpenGLWindow::exposeEvent()";
    renderNow();
}

void OpenGLWindow::resizeEvent(QResizeEvent *event)
{
    qDebug() << "OpenGLWindow::resizeEvent()";
    QWindow::resizeEvent(event);

    if(m_context == nullptr){
        qDebug() << "OpenGLWindow::resizeEvent() -> OpenGLWindow::initOpenGL()";
        initOpenGL();
    }

    resizeGL(width(), height());
}

void OpenGLWindow::onMessageLogged(const QOpenGLDebugMessage &msg)
{
    QString prefix;

    //Format based on serverity
    switch(msg.severity()){
    case QOpenGLDebugMessage::NotificationSeverity:
        prefix += "++++"; break;
    case QOpenGLDebugMessage::HighSeverity:
        prefix += "+++"; break;
    case QOpenGLDebugMessage::MediumSeverity:
        prefix += "++"; break;
    case QOpenGLDebugMessage::LowSeverity:
        prefix += "+"; break;
        break;
    }
    prefix += " [";

    //Format based soure
#define CASE(c) case QOpenGLDebugMessage::c: prefix += #c; break
    switch (msg.source()) {
        CASE(APISource);
        CASE(WindowSystemSource);
        CASE(ShaderCompilerSource);
        CASE(ThirdPartySource);
        CASE(OtherSource);
        CASE(InvalidSource);
    }
#undef CASE

    prefix += ":";

    //Format based on type
#define CASE(c) case QOpenGLDebugMessage::c: prefix += #c; break
    switch (msg.type()) {
        CASE(ErrorType);
        CASE(DeprecatedBehaviorType);
        CASE(UndefinedBehaviorType);
        CASE(PerformanceType);
        CASE(OtherType);
        CASE(MarkerType);
        CASE(GroupPushType);
        CASE(GroupPopType);
    }
#undef CASE

    prefix += "] ";
    qDebug().noquote().nospace() << prefix << msg.message() << "\n";
}

void OpenGLWindow::initOpenGL()
{
    qDebug() << "OpenGLWindow::initOpenGL()";
    Q_ASSERT(m_context == nullptr);

    m_context = new QOpenGLContext(this);
    m_context->setFormat(requestedFormat());
    m_context->create();

    m_context->makeCurrent(this);
    Q_ASSERT(m_context->isValid());

    initializeOpenGLFunctions();

#ifdef GL_DEBUG
    if(m_context->hasExtension(QByteArrayLiteral("GL_KHR_debug"))){
        qDebug() << "GL_KHR_debug extention available";
    }
    else {
        qWarning() << "GL_KHR_debug extension *not* available";
    }
    m_debugLogger = new QOpenGLDebugLogger(this);
    if(m_debugLogger->initialize()){
        qDebug() << "Debug Logger initialize\n";
        connect()m_debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this, SLOT(onMessageLogged(QOpenGLDebugMessage)));
    }
    qDebug() << "DepthBufferSize = " << m_context->format().depthBufferSize();
#endif

    initializeGL();
}

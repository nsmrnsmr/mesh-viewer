QT       += core gui
QT += 3dcore 3drender 3dextras opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BoxObject.cpp \
    GridObject.cpp \
    KeyboardMouseHandler.cpp \
    MyMesh.cpp \
    MyObject.cpp \
    OpenGLException.cpp \
    OpenGLWindow.cpp \
    PickLineObject.cpp \
    PickObject.cpp \
    SceneView.cpp \
    ShaderProgram.cpp \
    TestDialog.cpp \
    Transform3d.cpp \
    boxmesh.cpp \
    main.cpp

HEADERS += \
    BoxMesh.h \
    BoxObject.h \
    Camera.h \
    DebugApplication.h \
    Face.h \
    GridObject.h \
    Input.h \
    KeyboardMouseHandler.h \
    MyMesh.h \
    MyObject.h \
    OpenGLException.h \
    OpenGLwindow.h \
    PickLineObject.h \
    PickObject.h \
    SceneView.h \
    ShaderProgram.h \
    TestDialog.h \
    Transform3d.h \
    Vertex.h

win32 {
    LIBS += -lopengl32
}
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc


DISTFILES += \
    MeshViewer.pro.user

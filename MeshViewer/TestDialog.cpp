#include "TestDialog.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "SceneView.h"

TestDialog::TestDialog() :
#ifdef Q_OS_WIN
    QDialog(nullptr, Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint)
#else
    QDialog(nullptr, Qt::Window)
#endif
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3,3);
    format.setSamples(4);	// enable multisampling (antialiasing)
    format.setDepthBufferSize(24);
#ifdef GL_DEBUG_
    format.setOption(QSurfaceFormat::DebugContext);
#endif

    m_sceneView = new SceneView;
    m_sceneView->setFormat(format);

    // *** create window container widget

    QWidget *container = QWidget::createWindowContainer(m_sceneView);
    container->setFocusPolicy(Qt::TabFocus);
    container->setMinimumSize(QSize(640,400));

    // *** create the layout and insert widget container

    QVBoxLayout * vlay = new QVBoxLayout;
    vlay->setMargin(0);
    vlay->setSpacing(0);
    vlay->addWidget(container);

    // now create some buttons at the bottom

    QHBoxLayout * hlay = new QHBoxLayout;
    hlay->setMargin(0);

    QLabel * navigationInfo = new QLabel(this);
    navigationInfo->setWordWrap(true);
    navigationInfo->setText("Hold right mouse button for free mouse look and to navigate "
                            "with keys WASDQE. Hold shift to slow down. Use scroll-wheel to move quickly forward and backward.");
    hlay->addWidget(navigationInfo);

    QPushButton * closeBtn = new QPushButton(tr("Close"), this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    QPushButton * meshImportBtn = new QPushButton(tr("Open"), this);
    connect(meshImportBtn, &QPushButton::clicked, this, &TestDialog::open);

    hlay->addWidget(closeBtn);
    hlay->addWidget(meshImportBtn);
    hlay->setStretch(0,1);

    vlay->addLayout(hlay);

    setLayout(vlay);

    resize(700,450);

    container->setFocus();
}

void TestDialog::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
    QFileInfo fileInfo = QFileInfo(fileName);
    QString suffix = fileInfo.suffix();

    //SceneViewにMyObjectを定義してファイル名と拡張子を渡す
    m_sceneView->inputMyObject(fileName, suffix);
    //if(!fileName.isEmpty()) scribbleArea->openImage(fileName);
}

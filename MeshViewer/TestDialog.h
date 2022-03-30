#ifndef TESTDIALOG_H
#define TESTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>

class SceneView;

class TestDialog : public QDialog
{
    Q_OBJECT

public:
    TestDialog();

public slots:
    void open();

private:
    SceneView *m_sceneView;
};
#endif // TESTDIALOG_H

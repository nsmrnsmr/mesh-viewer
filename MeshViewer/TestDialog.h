#ifndef TESTDIALOG_H
#define TESTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QComboBox>

class SceneView;

class TestDialog : public QDialog
{
    Q_OBJECT

public:
    TestDialog();

public slots:
    void open();
    void openFragmentShader();
    void changeFragmentShader();

private:
    SceneView *m_sceneView;
    QComboBox * changeFragmentShaderCbx;
};
#endif // TESTDIALOG_H

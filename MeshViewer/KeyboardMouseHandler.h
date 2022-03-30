#ifndef KEYBOARDMOUSEHANDLER_H
#define KEYBOARDMOUSEHANDLER_H

#include <QPoint>
#include <vector>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class KeyboardMouseHandler
{
public:
    KeyboardMouseHandler();
    virtual ~KeyboardMouseHandler();

    void addRecongnizedKey(Qt::Key k);
    void clearRecognizedKeys();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    bool pressKey(Qt::Key k);
    bool releaseKey(Qt::Key k);
    bool pressButton(Qt::MouseButton btn, QPoint currentPos);
    bool releaseButton(Qt::MouseButton btn, QPoint currentPos);

    bool keyDown(Qt::Key k) const;
    bool buttonDown(Qt::MouseButton btn) const;
    bool buttonReleased(Qt::MouseButton btn);

    QPoint mouseDownPos() const{
        return m_mouseDownPos;
    }

    QPoint mouseReleasePos() const{
        return m_mouseReleasePos;
    }

    QPoint resetMouseDelta(const QPoint currentPos);
    int wheelDelta() const;
    int resetWheelDelta();
    void clearWasPressedKeyStates();

private:
    enum KeyStates {
        StateNotPressed,
        StateHeld,
        StateWasPressed
    };

    std::vector<Qt::Key> m_keys;
    std::vector<KeyStates> m_keyStates;

    KeyStates m_leftButtonDown;
    KeyStates m_middleButtonDown;
    KeyStates m_rightButtonDown;

    QPoint m_mouseDownPos;
    QPoint m_mouseReleasePos;
    int m_wheelDelta;
};

#endif // KEYBOARDMOUSEHANDLER_H

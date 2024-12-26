#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QObject>

class MyGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MyGraphicsView(QWidget *parent = nullptr);  // Constructor declaration

signals:
    void mouseMoved(const QPointF &scenePos); // Signal for mouse movement
    void mousePressed(const QPointF &scenePos); // Signal for mouse press

protected:
    void mouseMoveEvent(QMouseEvent *event) override;  // Mouse move event handler
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // MYGRAPHICSVIEW_H

#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QObject>

class MyGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MyGraphicsView(QWidget *parent = nullptr);

signals:
    void mouseMoved(const QPointF &scenePos) const;
    void mousePressed(const QPointF &scenePos) const;
    void mouseDoubleClicked(const QPointF &scenePos) const;

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif

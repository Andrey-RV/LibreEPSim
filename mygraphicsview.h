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
    // Consider adding const to the QPointF argument since it's not modified.
    void mouseMoved(const QPointF &scenePos) const;
    void mousePressed(const QPointF &scenePos) const;
    void mouseDoubleClicked(const QPointF &scenePos) const;

protected:
    // These methods are virtual, so you could add the 'virtual' keyword
    // (although 'override' implies it). It's a matter of style preference.
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // MYGRAPHICSVIEW_H

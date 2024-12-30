#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mygraphicsview.h"
#include "linedrawer.h"
#include "grid.h"
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actiongen_triggered();
    void on_actionbus_triggered();
    void on_actionline_triggered();
    void on_actiontrafo_triggered();
    void onMouseMoved(const QPointF &scenePos); // Slot for mouse movement
    void onMousePressed(const QPointF &scenePos); // Slot for mouse press
    void onMouseDoubleClicked(const QPointF &scenePos);

private:
    static constexpr qreal ZOOM_STEP = 1.1;
    static constexpr qreal MIN_ZOOM = 0.5;
    static constexpr qreal MAX_ZOOM = 2.0;

    Ui::MainWindow *ui;    
    MyGraphicsView *graphicsView = nullptr;
    QGraphicsScene *graphicsScene = nullptr;
    qreal currentZoomFactor = 1.0;
    Grid grid;
    LineDrawer *lineDrawer;
    ComponentManager *componentManager;

    void keyPressEvent(QKeyEvent *event);
    void zoomIn();
    void zoomOut();
};
#endif // MAINWINDOW_H

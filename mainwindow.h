#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mygraphicsview.h"
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
    void onMouseMoved(const QPointF &scenePos); // Slot for mouse movement
    void onMousePressed(const QPointF &scenePos); // Slot for mouse press
    void onMouseDoubleClicked(const QPointF &scenePos);

private:
    Ui::MainWindow *ui;    
    MyGraphicsView *graphicsView = nullptr;
    QGraphicsScene *graphicsScene = nullptr;

    void keyPressEvent(QKeyEvent *event);
    void createComponent(const QString& imagePath);
    QPointF findNearestTerminal(const QPointF &point, bool &snapped);
    void updateImagePosition();
    void zoomIn();
    void zoomOut();

    // Component placement
    struct Component {
        QGraphicsPixmapItem *item;
        QList<QPointF> terminals; // List of terminal positions
    };
    QList<Component> components;
    QGraphicsPixmapItem *currentComponent = nullptr;
    QList<QGraphicsPixmapItem*> componentsItems;
    bool componentIsMoving = false;
    QTimer *moveTimer = nullptr; // Timer for updating image position

    // Line drawing
    bool lineDrawing = false;
    QGraphicsLineItem* currentLine;
    QPointF startPoint;
    Qt::Orientation constraintDirection = Qt::Horizontal;

    // Screen zooming
    double currentZoomFactor = 1.0;
    const double maxZoomFactor = 1.2;
    const double minZoomFactor = 0.2;

    void placeComponent();
    void drawNextLine(const QPointF &scenePos);
};
#endif // MAINWINDOW_H

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
    // void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event);

protected:
    void createComponent(const QString& imagePath);
    void updateImagePosition();
    void zoomIn();
    void zoomOut();

private:
    Ui::MainWindow *ui;    
    MyGraphicsView *graphicsView = nullptr;
    QGraphicsScene *graphicsScene = nullptr;

    double currentZoomFactor = 1.0; // Start at 100% zoom
    const double maxZoomFactor = 1.2; // Maximum 500% zoom
    const double minZoomFactor = 0.2; // Minimum 20% zoom

    QGraphicsPixmapItem *currentItem = nullptr; // Currently dragged item
    QList<QGraphicsPixmapItem*> componentsItems; // List of all items
    bool componentIsMoving = false;
    QPoint lastMousePos;

    QTimer *moveTimer = nullptr; // Timer for updating image position

    bool lineDrawing = false;
    QGraphicsLineItem* currentLine;
    QPointF startPoint;
};
#endif // MAINWINDOW_H

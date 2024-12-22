#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void updateImagePosition();

    void on_actionbus_triggered();

protected:
    void createComponent(const QString& imagePath);
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;
    QList<QLabel*> componentsLabels;  // List to store all draggable labels
    QLabel* currentLabel = nullptr;
    bool componentIsMoving = false;
    QPoint lastMousePos;
    QTimer *moveTimer = nullptr; // Timer for updating image position
};
#endif // MAINWINDOW_H

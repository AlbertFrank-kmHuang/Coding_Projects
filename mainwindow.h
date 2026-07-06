#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    QPushButton* startButton;
    int gridHeight = 16;
    int gridLength = 30;

    //布局
    QWidget *central;
    QVBoxLayout *mainVerticalLayout;
    QHBoxLayout *buttonsLayout;
    QGridLayout *gridLayout = nullptr;



private slots:
    void generateGrid();
};
#endif // MAINWINDOW_H

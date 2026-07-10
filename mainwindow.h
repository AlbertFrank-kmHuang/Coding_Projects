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
    int mineNumber = 99;

    //布局
    QWidget *central = nullptr;




private slots:
    void generateGrid();

    void onCellClicked();
};
#endif // MAINWINDOW_H
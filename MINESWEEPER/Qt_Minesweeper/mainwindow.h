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
    QGridLayout* gridLayout;
    int gridHeight = 16;
    int gridLength = 30;
    int mineNumber = 99;

    bool firstClick = true;

    void expand(int row, int col);

    QWidget *central = nullptr;

    bool gameover = false;
    bool gamewin = false;

    void gameoverFunc();
    void gamewinFunc();

    void touchMineFunc(int row, int col);

    QPushButton* modeButton;

    int  gamemode = 0;
//游戏模式：
//0:默认模式（Sudden Death）
//1:cs拆弹模式(Counter Strike)
//
//2:扣血模式(Explosion Proof)
//3:连锁爆炸模式(Chain Reaction)

    void expandExplode(int row, int col);


    int health = 100;

    int minesRemain = mineNumber;

    int flaggedCellNumber = 0;

    int minesTouched = 0;

    void gamewinTest();


private slots:
    void onCellLeftClicked();

    void onCellRightClicked();

    void onStartButtonClicked();

    void onModeButtonClicked();
};
#endif // MAINWINDOW_H

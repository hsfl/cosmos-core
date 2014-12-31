#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#define NUM_TESTS       3

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static char names[NUM_TESTS][11];
    
private slots:

    void on_ChangeFolder_clicked();

    void on_CollapseAll_clicked();

    void on_SelectAll_clicked();

    void on_DeselectAll_clicked();

    void recountTests();

    void on_RunAll_clicked();

    void on_RunSelected_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

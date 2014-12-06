#ifndef REARTH_TEST_H
#define REARTH_TEST_H

#include <QWidget>
#include "testcontainer.h"
#include "ui_rearth_test.h"

namespace Ui {
    class rearth_test;
}

class rearth_test : public QWidget
{
    Q_OBJECT

public:
    explicit rearth_test(TestContainer* parent = 0);
    ~rearth_test();

private slots:
    void on_TestList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_SelectMax_clicked();

    void on_SelectMin_clicked();

private:
    Ui::rearth_test *ui;

    int MaxErrIndex;
    int MinErrIndex;
};

#endif // REARTH_TEST_H

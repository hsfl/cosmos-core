#ifndef TESTCONTAINER_H
#define TESTCONTAINER_H

#include <QWidget>
#include <QListWidget>
#include "convertlib.h"
#include "mathlib.h"
#include <stdio.h>
#include "mainwindow.h"

namespace Ui {
    class TestContainer;
}

class TestContainer : public QWidget
{
    Q_OBJECT

public:
    explicit TestContainer(MainWindow* parent = 0, int testIndex = 0);
    ~TestContainer();

    void toggleHidden(bool show);

    void setError(double min, double max, double avg);

    void setSelected(bool selected);

    void runTest();

    /*My own little set of file reading functions*/
    static void skipLines(FILE *fp, int lines);//skips 'lines' many lines, stops at EOF if necessary
    static int openFileLine(FILE **fp, const char filename[], int startLine);//opens a file for reading and moves the pointer to startLine

    /*Set of functions to write cosmos datastructures to QListWidgetItems*/
    static void listWriteRVec(QListWidgetItem *listItem, rvector vect, int *roleIndex);
    static void listWriteCartpos(QListWidgetItem *listItem, cartpos pos, int *roleIndex);
    static void listWriteKepstruc(QListWidgetItem *listItem, kepstruc kep, int *roleIndex);

    static double angleDelta(double angle1, double angle2); //compares the two angles (radians) returns angle1-angle2
    static kepstruc keplerDelta(kepstruc kep1, kepstruc kep2); //compares two kepstrucs and returns kep1-kep2

    bool isSelected;
    bool completed;
    bool hidden;
    double MinError;
    double MaxError;
    double AvgError;
    int number;

private slots:
    void on_Expand_clicked();

    void on_checkBox_stateChanged(int arg1);

signals:
    void selectionChanged();

private:
    Ui::TestContainer *ui;

    void fadeBanner(bool faded);

};

#endif // TESTCONTAINER_H

#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <QWidget>
#include <QtGui>
#include <QUiLoader>
#include <QtUiTools>
#include <QXmlStreamWriter>
#include <QFile>
#include <QSizePolicy>

//namespace Ui {
//class DataView;
//}

class DataView : public QWidget
{
    Q_OBJECT

public:
    DataView(QWidget *parent = 0);

    QLineEdit *textField;

    ~DataView();

    
signals:
    
public slots:
    
};

#endif // DATAVIEW_H

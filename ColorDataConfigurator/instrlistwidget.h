#ifndef INSTRLISTWIDGET_H
#define INSTRLISTWIDGET_H

#define DEBUG
#ifdef DEBUG
    #include <QDebug>
#endif

#include <QVector>
#include <QString>

#include "instructionwidget.h"

class InstrListWidget : public QWidget
{
    Q_OBJECT

public:
    InstrListWidget(QWidget *parent = nullptr);
    InstrListWidget(const QString* src, const uint8_t* mode, QWidget *parent = nullptr);
    void setSrcRef(const QString* src = nullptr);
    void setDispModeRef(const uint8_t* mode = nullptr);

    QVector<InstructionWidget*> instrList;

private:
    void init();

    const QString* srcRef;
    const uint8_t* dispModeRef = nullptr;
    //QScrollArea* listBox;     //better implemented outside

protected:
    void paintEvent(QPaintEvent* event);

signals:
    void parseError(QString err);
    void elemSelected(const RGBInstruction* src);

public slots:
    void buildList();
    void clearList();
    QString exportList() const;

};

#endif // INSTRLISTWIDGET_H

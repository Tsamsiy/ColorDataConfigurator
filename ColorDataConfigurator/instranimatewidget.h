#ifndef INSTRANIMATEWIDGET_H
#define INSTRANIMATEWIDGET_H

#define DEBUG
#ifdef DEBUG
    #include <QDebug>
#endif

#include <QWidget>
#include <QPainter>
#include <QGradient>
#include <QColor>
#include <QPaintEvent>
#include <QPoint>
#include <QRect>

#include <QTimer>

#include "instructionwidget.h"

class InstrAnimateWidget : public QWidget
{
    Q_OBJECT
public:
    InstrAnimateWidget(QWidget *parent = nullptr);
    InstrAnimateWidget(const RGBInstruction* instr = nullptr, QWidget *parent = nullptr);

    void setSourceRef(const RGBInstruction* src);
    void setLoop(bool l);

    void startAnim();

private:
    const RGBInstruction* instr = nullptr;
    QTimer*  instrTmr;
    QTimer*  rTmr;
    QTimer*  gTmr;
    QTimer*  bTmr;
    void init();

    bool loop = false;

    QColor curColor;
    int16_t deltaR;
    int16_t deltaG;
    int16_t deltaB;

    void setup();
    void refrR();
    void refrG();
    void refrB();

protected:
    void paintEvent(QPaintEvent* event);

signals:
    void done();
    //void currentColor(QColor c);

public slots:
    void start(const RGBInstruction& instr);
    void stop();
    void clear();   //sets the view area to black
};

#endif // INSTRANIMATEWIDGET_H

#ifndef INSTRUCTIONWIDGET_H
#define INSTRUCTIONWIDGET_H

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

#include <QLabel>
#include <QSpinBox>

#include <QMenu>
#include <QAction>
#include <QColorDialog>

//label size definitions
#define PixTxtHeight 8
//#define PixTxtTxtSpace 0
#define PixTxtTxtLine 6
#define PixTxtCharSize 8
#define PixXSpace 8
#define PixYSpace 5
#define PixParam1Tab 70
#define PixParam2Tab 150
#define PixTotalWdgtH (2*PixTxtHeight + 2*PixTxtTxtLine + 2*PixYSpace)
#define PixTotalWdgtW (PixParam2Tab + 4*PixTxtCharSize + 2*PixXSpace)

#include "rgbinstruction.h"

//#####################################################################################################################################################################################
class InstructionWidget : public QWidget
{
    Q_OBJECT

public:
    InstructionWidget(QWidget* parent = nullptr);
    InstructionWidget(const QString& str, QWidget* parent = nullptr);
    InstructionWidget(const RGBInstruction& instr, QWidget* parent = nullptr);
    InstructionWidget(uint8_t com, RGBInstruction::HEXColor c1, RGBInstruction::HEXColor c2, uint16_t x, uint16_t t, QWidget* parent = nullptr);

    enum DispStatus
    {
        NoFocus,
        HasFocus,
        Error
    };

    void setDispMode(const uint8_t* mode = nullptr);
    void setDispMode(const uint8_t& mode);

    void setStatus(uint8_t s = InstructionWidget::DispStatus::NoFocus);

    void setContent(const RGBInstruction& instr);
    void setContent(const RGBInstruction::HEXColor& c1, const RGBInstruction::HEXColor& c2);
    void setContent(uint8_t com, RGBInstruction::HEXColor c1, RGBInstruction::HEXColor c2, uint16_t x, uint16_t t);
    const RGBInstruction* content();

    void setPriColor(QColor c);
    void setSecColor(QColor c);

private:
    RGBInstruction instr;
    //pointer to global mode variable outside the objects (prevents setting of every object in a list)
    const uint8_t* globDispMode = nullptr;      //(2:BIN, 8:OCT,) 10:DEC, 16:HEX    //default is HEX
    uint8_t dispMode = 16;     //used only of globDispMode is null
    uint8_t status = NoFocus;

    void initElements();
    QLabel* dispComText;
    QLabel* dispC1Text;
    QLabel* dispC2Text;
    QLabel* dispXText;
    QLabel* dispTText;

    QColorDialog* c1Pick;
    QColorDialog* c2Pick;

protected:
    void paintEvent(QPaintEvent* event);
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);

    void showContextMenu(const QPoint& pos);

signals:
    void selected(const RGBInstruction& instr);

public slots:
    void pickPriColor();
    void pickSecColor();
};

#endif // INSTRUCTIONWIDGET_H

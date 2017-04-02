#ifndef COLOR_SELECTOR_H
#define COLOR_SELECTOR_H

#include <vector>
#include <stack>
#include <map>
#include <string>
#include <QWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QGridLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QDialog>
#include <QDoubleSpinBox>
#include "render_area.h"

class ColorScale {

    friend class ColorSelector;

    std::vector<double> scale;
    std::vector<QColor> color;
    double minimum,maximum;

public:

    ColorScale(double min = 0.0, double max = 1.0, int = 0);
    ColorScale(QString);

    QString toStr();
    void setScale(const std::vector<double> &);
    void setColor(const std::vector<QColor> &);
    void setRange(double,double);
    double getMin();
    double getMax();

    bool isOk();

    QColor getColor(double);

};

class ColorSelector : public QDialog
{
    Q_OBJECT

public:
    explicit ColorSelector(ColorScale *, QString *, std::string name = "", QWidget *parent = 0);
    ~ColorSelector();
    virtual void resizeEvent(QResizeEvent *) ;
    virtual void paintEvent(QPaintEvent *) ;

    virtual QSize minimumSizeHint() const ;
    virtual QSize sizeHint() const ;
    void setCS(ColorScale * c) {cols=c;}
    ColorScale getCS();

protected:


private:

    QString *defaultDir;
    ColorScale *cols;
    ColorScale oldCols;
    bool dragStrip;
    QColor currentColor;
    double currentDarkness;
    double currentAlpha;
    double currentColorAngle;
    double currentColorRadius;
    double minimumScale,maximumScale;
    std::string nameScale,currentColorString;
    std::vector<double> colorScaleList;   // they have to be sorted
    std::vector<QColor> colorValueList;
    int currentIndex;
    std::stack<bool> state1,state2,state3,state4,state5,state6,state7,state8,state9,state10;

    QGridLayout *mainLayout;

    //RatioRenderArea *colorCircleRA;
    RenderArea *colorCircleRA;
    RenderArea *colorDarknessRA;
    RenderArea *colorStripRA;
    RenderArea *colorAlphaRA;
    RenderArea *colorBoxRA;
    RenderArea *transparRA;
    QPushButton *addCheckColorButton;
    QPushButton *removeCheckColorButton;
    QPushButton *exitButton;
    QPushButton *linearButton;
    QPushButton *quadraticButton;
    QPushButton *exponentialButton;
    QPushButton *rainbowButton;
    QPushButton *reverseButton;
    QSlider *expConstant;
    QPushButton *cancelButton;
    QPushButton *spreadScaleButton;
    QPushButton *savePaletteButton;
    QPushButton *loadPaletteButton;
    QSpinBox *brightnessLineEdit;
    QSpinBox *alphaLineEdit;
    QSpinBox *redchLineEdit;
    QSpinBox *greenchLineEdit;
    QSpinBox *bluechLineEdit;
    QSpinBox *alphachLineEdit;
    QSpinBox *currentIndexLineEdit;
    QDoubleSpinBox *currentScaleLineEdit;
    QLabel *currentColorLabel;
    QLabel *currentRedLabel;
    QLabel *currentGreenLabel;
    QLabel *currentBlueLabel;
    QLabel *currentAlphaLabel;
    QLabel *colorWheelLabel;
    QLabel *brightnessLabel;
    QLabel *alphaLabel;
    QLabel *colorScaleLabel;
    QLabel *transparencyLabel;
    QDoubleSpinBox *minimumScaleEdit;
    QDoubleSpinBox *maximumScaleEdit;
    QLabel *currentIndexLabel;
    QLabel *currentScaleLabel;
    QFrame *hSeparator;

    void updateRender();
    void updateFields();
    void updateFieldsColor();
    void updateFieldsDAndA();
    void plotCurrentColorCircle();
    void plotCurrentColorDarkness();
    void plotCurrentColorAlpha();
    void plotCurrentColorScale();
    void plotTranspar();
    void sort();

    QColor getColorWheel();
    void block();
    void release();

public slots:

    void setColorFromFields();
    void setFieldsFromColor();
    void darknessPressed();
    void alphaPressed();
    void circlePressed();
    void stripPressed();
    void stripReleased();
    void addColor();
    void removeColor();
    void saveAndClose();
    void cancelAndClose();
    void changeCurrentIndex();
    void changeCurrentScale();
    void changeCurrentRange();
    void spreadScale();
    int savePalette();
    int loadPalette();
    void linearTransparency();
    void quadraticTransparency();
    void exponentialTransparency();
    void rainbow();
    void reverse();

signals:

    void accepted();

};

#endif // COLOR_SELECTOR_H

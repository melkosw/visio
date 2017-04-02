#ifndef TEXT_H
#define TEXT_H

#include <QString>
#include <QFont>
#include <QColor>

class Text {

public:

    double relPosX,relPosY;
    QString textString;
    QFont font;
    QColor color;

    Text(double x =-1,
         double y =-1,
         QString str ="",
         QFont f =QFont("system",10,QFont::Bold),
         QColor c =QColor(Qt::green))   :relPosX(x),relPosY(y),textString(str),font(f),color(c) {}

    Text(QString);
    QString toStr();

};

#endif // TEXT_H


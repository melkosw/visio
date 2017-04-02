#include "text.h"
#include <QTextStream>
#include <QString>


QString Text::toStr() {

    QString output;
    QTextStream store(&output);

    store << relPosX << '\n';
    store << relPosY << '\n';
    store << color.red() << '\n';
    store << color.green() << '\n';
    store << color.blue() << '\n';
    store << font.family() << '\n';
    store << font.pointSize() << '\n';
    store << font.weight() << '\n';
    store << int(font.italic()) << '\n';

    store << textString.toLocal8Bit().constData();
    store << "<END_OF_TEXT>\n";

    return output;

}

Text::Text(QString input) {

    QTextStream store(&input);
    QString info;
    int r,g,b,ps,w,it;
    QString fam,l,rr,line;

    store >> info;
    relPosX=info.toDouble();
    store >> info;
    relPosY=info.toDouble();
    store >> info;
    r=info.toInt();
    store >> info;
    g=info.toInt();
    store >> info;
    b=info.toInt();
    store >> info;
    fam=info;
    store >> info;
    ps=info.toInt();
    store >> info;
    w=info.toInt();
    store >> info;
    it=info.toInt();
    color=QColor(r,g,b);
    font=QFont(fam,ps,w,it);


    l=store.readLine();
    rr=l.right(13);
    line="";
    while(rr!="<END_OF_TEXT>")  {
        l=store.readLine();
        rr=l.right(13);
        if (rr!="<END_OF_TEXT>")
            line=line+l+'\n';
        else
            line=line+l.left(l.size()-13);
    }
    textString=line;

}

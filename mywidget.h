#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QGradient>

#include "quadcontrolpolygon.h"


class MyWidget : public QWidget
{
    Q_OBJECT

public:
    MyWidget(QWidget *parent = 0);

    const QVector<QuadControlPolygon*>& polygons() const {
        return points;
    }

    void addPolygon(const QPointF& center);
    void removePolygon(int polygonIndex);

    void updateSelection(int polygonIndex = -1, int controlPointIndex = -1) {
        selectedControlPoint = controlPointIndex;

        if (selectedPolygon != -1)
            points.at(selectedPolygon)->setSelected(false);

        selectedPolygon = polygonIndex;
        if (selectedPolygon != -1)
            points.at(selectedPolygon)->setSelected(true);

        qDebug("Selection: %d, %d", selectedPolygon, selectedControlPoint);
    }

    bool movePolygonDelta(int polygonIndex, QPointF delta);
    bool movePointDelta(int polygonIndex, int pointIndex, QPointF delta);

public slots:
    void selectPolygon(int index) {
        updateSelection(index);
        update();
    }



signals:
    void selectionUpdated(int polygonIndex, int controlPointIndex);
    void polygonChanged();

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    //ControlPoints* findPoint(QPointF);
    int selectControlPoints(const QPointF& clickPos);


protected:
    bool mouseMoved_;
    qreal radius_;

    int selectedPolygon;
    int selectedControlPoint;

    QPoint oldMousePos;
    QVector<QuadControlPolygon*> points;
};

#endif // MYWIDGET_H

#include "mywidget.h"

#include <QtGui>
#include <QLinearGradient>
#include <QRadialGradient>



MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
    , mouseMoved_(false)
    , radius_(2)
    , selectedPolygon(-1)
    , selectedControlPoint(-1)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::transparent);
    setAutoFillBackground(true);
    setPalette(pal);

    points.clear();

    setMinimumSize(400, 400);
}

//ColorRange* MyWidget::findPoint(QPointF pos)
//{
//    for (int i = 0; i < points.size(); ++i) {
//        if (points[i]->included(pos)) {
//            return points[i];
//        }
//    }
//
//    return nullptr;
//}

int MyWidget::selectControlPoints(const QPointF& clickPos)
{
    for (int idx = 0; idx < points.size(); ++idx) {
        if (points[idx]->isPointInside(clickPos))
            return idx;
    }

    return -1;
}

void MyWidget::resizeEvent(QResizeEvent *event)
{
    for (QuadControlPolygon* const qcp : points)
        qcp->resize(event);

    QWidget::resizeEvent(event);
}

void MyWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    for (int i = 0; i < points.size(); ++i)
        points.at(i)->paintPoints(
#ifndef NDEBUG
            QString::number(i)
#endif
        );

    QWidget::paintEvent(event);
}

void MyWidget::mousePressEvent(QMouseEvent *event)
{
    mouseMoved_ = false;

    updateSelection(-1, -1);

    QPoint clickPos = event->pos();
    for (int i = 0; i < points.size(); ++i) {
        QuadControlPolygon* qcp = points.at(i);
        int idx = qcp->selectControlPoint(clickPos);
        if (idx != -1) {
            updateSelection(i, idx);
            break;
        }
        else if (qcp->isPointInside(clickPos)) {
            updateSelection(i, -1);
            break;
        }
    }

    oldMousePos = clickPos;

    emit selectionUpdated(selectedPolygon, selectedControlPoint);

    QWidget::mousePressEvent(event);
}

void MyWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint newMousePos = event->pos();
    mouseMoved_ = true;

    if (selectedPolygon != -1 && event->buttons() & Qt::LeftButton) {
        if (selectedControlPoint == -1) {
            movePolygonDelta(selectedPolygon, newMousePos - oldMousePos);
        }
        else {
            movePointDelta(selectedPolygon, selectedControlPoint, newMousePos - oldMousePos);
//            points.at(selectedPolygon)->movePointDelta(selectedControlPoint, newMousePos - oldMousePos);
        }
        update();
    }

    oldMousePos = newMousePos;

    QWidget::mouseMoveEvent(event);
}

void MyWidget::mouseReleaseEvent(QMouseEvent *event)
{
    selectedControlPoint = -1;

    if (!mouseMoved_ && event->button() & Qt::RightButton)
    {
        if (selectedPolygon == -1)
            addPolygon(event->pos());
        else
            removePolygon(selectedPolygon);
    }

    mouseMoved_ = false;
    update();
    QWidget::mouseReleaseEvent(event);
}

void MyWidget::addPolygon(const QPointF& center)
{
    QuadControlPolygon* cr = QuadControlPolygon::fromCenterPoint(this, center);
    points.push_back(cr);
    emit polygonChanged();

    updateSelection(points.size() - 1, -1);
    emit selectionUpdated(selectedPolygon, selectedControlPoint);
    qDebug("Point added.");
}

void MyWidget::removePolygon(int polygonIndex)
{
    bool askToRemove = true;
    if (polygonIndex < 0 || polygonIndex >= points.size()) {
        qDebug("%s: %s", __FUNCTION__, "Invalid index");
    }
    else {
        if (askToRemove) {
            QMessageBox::StandardButton reply = QMessageBox::question(this, 
                "Delete", 
                "Are you sure to delete?\nThis is not reversible.", 
                QMessageBox::Ok | QMessageBox::Cancel);

            if (reply == QMessageBox::Cancel) {
                return;
            }
        }
        updateSelection(-1, -1);
        points.remove(polygonIndex);

        emit polygonChanged();
        qDebug("Point removed.");
    }
}

bool MyWidget::movePolygonDelta(int polygonIndex, QPointF delta)
{
    QPolygonF newPolygon = polygons().at(polygonIndex)->tryToMoveAllPointsDelta(delta);
    QRectF newBoundingBox = newPolygon.boundingRect();

    std::vector<QuadControlPolygon*> duplicated = polygons().toStdVector();
    duplicated.erase(duplicated.begin() + polygonIndex);

    for (int i = 0; i < duplicated.size(); ++i) {
        if (duplicated.at(i)->getBoundingBox().intersects(newBoundingBox)) {
            if (static_cast<ControlPoints*>(duplicated.at(i))->isOverlap(newPolygon)) {
                return false;
            }
        }
    }
    polygons().at(polygonIndex)->setPoints(newPolygon);
    return true;
}

bool MyWidget::movePointDelta(int polygonIndex, int pointIndex, QPointF delta)
{
    QPolygonF newPolygon = polygons().at(polygonIndex)->tryToMovePointDelta(pointIndex, delta);
    QRectF newBoundingBox = newPolygon.boundingRect();

    std::vector<QuadControlPolygon*> duplicated = polygons().toStdVector();
    duplicated.erase(duplicated.begin() + polygonIndex);

    for (int i = 0; i < duplicated.size(); ++i) {
        if (duplicated.at(i)->getBoundingBox().intersects(newBoundingBox)) {
            if (static_cast<ControlPoints*>(duplicated.at(i))->isOverlap(newPolygon)) {
                return false;
            }
        }
    }
    polygons().at(polygonIndex)->movePointDelta(pointIndex, delta);
}

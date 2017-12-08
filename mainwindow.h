#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MyWidget;
class QComboBox;
class QPushButton;
class QRadioButton;
class QuadControlPolygon;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateGradientType();
    void updatePolygonList();
    void updateBackgroundType() {};

    void updateCenterColor();
    void updateEdgeColor();

    void updatePanelInfo(int polygonIndex);
    void updateProperties();

private:
    MyWidget* widget;

    QComboBox *combo_polygons;
    QRadioButton *rb_solid;
    QRadioButton *rb_linear;
    QRadioButton *rb_radial;

    QPushButton* bt_color_1;
    QPushButton* bt_color_2;

    QuadControlPolygon* selectedPolygon;
};

#endif // MAINWINDOW_H

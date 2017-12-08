#include "mainwindow.h"
#include "mywidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    selectedPolygon = nullptr;

    combo_polygons = new QComboBox;
    combo_polygons->setVisible(false);

    // Select type
    rb_solid  = new QRadioButton(tr("Solid"));
    rb_linear = new QRadioButton(tr("Linear"));
    rb_radial = new QRadioButton(tr("Radial"));

    rb_solid->setChecked(true);

    QHBoxLayout *hl_type = new QHBoxLayout;
    hl_type->addWidget(rb_solid);
    hl_type->addWidget(rb_linear);
    hl_type->addWidget(rb_radial);

    QGroupBox *gb_type = new QGroupBox(tr("Background type"));
    gb_type->setLayout(hl_type);


    // Color
    bt_color_1 = new QPushButton;
    bt_color_1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    bt_color_1->setEnabled(false);

    bt_color_2 = new QPushButton;
    bt_color_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    bt_color_2->setEnabled(false);

    QGridLayout *gl_color = new QGridLayout;
    gl_color->addWidget(new QLabel(tr("Center: ")), 0, 0, Qt::AlignRight);
    gl_color->addWidget(new QLabel(tr("Edge: ")), 1, 0, Qt::AlignRight);
    gl_color->addWidget(bt_color_1, 0, 1);
    gl_color->addWidget(bt_color_2, 1, 1);

    QGroupBox *gb_color = new QGroupBox(tr("Color"));
    gb_color->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    gb_color->setLayout(gl_color);

    QVBoxLayout* vb_prop = new QVBoxLayout;
    vb_prop->setSizeConstraint(QLayout::SetMinAndMaxSize);
    vb_prop->addWidget(gb_type);
    vb_prop->addWidget(gb_color);

    QGroupBox *gb_prop = new QGroupBox(tr("Properties"));
    gb_prop->setLayout(vb_prop);

    // Panel
    QVBoxLayout *vb_panel = new QVBoxLayout;
    vb_panel->setSizeConstraint(QLayout::SetMinAndMaxSize);
#ifndef NDEBUG
    combo_polygons->setVisible(true);
    vb_panel->addWidget(combo_polygons);
#endif
    vb_panel->addWidget(gb_prop);
    vb_panel->addStretch();

    QScrollArea *sc = new QScrollArea;
    sc->setLayout(vb_panel);


    // 
    widget = new MyWidget(this);

    QHBoxLayout* hl_main = new QHBoxLayout;
    hl_main->addWidget(sc);
    hl_main->addWidget(widget);


    // Main
    QWidget* mainWidget = new QWidget;
    mainWidget->setLayout(hl_main);
    setCentralWidget(mainWidget);


    // Connection
    connect(rb_solid, SIGNAL(clicked(bool)), this, SLOT(updateGradientType()));
    connect(rb_linear, SIGNAL(clicked(bool)), this, SLOT(updateGradientType()));
    connect(rb_radial, SIGNAL(clicked(bool)), this, SLOT(updateGradientType()));

    connect(bt_color_1, SIGNAL(clicked()), this, SLOT(updateCenterColor()));
    connect(bt_color_2, SIGNAL(clicked()), this, SLOT(updateEdgeColor()));
    connect(widget, SIGNAL(selectionUpdated(int, int)), this, SLOT(updatePanelInfo(int)));
    connect(widget, SIGNAL(polygonChanged()), this, SLOT(updatePolygonList()));
    connect(combo_polygons, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePanelInfo(int)));
    connect(combo_polygons, SIGNAL(currentIndexChanged(int)), widget, SLOT(selectPolygon(int)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateGradientType()
{
    if (selectedPolygon) {
        if (rb_solid->isChecked()) selectedPolygon->setGradientType(QuadControlPolygon::Solid);
        if (rb_linear->isChecked()) selectedPolygon->setGradientType(QuadControlPolygon::Linear);
        if (rb_radial->isChecked()) selectedPolygon->setGradientType(QuadControlPolygon::Radial);
        updateProperties();
        widget->update();
    }
}

void MainWindow::updatePolygonList()
{
    combo_polygons->clear();
    for (int i = 0; i < widget->polygons().size(); ++i)
        combo_polygons->addItem(QString::number(i));
}

void MainWindow::updateCenterColor()
{
    if (selectedPolygon) {
        QColorDialog colorDialog;
        colorDialog.setCurrentColor(selectedPolygon->getCenterColor());
        colorDialog.setOption(QColorDialog::ShowAlphaChannel, true);
        colorDialog.exec();

        QColor color = colorDialog.selectedColor();
        if (color.isValid()) {
            selectedPolygon->setCenterColor(color);
            updateProperties();
        }
    }
}

void MainWindow::updateEdgeColor()
{
    if (selectedPolygon) {
        QColorDialog colorDialog;
        colorDialog.setCurrentColor(selectedPolygon->getEdgeColor());
        colorDialog.setOption(QColorDialog::ShowAlphaChannel, true);
        colorDialog.exec();

        QColor color = colorDialog.selectedColor();
        if (color.isValid()) {
            selectedPolygon->setEdgeColor(color);
            updateProperties();
        }
    }
}

void MainWindow::updatePanelInfo(int polygonIndex)
{
    if (polygonIndex == -1)
        selectedPolygon = nullptr;
    else
        selectedPolygon = widget->polygons().at(polygonIndex);
    combo_polygons->setCurrentIndex(polygonIndex);
    updateProperties();
}

void MainWindow::updateProperties()
{
    qDebug() << "updateProperies";
    if (selectedPolygon) {
        rb_solid->setEnabled(true);
        rb_linear->setEnabled(true);
        rb_radial->setEnabled(true);

        bt_color_1->setEnabled(true);
        bt_color_2->setEnabled(true);

        switch (selectedPolygon->getGradientType()) {
        case QuadControlPolygon::Solid:
            rb_solid->setChecked(true);
            bt_color_2->setEnabled(false);
            break;
        case QuadControlPolygon::Linear:
            rb_linear->setChecked(true);
            break;
        case QuadControlPolygon::Radial:
            rb_radial->setChecked(true);
            break;
        }

        bt_color_1->setStyleSheet(QString("background-color: %1").arg(selectedPolygon->getCenterColor().name()));
        bt_color_2->setStyleSheet(QString("background-color: %1").arg(selectedPolygon->getEdgeColor().name()));
    }
    else {
        rb_solid->setEnabled(false);
        rb_linear->setEnabled(false);
        rb_radial->setEnabled(false);

        bt_color_1->setEnabled(false);
        bt_color_2->setEnabled(false);
    }
}

#include "myglwidget.h"
#include "myglwidget.h"

#include <QFile>
#include <QTextStream>

QString loadFileText(QString filepath)
{
    QFile file(filepath);
    file.open(QFile::ReadOnly | QFile::Text);
    QString text = QTextStream(&file).readAll();
    file.close();

    return text;
}

GLuint ShaderProgram::compile(QString vertexFilePath, QString fragmentFilePath)
{
    QString vertexSource = loadFileText(vertexFilePath);
    QString fragSource = loadFileText(fragmentFilePath);

    qDebug() << vertexSource;
    qDebug() << fragSource;
}

void MyGLWidget::initializeGL()
{
    // initialize shader program

    // initialize VAO, VBO

    // initialize texture
}

void MyGLWidget::paintGL()
{

}

void MyGLWidget::resizeGL(int width, int height)
{

}

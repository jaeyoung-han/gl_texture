#ifndef MY_GL_WIDGET_H
#define MY_GL_WIDGET_H

#include <QGLWidget>

class ShaderProgram
{
public:
    static GLuint compile(QString vertexFilePath, QString fragmentFilePath);
};

class MyGLWidget : public QGLWidget
{
    Q_OBJECT

public:
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
};

#endif // MY_GL_WIDGET_H

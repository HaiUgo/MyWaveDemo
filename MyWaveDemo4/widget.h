#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtCharts>
#include <QList>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QChart>
#include <QChartView>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void initSplineWaveChart(); //初始化图表
    void setSplineChartMargins(QChart *chart, int margin);

    bool eventFilter(QObject *obj,QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::Widget *ui;

    QChart *chart;      //图例
    QValueAxis *axisX;  //X坐标轴
    QValueAxis *axisY;  //Y坐标轴

    QSplineSeries *splineSeries;  //曲线序列
    QScatterSeries *scatterSeries; //点序列

    QStringList originList;    //CSV数据列表
    QString origin;

    QTimer *drawTimer;  //定时器
    bool isStoppingTimer; //判断定时器是否开启

    QVector<QPointF> pointBuffer;

    int originListSize;    //CSV数据长度
    int originListIndex;    //当前CSV数据索引

    bool isClickingChart; //chart图表鼠标移动事件的开关
    int xOld;    //chart图表鼠标移动的X轴距离
    int yOld;    //chart图表鼠标移动的Y轴距离

    void drawSplineWave(int axis_x, qint16 data);

private slots:
    void displayButtonClick(); //显示按钮
    void stopButtonClick();    //暂停按钮
    void timeOutEvent();  //定时器超时事件

};
#endif // WIDGET_H

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

//using namespace QtCharts;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void initSplineWaveChart(); //初始化图表
    void initSplineWaveChart2();

    bool eventFilter(QObject *obj,QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void charViewEventFilter(QEvent *event,QChart *tempChart); //chartView事件过滤封装函数
private slots:
    void displayButtonClick();
    void timeOutEvent();  //定时器超时事件
    void stopButtonClick();    //暂停按钮
private:
    Ui::Widget *ui;

    QVector<QPointF> pointBuffer;
    QTimer *drawTimer;  //定时器
    bool isStoppingTimer; //判断定时器是否开启

    QStringList originList;    //CSV数据列表
    QString origin;

    int originListSize;    //CSV数据长度
    int originListIndex;    //当前CSV数据索引

    QSplineSeries *splineSeries;
    QScatterSeries *scatterSeries;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;

    QSplineSeries *splineSeries2;
    QScatterSeries *scatterSeries2;
    QChart *chart2;
    QValueAxis *axisX2;
    QValueAxis *axisY2;

    QSplineSeries *splineSeries3;
    QScatterSeries *scatterSeries3;
    QChart *chart3;
    QValueAxis *axisX3;
    QValueAxis *axisY3;

    QChartView *view;
    QChartView *view2;
    QChartView *view3;

    void drawSplineWave(int axis_x, double data); //绘制曲线图

    bool isClickingChart; //chart图表鼠标移动事件的开关
    int xOld;    //chart图表鼠标移动的X轴距离
    int yOld;    //chart图表鼠标移动的Y轴距离
};
#endif // WIDGET_H

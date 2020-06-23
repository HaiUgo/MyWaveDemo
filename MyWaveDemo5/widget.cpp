#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    originListIndex = 0;
    isStoppingTimer = false;

    view = new QChartView();
    initSplineWaveChart();
    view->setChart(chart);
    view->setRubberBand(QChartView::RectangleRubberBand);

    view2 = new QChartView();
    initSplineWaveChart2();
    view2->setChart(chart2);
    view2->setRubberBand(QChartView::RectangleRubberBand);


    QVBoxLayout *viewLayout = new QVBoxLayout();
    viewLayout->addWidget(view);
    viewLayout->addWidget(view2);
    viewLayout->addWidget(ui->displayButton);
    this->setLayout(viewLayout);

    view->installEventFilter(this);  //注册部件事件过滤
    view2->installEventFilter(this);

    connect(ui->stopButton,SIGNAL(clicked()),this,SLOT(stopButtonClick()));
    connect(ui->displayButton,SIGNAL(clicked()),this,SLOT(displayButtonClick()));
}

Widget::~Widget()
{
    delete ui;
}


void Widget::initSplineWaveChart()
{
    splineSeries = new QSplineSeries(); //曲线序列
    splineSeries->setColor(QColor(Qt::black));//设置线的颜色
    splineSeries->setUseOpenGL(true);//openGL加速
    scatterSeries = new QScatterSeries(); //点序列
    scatterSeries->setMarkerSize(8);  //设置描点的大小

    chart = new QChart();
    chart->legend()->hide();    //不显示注释
    //chart->createDefaultAxes();
    chart->setTitle("实时动态曲线");
    chart->addSeries(splineSeries);  //添加曲线到chart中
    chart->addSeries(scatterSeries);


    axisX = new QValueAxis();
    axisX->setRange(0, 90000);  //设置坐标轴范围
    axisX->setTitleText("time(0.2millisecs)");//标题
    axisX->setTickCount(10); //18个区域，19个刻度
    axisX->setMinorTickCount(1);

    axisY = new QValueAxis();
    axisY->setRange(-2, 2);
    axisY->setTitleText("accelerated speed");//标题
    axisY->setTickCount(5);
    //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
    //只有四个选项，下方：Qt::AlignBottom，左边：Qt::AlignLeft，右边：Qt::AlignRight，上方：Qt::AlignTop
    chart->addAxis(axisX, Qt::AlignBottom);
    splineSeries->attachAxis(axisX);  //把曲线关联到坐标轴
    scatterSeries->attachAxis(axisX);

    chart->addAxis(axisY, Qt::AlignLeft);
    splineSeries->attachAxis(axisY);
    scatterSeries->attachAxis(axisY);
}

void Widget::initSplineWaveChart2()
{
    splineSeries2 = new QSplineSeries(); //曲线序列
    splineSeries2->setColor(QColor(Qt::black));//设置线的颜色
    splineSeries2->setUseOpenGL(true);//openGL加速
    scatterSeries2 = new QScatterSeries(); //点序列
    scatterSeries2->setMarkerSize(8);  //设置描点的大小

    chart2 = new QChart();
    chart2->legend()->hide();    //不显示注释
    //chart2->createDefaultAxes();
    chart2->setTitle("实时动态曲线");
    chart2->addSeries(splineSeries2);  //添加曲线到chart中
    chart2->addSeries(scatterSeries2);


    axisX2 = new QValueAxis();
    axisX2->setRange(0, 90000);  //设置坐标轴范围
    axisX2->setTitleText("time(0.2millisecs)");//标题
    axisX2->setTickCount(10); //18个区域，19个刻度
    axisX2->setMinorTickCount(1);

    axisY2 = new QValueAxis();
    axisY2->setRange(-2, 2);
    axisY2->setTitleText("accelerated speed");//标题
    axisY2->setTickCount(5);
    //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
    //只有四个选项，下方：Qt::AlignBottom，左边：Qt::AlignLeft，右边：Qt::AlignRight，上方：Qt::AlignTop
    chart2->addAxis(axisX2, Qt::AlignBottom);
    splineSeries2->attachAxis(axisX2);  //把曲线关联到坐标轴
    scatterSeries2->attachAxis(axisX2);

    chart2->addAxis(axisY2, Qt::AlignLeft);
    splineSeries2->attachAxis(axisY2);
    scatterSeries2->attachAxis(axisY2);
}


void Widget::displayButtonClick()
{
    QString fileName = "C:/Users/13696/Desktop/项目参考资料/new.csv";
    QDir dir = QDir::current();
    QFile file(dir.filePath(fileName));
    if(!file.open(QIODevice::ReadOnly))
        return ;

    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        origin.push_back(stream.readLine());
    }

    file.close();

    originList = origin.split(",");
    originListSize = originList.count();
    qDebug() << QString("数据大小%1").arg(originListSize);

    drawTimer = new QTimer(this);//定时任务
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(timeOutEvent()));
    drawTimer->start(8);//8ms执行一次
    qDebug()<<"执行"<<'\n';
}

//暂停按钮
void Widget::stopButtonClick()
{

    if (QObject::sender() == ui->stopButton) {
        if (!isStoppingTimer) {
            drawTimer->stop();
        } else {
            drawTimer->start();
        }
        isStoppingTimer = !isStoppingTimer;
    }
}

void Widget::timeOutEvent()
{
    qDebug()<<"timeout:"<<'\n';
    if (originListIndex >= originListSize)
    {
        drawTimer->stop();
        qDebug()<<"stop"<<'\n';
    }
    else {
        double parseData = originList.at(originListIndex).toDouble();
        qDebug() << QString("数据为：%1").arg(parseData);
        double tempData = parseData/10000;
        drawSplineWave(originListIndex, tempData);
        originListIndex++;
    }
}


void Widget::drawSplineWave(int axis_x, double data)
{
    pointBuffer.append(QPointF(axis_x, data));
    splineSeries->replace(pointBuffer);
    splineSeries2->replace(pointBuffer);
}

//chartView事件过滤封装
void Widget::charViewEventFilter(QEvent *event,QChart *tempChart)
{
    if(event->type()==QEvent::Wheel){
        //将event强制转换为发生的事件的类型
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        if(wheelEvent->delta()>0)tempChart->zoom(1.1);
        else tempChart->zoom(10.0/11);
    }
    else if(event->type()==QEvent::MouseButtonPress){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() & Qt::RightButton)
            tempChart->zoomReset();
        else if (mouseEvent->button() & Qt::LeftButton)isClickingChart = true;
    }
    else if(event->type()==QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
        case Qt::Key_Left:
            tempChart->scroll(-10, 0);
            break;
        case Qt::Key_Right:
            tempChart->scroll(10, 0);
            break;
        case Qt::Key_Up:
            tempChart->scroll(0, 10);
            break;
        case Qt::Key_Down:
            tempChart->scroll(0, -10);
            break;
        default:
            keyPressEvent(keyEvent);
            break;
        }
    }
}

//事件过滤
bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == view){
        charViewEventFilter(event,chart);
    }
    if(obj == view2){
        charViewEventFilter(event,chart2);
    }
    else return QWidget::eventFilter(obj,event);
}

//鼠标移动事件
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    int x, y;
    if (isClickingChart) {
        if (xOld == 0 && yOld == 0) {

        } else {
            x = event->x() - xOld;
            y = event->y() - yOld;
            chart->scroll(-x, y);
        }
        xOld = event->x();
        yOld = event->y();
        return;
    }
}

//鼠标释放事件
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if (isClickingChart) {
        xOld = yOld = 0;
        isClickingChart = false;
    }
}

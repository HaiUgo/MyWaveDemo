#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    originListIndex = 0;
    isStoppingTimer = false;

    initSplineWaveChart();

    ui->chartView->installEventFilter(this);  //注册部件事件过滤
    connect(ui->displayButton,SIGNAL(clicked()),this,SLOT(displayButtonClick()));
    connect(ui->stopButton,SIGNAL(clicked()),this,SLOT(stopButtonClick()));

}

Widget::~Widget()
{
    delete ui;
}


void Widget::initSplineWaveChart()
{
    //其实这里可以用默认的坐标轴
    axisY = new QValueAxis();
    axisX = new QValueAxis();

    splineSeries = new QSplineSeries(); //曲线序列
    scatterSeries = new QScatterSeries(); //点序列
    scatterSeries->setMarkerSize(8);  //设置描点的大小

    chart = new QChart();

    //添加曲线到chart中
    chart->addSeries(splineSeries);
    chart->addSeries(scatterSeries);
    chart->legend()->hide();    //不显示注释
    chart->setTheme(QChart::ChartThemeBlueCerulean);//设置系统主题
    chart->setTitle("实时动态曲线");
    chart->setTitleFont(QFont("微软雅黑",10));
    chart->createDefaultAxes();

    //设置坐标轴显示范围
    axisX->setRange(0, 600);
    axisY->setRange(-50000, 50000);
    axisX->setTickCount(30);
    axisY->setTickCount(10);

    //设置坐标轴的颜色，粗细和设置网格显示
    axisX->setGridLinePen(QPen(Qt::red, 1, Qt::DashDotDotLine, Qt::SquareCap, Qt::RoundJoin)); //网格样式
    axisY->setGridLinePen(QPen(Qt::red, 1, Qt::DashDotDotLine, Qt::SquareCap, Qt::RoundJoin));

    axisX->setLinePen(QPen(Qt::red, 1, Qt::DashDotDotLine, Qt::SquareCap, Qt::RoundJoin));//坐标轴样式
    axisY->setLinePen(QPen(Qt::red, 1, Qt::DashDotDotLine, Qt::SquareCap, Qt::RoundJoin));

    axisY->setGridLineVisible(true);//显示线框
    axisX->setGridLineVisible(true);

    axisX->setLabelsVisible(false);//不显示具体数值
    axisY->setLabelsVisible(false);

    //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
    //只有四个选项，下方：Qt::AlignBottom，左边：Qt::AlignLeft，右边：Qt::AlignRight，上方：Qt::AlignTop
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    //把曲线关联到坐标轴
    splineSeries->attachAxis(axisX);
    splineSeries->attachAxis(axisY);
    splineSeries->setColor(QColor(Qt::black));//设置线的颜色
    splineSeries->setUseOpenGL(true);//openGL加速
    setSplineChartMargins(chart, 2);//设置边距

    ui->chartView->setChart(chart);
    ui->chartView->setRubberBand(QChartView::RectangleRubberBand);
}

void Widget::setSplineChartMargins(QChart *chart, int margin)
{
    QMargins m_Margin;
    m_Margin.setLeft(margin);
    m_Margin.setBottom(margin);
    m_Margin.setRight(margin);
    m_Margin.setTop(margin);
    chart->setMargins(m_Margin);
}


void Widget::timeOutEvent()
{
    if (originListIndex >= originListSize)
    {
        drawTimer->stop();
    }
    else {
        qint16 tempInt16 = originList.at(originListIndex).toInt();
        //qDebug() << QString("数据为：%1").arg(tempInt16);
        drawSplineWave(originListIndex, tempInt16);
        originListIndex++;
    }
}

//显示按钮
void Widget::displayButtonClick()
{
    QString fileName = "C:/Users/13696/Desktop/项目参考资料/partcsvparameter.csv";
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

//    Q_FOREACH(QString str, originList){
//        QStringList valueSplit = str.split(",");
//    }
   // QString origin = ui->textEdit->toPlainText();
    originList = origin.split(",");
    originListSize = originList.count();
    qDebug() << QString("数据大小%1").arg(originListSize);

    drawTimer = new QTimer(this);//定时任务
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(timeOutEvent()));
    drawTimer->start(8);//8ms执行一次
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

//注意axis_x的输入，如果大于65535，
void Widget::drawSplineWave(int axis_x, qint16 data) {
    int timesCounts = axis_x / 600;//查看数据是否超过了600
    if (timesCounts > 0)//如果第一次界面绘制结束，之后存在了600个点
    {
        axis_x = axis_x - timesCounts * 600;
        pointBuffer[axis_x].setY(data);

    }
    else//如果是第一次界面
    {
        pointBuffer.append(QPointF(axis_x, data));
    }
    splineSeries->replace(pointBuffer);
}

//事件过滤
bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->chartView)
    {
        if(event->type()==QEvent::Wheel){
            //将event强制转换为发生的事件的类型
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
            if(wheelEvent->delta()>0)chart->zoom(1.1);
            else chart->zoom(10.0/11);
        }
        else if(event->type()==QEvent::MouseButtonPress){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() & Qt::RightButton)
                chart->zoomReset();
            else if (mouseEvent->button() & Qt::LeftButton)isClickingChart = true;
        }
        else if(event->type()==QEvent::KeyPress){
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            switch (keyEvent->key()) {
            case Qt::Key_Left:
                chart->scroll(-10, 0);
                break;
            case Qt::Key_Right:
                chart->scroll(10, 0);
                break;
            case Qt::Key_Up:
                chart->scroll(0, 10);
                break;
            case Qt::Key_Down:
                chart->scroll(0, -10);
                break;
            default:
                keyPressEvent(keyEvent);
                break;
            }
        }
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
    //关闭鼠标右键事件
    if (!(event->button() & Qt::RightButton)) {
      //  ui->chartView->mouseReleaseEvent(event);
    }
}

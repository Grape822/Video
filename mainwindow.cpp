#include "mainwindow.h"  // 引入 MainWindow 类的头文件
#include "onevideo.h"     // 引入 OneVideo 类的头文件

#include <QMenuBar>       // 用于菜单栏
#include <QMenu>          // 用于菜单
#include <QAction>        // 用于菜单项的动作
#include <QStatusBar>     // 用于状态栏
#include <QScrollArea>    // 用于滚动区域
#include <QPushButton>    // 用于按钮
#include <QMessageBox>    // 用于消息框
#include <QTimer>         // 用于定时器
#include <QDateTime>      // 用于日期时间操作

#include <QResizeEvent>   // 用于窗口大小调整事件
#include <QDebug>         // 用于调试输出

// MainWindow 构造函数，初始化窗口
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    resize(800, 600);  // 设置窗口大小为800x600
    setMinimumSize(OneVideo::WIDTH + 20, OneVideo::HEIGHT + 50);  // 设置窗口最小尺寸
    createMenu();  // 创建菜单栏
    setStatusBar(new QStatusBar(this));  // 创建状态栏
    createContent();  // 创建窗口内容

    timer = new QTimer(this);  // 初始化定时器
    connect(timer, SIGNAL(timeout()), SLOT(timerSlot()));  // 连接定时器超时信号到槽函数
    timer->start(1000);  // 每隔1000ms触发一次定时器
}

// MainWindow 析构函数，清理资源
MainWindow::~MainWindow()
{
    timer->deleteLater();  // 延迟删除定时器
    removeWindowSlot();  // 清除窗口内容
}

// 创建菜单栏及其功能项
void MainWindow::createMenu()
{
    QMenuBar *menuBar = new QMenuBar(this);  // 创建菜单栏

    QMenu *menuFile = new QMenu("文件(&F)", menuBar);  // 创建"文件"菜单
    QMenu *menuHelp = new QMenu("帮助(&H)", menuBar);  // 创建"帮助"菜单
    menuBar->addAction(menuFile->menuAction());  // 将文件菜单添加到菜单栏
    menuBar->addAction(menuHelp->menuAction());  // 将帮助菜单添加到菜单栏
    setMenuBar(menuBar);  // 设置菜单栏

    QAction *actionAdd = new QAction("添加窗口(&A)", menuFile);  // 创建"添加窗口"菜单项
    QAction *actionRemove = new QAction("删除窗口(&D)", menuFile);  // 创建"删除窗口"菜单项
    QAction *actionExit = new QAction("退出(&Q)", menuFile);  // 创建"退出"菜单项
    QAction *actionAbout = new QAction("关于(&A)", menuHelp);  // 创建"关于"菜单项
    menuFile->addAction(actionAdd);  // 将"添加窗口"菜单项添加到文件菜单
    menuFile->addAction(actionRemove);  // 将"删除窗口"菜单项添加到文件菜单
    menuFile->addSeparator();  // 添加分隔符
    menuFile->addAction(actionExit);  // 将"退出"菜单项添加到文件菜单
    menuHelp->addAction(actionAbout);  // 将"关于"菜单项添加到帮助菜单

    // 连接菜单项动作到对应槽函数
    connect(actionAdd, SIGNAL(triggered(bool)), SLOT(addWindowSlot()));
    connect(actionRemove, SIGNAL(triggered(bool)), SLOT(removeWindowSlot()));
    connect(actionExit, SIGNAL(triggered(bool)), SLOT(close()));
    connect(actionAbout, SIGNAL(triggered(bool)), SLOT(aboutSlot()));
}

// 创建窗口内容
void MainWindow::createContent()
{
    QScrollArea *scrollArea = new QScrollArea(this);  // 创建滚动区域
    scrollArea->setFrameStyle(QFrame::NoFrame);  // 设置滚动区域的边框样式

    mainContent = new QWidget(scrollArea);  // 创建内容窗口
    mainContent->resize(size());  // 设置内容窗口大小
    scrollArea->setWidget(mainContent);  // 将内容窗口设置为滚动区域的子窗口
    setCentralWidget(scrollArea);  // 设置滚动区域为中央窗口

    addBtn = new QPushButton("+", mainContent);  // 创建加号按钮
    addBtn->setGeometry(0, 0, OneVideo::WIDTH, OneVideo::HEIGHT);  // 设置按钮位置和大小
    addBtn->setFont(QFont("黑体", 200, 87));  // 设置按钮字体
    addBtn->setVisible(true);  // 设置按钮可见

    connect(addBtn, SIGNAL(clicked(bool)), SLOT(addWindowSlot()));  // 连接按钮点击信号到添加窗口的槽函数
}

// 布局子窗口
void MainWindow::layoutChild()
{
    int cols = (width() - 20) / OneVideo::WIDTH;  // 计算窗口内可以显示的列数
    int i;
    for(i = 0; i < showList.size(); ++i){  // 遍历所有子窗口
        showList.at(i)->move((i % cols) * OneVideo::WIDTH, (i / cols) * OneVideo::HEIGHT);  // 重新排列子窗口位置
    }

    addBtn->move((i % cols) * OneVideo::WIDTH, (i / cols) * OneVideo::HEIGHT);  // 设置添加按钮的位置

    int sw = width() - 20;  // 计算滚动区域的宽度
    int sh = ((i / cols) + 1) * OneVideo::HEIGHT;  // 计算滚动区域的高度
    mainContent->resize(sw,  sh);  // 调整内容窗口的大小
}

// 添加子窗口
void MainWindow::addWindowSlot()
{
    OneVideo *one = new OneVideo(mainContent);  // 创建一个新的子窗口
    connect(one, SIGNAL(closeSignal(OneVideo*)), SLOT(childClosed(OneVideo*)));  // 连接子窗口的关闭信号到槽函数
    one->show();  // 显示子窗口
    showList.append(one);  // 将子窗口添加到显示列表

    layoutChild();  // 重新布局所有子窗口
}

// 删除所有子窗口
void MainWindow::removeWindowSlot()
{
    for(OneVideo *one : showList){  // 遍历所有子窗口
        one->deleteLater();  // 延迟删除子窗口
    }
    showList.clear();  // 清空显示列表
    layoutChild();  // 重新布局
}

// 显示关于 Qt 的对话框
void MainWindow::aboutSlot()
{
    QMessageBox::aboutQt(this);  // 显示"关于"对话框
}

// 定时器触发的槽函数，显示当前时间在状态栏上
void MainWindow::timerSlot()
{
    statusBar()->showMessage(QDateTime::currentDateTime().toString("当前时间：yyyy-MM-dd hh:mm:ss"));  // 显示当前时间
}

// 子窗口关闭时调用的槽函数
void MainWindow::childClosed(OneVideo *who)
{
    showList.removeOne(who);  // 从显示列表中移除关闭的子窗口
    layoutChild();  // 重新布局
}

// 窗口大小调整事件处理函数
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);  // 调用基类的窗口调整事件处理
    layoutChild();  // 重新布局子窗口
}

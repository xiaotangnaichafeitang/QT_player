#include "mainwind.h"
#include "ui_mainwind.h"
#include <QDebug>
#include <thread>
#include <functional>
#include <iostream>
#include "ffmsg.h"
MainWind::MainWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWind)
{
    ui->setupUi(this);

    // 初始化信号槽相关的
    InitSignalsAndSlots();
}

MainWind::~MainWind()
{
    delete ui;
}

int MainWind::InitSignalsAndSlots()
{
    connect(ui->ctrlBarWind, &CtrlBar::SigPlayOrPause, this, &MainWind::OnPlayOrPause);
    connect(ui->ctrlBarWind, &CtrlBar::SigStop, this, &MainWind::OnStop);
}

int MainWind::message_loop(void *arg)
{
    IjkMediaPlayer *mp = (IjkMediaPlayer *)arg;
    // 线程循环
    qDebug() << "message_loop into";
    while (1) {
        AVMessage msg;
        //取消息队列的消息，如果没有消息就阻塞，直到有消息被发到消息队列。
        int retval = mp->ijkmp_get_msg(&msg, 1);    // 主要处理Java->C的消息

        if (retval < 0)
            break;
        switch (msg.what) {
        case FFP_MSG_FLUSH:
                qDebug() << __FUNCTION__ << " FFP_MSG_FLUSH";
            break;
        case FFP_MSG_PREPARED:
            std::cout << __FUNCTION__ << " FFP_MSG_PREPARED" << std::endl;
            mp->ijkmp_start();
            break;
        default:
           qDebug()  << __FUNCTION__ << " default " << msg.what ;
          break;
        }
        msg_free_res(&msg);
//        qDebug() << "message_loop sleep, mp:" << mp;
        // 先模拟线程运行
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    }
    qDebug() << "message_loop leave";
}

void MainWind::OnPlayOrPause()
{
    qDebug() << "OnPlayOrPause call";
    int ret = 0;
    // 1. 先检测mp是否已经创建
    if(!mp_) {
        mp_ = new IjkMediaPlayer();
        //1.1 创建
        ret = mp_->ijkmp_create(std::bind(&MainWind::message_loop, this, std::placeholders::_1));
        if(ret <0) {
            qDebug() << "IjkMediaPlayer create failed";
            delete mp_;
            mp_ = NULL;
            return;
        }
        // 1.2 设置url
        mp_->ijkmp_set_data_source("2_audio.mp4");
        // 1.3 准备工作
        ret = mp_->ijkmp_prepare_async();
        if(ret <0) {
            qDebug() << "IjkMediaPlayer create failed";
            delete mp_;
            mp_ = NULL;
            return;
        }
    } else {
        // 已经准备好了，则暂停或者恢复播放
    }
}

void MainWind::OnStop()
{
    qDebug() << "OnStop call";
    if(mp_) {
        mp_->ijkmp_stop();
        mp_->ijkmp_destroy();
        delete mp_;
        mp_ = NULL;
    }
}

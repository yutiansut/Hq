﻿#include "qsharehistoryinfomgr.h"
#include "qeastmoneystockhistoryinfothread.h"
#include "dbservices/dbservices.h"
#include "qsinasharevolinfothread.h"
#include "../qeastmoneyhsgtshareamount.h"
#include <QDebug>
#include <QThreadPool>

QShareHistoryInfoMgr::QShareHistoryInfoMgr(const QStringList& codes, QObject *parent) : QObject(parent)
{
    mCodesList = codes;
    QSinaShareVolInfoThread * vols = new QSinaShareVolInfoThread(mCodesList);
    connect(vols, SIGNAL(finished()), this, SLOT(slotShareFinanceInfoFinished()));
    this->moveToThread(&mWorkThread);
    mWorkThread.start();
    vols->start();
}

void QShareHistoryInfoMgr::slotShareFinanceInfoFinished()
{
    QSinaShareVolInfoThread *t1 = qobject_cast<QSinaShareVolInfoThread*>(sender());
    if(t1)
    {
        delete t1;
    }
    qDebug()<<QDateTime::currentDateTime();
    QThreadPool pool;
    pool.setExpiryTimeout(-1);
    pool.setMaxThreadCount(8);

    //开始更新日线数据
    foreach (QString code, mCodesList) {
        QEastmoneyStockHistoryInfoThread* thread = new QEastmoneyStockHistoryInfoThread(code);
        pool.start(thread);
//        QEventLoop loop;
//        connect(thread, SIGNAL(finished()), &loop, SLOT(quit()));
//        thread->start();
//        loop.exec();
//        delete thread;
    }
    pool.waitForDone();
     qDebug()<<QDateTime::currentDateTime();
    //开始更新持股信息
    QEastMoneyHSGTShareAmount * thread = new QEastMoneyHSGTShareAmount;
    connect(thread, SIGNAL(finished()), this, SLOT(slotUpdateForeignAmountFinished()));
    connect(thread, SIGNAL(signalAmountFinshedAtDate(QString)), this, SIGNAL(signalUpdateAmountProcess(QString)));
    thread->start();
    emit signalHistoryDataFinished();
}

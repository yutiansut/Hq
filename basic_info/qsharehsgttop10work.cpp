﻿#include "qsharehsgttop10work.h"
#include "utils/qhttpget.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include "data_structure/hqutils.h"
#include "data_structure/sharedata.h"
#include "dbservices/dbservices.h"
#include <QFile>

QShareHsgtTop10Work::QShareHsgtTop10Work(QObject *parent) : mParent(parent),QRunnable()
{
}

QShareHsgtTop10Work::~QShareHsgtTop10Work()
{

}

void QShareHsgtTop10Work::run()
{
    ShareDate last_update_date = DATA_SERVICE->getLastUpdateDateOfHsgtTop10();
    if(last_update_date == ShareDate::currentDate()) return;
    ShareDate curDate = ShareDate::currentDate();
    while (last_update_date <= curDate) {
        if(last_update_date.isWeekend())
        {
            last_update_date.next();
            continue;
        }
        //从网络获取
        QString url = QString("http://dcfm.eastmoney.com//EM_MutiSvcExpandInterface/api/js/get?type=HSGTCJB&token=70f12f2f4f091e459a279469fe49eca5&filter=(DetailDate=^%1^)&js=(x)&sr=1&st=Rank&rt=50014200")
                .arg(last_update_date.toString());
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(QHttpGet().getContentOfURL(url), &err);
        if(err.error != QJsonParseError::NoError) return;
        if(!doc.isArray())  return;
        //开始解析
        ShareHsgtList list;
        QJsonArray result = doc.array();
        for(int i=0; i<result.size(); i++)
        {
            QJsonObject obj = result.at(i).toObject();
            int market_type = obj.value("MarketType").toInt();
            if(market_type == 2 || market_type == 4) continue;
            ShareHsgt data;
            data.mCode = obj.value("Code").toString();
            data.mName = obj.value("Name").toString();
            if(data.mCode.left(1) == "6")
            {
                //上海
                data.mBuy = obj.value("HGTMRJE").toDouble();
                data.mSell = obj.value("HGTMCJE").toDouble();
            } else
            {
                //深圳
                data.mBuy = obj.value("SGTMRJE").toDouble();
                data.mSell = obj.value("SGTMCJE").toDouble();
            }
            data.mIsTop10 = true;
            data.mPure = data.mBuy - data.mSell;
            data.mDate = ShareDateTime(last_update_date.date());
            list.append(data);
        }
        if(list.size() > 0)
        {
            DATA_SERVICE->signalUpdateShareHsgtTop10Info(list);
        }
        last_update_date.next();
    }
}

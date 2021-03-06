﻿#ifndef DBSERVICE_H
#define DBSERVICE_H

#include <QObject>
#include <QThread>
#include "hqdbdatabase.h"
#include "utils/profiles.h"


#define DATA_SERVICE HqInfoService::instance()

class HqInfoService : public QObject
{
    Q_OBJECT
protected:
    HqInfoService(QObject *parent = 0);
    ~HqInfoService();

    ShareDate  getLastUpdateDateOfTable(const QString& table);
public:
    bool   isDBInitOk();
    friend class CGarbo;
    static HqInfoService* instance();
    ShareData* getShareData(const QString& code);
    double getProfit(const QString& code);
    foreignHolder amountForeigner(const QString& code);
    QStringList  getExchangeCodeList();
    QStringList  getAllShareCodes() {return mRealShareMap.keys();}
    ShareDate  getLastUpdateDateOfHSGT();
    ShareDate  getLastUpdateDateOfHSGTVol();
    ShareDate  getLastUpdateDateOfBasicInfo();
    ShareDate  getLastUpdateDateOfBonusInfo();
    ShareDate  getLastUpdateDateOfHsgtTop10();
    ShareDate  getLastUpdateDateOfFinanceInfo();
    ShareDate  getLastUpdateDateOfHistoryInfo(const QString& code);
    bool   GetHistoryInfoWithDate(const QString& table, const QDate& date, double& close, double& money, qint64& total_share, qint64& mutalble_share);
    double   GetMultiDaysChangePercent(const QString& table, int days);
    void   GetForeignVolChange(const QString& code, qint64& cur, qint64& pre);
    BlockData*   getBlockDataOfCode(const QString& code);
    //BlockData*   setBlockData(const BlockData& data);
    void         setShareBlock(const QString& code, const QString& block);
    void         setBlockShareCodes(const QString& block, const QStringList& codes);
    void         saveShares();
    BlockDataList  getAllBlock();
    ShareDataList   getShareHistoryDataList(const QString& code);
    //交易日期处理
#if 0
    bool        weekend(const QDate& date);
    bool        activeDay(const QDate& date);
    bool        isCurrentActive();
    bool        isActiveTime(const QTime& time);
    QDate       latestActiveDay();
    QDate       lastActiveDay();
    QDate       preActiveDay(const QDate& date);
    QDate       nextActiveDay(const QDate& date);
    int         activeDaysNum(const QDate& start);
    QDate       getActiveDayBefore1HYear();
    QString     date2Str(const QDate& date);
    QDate       dateFromStr(const QString& str);
#endif
    QDate       getLgtStartDate();


signals:
    //开始创建数据库需要的表信息，初始化数据库
    void signalInitDBTables();
    void signalDbInitFinished();
    void signalAllShareCodeList(const QStringList& codes);
    void signalUpdateShareBasicInfo(const ShareDataList& list);

    //从数据库查询板块信息
    void signalQueryBlockInfo(int type = 1);
    void signalRecvRealBlockInfo(const QList<BlockRealInfo>& list);
    void signalSendBlockInfoList(const QList<BlockRealInfo>& list);
     void signalQueryShareForeignVol(const QString& code);
    void signalSendShareForeignVol(const ShareDataList& list);
    //历史数据写入数据库
    void signalRecvShareHistoryInfos(const ShareDataList& list, int mode);
    void signalQueryShareHistoryLastDate(const QString& code);
    void signalSendShareHistoryLastDate(const QString& code, const QDate& date);
    void signalUpdateHistoryInfoFinished();
    void signalUpdateShareHistoryFinished(const QString &code);
    //基本信息相关的数据库操作
    void signalQueryAllShareBasicInfo();
    void signalAddShareBasicInfo(const ShareData& data);
    void signalAddShareBasicInfoList(const ShareDataList& list);
    void signalUpdateShareinfoWithHistory(const QString& code,\
                                          double lastMoney,\
                                          double last3Change,\
                                          double last5Change,\
                                          double last10Change,\
                                          double lastMonthChange,\
                                          double lastYearChange,\
                                          qint64 vol,\
                                          qint64 vol_chnage,\
                                          const ShareDataList& list);
    void signalUpdateStkProfitList(const ShareDataList& list);
    void signalInitShareRealInfos(const QStringList& codes);
    void signalSetFavCode(const QString& code);
    //沪港通持股写入数据据
    void signalAddShareAmoutByForeigner(const ShareDataList& list);
    void signalUpdateShareAmountByForeigner();
    void signalUpdateShareBonusInfo(const ShareBonusList& list);
    void signalUpdateShareHsgtTop10Info(const ShareHsgtList& list);
    void signalUpdateShareFinanceInfo(const FinancialDataList& list);
    //实时数据查询信息获取
    void signalSearchCodesOfText(const QString& text);
    void signalSendSearchCodesOfText(const QStringList& codes);
    //错误信息输出
    void signalDBErrorMsg(const QString& msg);

public slots:
    void slotSearchCodesOfText(const QString &text);
    void slotInitDBTables();
    void slotRecvShareHistoryInfos(const ShareDataList& list, int mode);
    //void slotUpdateShareHistoryInfos(const QMap<QString, ShareDataList> map);
    bool slotAddHistoryData(const ShareData& data);
    void initBlockData(int type = 0);
    void initShareData();
//    void slotQueryShareHistoryLastDate(const QString& code);
    void slotUpdateShareinfoWithHistory(const QString& code,\
                                        double lastMoney,\
                                        double last3Change,\
                                        double last5Change,\
                                        double last10Change,\
                                        double lastMonthChange,\
                                        double lastYearChange,\
                                        qint64 vol,\
                                        qint64 vol_chnage,\
                                        const ShareDataList& list);
    void slotUpdateHistoryChange(const QString& code);
    void slotUpdateStkProfitList(const ShareDataList& list);
    void slotAddShareAmoutByForeigner(const ShareDataList& list);
    void slotUpdateShareAmountByForeigner();
    void slotUpdateShareBasicInfo(const ShareDataList& list);
    void slotUpdateShareBonusInfo(const ShareBonusList& list);
    void slotUpdateHsgtTop10Info(const ShareHsgtList& list);
    void slotUpdateShareFinanceInfo(const FinancialDataList& list);

private:
    void initSignalSlot();
    bool createHistoryTable(const QString& pTableName);
    //bool createShareBaseInfoTable(const QString& code);
    //bool createBlockTable();
    bool createHSGTShareAmountTable();
    bool isActive();
    QString errMsg();

private:    //本类使用的变量
    static HqInfoService *m_pInstance;
    static QMutex mutex;//实例互斥锁。
    //static QAtomicPointer<HqInfoService> m_pInstance;/*!<使用原子指针,默认初始化为0。*/

    class CGarbo        // 它的唯一工作就是在析构函数中删除CSingleton的实例
    {
    public:
        ~CGarbo()
        {
            if (HqInfoService::m_pInstance)
            {
                delete HqInfoService::m_pInstance;
                HqInfoService::m_pInstance = NULL;
            }
        }
    };
    static CGarbo s_Garbo; // 定义一个静态成员，在程序结束时，系统会调用它的析构函数
    QThread             m_threadWork;       //工作线程
    QStringList                 mNotExchangeDaysList;
    ShareDataMap                mRealShareMap;
    QDate                       mLast3DaysDate;
    QDate                       mLast5DaysDate;
    QDate                       mLast10DaysDate;
    QDate                       mLast1MonthDate;
    QDate                       mLastActiveDate;
    QMap<QString, double>       mStkProfitMap;
    QMap<QString, foreignHolder>       mStkForeignerHoldMap;
    BlockDataMap                mRealBlockMap;
    QMutex                      mBlockMutex;
    QMutex                      mShareMutex;
    HQDBDataBase                mDataBase;
    QStringList                 mFavCodeList;
    QStringList                 mClosedDateList;
};

#endif // DBSERVICE_H

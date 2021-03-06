﻿#include "qsinastkresultmergethread.h"
#include "qsinastkinfothread.h"
#include <QDebug>
#include "data_structure/sharedata.h"

QSinaStkResultMergeThread::QSinaStkResultMergeThread(QObject *parent) : QThread(parent)
{
    mCodeChangeFlag = false;
    mStkCntPerTrd = 200;
    mTotalPage = -1;
    mPageSize = 50;
    mCurPage = 1;
    mMktType = MKT_ALL;
    mSortType = STK_DISPLAY_SORT_TYPE_CHGPER;
    mSortRule = -1;
    mActive = true;
    QEastMoneyZjlxThread *zjt = new QEastMoneyZjlxThread();
}

QSinaStkResultMergeThread::~QSinaStkResultMergeThread()
{

}

void QSinaStkResultMergeThread::setActive(bool active)
{
    mActive = active;
}

bool QSinaStkResultMergeThread::isActive()
{
    return mActive;
}

void QSinaStkResultMergeThread::setSortType(int type)
{
    mCurPage = 1;
    if(mSortType == type)
    {
        mSortRule = (mSortRule == -1 ? 1: -1);

    } else
    {
        mSortType = type;
        mSortRule = -1;
    }
}

void QSinaStkResultMergeThread::run()
{
    while (true) {
        //检查当前的代码是否发生了变化
        if(mStkCodesList.length() == 0)
        {
            sleep(1);
            continue;
        }
        if(mThreadList.length() == 0)
        {
            //还没有初始化行情线程
            int thread_code = 200;
            int nthread = (mStkCodesList.length() + thread_code-1 ) / thread_code;
            for(int i=0; i<nthread; i++)
            {
                QStringList wklist = mStkCodesList.mid(i*thread_code, thread_code);
                QSinaStkInfoThread *wkthread = new QSinaStkInfoThread();
                mThreadList.append(wkthread);
                connect(wkthread, SIGNAL(sendStkDataList(ShareDataList)), this, SLOT(slotRevResList(ShareDataList)));
                wkthread->signalSetStkList(wklist);
            }
        }
        ShareDataList wklist;
        mListMutex.lock();
        if(mMktType == MKT_ALL)
        {
            //全部进行排序，不过滤
            wklist = mMidStkDataMapList.values();
        } else
        {
            foreach (QString key, mMidStkDataMapList.keys()) {
                ShareData data = mMidStkDataMapList.value(key);
                bool sts = (mMktType == MKT_SH && (data.mShareType & SHARE_SH))||\
                        (mMktType == MKT_SZ && (data.mShareType & SHARE_SZ)) ||\
                        (mMktType == MKT_ZXB && (data.mShareType & SHARE_SZ_ZXB))||\
                        (mMktType == MKT_CYB && (data.mShareType & SHARE_SZ_CYB))||\
                        (mMktType == MKT_ZXG && data.mIsFav)||\
                        (mMktType == MKT_LGT_TOP10 && data.mHsgtData.mIsTop10) ||\
                        (mMktType == MKT_JJ && (data.mShareType & SHARE_FUND))||\
                        (mMktType == MKT_OTHER && mSelfCodesList.contains(data.mCode));
                if(sts)
                {
                    wklist.append(data);
                }
            }
        }
        mListMutex.unlock();
        mTotalPage = (wklist.length() + mPageSize -1) / mPageSize;

        if(wklist.length())
        {
            //开始排序
            if(mSortType == STK_DISPLAY_SORT_TYPE_CHGPER)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByPerDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByPerAsc);
                }
            }

            else if(mSortType == STK_DISPLAY_SORT_TYPE_CJE)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByMonDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByMonAsc);
                }
            }

            else if(mSortType == STK_DISPLAY_SORT_TYPE_PRICE)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByCurDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByCurAsc);
                }
            }else if(mSortType == STK_DISPLAY_SORT_TYPE_HSL)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByHslDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByHslAsc);
                }
            }else if(mSortType == STK_DISPLAY_SORT_TYPE_MONEYR)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByMonRatioDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByMonRatioAsc);
                }
            }
            else if(mSortType == STK_DISPLAY_SORT_TYPE_LAST3)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortBy3DayChgDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortBy3DayChgAsc);
                }
            }
            else if(mSortType == STK_DISPLAY_SORT_TYPE_LAST5)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortBy5DayChgDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortBy5DayChgAsc);
                }
            }

            else if(mSortType == STK_DISPLAY_SORT_TYPE_LAST10)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortBy10DayChgDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortBy10DayChgAsc);
                }
            }

            else if(mSortType == STK_DISPLAY_SORT_TYPE_LAST_MONTH)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByMonthChgDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByMonthChgAsc);
                }
            }

            else if(mSortType == STK_DISPLAY_SORT_TYPE_LAST_YEAR)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByYearChgDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByYearChgAsc);
                }
            }

            else if(mSortType == STK_DISPLAY_SORT_TYPE_ZJLX)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortBy3DayZjlxDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortBy3DayZjlxAsc);
                }
            }
            else if(mSortType == STK_DISPLAY_SORT_TYPE_GXL)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByGxlDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByGxlAsc);
                }
            }
            else if(mSortType == STK_DISPLAY_SORT_TYPE_SZZBL)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortBySzzblDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortBySzzblAsc);
                }
            }
            else if(mSortType == STK_DISPLAY_SORT_TYPE_GQDJR)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByGqdjrDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByGqdjrAsc);
                }
            }
            else if(mSortType == STK_DISPLAY_SORT_TYPE_TCAP)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByTcapDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByTcapAsc);
                }
            }
            else if(mSortType == STK_DISPLAY_SORT_TYPE_MCAP)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByMcapDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByMcapAsc);
                }
            }

            else if(mSortType == STK_DISPLAY_SORT_TYPE_PROFIT)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByProfitDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByProfitAsc);
                }
            }
            else if(mSortType == STK_DISPLAY_SORT_TYPE_FOREIGN_VOL)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByForVolDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByForVolAsc);
                }
            }
            else if(mSortType == STK_DISPLAY_SORT_TYPE_FOREIGN_CAP)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByForCapDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByForCapAsc);
                }
            }

            else if(mSortType == STK_DISPLAY_SORT_TYPE_FOREIGN_VOL_CHG)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByForVolChgDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByForVolChgAsc);
                }
            }
            else if(mSortType == STK_DISPLAY_SORT_TYPE_FOREIGN_CAP_CHG)
            {
                if(mSortRule == -1)
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByForCapChgDesc);
                } else
                {
                    qSort(wklist.begin(), wklist.end(), ShareData::sortByForCapChgAsc);
                }
            }

            if(mActive)
            {
                ShareDataList mid = wklist.mid((mCurPage - 1) * mPageSize, mPageSize);
                //qDebug()<<"mid:"<<mid.length();
                emit sendStkDataList(mid);
            }
        } else
        {
            emit sendStkDataList(ShareDataList());
        }

        sleep(1);
    }


}

void QSinaStkResultMergeThread::slotRevResList(const ShareDataList &mid)
{
    mListMutex.lock();
    foreach (ShareData data, mid) {
        if(data.mCur < 0.1) continue;
        //if(data.mMoney < 10000) continue;
        mMidStkDataMapList[data.mCode] = data;
    }
    mListMutex.unlock();
}

void QSinaStkResultMergeThread::setStkList(const QStringList &list)
{
    mStkCodesList = list;
    mCodeChangeFlag = true;
}

void QSinaStkResultMergeThread::updateStkInfoList(const QList<QStringList>& pStkSectionList)
{

}

void QSinaStkResultMergeThread::setMktType(int type)
{
    mMktType = (MKT_TYPE)type;
    mCurPage = 1;
    //updateStkCodes(mMktType);
}

void QSinaStkResultMergeThread::updateStkCodes(MKT_TYPE type)
{
    mSecCodesLock.lock();
    mSecCodesList.clear();
    if(type == MKT_ALL)
    {
        QStringList codes;
        foreach (QString code, mStkCodesList) {
            codes.append(code);
            if(codes.length() == 100)
            {
                mSecCodesList.append(codes);
                codes.clear();
            }
        }
        if(codes.length() > 0) mSecCodesList.append(codes);
    } else if(type == MKT_SH)
    {
        QStringList codes;
        foreach (QString code, mStkCodesList) {
            if(code.contains("sh"))
            {
                codes.append(code);
            } else
            {
                continue;
            }
            if(codes.length() == 100)
            {
                mSecCodesList.append(codes);
                codes.clear();
            }
        }
        if(codes.length() > 0) mSecCodesList.append(codes);
    } else if(type == MKT_SZ)
    {
        QStringList codes;
        foreach (QString code, mStkCodesList) {
            if(code.contains("sz"))
            {
                codes.append(code);
            } else
            {
                continue;
            }
            if(codes.length() == 100)
            {
                mSecCodesList.append(codes);
                codes.clear();
            }
        }
        if(codes.length() > 0) mSecCodesList.append(codes);
    } else if(type == MKT_ZXB)
    {
        QStringList codes;
        foreach (QString code, mStkCodesList) {
            if(code.startsWith("sz002"))
            {
                codes.append(code);
            } else
            {
                continue;
            }
            if(codes.length() == 100)
            {
                mSecCodesList.append(codes);
                codes.clear();
            }
        }
        if(codes.length() > 0) mSecCodesList.append(codes);
    } else if(type == MKT_CYB)
    {
        QStringList codes;
        foreach (QString code, mStkCodesList) {
            if(code.startsWith("sz30"))
            {
                codes.append(code);
            } else
            {
                continue;
            }
            if(codes.length() == 100)
            {
                mSecCodesList.append(codes);
                codes.clear();
            }
        }
        if(codes.length() > 0)mSecCodesList.append(codes);
    }
    mSecCodesLock.unlock();
//    updateStkInfoList(sec_codes_list);
}

void QSinaStkResultMergeThread::stopUpdate()
{

}

void QSinaStkResultMergeThread::SortResultList(ShareDataList &result, const ShareDataList &mid)
{

//    if(wklist.length())
//    {
//        //开始排序
//        if(mOptType == STK_DISPLAY_SORT_TYPE_CHGPER)
//        {
//            if(mSortRule == -1)
//            {
//                qSort(wklist.begin(), wklist.end(), ShareData::sortByPerDesc);
//            } else
//            {
//                qSort(wklist.begin(), wklist.end(), ShareData::sortByPerAsc);
//            }
//        }

//        else if(mOptType == STK_DISPLAY_SORT_TYPE_CJE)
//        {
//            if(mSortRule == -1)
//            {
//                qSort(wklist.begin(), wklist.end(), ShareData::sortByMonDesc);
//            } else
//            {
//                qSort(wklist.begin(), wklist.end(), ShareData::sortByMonAsc);
//            }
//        }

//        else if(mOptType == STK_DISPLAY_SORT_TYPE_PRICE)
//        {
//            if(mSortRule == -1)
//            {
//                qSort(wklist.begin(), wklist.end(), ShareData::sortByCurDesc);
//            } else
//            {
//                qSort(wklist.begin(), wklist.end(), ShareData::sortByCurAsc);
//            }
//        }
//        emit sendStkDataList(wklist);
//    }

//    reply->deleteLater();
//    QThread::sleep(1);
//}
}

//ShareDataList QSinaStkResultMergeThread::RealtimeInfo(const QStringList& codes)
//{

//}

void QSinaStkResultMergeThread::setSelfCodesList(const QStringList &list)
{
    mSelfCodesList = list;
    if(list.length() > 0)
    {
        mMktType = MKT_OTHER;
    } else
    {
        mMktType = MKT_ALL;
    }
    //qDebug()<<"selfcodes:"<<mSelfCodesList;
}

MktType QSinaStkResultMergeThread::getMktType()
{
    return mMktType;
}

void QSinaStkResultMergeThread::displayFirst()
{
    mCurPage = 1;
}

void QSinaStkResultMergeThread::displayLast()
{
    mCurPage = mTotalPage;
}

void QSinaStkResultMergeThread::displayPrevious()
{
    mCurPage--;
    if(mCurPage <= 0) mCurPage = 1;
}

void QSinaStkResultMergeThread::displayNext()
{
    mCurPage++;
    if(mCurPage > mTotalPage) mCurPage = mTotalPage;
}

void QSinaStkResultMergeThread::setCurPage(int page)
{
    mCurPage = page;
    if(mCurPage >= mTotalPage)
    {
        mCurPage = mTotalPage;
    }
    if(mCurPage <= 0)
        mCurPage = 1;
}

void QSinaStkResultMergeThread::setDisplayPage(int val)
{
    if(val == FIRST_PAGE)
    {
        displayFirst();
    } else if(val == NEXT_PAGE)
    {
        displayNext();
    } else if(val == PRE_PAGE)
    {
        displayPrevious();
    } else if(val == END_PAGE)
    {
        displayLast();
    }

}

void QSinaStkResultMergeThread::setDisplayChinaTop10()
{
    setMktType( MKT_LGT_TOP10);
}


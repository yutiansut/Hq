#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSystemTrayIcon>
#include <QTableWidget>
#include "qeastmoneyblockthread.h"
#include "qsinastkinfothread.h"
#include "qsinasearchthread.h"
#include "qsinastkresultmergethread.h"
#include <QTimer>

namespace Ui {
class MainDialog;
}

enum{
    MENU_OPT_MINUTE,
    MENU_OPT_DAY,
    MENU_OPT_BLOCK,
};
class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    //static void HQLISIINFOCBKFUNC(StockDataList& pDataList, void *pUser);
    //static void HQBLOCKINFOCBKFUNC(BlockDataList& pDataList, void *pUser);


protected:
    void resizeEvent(QResizeEvent *event);
    //void keyPressEvent(QKeyEvent *);
public slots:
    void setSortType(int index);
    void setBlockSort(int val);
    void setBlockName();
    void setStockMarket();

    void updateBlockTable(const BlockDataList& pDataList);
    void displayBlockRealtimeInfo();
    void updateHqTable(const StockDataList& pDataList);
    void displayBlockDetailInfoInTable(const QStringList& stklist);
    void editFavorite();
    void setDlgShow(QSystemTrayIcon::ActivationReason val);
    void slotUpdate(int cur, int total);
    void hqMenuOpt();
    void slotUpdateMsg(const QString& msg);
    void slotUpdateIndex(const StockDataList& pDataList);
    void slotSystemTrayMenuClicked();
    void slotWhetherDisplay();
    void slotRestartMyself();

private slots:
    void on_zxgBtn_clicked();

    void on_hqcenterBtn_clicked();

    void on_blkbtn_clicked();

    void on_zjlxBtn_clicked();

    void on_lhbBtn_clicked();

    void on_closeBtn_clicked();

    void on_srchBtn_clicked();

    void on_minBtn_clicked();

    void on_blocktbl_itemDoubleClicked(QTableWidgetItem *item);

    void on_blocktbl_customContextMenuRequested(const QPoint &pos);

    void on_hqtbl_customContextMenuRequested(const QPoint &pos);

    void on_searchTxt_textChanged(const QString &arg1);

    void on_DateMgrBtn_clicked();

    void on_hqtbl_itemDoubleClicked(QTableWidgetItem *item);

    void on_hqtbl_itemEntered(QTableWidgetItem *item);

private:
    Ui::MainDialog *ui;
    QSystemTrayIcon *systemIcon;
    QSinaStkInfoThread            *mIndexThread;
    QEastMoneyBlockThread         *mBlockThread;
    QSinaStkResultMergeThread     *mMergeThread;
    QSinaSearchThread             *mSearchThread;
    QMap<QString, double> mStockMap;
    QMap<QString, double> mBlockMap;
    QMap<QString, QString> mBlockNameMap;
    QMap<QString, QStringList> mBlockStkList;
    QStringList           mFavStkList;
    int                   mCurBlockType;
    QTimer                 *mRestartTimer;
};

#endif // DIALOG_H
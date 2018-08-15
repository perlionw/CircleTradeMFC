#pragma once
#include "strategy.h"
#include "CustomTradeSpi.h"
#include <Windows.h>
#include <map>
#include <queue>
#include "mysqlinterface.h"
#define DIFFPRICE 10
static int orderRef;
typedef struct
{
	int buy_count;
	int buy_max_count;
	float open_buy_hig_price;
	int sell_count;
	int sell_max_count;
	float open_sell_low_price;
	bool close_buy;
	bool close_sell;
	int close_buy_count;
	int close_sell_count;
	int close_buy_run_count;
	int close_sell_run_count;
}trade_work;

class Strategy1 : public Strategy
{
public:
	Strategy1();
	~Strategy1();

public:
	virtual void marketMessage(CThostFtdcDepthMarketDataField *market);
	virtual void tradeMessage(CThostFtdcOrderField *order);
	virtual void dealTradeMessage(CThostFtdcTradeField *pTrade);
	virtual void orderInsertError(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	
	void setTradeSpi(map<string, CustomTradeSpi *> m);
	void addWork(string instru, bool isOpen, bool isBuy, int count);
	//void ReplaceOldIntru(string oldInstru, string newInstru);
	void InsertTrade(CThostFtdcTradeField * pTrade);
public:
	CustomTradeSpi * p_TraderUserSpi;

	map<string, CustomTradeSpi *> m_trade;
	HANDLE m_hMutex;
	map<string, trade_work> m_work;
	map<string, bool> enable_open_buy;
	map<string, bool> enable_open_sell;
	map<string, bool> enable_close_buy;
	map<string, bool> enable_close_sell;
	string oldInstru;
	string newInstru;
	bool flag;
	int tradeCount;
	MYSQL* mydata;
	MYSQL* res;
};


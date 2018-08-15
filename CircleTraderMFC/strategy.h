#ifndef STRATEGY_H
#define STRATEGY_H

/*
 * handle strategy interface class
 */

#include "CTP_API/ThostFtdcUserApiStruct.h"
//#include "CustomTradeSpi.h"

#include "stdio.h"
#include <iostream>
using namespace std;

class Strategy
{
public:
    virtual void marketMessage(CThostFtdcDepthMarketDataField *market)=0;
    virtual void tradeMessage(CThostFtdcOrderField *order)=0;
    virtual void dealTradeMessage(CThostFtdcTradeField *pTrade)=0;
    virtual void orderInsertError(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)=0;
};

#endif // STRATEGY_H

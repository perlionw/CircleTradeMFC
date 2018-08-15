#include "stdafx.h"
#include "CustomTradeSpi.h"

CustomTradeSpi::CustomTradeSpi(
                                    TThostFtdcBrokerIDType gBrokerID,
                                    TThostFtdcInvestorIDType gInvesterID,
                                    TThostFtdcPasswordType gInvesterPassword,
                                   FrontAddress gTradeFrontAddr,
                                   const TThostFtdcInstrumentIDType g_pTradeInstrumentID,
                                    Strategy *stra)
{
    strcpy_s(this->gBrokerID,strlen(gBrokerID)+1, gBrokerID);
    strcpy_s(this->gInvesterID, strlen(gInvesterID)+1,gInvesterID);
    strcpy_s(this->gInvesterPassword, strlen(gInvesterPassword)+1, gInvesterPassword);
    strcpy_s(this->gTradeFrontAddr, strlen(gTradeFrontAddr)+1, gTradeFrontAddr);
    strcpy_s(this->g_pTradeInstrumentID, strlen(g_pTradeInstrumentID)+1,g_pTradeInstrumentID);

    this->strategy=stra;

    g_pTradeUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // 创建交易实例

	memset(&buy, 0, sizeof(CThostFtdcInvestorPositionField));
	memset(&sell, 0, sizeof(CThostFtdcInvestorPositionField));

    PriceTick=-1;

}

CustomTradeSpi::~CustomTradeSpi()
{
	g_pTradeUserApi->Release();
	g_pTradeUserApi = NULL;
    order_trade.erase(order_trade.begin(),order_trade.end());
}

void CustomTradeSpi::connectTrade()
{
    g_pTradeUserApi->RegisterSpi(this);                      // 注册事件类
    g_pTradeUserApi->SubscribePublicTopic(THOST_TERT_QUICK);    // 订阅公共流
    g_pTradeUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);   // 订阅私有流
    g_pTradeUserApi->RegisterFront(this->gTradeFrontAddr);              // 设置交易前置地址
    g_pTradeUserApi->Init();
}

void CustomTradeSpi::tradeJoin()
{
    this->g_pTradeUserApi->Join();	
    this->g_pTradeUserApi->Release();
}

void CustomTradeSpi::OnFrontConnected()
{
    ////cout << "connect network success" << endl;
    ////cout << "send login request" << endl;


    CThostFtdcReqUserLoginField login;
    strcpy_s(login.BrokerID,strlen(gBrokerID)+1,gBrokerID);
    strcpy_s(login.UserID, strlen(gInvesterID)+1,gInvesterID);
    strcpy_s(login.Password, strlen(gInvesterPassword)+1,gInvesterPassword);
    g_pTradeUserApi->ReqUserLogin(&login,0);

    return ;
}

void CustomTradeSpi::OnFrontDisconnected(int nReason)
{
}

//登录请求响应
void CustomTradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                                    CThostFtdcRspInfoField *pRspInfo,
                                    int nRequestID,
                                    bool bIsLast)
{
    ////cout << "login request reponse" << endl;
    IS_NULL(pRspUserLogin);

    if(pRspInfo!=NULL && pRspInfo->ErrorID!=0)
    {
        ////cout << "login error:" << pRspInfo->ErrorMsg << endl;
        return;
    }
    if(pRspUserLogin!=NULL)
    {
        //update the newest frontID sessionID
        this->FrontID=pRspUserLogin->FrontID;
        this->SessionID=pRspUserLogin->SessionID;
    }

    //投资者结算结果确认
    struct CThostFtdcSettlementInfoConfirmField lement;
    memset(&lement,0,sizeof(lement));
    strcpy_s(lement.BrokerID, strlen(gBrokerID)+1, gBrokerID);
    strcpy_s(lement.InvestorID, strlen(gInvesterID)+1, gInvesterID);

    g_pTradeUserApi->ReqSettlementInfoConfirm(&lement,0);

    QryInvestorPosition();

}

//投资者结算结果确认响应
void CustomTradeSpi::OnRspSettlementInfoConfirm(
        CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
        CThostFtdcRspInfoField *pRspInfo,
        int nRequestID,
        bool bIsLast)
{
    ////cout << "border confirm" << endl;
    //IS_NULL(pSettlementInfoConfirm);

    if(pRspInfo!=NULL && pRspInfo->ErrorID!=0)
    {
        ////cout << "ErrorId : " << pRspInfo->ErrorID << pRspInfo->ErrorMsg << endl;
    }
}

//请求查询投资者响应
void CustomTradeSpi::OnRspQryInvestor(CThostFtdcInvestorField *pInvestor,
                                      CThostFtdcRspInfoField *pRspInfo,
                                      int nRequestID,
                                      bool bIsLast)
{
    ////cout << "request border reponse" << endl;
    //IS_NULL(pInvestor);

    if(pRspInfo!=NULL && pRspInfo->ErrorID!=0)
    {
        ////cout << "ErrorID : " << pRspInfo->ErrorID << pRspInfo->ErrorMsg << endl;
        return ;
    }
}

//请求查询资金账户响应
void CustomTradeSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
                                            CThostFtdcRspInfoField *pRspInfo,
                                            int nRequestID,
                                            bool bIsLast)
{
    IS_NULL(pTradingAccount);
    ////cout << "request account money reponse" << endl;
    if(pRspInfo!=NULL && pRspInfo->ErrorID!=0)
    {
        ////cout << "ErrorID : " << pRspInfo->ErrorID << pRspInfo->ErrorMsg << endl;
        return ;
    }
}


//报单录入错误回报
void CustomTradeSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                         CThostFtdcRspInfoField *pRspInfo)
{
    IS_NULL(pInputOrder);

    if(pInputOrder!=NULL)
    {
        if(strcmp(pInputOrder->InstrumentID,this->g_pTradeInstrumentID))
            return ;
    }
    ////cout << "order input error " << pInputOrder->InstrumentID <<  endl;
    if(pRspInfo!=NULL && pRspInfo->ErrorID!=0)
    {
        ////cout << "ErrorID : " << pRspInfo->ErrorID << pRspInfo->ErrorMsg << endl;
        strategy->orderInsertError(pInputOrder, pRspInfo);
        return ;
    }

}


//报单通知
void CustomTradeSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    IS_NULL(pOrder);

    if(strcmp(pOrder->InstrumentID,this->g_pTradeInstrumentID))
        return ;

    //update position
    if(pOrder->OrderStatus==THOST_FTDC_OST_AllTraded)
    {
        if(pOrder->CombOffsetFlag[0]==THOST_FTDC_OF_Open)
        {
            if(pOrder->Direction==THOST_FTDC_D_Buy)
            {
                buy.Position+=pOrder->VolumeTraded;
                buy.TodayPosition+=pOrder->VolumeTraded;
            }
            if(pOrder->Direction==THOST_FTDC_D_Sell)
            {
                sell.Position+=pOrder->VolumeTraded;
                sell.TodayPosition+=pOrder->VolumeTraded;
            }
        }
        else if(pOrder->CombOffsetFlag[0]==THOST_FTDC_OF_CloseToday)
        {
            if(pOrder->Direction==THOST_FTDC_D_Buy)
            {
                sell.Position-=pOrder->VolumeTraded;
                sell.TodayPosition-=pOrder->VolumeTraded;
            }
            if(pOrder->Direction==THOST_FTDC_D_Sell)
            {
                buy.Position-=pOrder->VolumeTraded;
                buy.TodayPosition-=pOrder->VolumeTraded;
            }
        }
        else if(pOrder->CombOffsetFlag[0]==THOST_FTDC_OF_Close ||pOrder->CombOffsetFlag[0]==THOST_FTDC_OF_CloseYesterday)
        {
            if(pOrder->Direction==THOST_FTDC_D_Buy)
            {
                sell.Position-=pOrder->VolumeTraded;
            }
            if(pOrder->Direction==THOST_FTDC_D_Sell)
            {
                buy.Position-=pOrder->VolumeTraded;
            }
        }

    }
    if(pOrder->OrderStatus==THOST_FTDC_OST_Canceled)
    {
        if(order_trade[pOrder->OrderRef]!=pOrder->VolumeTraded)
        {
            int count=pOrder->VolumeTraded-order_trade[pOrder->OrderRef];
            if(pOrder->CombOffsetFlag[0]==THOST_FTDC_OF_Open)
            {
                if(pOrder->Direction==THOST_FTDC_D_Buy)
                {
                    buy.Position+=count;
                    buy.TodayPosition+=count;
                }
                else
                {
                    sell.Position+=count;
                    sell.TodayPosition+=count;
                }
            }
            else if(pOrder->CombOffsetFlag[0]==THOST_FTDC_OF_CloseToday)
            {
                if(pOrder->Direction==THOST_FTDC_D_Buy)
                {
                    sell.Position-=count;
                    sell.TodayPosition-=count;
                }
                else
                {
                    buy.Position-=count;
                    buy.TodayPosition-=count;
                }
            }
            else
            {
                if(pOrder->Direction==THOST_FTDC_D_Buy)
                {
                    sell.Position-=count;
                }
                else
                {
                    buy.Position-=count;
                }
            }
        }
        if(pOrder->TimeCondition!=THOST_FTDC_TC_GFD)
        {
            order_trade[pOrder->OrderRef]=pOrder->VolumeTraded;
        }
    }

    if(this->strategy!=NULL)
        this->strategy->tradeMessage(pOrder);
    return ;
}

//成交通知
void CustomTradeSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    IS_NULL(pTrade);

    if(pTrade!=NULL)
    {
        if(strcmp(pTrade->InstrumentID,this->g_pTradeInstrumentID))
            return ;
    }
    ////cout << "bordered message" << endl;

    if(this->strategy!=NULL)
        this->strategy->dealTradeMessage(pTrade);
}

//请求查询投资者持仓响应
void CustomTradeSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
                                              CThostFtdcRspInfoField *pRspInfo,
                                              int nRequestID,
                                              bool bIsLast)
{
    IS_NULL(pInvestorPosition);

    //////cout << "持仓成本 " << pInvestorPosition->PositionCost << endl;

     if(pInvestorPosition!=NULL)
    {
        if(strcmp(pInvestorPosition->InstrumentID,this->g_pTradeInstrumentID))
            return ;
    }

    if(pRspInfo!=NULL && pRspInfo->ErrorID!=0)
    {
        ////cout << "ErrorID : " << pRspInfo->ErrorID << pRspInfo->ErrorMsg << endl;
        return ;
    }
    if(pInvestorPosition->PosiDirection==THOST_FTDC_PD_Long)
    {
        ////cout <<pInvestorPosition->Position << endl;
        this->buy.Position+=pInvestorPosition->Position;
        if(pInvestorPosition->PositionDate==THOST_FTDC_PSD_Today)
            this->buy.TodayPosition+=pInvestorPosition->TodayPosition;
        ////cout << this->g_pTradeInstrumentID << pInvestorPosition->InstrumentID << " buy : " << this->buy.Position << endl;

    }
    if(pInvestorPosition->PosiDirection==THOST_FTDC_PD_Short)
    {
        ////cout <<pInvestorPosition->Position << endl;
        this->sell.Position+=pInvestorPosition->Position;
        if(pInvestorPosition->PositionDate==THOST_FTDC_PSD_Today)
            this->sell.TodayPosition+=pInvestorPosition->TodayPosition;
        ////cout << this->g_pTradeInstrumentID << pInvestorPosition->InstrumentID << "sell : " << this->sell.Position << endl;

        
    }
}

//请求查询投资者持仓明细响应
void CustomTradeSpi::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail,
                                                    CThostFtdcRspInfoField *pRspInfo,
                                                    int nRequestID,
                                                    bool bIsLast)
{
    IS_NULL(pInvestorPositionDetail);
    if(pInvestorPositionDetail!=NULL)
    {
        if(strcmp(pInvestorPositionDetail->InstrumentID,this->g_pTradeInstrumentID))
            return ;
    }
    ////cout << "request border position detail" << endl;
    if(pRspInfo!=NULL && pRspInfo->ErrorID!=0)
    {
        ////cout << "ErrorID : " << pRspInfo->ErrorID << pRspInfo->ErrorMsg << endl;
        return ;
    }
}


void CustomTradeSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
                                      CThostFtdcRspInfoField *pRspInfo,
                                      int nRequestID,
                                      bool bIsLast)
{
    IS_NULL(pInputOrderAction);

    if(pInputOrderAction!=NULL)
    {
        if(strcmp(pInputOrderAction->InstrumentID,this->g_pTradeInstrumentID))
            return ;
    }

    ////cout << endl <<  "border handle request reponse" << endl;
    if(pRspInfo!=NULL && pRspInfo->ErrorID!=0)
    {
        ////cout << "ErrorID : " << pRspInfo->ErrorID << pRspInfo->ErrorMsg << endl;
        return ;
    }

}


//报单操作错误回报
void CustomTradeSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction,
                                         CThostFtdcRspInfoField *pRspInfo)
{
    IS_NULL(pOrderAction);
    if(pOrderAction!=NULL)
    {
        if(strcmp(pOrderAction->InstrumentID,this->g_pTradeInstrumentID))
            return ;
    }

    ////cout << endl << endl << "border handle fail message" << endl;
    if(pRspInfo!=NULL && pRspInfo->ErrorID!=0)
    {
        ////cout << "ErrorID : " << pRspInfo->ErrorID << pRspInfo->ErrorMsg << endl;
    }

}

void CustomTradeSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if(strcmp(pInstrument->InstrumentID,this->g_pTradeInstrumentID))
        return ;
    this->PriceTick=pInstrument->PriceTick;
    ////cout << "price tick " << this->PriceTick << endl;
}

void CustomTradeSpi::getPosition(CThostFtdcInvestorPositionField & buy,
                                 CThostFtdcInvestorPositionField& sell)
{
    buy=this->buy;
    sell=this->sell;

    return ;
}

void CustomTradeSpi::QryAccountMemory()
{
    CThostFtdcQryTradingAccountField req;
    memset(&req,0,sizeof(req));
    strcpy_s(req.BrokerID, strlen(gBrokerID)+1, gBrokerID);
    strcpy_s(req.InvestorID, strlen(gInvesterID)+1, gInvesterID);
    strcpy_s(req.CurrencyID, strlen("CNY")+1, "CNY");
    g_pTradeUserApi->ReqQryTradingAccount(&req,0);
}

void CustomTradeSpi::QryInvestorPosition()
{
    CThostFtdcQryInvestorPositionField req;
	memset(&req,0,sizeof(req));
    strcpy_s(req.BrokerID,strlen(gBrokerID)+1, gBrokerID);
    strcpy_s(req.InvestorID, strlen(gInvesterID)+1, gInvesterID);
    strcpy_s(req.InstrumentID, strlen(g_pTradeInstrumentID)+1, g_pTradeInstrumentID);

    g_pTradeUserApi->ReqQryInvestorPosition(&req,0);
}

void CustomTradeSpi::QryInvestorPositionDetail()
{
    CThostFtdcQryInvestorPositionDetailField req;
    memset(&req,0,sizeof(req));
    strcpy_s(req.BrokerID,strlen(gBrokerID)+1,gBrokerID);
    strcpy_s(req.InvestorID, strlen(gInvesterID)+1,gInvesterID);
    strcpy_s(req.InstrumentID, strlen(g_pTradeInstrumentID)+1,g_pTradeInstrumentID);

    g_pTradeUserApi->ReqQryInvestorPositionDetail(&req,0);
}

void CustomTradeSpi::OrderAction(char offsetFlag,
                 TThostFtdcDirectionType	Direction,
                 TThostFtdcVolumeType volume,
                 TThostFtdcVolumeConditionType volumeCondition,
                 TThostFtdcVolumeType minVolume,
                 TThostFtdcPriceType price,
                 TThostFtdcTimeConditionType timeCondition,
				 TThostFtdcExchangeIDType ExchangeID,
				 int orderRef,
                 TThostFtdcRequestIDType	RequestID
                 )
{
	
    CThostFtdcInputOrderField ord;
    memset(&ord, 0, sizeof(ord));
    strcpy_s(ord.BrokerID,strlen(gBrokerID)+1, gBrokerID);
    strcpy_s(ord.InvestorID, strlen(gInvesterID) + 1, gInvesterID);
    strcpy_s(ord.InstrumentID, strlen(g_pTradeInstrumentID) + 1, g_pTradeInstrumentID);
    strcpy_s(ord.UserID, strlen(gInvesterID) + 1, gInvesterID);
	sprintf_s(ord.OrderRef, "%d", orderRef);
    ord.Direction = Direction;
    ord.CombOffsetFlag[0] = offsetFlag;
    ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    ord.VolumeTotalOriginal = volume;
    ord.ContingentCondition = THOST_FTDC_CC_Immediately;
    ord.VolumeCondition = volumeCondition;
    ord.MinVolume = minVolume;
    ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    ord.IsAutoSuspend = 0;
    ord.UserForceClose = 0;
    ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    ord.LimitPrice = price; // place a price in double here
    ord.TimeCondition = timeCondition;//立即撤单
    //ord.RequestID=RequestID;
	if (!strcmp(ExchangeID, ZHENGZHOUEXCHANGID))
	{
		strcpy_s(ord.ExchangeID, strlen(ZHENGZHOUEXCHANGID) + 1, ZHENGZHOUEXCHANGID);
	}
	else if (!strcmp(ExchangeID, SHANGHAIEXCHANGID))
	{
		strcpy_s(ord.ExchangeID, strlen(SHANGHAIEXCHANGID) + 1, SHANGHAIEXCHANGID);
	}
	else if (!strcmp(ExchangeID, DALIANEXCHANGID))
	{
		strcpy_s(ord.ExchangeID, strlen(DALIANEXCHANGID) + 1, DALIANEXCHANGID);
	}
    int ret=g_pTradeUserApi->ReqOrderInsert(&ord, RequestID);
    ////cout << "ret == " << ret << endl;
    return ;
}

void CustomTradeSpi::CancelOrderAction(
        TThostFtdcOrderRefType	OrderRef,
        TThostFtdcExchangeIDType	ExchangeID,
        TThostFtdcOrderSysIDType	OrderSysID
        )
{
    CThostFtdcInputOrderActionField req;
    memset(&req,0,sizeof(req));
    strcpy_s(req.BrokerID,strlen(gBrokerID)+1,gBrokerID);
    strcpy_s(req.InvestorID, strlen(gInvesterID) + 1,gInvesterID);
    strcpy_s(req.UserID, strlen(gInvesterID) + 1,gInvesterID);
    req.FrontID=this->FrontID;
    req.SessionID=this->SessionID;
    strcpy_s(req.OrderRef, strlen(OrderRef) + 1,OrderRef);
    strcpy_s(req.ExchangeID, strlen(ExchangeID) + 1,ExchangeID);
    strcpy_s(req.OrderSysID, strlen(OrderSysID) + 1, OrderSysID);
    req.RequestID=0;
    strcpy_s(req.UserID, strlen(gInvesterID) + 1,gInvesterID);
    strcpy_s(req.InstrumentID, strlen(g_pTradeInstrumentID) + 1,g_pTradeInstrumentID);
    //撤单标识
    req.ActionFlag=THOST_FTDC_AF_Delete;

    g_pTradeUserApi->ReqOrderAction(&req, 0);
}

void CustomTradeSpi::QryInstrument()
{
    CThostFtdcQryInstrumentField req;
    strcpy_s(req.InstrumentID,sizeof(this->g_pTradeInstrumentID),this->g_pTradeInstrumentID);
    g_pTradeUserApi->ReqQryInstrument(&req,0);
}


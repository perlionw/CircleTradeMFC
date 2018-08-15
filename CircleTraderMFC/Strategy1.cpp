#include "stdafx.h"
#include "Strategy1.h"
#include <iostream>
#include <string>
#include <sstream>
Strategy1::Strategy1()
{
	m_hMutex = CreateMutex(nullptr, FALSE, nullptr);
	InitDB(mydata);
	orderRef = 800;
}


Strategy1::~Strategy1()
{
	DestroyDB(mydata);
}

void Strategy1::marketMessage(CThostFtdcDepthMarketDataField * market)
{
	if (market == NULL)
		return;

	//cout << market->InstrumentID << endl;
	////cout << m_trade[market->InstrumentID] << endl;
	if (m_trade[market->InstrumentID] == NULL)
		return;
	//cout << "============================" << endl;
	cout << market->InstrumentID << endl;
	cout << market->LastPrice << endl;
	cout << market->BidPrice1 << endl;
	cout << market->BidVolume1 << endl;	
	cout << market->AskPrice1 << endl;
	cout << market->AskVolume1 << endl;
	cout << "ExchangeID: " << market->ExchangeID << endl;
 	WaitForSingleObject(m_hMutex, INFINITE);

	if (market->LastPrice > m_work[market->InstrumentID].open_buy_hig_price && m_work[market->InstrumentID].buy_count>0)
	{
		m_work[market->InstrumentID].buy_max_count = 0;
		m_work[market->InstrumentID].buy_count = 0;
	}
	if (market->LastPrice < m_work[market->InstrumentID].open_sell_low_price && m_work[market->InstrumentID].sell_count>0)
	{
		m_work[market->InstrumentID].sell_max_count = 0;
		m_work[market->InstrumentID].sell_count = 0;
	}


	CThostFtdcInvestorPositionField buy, sell;
	m_trade[market->InstrumentID]->getPosition(buy, sell);

	if (buy.Position == 0)
	{
		m_work[market->InstrumentID].close_sell = false;
		//cout << "买持仓为0,不能平仓" << endl;
	}
	if (sell.Position == 0)
	{
		m_work[market->InstrumentID].close_buy = false;
		//cout << "卖持仓为0,不能平仓" << endl;
	}

	m_trade[market->InstrumentID]->getPosition(buy, sell);

	//close sell 
	if (m_work[market->InstrumentID].close_sell && enable_close_sell[market->InstrumentID] && buy.Position>0)
	{
		TThostFtdcOffsetFlagType type = THOST_FTDC_OF_CloseToday;

		//int pos_count = buy.TodayPosition;
		int pos_count = m_work[market->InstrumentID].close_sell_count - m_work[market->InstrumentID].close_sell_run_count;
		if (pos_count == 0)
			m_work[market->InstrumentID].close_sell = false;
		pos_count = buy.TodayPosition < pos_count ? buy.TodayPosition : pos_count;
		
		if (buy.Position != buy.TodayPosition)
		{
			type = THOST_FTDC_OF_CloseYesterday;
			//pos_count = buy.Position - buy.TodayPosition;
			pos_count = (buy.Position - buy.TodayPosition) < pos_count ? (buy.Position - buy.TodayPosition) : pos_count;
		}
		pos_count = pos_count<market->BidVolume1/2 ? pos_count : market->BidVolume1/2;
		if (pos_count>0)
		{
			m_trade[market->InstrumentID]->OrderAction(type,
				THOST_FTDC_D_Sell,
				pos_count,
				THOST_FTDC_VC_AV,
				1,
				market->BidPrice1,
				THOST_FTDC_TC_IOC,
				market->ExchangeID,
				orderRef);
			orderRef++;
		}
	}

	//close buy
	if (m_work[market->InstrumentID].close_buy && enable_close_buy[market->InstrumentID] && sell.Position > 0)
	{
		TThostFtdcOffsetFlagType type = THOST_FTDC_OF_CloseToday;
		//int pos_count = sell.TodayPosition;
		int pos_count = m_work[market->InstrumentID].close_buy_count - m_work[market->InstrumentID].close_buy_run_count;
		pos_count = sell.TodayPosition < pos_count ? sell.TodayPosition : pos_count;
		if (pos_count == 0)
			m_work[market->InstrumentID].close_buy = false;
		if (sell.Position != sell.TodayPosition)
		{
			type = THOST_FTDC_OF_CloseYesterday;
			pos_count = (sell.Position - sell.TodayPosition) < pos_count ? (sell.Position - sell.TodayPosition) : pos_count;
		}

		pos_count = pos_count<market->AskVolume1/2  ? pos_count : market->AskVolume1/2;
		if (pos_count>0)
		{
			m_trade[market->InstrumentID]->OrderAction(type,
				THOST_FTDC_D_Buy, 
				pos_count,
				THOST_FTDC_VC_AV,
				1,
				market->AskPrice1,
				THOST_FTDC_TC_IOC,
				market->ExchangeID,
				orderRef);
			orderRef++;
		}
	}

	//open buy
	if (m_work[market->InstrumentID].buy_count < m_work[market->InstrumentID].buy_max_count)
	{
		int pos_count = m_work[market->InstrumentID].buy_max_count - m_work[market->InstrumentID].buy_count;
		pos_count = pos_count < market->AskVolume1/2 ? pos_count : market->AskVolume1/2;
		
		//卖量只剩1时
		if (market->AskVolume1 == 1)
		{
			pos_count = market->AskVolume1;
		}
		else
		{
			pos_count = pos_count < market->AskVolume1 / 2 ? pos_count : market->AskVolume1 / 2;
		}
		if (pos_count > 0 && enable_open_buy[market->InstrumentID])
		{
			m_trade[market->InstrumentID]->OrderAction(THOST_FTDC_OF_Open,
				THOST_FTDC_D_Buy,
				pos_count,
				THOST_FTDC_VC_AV,
				1,
				market->AskPrice1,
				THOST_FTDC_TC_IOC,
				market->ExchangeID,
				orderRef);
			orderRef++;
			if (m_work[market->InstrumentID].buy_count == 0)
			{
				m_work[market->InstrumentID].open_buy_hig_price = market->AskPrice1 + DIFFPRICE;
			}
		}
	}

	if (m_work[market->InstrumentID].sell_count < m_work[market->InstrumentID].sell_max_count)
	{
		int pos_count = m_work[market->InstrumentID].sell_max_count - m_work[market->InstrumentID].sell_count;
		if (market->BidVolume1 == 1)
		{
			pos_count = market->BidVolume1;
		}
		else
		{
			pos_count = pos_count < market->BidVolume1 / 2 ? pos_count : market->BidVolume1 / 2;
		}
		
		if (pos_count > 0 && enable_open_sell[market->InstrumentID])
		{
			m_trade[market->InstrumentID]->OrderAction(THOST_FTDC_OF_Open,
				THOST_FTDC_D_Sell,
				pos_count,
				THOST_FTDC_VC_AV,
				1,
				market->BidPrice1,
				THOST_FTDC_TC_IOC,
				market->ExchangeID,
				orderRef);
			orderRef++;

			if (m_work[market->InstrumentID].sell_count == 0)
			{
				m_work[market->InstrumentID].open_sell_low_price = market->BidPrice1 - DIFFPRICE;
			}
		}
	}

	ReleaseMutex(m_hMutex);

	return;

}

void Strategy1::tradeMessage(CThostFtdcOrderField * order)
{
	if (order == NULL)
		return;


	if (m_trade[order->InstrumentID] == NULL)
		return;

	if (order->OrderStatus != THOST_FTDC_OST_Canceled && order->OrderStatus != THOST_FTDC_OST_AllTraded)
		return;

	WaitForSingleObject(m_hMutex, INFINITE);

	if (order->CombOffsetFlag[0] == THOST_FTDC_OF_Open)
	{
		if (order->Direction == THOST_FTDC_D_Buy)
		{
			m_work[order->InstrumentID].buy_count += order->VolumeTraded;
		}
		else
		{
			m_work[order->InstrumentID].sell_count += order->VolumeTraded;
		}
	}

	if (order->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday)
	{
		if (order->Direction == THOST_FTDC_D_Buy)
		{
			m_work[order->InstrumentID].close_buy_run_count += order->VolumeTraded;
		}
		else
		{
			m_work[order->InstrumentID].close_sell_run_count += order->VolumeTraded;
		}
	}

	if (order->CombOffsetFlag[0] == THOST_FTDC_OF_CloseYesterday)
	{
		if (order->Direction == THOST_FTDC_D_Buy)
		{
			m_work[order->InstrumentID].close_buy_run_count += order->VolumeTraded;
		}
		else
		{
			m_work[order->InstrumentID].close_sell_run_count += order->VolumeTraded;
		}
	}
		
	ReleaseMutex(m_hMutex);
	return;
  }

void Strategy1::dealTradeMessage(CThostFtdcTradeField * pTrade)
{
	if (pTrade->OffsetFlag == THOST_FTDC_OF_Open)
	{
		if (pTrade->Direction == THOST_FTDC_D_Sell)
		{
			InsertTrade(pTrade);
			cout << "卖出开仓：" << endl;
			cout << "pTrade->InstrumentID: " << pTrade->InstrumentID << endl;
			cout << "pTrade->OffsetFlag: " << pTrade->OffsetFlag << endl;
			cout << "pTrade->Direction: " << pTrade->Direction << endl;
			cout << "pTrade->Price: " << pTrade->Price << endl;
			cout << "pTrade->Volume: " << pTrade->Volume << endl;
		}
		else 
		{
			InsertTrade(pTrade);
			cout << "买入开仓：" << endl;
			cout << "pTrade->InstrumentID: " << pTrade->InstrumentID << endl;
			cout << "pTrade->OffsetFlag: " << pTrade->OffsetFlag << endl;
			cout << "pTrade->Direction: " << pTrade->Direction << endl;
			cout << "pTrade->Price: " << pTrade->Price << endl;
			cout << "pTrade->Volume: " << pTrade->Volume << endl;
		}
	}
	if (pTrade->OffsetFlag == THOST_FTDC_OF_Close)
	{
		if (pTrade->Direction == THOST_FTDC_D_Sell)
		{
			InsertTrade(pTrade);
			cout << "卖出平仓：" << endl;
			cout << "pTrade->InstrumentID: " << pTrade->InstrumentID << endl;
			cout << "pTrade->OffsetFlag: " << pTrade->OffsetFlag << endl;
			cout << "pTrade->Direction: " << pTrade->Direction << endl;
			cout << "pTrade->Price: " << pTrade->Price << endl;
			cout << "pTrade->Volume: " << pTrade->Volume << endl;
		}
		else
		{
			InsertTrade(pTrade);
			cout << "买入平仓：" << endl;
			cout << "pTrade->InstrumentID: " << pTrade->InstrumentID << endl;
			cout << "pTrade->OffsetFlag: " << pTrade->OffsetFlag << endl;
			cout << "pTrade->Direction: " << pTrade->Direction << endl;
			cout << "pTrade->Price: " << pTrade->Price << endl;
			cout << "pTrade->Volume: " << pTrade->Volume << endl;
		}
	}

}

void Strategy1::orderInsertError(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo)
{
	cout << pRspInfo->ErrorID << endl;
	cout << pRspInfo->ErrorMsg << endl;
	if (pInputOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Open)
	{
		if (pInputOrder->Direction == THOST_FTDC_D_Buy)
		{
			enable_open_buy[pInputOrder->InstrumentID] = true;
		}
		else
		{
			enable_open_sell[pInputOrder->InstrumentID] = true;
		}
	}
	else
	{
		if (pInputOrder->Direction == THOST_FTDC_D_Buy)
		{
			enable_close_buy[pInputOrder->InstrumentID] = true;
		}
		else
		{
			enable_close_sell[pInputOrder->InstrumentID] = true;
		}
	}
	return;
}

void Strategy1::setTradeSpi(map<string, CustomTradeSpi *> m)
{
	this->m_trade = m;
	for (map<string, CustomTradeSpi *>::iterator iter = m_trade.begin(); iter != m_trade.end(); iter++)
	{

		enable_open_buy[iter->second->g_pTradeInstrumentID] = true;
		enable_open_sell[iter->second->g_pTradeInstrumentID] = true;

		enable_close_buy[iter->second->g_pTradeInstrumentID] = true;
		enable_close_sell[iter->second->g_pTradeInstrumentID] = true;


		m_work[iter->second->g_pTradeInstrumentID].buy_count = 0;
		m_work[iter->second->g_pTradeInstrumentID].buy_max_count = 0;
		m_work[iter->second->g_pTradeInstrumentID].open_buy_hig_price = 0;
		m_work[iter->second->g_pTradeInstrumentID].sell_count = 0;
		m_work[iter->second->g_pTradeInstrumentID].sell_max_count = 0;
		m_work[iter->second->g_pTradeInstrumentID].open_sell_low_price = 0;
		m_work[iter->second->g_pTradeInstrumentID].close_buy = false;
		m_work[iter->second->g_pTradeInstrumentID].close_sell = false;

	}
}

void Strategy1::addWork(string instru, bool isOpen, bool isBuy, int count)
{
	WaitForSingleObject(m_hMutex, INFINITE);

	//close position
	if (!isOpen)
	{
		//buy
		if (isBuy)
		{
			m_work[instru].close_buy_count = count;
			m_work[instru].close_buy_run_count = 0;
			m_work[instru].close_buy = true;
		}
		else
		{
			m_work[instru].close_sell_count = count;
			m_work[instru].close_sell_run_count = 0;
			m_work[instru].close_sell = true;
		}
			
	}
	else
	{
		if (isBuy)
		{
			m_work[instru].buy_max_count = count;
			m_work[instru].buy_count = 0;
		}
		else
		{
			m_work[instru].sell_max_count = count;
			m_work[instru].sell_count = 0;
		}
	}

	ReleaseMutex(m_hMutex);
}

//void Strategy1::ReplaceOldIntru(string oldInstru, string newInstru)
//{
//	this->oldInstru = oldInstru;
//	this->newInstru = newInstru;
//}

void Strategy1::InsertTrade(CThostFtdcTradeField * pTrade)
{
	string insTrade = "insert into trade(BrokerID,InvestorID,InstrumentID,OrderRef,UserID,\
												ExchangeID,TradeID,Direction,OrderSysID,ParticipantID,\
												ClientID,TradingRole,ExchangeInstID,OffsetFlag,HedgeFlag,\
												Price,Volume,TradeDate,TradeTime,TradeType,PriceSource,TraderID,OrderLocalID,\
												ClearingPartID,BusinessUnit,SequenceNo,TradingDay,SettlementID,\
												BrokerOrderSeq,TradeSource) values(";
	std::stringstream sqlstr;
	sqlstr << "'" << pTrade->BrokerID << "'" << "," << "'" << pTrade->InvestorID << "'" << "," << "'" << pTrade->InstrumentID << "'" << "," << "'" << pTrade->OrderRef << "'"
		<< "," << "'" << pTrade->UserID << "'" << "," << "'" << pTrade->ExchangeID << "'" << "," << "'" << pTrade->TradeID << "'" << "," << "'" << pTrade->Direction << "'"
		<< "," << "'" << pTrade->OrderSysID << "'" << "," << "'" << pTrade->ParticipantID << "'" << "," << "'" << pTrade->ClientID << "'" << "," << "'" << pTrade->TradingRole << "'"
		<< "," << "'" << pTrade->ExchangeInstID << "'" << "," << "'" << pTrade->OffsetFlag << "'" << "," << "'" << pTrade->HedgeFlag << "'" << "," << pTrade->Price
		<< "," << pTrade->Volume << "," << "'" << pTrade->TradeDate << "'" << "," << "'" << pTrade->TradeTime << "'" << "," << "'" << pTrade->TradeType << "'" << "," << "'" << pTrade->PriceSource << "'"
		<< "," << "'" << pTrade->TraderID << "'" << "," << "'" << pTrade->OrderLocalID << "'" << "," << "'" << pTrade->ClearingPartID << "'" << "," << "'" << pTrade->BusinessUnit << "'"
		<< "," << pTrade->SequenceNo << "," << pTrade->TradingDay << "," << pTrade->SettlementID << "," << "'" << pTrade->BrokerOrderSeq << "'"
		<< "," << "'" << pTrade->TradeSource << "'" << ")";
	insTrade += sqlstr.str();
	QueryDBNoResult(mydata, insTrade.c_str());
}

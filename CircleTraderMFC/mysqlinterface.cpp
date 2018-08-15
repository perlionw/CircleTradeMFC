#include "stdafx.h"
#include "mysqlinterface.h"

int InitDB(MYSQL*& mydata)
{
	mysql_library_init(0, NULL, NULL);

	mydata = mysql_init(NULL);
	if (mydata == NULL)
		return -1; 

	mysql_options(mydata, MYSQL_SET_CHARSET_NAME, "gbk");

	mysql_real_connect(mydata, "localhost", "root", "123456", "marketdata", 3306, NULL, 0);

	return 0;
}

int GetFieldCount(MYSQL * mydata)
{
	int count = mysql_field_count(mydata);
	return count;
}

int GetTableFields(MYSQL_RES * myres, MYSQL_FIELD*& myfields)
{
	myfields = mysql_fetch_fields(myres);
	return 0;
}

int GetDBResult(MYSQL * mydata, MYSQL_RES *& myres, const char * mysql)
{
	int ret = mysql_query(mydata, mysql);
	if (ret)
		return ret;
	myres = mysql_store_result(mydata);
	return 0;
}

int QueryDBResult(MYSQL_RES* myres, MYSQL_ROW &myrow)
{
	myrow = mysql_fetch_row(myres);
	if (!myrow)
		return -1;

	return 0;
}

int QueryDBNoResult(MYSQL * mydata, const char * mysql)
{
	int ret = mysql_query(mydata, mysql);
	if (ret)
		return ret;
	return 0;
}

int DestroyDB(MYSQL * mydata)
{
	mysql_close(mydata);
	return 0;
}

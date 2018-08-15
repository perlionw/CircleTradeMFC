#pragma once
#include "mysql.h"

int InitDB(MYSQL*& mydata);

int GetFieldCount(MYSQL* mydata);

int GetTableFields(MYSQL_RES* myres, MYSQL_FIELD*& myfields);

int GetDBResult(MYSQL* mydata, MYSQL_RES*& myres, const char* mysql);

int QueryDBResult(MYSQL_RES* myres, MYSQL_ROW& myrow);

int QueryDBNoResult(MYSQL* mydata, const char* mysql);

int DestroyDB(MYSQL* mydata);

#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite/sqlite3.h"
#include "helpers.h"

using namespace std;

typedef vector<vector<string> > dbResults;

class Database
{
public:
    Database();
    ~Database();
    bool Open(string filename);
    dbResults Query(string query);
    void Close();
    void ShowLastResults();
    bool IsOpen();
    void Remove();
    void SQL();
    bool GetAbortFlag();
    void SetAbortFlag();
    dbResults Results;
    string GetTableHeader(string tbl_name, char separator);
    //int QueryCount;
    bool protect_db;

private:
    sqlite3 *database;
    bool opened;
    string db_filename;
    bool abort;
};


#endif // __DATABASE_H__

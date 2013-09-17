#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite/sqlite3.h"
#include "helpers.h"

typedef std::vector<std::vector<std::string> > dbResults;

class Database
{
public:
    Database();
    ~Database();
    bool Open(std::string filename);
    dbResults Query(std::string query);
    void Close();
    void ShowLastResults();
    bool IsOpen();
    void Remove();
    void SQL();
    bool GetAbortFlag();
    void SetAbortFlag();
    dbResults Results;
    std::string GetTableHeader(std::string tbl_name, char separator);
    //int QueryCount;
    bool protect_db;

private:
    sqlite3 *database;
    bool opened;
    std::string db_filename;
    bool abort;
};


#endif // __DATABASE_H__

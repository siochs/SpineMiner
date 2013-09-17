#include "database.h"

using namespace std;

extern std::ofstream _stdoutput;

//constructor
Database::Database()
{
    opened = false;
    protect_db = false;
    abort = false;
    //QueryCount = 0;
}

//destructor
Database::~Database()
{
}

//Shows the result of the last query on _stdoutput.
void Database::ShowLastResults()
{
    for (vInt i = 0; i < Results.size(); i++)
    {
        for (vInt j = 0; j < Results[i].size(); j++)
            _stdoutput << Results[i][j] << "\t";

        _stdoutput << endl;
    }
}

//opens a sqlite file
bool Database::Open(string filename)
{
    database = NULL;
    if(sqlite3_open(filename.c_str(), &database) == SQLITE_OK)
    {
        opened = true;
        db_filename = filename;
        return true;
    }
    return false;
}

//places a SQL query
dbResults Database::Query(string query)
{

    dbResults local_r;

    if (!opened) return local_r;

    sqlite3_stmt *statement;
    Results.clear();
    local_r.clear();

    if(sqlite3_prepare_v2(database, query.c_str(), query.length(), &statement, 0) == SQLITE_OK)
    {
        int cols = sqlite3_column_count(statement);
        int result = 0;
        while(true)
        {
            result = sqlite3_step(statement);
            if(result == SQLITE_ROW)
            {
               vector<string> values;
               for(int col = 0; col < cols; col++)
               {
                 string  val;
                 char * ptr = (char*)sqlite3_column_text(statement, col);

                 if(ptr)
                 {
                   val = ptr;
                 }
                 else val = "";

                 values.push_back(val);  //NULL will never be pushed
               }
               local_r.push_back(values);
            }
            else
            {
               break;
            }
        }
        sqlite3_finalize(statement);
    }

    //error handling
    string error = sqlite3_errmsg(database);
    if(error != "not an error"){
        _stdoutput << "Error> Unable to perform SQL Query: \"" << query <<"\". " << endl << "SQL Error> "<< error << endl;
        if (protect_db == false)
        {
            //if an error occurs, delete the database! This will only happen in non-protected mode, e.g. when reading OVL files.
            //this is a ( - I admit, it's hard) protection to avoid calculation crashes...
            Close();
            Remove();
            _stdoutput << "Error> Database "<<db_filename<<" deleted." << endl;
            //Quit();
            exit(-1);
        }
    }
    Results.clear();

    Results = local_r;

    //QueryCount ++;

    return local_r;
}

//closes the database
void Database::Close()
{
    sqlite3_close(database);
}

//deletes sqlite file
void Database::Remove()
{
    remove(db_filename.c_str());
}

//checks if database is opened. opened flag is set by Database::Open
bool Database::IsOpen()
{
    return opened;
}

//retrieves abort flag
bool Database::GetAbortFlag()
{
    return abort;
}

//sets abort flag.
//this is useful because it makes debugging the OVL files easier when you see all errors produced from all files.
void Database::SetAbortFlag()
{
    abort = true;
    return;
}

//opens a SQL session with the database in protected mode. So if you misspell a SQL statement, the databasefile will not be deleted.
void Database::SQL()
{
    string Q;
    protect_db = true;
    _stdoutput << "Info> Database ready to accept SQL Query. Enter \"!q\" to exit Query mode." << endl << "SQL>";
    while(1)
    {
        getline(cin , Q);
        if (Q != "!q")
        {
            Query(Q);
            ShowLastResults();
            _stdoutput << endl << "SQL>";
        }
        else
        break;
    }
}

//retrieves table's headers
string Database::GetTableHeader(string tbl_name, char separator)
{
    string header = "";
    string Q = "PRAGMA table_info(\"";
    Q += tbl_name;
    Q += "\");";
    dbResults r = Query(Q);

    if (r.size() > 0)
    {
        for (unsigned int i = 0; i < r.size(); i++)
        {
            header += r[i][1];
            header += separator;
        }
    }

    return header;
}

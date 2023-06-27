#include<iostream>
#include<SQLiteException>
#include<mysqlite>
#include<stringutils>
using namespace sqlite;
using namespace stringutils;
using namespace std;


Row::Row()
{
// do Nothing
}

Row::Row(string columnName, string data) 
{
row[columnName] = data;
}

void Row::setElement(string columnName, string data)
{
row[columnName] = data;
}

int Row::getInt(string columnName)
{
try
{
return stol(row[columnName]);
}catch(invalid_argument& ia)
{
throw SQLiteException("Invalid Conversion to int");
}
}

string Row::getString(string columnName)
{
return row[columnName];
}

Rows::Rows()
{
allocationFlag = 0;
rows = new forward_list<Row *>();
insertIterator = rows->before_begin();
traverseIterator = rows->begin();
}

Rows::Rows(Row *row)
{
if(row == NULL) throw SQLiteException("Row pointer is null");
allocationFlag = 0;
rows = new forward_list<Row *>();
insertIterator = rows->before_begin();
insertIterator = rows->insert_after(insertIterator,row);
traverseIterator = rows->begin();
}

Rows::~Rows()
{
if(allocationFlag == 0)
{
forward_list<Row *>::iterator it = rows->begin();
while(it != rows->end())
{
delete (*it);
++it;
}
delete rows;
}
}

void Rows::addRow(Row *row)
{
if(row == NULL) throw SQLiteException("Row pointer is null");
if(rows->empty())
{
insertIterator = rows->insert_after(insertIterator,row);
traverseIterator = rows->begin();
}
else
{
insertIterator = rows->insert_after(insertIterator,row);
}
}

Row Rows::getRow()
{
Row *row = *this->traverseIterator;
++this->traverseIterator;
return *row;
}

int Rows::hasMoreRows()
{
if(traverseIterator == rows->end()) return 0;
return 1;
}


int SQLiteDB::rowHandler(void *ptr, int columnCount, char **dataPtr, char **columnName)
{
Rows *rows = (Rows *)ptr;
Row *row = new Row();
for(int x=0 ; x<columnCount; x++)
{
if(dataPtr[x] == NULL)
{
row->setElement(string(columnName[x]),"");
}
else
{
row->setElement(string(columnName[x]),string(dataPtr[x]));
}
}
rows->addRow(row);
return 0;
}

SQLiteDB::SQLiteDB() 
{
db = NULL;
}
SQLiteDB::SQLiteDB(string file) 
{
string trimmedFile = trimmed(file);
if(trimmedFile.size() == 0)
{
throw SQLiteException("Invalid Database File");
}
sqlite3_open(trimmedFile.c_str(),&db);
}

void SQLiteDB::open(string file) 
{
string trimmedFile = trimmed(file);
if(trimmedFile.size() == 0)
{
throw SQLiteException("Invalid Database File");
}
sqlite3_open(trimmedFile.c_str(),&db);
}

void SQLiteDB::createTable(string sql) 
{
string trimmedSql(trimmed(sql),0,6);
if(compareStringIgnoreCase(trimmedSql.c_str(),"create") != 0)
{
throw SQLiteException("Invalid SQL Statement To Create Table");
}
char *errorMessage;
int result;
result = sqlite3_exec(db,sql.c_str(),0,0,&errorMessage);
if(result != SQLITE_OK)
{
throw SQLiteException(string(errorMessage));
}
}

void SQLiteDB::executeInsert(string sql) 
{
if(sql.size()==0) throw SQLiteException("Invalid SQL Statement To Insert Record");
string trim = trimmed(sql);
if(trim.size()==0) throw SQLiteException("Invalid SQL Statement To Insert Record");
string trimmedSql(trim,0,6);
if(compareStringIgnoreCase(trimmedSql.c_str(),"insert") != 0)
{
throw SQLiteException("Invalid SQL Statement To Insert Record");
}
char *errorMessage;
int result;
result = sqlite3_exec(db,sql.c_str(),0,0,&errorMessage);
if(result != SQLITE_OK)
{
throw SQLiteException(string(errorMessage));
}
}

void SQLiteDB::executeUpdate(string sql) 
{
if(sql.size()==0) throw SQLiteException("Invalid SQL Statement To Update Record");
string trim = trimmed(sql);
if(trim.size()==0) throw SQLiteException("Invalid SQL Statement To Update Record");
string trimmedSql(trim,0,6);
if(compareStringIgnoreCase(trimmedSql.c_str(),"update") != 0)
{
throw SQLiteException("Invalid SQL Statement To Update Record");
}
char *errorMessage;
int result;
result = sqlite3_exec(db,sql.c_str(),0,0,&errorMessage);
if(result != SQLITE_OK)
{
throw SQLiteException(string(errorMessage));
}
}

void SQLiteDB::executeDelete(string sql) 
{
if(sql.size()==0) throw SQLiteException("Invalid SQL Statement To Delete Record");
string trim = trimmed(sql);
if(trim.size()==0) throw SQLiteException("Invalid SQL Statement To Delete Record");
string trimmedSql(trim,0,6);
if(compareStringIgnoreCase(trimmedSql.c_str(),"delete") != 0)
{
throw SQLiteException("Invalid SQL Statement To Delete Record");
}
char *errorMessage;
int result;
result = sqlite3_exec(db,sql.c_str(),0,0,&errorMessage);
if(result != SQLITE_OK)
{
throw SQLiteException(string(errorMessage));
}
}

Rows SQLiteDB::selectRows(string sql) 
{
if(sql.size()==0) throw SQLiteException("Invalid SQL Statement To Select Record(s)");
string trim = trimmed(sql);
if(trim.size()==0) throw SQLiteException("Invalid SQL Statement To Select Record(s)");
string trimmedSql(trim,0,6);
if(compareStringIgnoreCase(trimmedSql.c_str(),"select") != 0)
{
throw SQLiteException("Invalid SQL Statement To Select Record(s)");
};
char *errorMessage;
int result = 0;
Rows rows;
result = sqlite3_exec(db,sql.c_str(),SQLiteDB::rowHandler,&rows,&errorMessage);
if(result != SQLITE_OK)
{
throw SQLiteException(string(errorMessage));
}
rows.allocationFlag = 1;
return rows;
}

void SQLiteDB::close() 
{
int result = sqlite3_close(db);
if(result != SQLITE_OK)
{
throw SQLiteException("Failed Attempt To close the closed file");
}
}

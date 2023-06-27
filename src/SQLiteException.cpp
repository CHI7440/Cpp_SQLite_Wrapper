#include<SQLiteException>

using namespace sqlite;

SQLiteException::SQLiteException()
{
this->message = "";
}
SQLiteException::SQLiteException(string message)
{
this->message = message;
}
SQLiteException::SQLiteException(const SQLiteException &other)
{
this->message = other.message;
}
SQLiteException::~SQLiteException() throw()
{
//do nothing
}
SQLiteException& SQLiteException::operator = (const SQLiteException &other)
{
this->message = other.message;
return (*this);
}
const char * SQLiteException::what() const throw()
{
return this->message.c_str();
}
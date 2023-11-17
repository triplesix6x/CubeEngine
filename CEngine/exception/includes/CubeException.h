//класс обраточика исключений

#pragma once
#include <exception>
#include <string>



class CubeException : public std::exception
{
public:
	//Конструктор выводит MessageBox с параметрами исключения
	CubeException(int line, const char* file);

	//Функция возвращающая строку с описыванием исключения
	virtual const char* whatEx() const;
	virtual const char* getType() const;
	int getLine() const;
	const std::string& getFile() const;
	std::string getOriginString() const;
private:
	int line;
	std::string file;
protected:
	mutable std::string whatExBuffer;
};


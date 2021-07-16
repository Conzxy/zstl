/*
 * @version: 0.1 2021-6-2
 * @author: Conzxy
 * simple wrapper of O of STDIO
 *
 * use CRTP technique to implement static polymorphic so that avoid the cost of virtual pointer and table
 * but CRTP are only used for types that are not designed to be a container element type, because "Class<T1>" and "Class<T2>" is different type, but don't exist a container type "Container<?>" in C++
 */

#ifndef _WRITER_H
#define _WRITER_H

#include <stdio.h>
#include "../util/noncopyable.h"
#include <limits.h>
#include <stdexcept>
#include "rw_common.h"

namespace zxy{

class WriterException : public std::runtime_error
{
public:
	explicit WriterException(char const* msg)
		: std::runtime_error{msg}
	{}
};

/*
 * CLASS TEMPLATE Writer
 * writer base class
 */
template<typename Derived>
class Writer : noncopyable
{
public:
	Writer() = default;
	~Writer() = default;

	template<typename... Args>
	void Write(char const* fmt, Args&&... args) const
	{
		static_cast<Derived const*>(this)->write(fmt, args...);
	}
	
	int Write(int c) const
	{
		return static_cast<Derived const*>(this)->putc(c);
	}

	int Write(char const* str) const
	{
		return static_cast<Derived const*>(this)->puts(str);
	}
	
};

/*
 * CLASS ConsoleWriter
 * used for Console logging
 */
class ConsoleWriter : public Writer<ConsoleWriter>
{
public:
	template<typename... Args>
	void Write(char const* fmt, Args&&... args) const
	{
		printf(fmt, args...);
	}
	
	int Write(int c) const
	{
		return fputc(c, stdout);
	}

	int Write(char const* str) const
	{
		return fputs(str, stdout);
	}
};

class ErrorWriter : public Writer<ErrorWriter> {
	template<typename... Args>
	void Write(char const* fmt, Args&&... args) const
	{
		printf(fmt, args...);
	}
	
	int Write(int c) const
	{
		return fputc(c, stdout);
	}

	int Write(char const* str) const
	{
		return fputs(str, stdout);
	}
};

/*
 * CLASS FileWriter
 * used for file logging
 */
class FileWriter : public Writer<FileWriter>
{
public:
	explicit FileWriter(char const* filename, OpenMode mode=OpenMode::trunc)
		: filename_{filename}
	{
		switch(mode)
		{
			case OpenMode::append:
				fp_ = fopen(filename, "a");break;
			case OpenMode::trunc:
				fp_ = fopen(filename, "w");break;
			case OpenMode::rappend:
				fp_ = fopen(filename, "a+");break;
			case OpenMode::rtrunc:
				fp_ = fopen(filename, "w+");break;
		}

		char buf[PATH_MAX+32];
		snprintf(buf, sizeof(buf), "Faild in open file: <%s>", filename);
		if(!fp_) throw WriterException{buf};
	}
	
	FileWriter(FILE* fp)
		: fp_{fp}
	{ 
		fp = NULL; 
	}

	~FileWriter()
	{
		if(fp_ != stdin || fp_ != stdout || fp_ != stderr)
			fclose(fp_);
	}

	template<typename... Args>
	void Write(char const* fmt, Args&&... args) const
	{
		fprintf(fp_, fmt, args...);
	}
	
	//Although you can declare this as "write"
	//but I want to add prefix "b" to distinguish binary and text
	int BWrite(void* data, size_t size, size_t n) const
	{
		return fwrite(data, size, n, fp_);	
	}

	int Write(int c) const
	{
		return fputc(c, fp_);
	}

	int Write(char const* str) const
	{
		return fputs(str, fp_);
	}

	char const* filename() const noexcept
	{
		return filename_;
	}
	
	FILE* fp() noexcept
	{
		return fp_;
	}

	FILE const* fp() const noexcept
	{
		return fp_;
	}
	
private:
	FILE* fp_;
	char const* filename_;
};

extern ConsoleWriter kConsoleWriter;
extern ErrorWriter kErrorWriter;

template<typename ...Args>
void Log(Args&&... args)
{ kConsoleWriter.Write(args...); }

template<typename ...Args>
void ErrorLog(Args&&... args)
{ kErrorWriter.Write(args...); }

}//namespace zxy

#endif

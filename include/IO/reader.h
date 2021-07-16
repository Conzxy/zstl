/*
 * @version: 0.1 2021-6-2
 * @author: Conzxy
 * simple wrapper of I of STDIO
 * also, use RAII technique 
 */

#ifndef _READER_H
#define _READER_H

#include <stdio.h>
#include "../util/noncopyable.h"
#include <stdexcept>
#include <limits.h>
#include "rw_common.h"

namespace zxy{

class ReaderException : public std::runtime_error
{
public:
	explicit ReaderException(char const* msg)
		: std::runtime_error{msg}
	{}
};

/*
 * CLASS TEMPLATE Reader
 * Reader base class
 */
template<typename D>
class Reader : noncopyable
{
public:
	Reader()  = default;
	~Reader() = default;

	template<typename... Args>
	void Read(char const* fmt, Args&... args) const
	{
		static_cast<D const*>(this)->read(fmt, args...);
	}

	int Read() const
	{
		return static_cast<D const*>(this)->getc();
	}

	char* Read(char* buf, int n) const
	{
		return static_cast<D const*>(this)->gets(buf, n);
	}	
};

/*
 * CLASS ConsoleReader
 * simple wrapper of console read in C style
 */
class ConsoleReader : public Reader<ConsoleReader>
{
public:
	template<typename... Args>
	void Read(char const* fmt, Args&... args) const
	{
		scanf(fmt, &args...);
	}

	int Read() const
	{
		return fgetc(stdin);
	}

	char* Read(char* buf, int n) const
	{
		return fgets(buf, n, stdin);
	}
};

/*
 * CLASS FileReader
 * File wrapper in C style
 */
class FileReader : public Reader<FileReader>
{
public:
	explicit FileReader(char const* filename, OpenMode md)
		: filename_{filename}
	{
		switch(md)
		{
			case OpenMode::r:
				fp_ = fopen(filename, "r");break;
			case OpenMode::rw:
				fp_ = fopen(filename, "r+");break;
		}
		
		char buf[PATH_MAX+30];
		snprintf(buf, sizeof(buf), "Failed in open: < %s >\n", filename);
		
		if(!fp_) throw ReaderException{buf};

	}

	FileReader(FILE* fp)
		: fp_{fp}
	{
		fp = NULL;
	}

	~FileReader()
	{
		if(fp_ != stdin || fp_ != stdout || fp_ != stderr)
			fclose(fp_);
	}

	template<typename... Args>
	void Read(char const* fmt, Args&... args) const
	{
		fscanf(fp_, fmt, &args...);
	}

	int BRead(void* data, size_t size, size_t n) const
	{
		return fread(data, size, n, fp_);
	}

	int Read() const
	{
		return fgetc(fp_);
	}

	char* Read(char* buf, int n) const
	{
		return fgets(buf, n, fp_);
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

extern ConsoleReader kConsoleReader;

template<typename ...Args>
void Scan(Args&&... args)
{ kConsoleReader.Read(args...); }

}//namespace zxy

#endif

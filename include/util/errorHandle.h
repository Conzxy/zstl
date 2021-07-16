/*
 * APUE ERROR ROUTINE HEADER
 * include as follows:
 * function		Add string from strerror?  parameter to strerror?  terminate?
 * err_dump			yes							errno				abort()	
 * err_exit			yes							explicit specify	exit(1)
 * err_cond			yes							explicit specify	return
 * err_msg			no							(NULL)				return
 * err_quit			no							(NULL)				exit(1)
 * err_ret			yes							errno				return
 * err_sys			yes							errno				exit(1)
 *
 * err_dump, err_exit, err_quit, err_sys --> fatal error
 * err_cond, err_msg, err_ret			 --> non fatal error
 * err_dump, err_ret, err_sys			 --> system call(errno)
 * err_cond, err_exit, err_quit, err_msg --> non system call(explicit specify | NULL)
 */

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <errno.h>
#include <utility>
#include <string.h>
#include <stdlib.h>

constexpr int MAXLINE = 4096;

//print a message and return to caller
//caller can specify errnoflag to add string from strerror
template<typename... FMT>
void error_doit(int errnoflag, int error, char const* fmt,FMT&&... args)
{
	char buf[MAXLINE];
	snprintf(buf,MAXLINE-1,fmt,std::forward<FMT>(args)...);
	if(errnoflag)
		snprintf(buf + strlen(buf), MAXLINE - strlen(buf) - 1, ": %s", strerror(error));

	strcat(buf,"\n");
	fflush(stdout);	//in case stdout and stderr are the same
	fputs(buf,stderr);
	fflush(NULL);	//flushes all stdio output streams

}

//Fatal error related to system call
//print a message,dump core and terminate
template<typename... FMT>
void err_dump(char const* fmt,FMT&&... args)
{
	error_doit(1, errno, fmt, std::forward<FMT>(args)...);
	abort();	//dump core and terminate
	exit(1);
}

//Fatal error unrelated to a system call
//print a message and terminate
template<typename... FMT>
void err_quit(char const* fmt,FMT&&... args)
{
	error_doit(0, 0, fmt, std::forward<FMT>(args)...);
	exit(1);
}

//NonFatal error unrelated to a system call
//print a message and return
template<typename... FMT>
void err_msg(char const* fmt, FMT&&... args)
{
	error_doit(0, 0, fmt, std::forward<FMT>(args)...);
}

//Nonfatal error related to a system call
//print a message and return
template<typename... FMT>
void err_ret(char const* fmt,FMT&&... args)
{
	error_doit(1, errno, fmt, std::forward<FMT>(args)...);
}

//Fatal error related to a system call
//print a message and terminate
template<typename... FMT>
void err_sys(char const* fmt, FMT&&... args)
{
	error_doit(1, errno, fmt, std::forward<FMT>(args)...);
	exit(1);
}

//Fatal error unrelated to a system call
//error code passed as explict parameter
//print message and terminate
template<typename... FMT>
void err_exit(int error, char const* fmt,FMT&&... args)
{
	error_doit(1, error, fmt, std::forward<FMT>(args)...);
	exit(1);
}

//NonFatal error unrelated error to a system call
//error code passed as explicit parameter
//print message and return
template<typename... FMT>
void err_cond(int error,char const* fmt,FMT&&... args)
{
	error_doit(1, error, fmt, std::forward<FMT>(args)...);
}


#endif

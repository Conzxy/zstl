/*
 * @version: 2021-5-17
 * @author: Conzxy
 * noncopyable base class
 */

#ifndef _NONCOPYABLE_H
#define _NONCOPYABLE_H

class noncopyable{
public:
	//disable copy constructor and operator=
	noncopyable(noncopyable const&) = delete;
	noncopyable& operator=(noncopyable const&) = delete;
	
	
protected:
	//only derived class use
	noncopyable() = default;
	~noncopyable() = default;
};

#endif

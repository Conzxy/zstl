/*
 * @version: 0.1 2021-6-2
 * @author: Conzxy
 * common part of reader and writer
 */

#ifndef _RW_COMMON_H
#define _RW_COMMON_H

namespace zxy{

enum class OpenMode: int
{
	r,
	rw,
	append,
	rappend,
	trunc,
	rtrunc,
};

} //namespace zxy

#endif

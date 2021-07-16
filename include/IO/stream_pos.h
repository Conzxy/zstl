/*
 * @version: 0.1 2021-6-2
 * @author: Conzxy
 * simple wrapper of stream position API
 */

#ifndef _STREAM_POS_H
#define _STREAM_POS_H

#include <stdio.h>

namespace zxy{

enum class SeekMode: int
{
	set, 
	cur,
	end, 
};

class StreamPos final
{
	

	static long tell(FILE* fp_)
	{
		return ftell(fp_);
	}

	static int seek(FILE* fp_, long off, SeekMode mode)
	{
		switch(mode)
		{
			case SeekMode::set:
				return fseek(fp_, off, SEEK_SET);break;
			case SeekMode::cur:
				return fseek(fp_, off, SEEK_CUR);break;
			case SeekMode::end:
				return fseek(fp_, off, SEEK_END);break;
		}
	}
};

}//namespace zxy

#endif

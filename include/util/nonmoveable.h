#ifndef _ZXY_NONMOVEABLE_H
#define _ZXY_NONMOVEABLE_H

namespace zxy{

struct nonmoveable {
#if __cpluscplus > 201103L
    nonmoveable(nonmoveable&&) = delete;
    nonmoveable&& operator=(nonmoveable&&) = delete;
#endif
};

} //namespace zxy

#endif //_ZXY_NONMOVEABLE_H
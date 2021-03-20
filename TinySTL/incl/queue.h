//
// Created by conzxy on 2021/3/20.
//

#ifndef TINYSTL_QUEUE_H
#define TINYSTL_QUEUE_H

#include "deque.h"
#include <config.h>

namespace TinySTL{
    template<typename T,typename C = TinySTL::deque<T>>
    class queue{
    public:
        using value_type        =typename C::value_type;
        using pointer           =typename C::pointer;
        using reference         =typename C::reference;
        using const_reference   =typename C::const_reference;
        using size_type         =typename C::size_type;
    protected:
        C c;
    public:
        explicit queue(C const& other):c{other}{}

        explicit queue(C&& other=C{}):c{STL_MOVE(other)}{}


        bool            empty() const   { return c.empty(); }
        size_type       size() const    { return c.size(); }
        reference       front()         { return c.front(); }
        const_reference front() const   { return c.front(); }
        reference       back()          { return c.back(); }
        const_reference back() const    { return c.back(); }
        void            push(value_type const& x)          { c.push_back(x); }
        void            push(value_type&& x)               { c.push_back(STL_MOVE(x)); }
        template<typename ...Args>
        void emplace(Args&&... args){
            c.emplace_back(TinySTL::forward<Args>(args)...);
        }
        void pop()   { c.pop_front(); }
        void swap(queue<T,C> & other) noexcept{
            STL_SWAP(c,other.c);
        }

        template<typename T2,typename C2>
        friend bool operator==(queue<T2,C2> &q1,queue<T2,C2> &q2){
            return q1.c == q2.c;
        }

        template<typename T2,typename C2>
        friend bool operator<(queue<T2,C2> &q1,queue<T2,C2> &q2){
            return q1.c < q2.c;
        }

        template<typename T2,typename C2>
        friend bool operator >=(queue<T2,C2>& q1,queue<T2,C2> &q2){
            return !(q1 < q2);
        }

        template<typename T2,typename C2>
        friend bool operator >(queue<T2,C2> &q1,queue<T2,C2> &q2){
            return q2 < q1;
        }

        template<typename T2,typename C2>
        friend bool operator <=(queue<T2,C2> &q1,queue<T2,C2> &q2){
            return !(q1 > q2);
        }

    };

    template<typename T,typename C>
    void swap(queue<T,C> &q1,queue<T,C> &q2) noexcept(noexcept(q1.swap(q2))){
        q1.swap(q2);
    }


}
#endif //TINYSTL_QUEUE_H

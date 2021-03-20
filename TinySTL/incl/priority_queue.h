//
// Created by 谁敢反对宁宁，我就打爆他狗头！ on 2021/2/11.
//

#ifndef TINYSTL_PRIORITY_QUEUE_H
#define TINYSTL_PRIORITY_QUEUE_H
#include "vector.h"
#include <config.h>
#include "Functional.h"
#include "stl_heap.h"
template<typename T,typename Container=STL_ vector<T>,
        typename Compare=STL_ less<typename Container::value_type>>
class priority_queue{
public:
    using container_type    =Container;
    using value_compare     =Compare;
    using value_type        =typename Container::value_type;
    using size_type         =typename Container::size_type;
    using reference         =typename Container::reference;
    using const_reference   =typename Container::const_reference;

    priority_queue()
    :c(),cmp(){}

    explicit priority_queue(Compare const& compare) : c(),cmp(compare){
    }

    priority_queue(Compare const& compare,Container const& cont) : c(cont),cmp(compare){
        make_heap(c.begin(),c.end(),cmp);
    }

    priority_queue(Compare const& compare,Container && cont) : c(STL_MOVE(cont)),cmp(compare){
        make_heap(c.begin(),c.end(),cmp);
    }

    template<typename InputIt>
    priority_queue(InputIt first,InputIt last,
                   Compare const& compare,Container const& cont) : c(cont),cmp(compare){
        c.insert(c.end(),first,last);
        make_heap(c.begin(),c.end(),cmp);
    }

    template<typename InputIt>
    priority_queue(InputIt first,InputIt last,
                   Compare const& compare=Compare(),Container &&cont=Container()) : c(STL_MOVE(cont)),cmp(compare){
        c.insert(c.end(),first,last);
        make_heap(c.begin(),c.end(),cmp);
    }

    priority_queue(STD_ initializer_list<value_type> il,Compare const& compare=Compare()) : c(il.begin(),il.end()),cmp(compare){
        make_heap(c.begin(),c.end());
    }

    priority_queue(priority_queue const& other) : c(other.c),cmp(other.cmp){
    }

    priority_queue(priority_queue && other) : c(STL_MOVE(other.c)),cmp(STL_MOVE(other.cmp)){
    }

    priority_queue& operator=(priority_queue const& rhs){
        using std::swap;
        auto copy=rhs;
        swap(*this,copy);
        return *this;
    }

    priority_queue& operator=(priority_queue && rhs){
        swap(*this,rhs);
        rhs.c.~Container();
    }

    ~priority_queue(){}

    const_reference top()const { return c.front(); }

    bool empty()const { return c.empty(); }
    size_type size()const { return c.size(); }

    void push(value_type const& x){
        c.push_back(x);
        STL_ push_heap(c.begin(),c.end(),cmp);
    }

    void push(value_type && x){
        c.push_back(STL_MOVE(x));
        STL_ push_heap(c.begin(),c.end(),cmp);
    }

    template<typename... Args>
    void emplace(Args&&... args){
        c.emplace_back(STL_ forward<Args>(args)...);
        STL_ push_heap(c.begin(),c.end(),cmp);
    }

    void pop(){
        STL_ pop_heap(c.begin(),c.end(),cmp);
        c.pop_back();
    }

    void swap(priority_queue const& other)noexcept{
        STL_SWAP(c,other.c);
        STL_SWAP(cmp,other.cmp);
    }
protected:
    Container c;
    Compare cmp;
private:
    friend bool operator==(priority_queue const& lhs,priority_queue const& rhs){ return lhs.c==rhs.c; }
    friend bool operator!=(priority_queue const& lhs,priority_queue const& rhs){ return lhs.c!=rhs.c; }
};

template<typename T,typename Container,typename Compare>
void swap(priority_queue<T,Container,Compare> &lhs,
          priority_queue<T,Container,Compare> &rhs)noexcept(noexcept(lhs.swap(rhs))){
    lhs.swap(rhs);
}

template<typename Compare,typename Container>
priority_queue(Compare,Container)
->priority_queue<typename Container::value_type,Container,Compare>;

template<typename InputIt,
        typename Compare=STL_ less<typename STL_ iterator_traits<InputIt>::value_type>,
        typename Container=STL_ vector<typename STL_ iterator_traits<InputIt>::value_type>>
priority_queue(InputIt,InputIt,Compare=Compare(),Container=Container())
->priority_queue<typename Container::value_type,Container,Compare>;

#endif //TINYSTL_PRIORITY_QUEUE_H

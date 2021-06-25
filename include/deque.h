//
// Created by conzxy on 2021/2/14.
// 该文件包含:
// CLASS TEMPLATE deque_iterator
// CLASS TEMPLATE deque

#ifndef TINYSTL_DEQUE_H
#define TINYSTL_DEQUE_H

#include "config.h"
#include "stl_iterator.h"
#include "stl_algorithm.h"
#include "stl_uninitialized.h"
#include "Allocator.h"
#include "stl_utility.h"
#include <initializer_list>
#include "stl_exception.h"

#ifndef DEQUEUE_MAP_INIT_SIZE
#define DEQUEUE_MAP_INIT_SIZE 8
#endif

namespace TinySTL {
    //如果buf_size不是默认值(0),我们根据value_sz决定缓冲区大小
    constexpr int_ deque_buffer_size(int_ buf_sz,int_ value_sz){
        return (buf_sz!=0) ? buf_sz : ((value_sz<512) ? 512/value_sz : 1);
    }


    template<typename T, typename Ref, typename Ptr,int_ BufSize>
    struct deque_iterator:public iterator<Random_access_iterator_tag,T>{
        //为什么要弄个self?
        //因为iterator(和const_iterator)都是标准的，而self是支持自定义的
        template<typename T_,typename Alloc>
        friend class deque;

        using iterator       =deque_iterator<T,T&,T*,BufSize>;
        using const_iterator =deque_iterator<T,T const&,T const*,BufSize>;
        using self           =deque_iterator;
        using buf            =T*;
        using map_pointer    =buf*;

        using difference_type=STD_ ptrdiff_t;
        using value_type     =T;
        using reference      =Ref;
        using pointer        =Ptr;

        static constexpr int_ buffer_size(){ return deque_buffer_size(BufSize,sizeof(T)); }

        deque_iterator() : cur(nullptr),first(nullptr),last(nullptr),node(nullptr){
        }

        deque_iterator(const_iterator const& iter) : cur(iter.cur),first(iter.first),last(iter.last),node(iter.node){
        }

        deque_iterator(const_iterator&& iter) : cur(iter.cur),first(iter.first),last(iter.last),node(iter.node){
            iter.cur=iter.first=iter.last=iter.node=nullptr;
        }

        deque_iterator(buf buf,map_pointer map) : cur(buf),first(*map),last(*map+buffer_size()),node(map){
        }

        operator const_iterator()const{
            return const_iterator(cur,node);
        }

        void swap(const_iterator const& other)noexcept{
            STL_SWAP(cur,other.cur);
            STL_SWAP(first,other.cur);
            STL_SWAP(last,other.last);
            STL_SWAP(node,other.node);
        }

        //**********key operations***********//
        //到达缓冲区边缘，需要跳到其他缓冲区
        void set_node(map_pointer new_node){
            node=new_node;
            first=*new_node;
            last=first+difference_type(buffer_size());
        }

        reference operator*()const{
            return *cur;
        }

        pointer operator->()const{
            return &(*cur);
        }

        //计算两个迭代器cur的距离
        //需要两个迭代器之间的缓冲区个数以及this的cur与first距离，减数的last的cur与last距离
        //当首迭代器和尾后迭代器相等时，也有一个缓冲区-->缓冲区个数>=1
        difference_type operator-(self const& iter)const{
            return difference_type(buffer_size())*(node-iter.node-1)
                    +(cur-first)+(iter.last-iter.cur);
        }

        self& operator++(){
            //到达缓冲区尾部我们需要跳到新的缓冲区去
            if(++cur == last){
                set_node(node+1);
                cur=first;
            }
            return *this;
        }

        self operator++(int){
            self tmp=*this;
            ++*this;
            return tmp;
        }

        self& operator--(){
            if(cur == first){
                set_node(node-1);
                cur=last;
            }
            --cur;
            return *this;
        }

        self operator--(int){
            self tmp=*this;
            --*this;
            return tmp;
        }

        //what:给一个偏移量，可以跳到任何缓冲区去，实现Random access
        self& operator+=(difference_type distance){
            difference_type offset=(cur-first)+distance;
            if(offset>=0 && offset < buffer_size()){
                //仍在同一缓冲区中
                cur+=distance;
            }
            else{//跳转到不同缓冲区

                //以下公式需要自己动手画图推导：
                //新cur=新first+offset-node_offset*buffer_size
                //总的偏移量为距原first的偏移量，即offset，减去要跳转的缓冲区数量个buffer_size
                //如果是正向偏移，那么总的偏移量-新缓冲区前面的（包括原缓冲区）即可
                //如果是逆向偏移，那么原缓冲区前面的（包括新缓冲区）-总的偏移量，符号逆转下就可以得到该公式
                //因此，我们需要node_offset确定跳转的缓冲区个数
                //正向理解简单，而逆向，offset=-1对应的是前一缓冲区最后一个元素，该元素是逆向第一元素对应0，需要+1调整
                //而且逆向的offset不包含原缓冲区，因此需要-1（正向的offset包含原缓存区不要-1）
                difference_type node_offset=offset>0?
                        offset/buffer_size():
                        (-static_cast<difference_type>((-offset-1)/buffer_size())-1);
                set_node(node+node_offset);
                cur=first+(offset-node_offset*static_cast<difference_type>(buffer_size()));
            }
            return *this;
        }

        self& operator-=(difference_type distance){
            return *this+=-distance;
        }

        self operator+(difference_type n)const{
            self tmp=*this;
            return tmp+=n;
        }

        self operator-(difference_type n)const{
            self tmp=*this;
            return tmp-=n;
        }

        reference operator[](difference_type n)const{
            return *(*this+n);
        }

        friend bool operator==(deque_iterator const& lhs,deque_iterator const& rhs){
            return lhs.cur == rhs.cur;
        }

        friend bool operator!=(deque_iterator const& lhs,deque_iterator const& rhs){
            return !(lhs == rhs);
        }

        friend bool operator<(deque_iterator const& lhs,deque_iterator const& rhs){
            //如果管理同一个buffer，那么就比较cur;否则比较buffer
            return lhs.node == rhs.node?lhs.cur<rhs.cur:lhs.node<rhs.node;
        }

        friend bool operator>=(deque_iterator const& lhs,deque_iterator const& rhs){
            return !(lhs < rhs);
        }

        friend bool operator>(deque_iterator const& lhs,deque_iterator const& rhs){
            return rhs < rhs;
        }

        friend bool operator<=(deque_iterator const& lhs,deque_iterator const& rhs){
            return !(lhs > rhs);
        }
    private:
        //buf表示管理buffer的指针
        //node表示node的位置，方便在map中的移动
        buf cur;            //指向当前索取的元素
        buf first;          //指向buffer的头部
        buf last;           //指向buffer的尾后(虚拟位置）
        map_pointer node;   //(Buffer的)管控中心
    };


    template<typename T,typename Allocator=STL_ allocator<T>>
    class deque{
    public:
        using value_type                =T;
        using allocator_type            =Allocator;
        using pointer                   =typename allocator_traits<allocator_type>::pointer;
        using const_pointer             =typename allocator_traits<allocator_type>::const_pointer;
        using reference                 =value_type&;
        using const_reference           =value_type const&;
        using iterator                  =deque_iterator<value_type,reference,pointer,0>;
        using const_iterator            =deque_iterator<value_type,const_reference,const_pointer,0>;
        using size_type                 =STD_ size_t;
        using difference_type           =STD_ ptrdiff_t;
        using reverse_iterator          =STL_ reverse_iterator<iterator>;
        using const_reverse_iterator    =STL_ reverse_iterator<const_iterator>;

        deque();
        deque(size_type n);
        deque(size_type n,value_type const& val);
        template<typename Input>
        deque(Input first,Input last);
        deque(deque const&);
        deque(deque&&)noexcept;
        deque(STD_ initializer_list<value_type>);

        deque& operator=(deque const&);
        deque& operator=(deque&&)noexcept;
        ~deque();

        template<typename Input>
        void assign(Input first,Input last);
        void assign(size_type n,value_type const&);
        void assign(STD_ initializer_list<value_type>);

        //iterator:
        iterator                    begin()       { return start; }
        iterator                    end()         { return finish; }
        const_iterator              begin()  const{ return static_cast<const_iterator>(start); }
        const_iterator              end()    const{ return static_cast<const_iterator>(finish); }
        const_iterator              cbegin() const{ return begin(); }
        const_iterator              cend()   const{ return end(); }
        reverse_iterator            rbegin()      { return reverse_iterator(begin()); }
        reverse_iterator            rend()        { return reverse_iterator(end()); }
        const_reverse_iterator      rbegin() const{ return const_reverse_iterator(begin()); }
        const_reverse_iterator      rend()   const{ return const_reverse_iterator(end()); }
        const_reverse_iterator      crbegin()const{ return const_reverse_iterator(cbegin()); }
        const_reverse_iterator      crend()  const{ return const_reverse_iterator(cend()); }

        //capacity
        size_type size() const{ return finish-start; }
        bool      empty()const{ return start==finish; }
        void      resize(size_type sz);
        void      resize(size_type sz,value_type const& val);
        void      shrink_to_fit();
        //element access
        reference       operator[](size_type index)     { return *(start+index); }
        const_reference operator[](size_type index)const{ return *(start+index); }
        reference       at(size_type index)     { TINYSTL_DEBUG(index > 0 && index < size()); return *(start+index); }
        const_reference at(size_type index)const{ TINYSTL_DEBUG(index >0 && index < size()); return *(start+index); }
        reference       front()      { return *start; }
        const_reference front()const { return *start; }
        reference       back()     { return *(finish-1); }
        const_reference back()const{ return *(finish-1); }

        //modifies
        template<typename ...Args> reference emplace_back(Args&&... args);
        template<typename ...Args> reference emplace_front(Args&&... args);
        template<typename ...Args> iterator emplace(const_iterator pos,Args&&... args);

        void push_back(value_type const& val);
        void push_back(value_type &&val);
        void push_front(value_type const& val);
        void push_front(value_type &&val);

        iterator insert(const_iterator pos,value_type const& val);
        iterator insert(const_iterator pos,value_type&& val);
        iterator insert(const_iterator pos,size_type n,value_type const &val);
        iterator insert(const_iterator pos,STD_ initializer_list<value_type>);
        template<typename Input>
        iterator insert(const_iterator pos,Input first,Input last);

        void pop_back();
        void pop_front();
        
        iterator erase(const_iterator pos);
        iterator erase(const_iterator first,const_iterator last);

        void swap(deque& )noexcept;
        void clear();
    protected:
        using buf                       = pointer;
        using map_pointer               = buf*;
        using map_allocator             = typename Allocator::template rebind <buf>;
        using data_allocator            = Allocator;
    private:
        void map_init(size_type numElem);
        void create_map(size_type mapSize);
        void create_buffer(map_pointer _start,map_pointer _finish);
        void fill_init(size_type n,value_type const& val);
        template<typename Input>
        void range_init(Input first,Input last);
        void push_back_aux(value_type const& val);
        void push_front_aux(value_type const& val);
        iterator insert_aux(const_iterator pos,value_type const& val);
        iterator fill_insert(const_iterator pos,size_type n,value_type const& val);
        void reserve_map_back(size_type add_node=1);
        void reserve_map_front(size_type add_node=1);
        iterator make_iter(const_iterator iter);
        void reallocate_map(size_type add_node,bool isFront);

        friend void swap(deque& lhs,deque& rhs)noexcept(noexcept(lhs.swap(rhs))){ lhs.swap(rhs); }

    protected:
        iterator start;         //管理第一个缓冲区(对应第一个node)
        iterator finish;        //管理最后一个缓冲区(对应最后一个node)
        //实际上这个map和迭代器的node为同一类型，但是意义不同
        //指针有两个内存解释：1)线性连续内存，或数组;2)内存地址，代表一个位置
        //map是1),而node是2)
        map_pointer map;        //映射：node->buffer
        size_type map_size;
        static constexpr size_type buf_size=deque_buffer_size(0,sizeof(value_type));
    };
}

namespace TinySTL {
    template<typename T, typename Alloc>
    deque<T, Alloc>::deque() {
        fill_init(0,value_type{});
    }

    template<typename T,typename Alloc>
    deque<T,Alloc>::deque(size_type n){
        fill_init(n,value_type{});
    }

    template<typename T,typename Alloc>
    deque<T,Alloc>::deque(size_type n,value_type const& val){
        fill_init(n,val);
    }

    template<typename T,typename Alloc>
    template<typename Input>
    deque<T,Alloc>::deque(Input first,Input last){
        range_init(first,last);
    }

    template<typename T,typename Alloc>
    deque<T,Alloc>::deque(STD_ initializer_list<value_type> il) : deque(il.begin(),il.end()){
    }


    template<typename T,typename Alloc>
    deque<T,Alloc>::deque(deque const& other){
        range_init(other.begin(),other.end());
    }

    template<typename T,typename Alloc>
    deque<T,Alloc>::deque(deque && other)noexcept : start(STL_MOVE(other.start)),finish(STL_MOVE(other.finish)),
    map(STL_MOVE(other.map)),map_size(STL_MOVE(other.map_size)) {
        other.map=nullptr;
        other.map_size=0;
    }

    template<typename T,typename Alloc>
    deque<T,Alloc>& deque<T,Alloc>::operator=(deque const& other){
        using TinySTL::swap;
        deque tmp=other;
        swap(*this,tmp);
        return *this;
    }

    template<typename T,typename Alloc>
    deque<T,Alloc>& deque<T,Alloc>::operator=(deque&& other)noexcept{
        using TinySTL::swap;
        this->~deque();
        swap(*this,other);
        other.map=nullptr;
        other.map_size=0;
        return *this;
    }

    template<typename T,typename Alloc>
    deque<T,Alloc>::~deque(){
        if(map != nullptr){
            clear();
            data_allocator::deallocate(*start.node,buf_size);
            map_allocator::deallocate(map,map_size);
            start.cur=finish.cur=nullptr;
            map=nullptr;
        }
    }

//**********************assign*************************************//
    template<typename T,typename Alloc>
    template<typename Input>
    void deque<T,Alloc>::assign(Input first,Input last){
        erase(begin(),end());
        insert(end(),first,last);
    }

    template<typename T,typename Alloc>
    void deque<T,Alloc>::assign(size_type n,value_type const& val){
        erase(begin(),end());
        insert(end(),n,val);
    }

    template<typename T,typename Alloc>
    void deque<T,Alloc>::assign(STD_ initializer_list<value_type> il){
        assign(il.begin(),il.end());
    }

//*************************capacity**********************************//
    template<typename T,typename Alloc>
    void deque<T,Alloc>::resize(size_type sz){
        resize(sz,value_type{});
    }

    template<typename T,typename Alloc>
    void deque<T,Alloc>::resize(size_type sz,value_type const& val){
        if(sz < size()){
            erase(begin()+sz,end());
        }
        else if(sz >size()){
            insert(end(),sz-size(),val);
        }
        else 
            ;
    }

//缩小容量以适应其size
    template<typename T,typename Alloc>
    void deque<T,Alloc>::shrink_to_fit(){
        for(auto cur=map;cur!=start.node;++cur){
            data_allocator(*cur,buf_size);
            *cur=nullptr;
        }
        for(auto cur=finish.node+1;cur!=map+map_size;--cur){
            data_allocator::allocator(*cur,buf_size);
            *cur=nullptr;
        }
    }

//*************************emplace-**********************************//
    template<typename T,typename Alloc>
    template<typename ...Args>
    typename deque<T,Alloc>::reference deque<T,Alloc>::emplace_front(Args&&... args){
        if(start.cur == start.first){
            STL_TRY{
                reserve_map_front();
                *(start.node-1)=data_allocator::allocate(buf_size);
                start.set_node(start.node-1);
                start.cur=start.last;
            }catch(...){
                Alloc::deallocate(*(start.node-1),buf_size);
            }
        }
        Alloc::construct(--start.cur,STL_ forward<Args>(args)...);
        return front();
    }

    template<typename T,typename Alloc>
    template<typename ...Args>
    typename deque<T,Alloc>::reference deque<T,Alloc>::emplace_back(Args&&... args){
        if(finish.cur+1 != finish.last){
            Alloc::construct(finish.cur++,STL_ forward<Args>(args)...);
        }else{
            reserve_map_back();
            STL_TRY{
                *(finish.node+1)=data_allocator::allocate(buf_size);
                Alloc::construct(finish.cur,STL_ forward<Args>(args)...);
                finish.set_node(finish.node+1);
                finish.cur=finish.first;
            }catch(...){
                Alloc::deallocate(*(finish.node+1));
            }
        }
        return back();
    }

    template<typename T,typename Alloc>
    template<typename ...Args>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::emplace(const_iterator pos,Args&&... args){
        const size_type Off=pos-begin();

        if(Off < size()/2){
            emplace_front(STL_ forward<Args>(args)...);
            STL_ rotate(begin(),Next_Iter(begin()),begin()+static_cast<difference_type>(Off+1));
        }
        else{
            emplace_back(STL_ forward<Args>(args)...);
            STL_ rotate(begin()+static_cast<difference_type>(Off),Prev_Iter(end()),end());
        }
        return begin()+static_cast<difference_type>(Off);
    }

//*******************************push-*********************************//
    template<typename T, typename Alloc>
    void deque<T, Alloc>::push_back(value_type const &val) {
        if (finish.cur+1 != finish.last) {
            Alloc::construct(finish.cur++, val);
        } else {
            push_back_aux(val);
        }
    }

    template<typename T,typename Alloc>
    void deque<T,Alloc>::push_back(value_type && val){
        emplace_back(STL_MOVE(val));
    }

    template<typename T,typename Alloc>
    void deque<T,Alloc>::push_front(value_type const& val){
        if(start.cur != start.first){
            auto tmp=start.cur;
            Alloc::construct(--tmp,val);
            --start.cur;
        }else{
            push_front_aux(val);
        }
    }

    template<typename T,typename Alloc>
    void deque<T,Alloc>::push_front(value_type && val){
        emplace_front(STL_MOVE(val));
    }

    template<typename T, typename Alloc>
    void deque<T, Alloc>::push_back_aux(value_type const &val) {
        reserve_map_back();
        STL_TRY {
            *(finish.node + 1) = data_allocator::allocate(buf_size);
            Alloc::construct(finish.cur, val);
            finish.set_node(finish.node+1);
            finish.cur=finish.first;
        } catch (...) {
            data_allocator::deallocate(*(finish.node + 1));
            throw;
        }
    }

    template<typename T,typename Alloc>
    void deque<T,Alloc>::push_front_aux(value_type const& val){
        reserve_map_front();
        STL_TRY{
            *(start.node-1)=data_allocator::allocate(buf_size);
            start.set_node(start.node-1);
            start.cur=start.last-1;
            Alloc::construct(start.cur,val);
        }catch(...){
            start.set_node(start.node+1);
            start.cur=start.first;
            Alloc::deallocate(*(start.node-1));
            throw;
        }
    }

    
//************************************insert**********************************//
    template<typename T,typename Alloc>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::insert(const_iterator pos,value_type const& val){
        if(pos == start) {
            push_front(val);
            return start;
        }
        else if(pos == finish) {
            push_back(val);
            return finish-1;
            //不用finish-1是考虑-操作代价相比拷贝--更大
        }
        else{
            return insert_aux(pos,val);
        }
    }

    template<typename T,typename Alloc>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::insert(const_iterator pos,value_type && val){
        return emplace(pos,STL_MOVE(val));
    }

    template<typename T,typename Alloc>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::insert(const_iterator pos,size_type n,value_type const& val){
        const size_type need_node=n/buf_size+1;
    
        if(pos == start){
            reserve_map_front(need_node);
            start=start-n;
            STL_ uninitialized_fill_n(start,n,val);
            return start;
        }
        else if(pos == finish){
            reserve_map_back(need_node);
            auto old_finish=finish;
            STL_ uninitialized_fill_n(finish,n,val);
            finish=finish+n;
            return old_finish;
        }
        else{
            return fill_insert(pos,n,val);
        }
    }


    template<typename T,typename Alloc>
    template<typename Input>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::insert(const_iterator pos,Input first,Input last){
        const size_type Off=static_cast<size_type>(pos-start);
        auto Old_size=size();

        if(Off < size()/2){
            STL_TRY{
                for(;first != last;++first)
                    emplace_front(*first);
            }catch(...){
                while(Old_size < size())
                    pop_front();
            }

            size_type Num=size()-Old_size;
            STL_ reverse(start,start+static_cast<difference_type>(Num));
            STL_ rotate(start,start+static_cast<difference_type>(Num),
                    start+static_cast<difference_type>(Num+Off));
        }else{
            STL_TRY{
                for(;first != last;++first)
                    emplace_back(*first);
            }catch(...){
                while(Old_size < size())
                    pop_back();
            }

            STL_ rotate(start,start+static_cast<difference_type>(Off),
                    finish);
        }
        return start+static_cast<difference_type>(Off);
    }

    template<typename T,typename Alloc>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::insert(const_iterator pos,STD_ initializer_list<value_type> il){
        return insert(pos,il.begin(),il.end());
    }


    template<typename T,typename Alloc>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::insert_aux(const_iterator pos,value_type const& val){
        //deque是双向的，因此考虑pos与中间位置的相对位置(比vector更灵活)
        const size_type index=pos-start;
        if(index < size()/2){
            push_front(front());
            auto new_front_front=start;
            ++new_front_front;
            auto old_front_front=new_front_front;
            ++old_front_front;
            pos=start+index;
            auto old_pos=pos;
            ++old_pos;
            copy(old_front_front,old_pos,new_front_front);
        }else{
            push_back(back());
            auto new_back=finish;
            --new_back;
            auto old_back=new_back;
            --old_back;
            pos=start+index;
            copy_backward(pos,old_back,new_back);
        }
        *pos=val;
        return pos;
    }

    template<typename T,typename Alloc>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::fill_insert(const_iterator pos,size_type n,value_type const& val){
        const size_type need_node=n/buf_size+1;
        const size_type elems_before=pos-start;
        const size_type len=size();

        if(elems_before < len/2){       //前面的相对更少，移动前面的
            reserve_map_front(need_node);
            auto new_start=start-n;
            pos=start+elems_before; //以防迭代器失效
            //需要比较uninitialized区域与插入点前元素个数
            //因为针对uninitialized区域采用uninitialized-函数
            if(elems_before > n){
                auto start_n=start+n;
                STL_ uninitialized_copy(start,start_n,new_start);
                STL_ copy(start_n,pos,start);
                STL_ fill_n(pos-n,pos,n);
                start=new_start;
            }else{
                STL_ uninitialized_fill_n(STL_ uninitialized_copy(start,pos,new_start),
                                            start,val);
                STL_ fill(start,pos,val);
                start=new_start;
            }
        }
        else{
            reserve_map_back(need_node);
            const size_type elems_after=len-elems_before;
            auto new_finish=finish+n;
            pos=finish-elems_after;
            if(elems_after > n){
                auto finish_n=finish-n;
                STL_ uninitialized_copy(finish_n,finish,finish);
                STL_ copy_backward(pos,finish_n,finish);
                STL_ fill_n(pos,n,val);
                finish=new_finish;
            }else{
                auto finish_after=new_finish-elems_after;
                STL_ uninitialized_copy(pos,finish,finish_after);
                STL_ uninitialized_fill(finish,finish_after,val);
                STL_ fill(pos,finish,val);
                finish=new_finish;
            }
        }
    }

//*************************pop-函数*********************************//
    template<typename T,typename Alloc>
    void deque<T,Alloc>::pop_front(){
        if(start.cur !=start.last-1){
            Alloc::destroy(start.cur);
            ++start.cur;
        }else{
            destroy(start.cur);
            data_allocator::deallocate(*start.node,buf_size);
            start.set_node(start.node+1);
            start.cur=start.first;
        }
    }

    template<typename T,typename Alloc>
    void deque<T,Alloc>::pop_back(){
        if(finish.cur != finish.first){
            --finish.cur;
            Alloc::destroy(finish.cur);
        }else{
            data_allocator::deallocate(*finish.node,buf_size);
            finish.set_node(finish.node-1);
            finish.cur=finish.last-1;
            Alloc::destroy(finish.cur);
        }
    }

//**********************erase*****************************************//
    template<typename T,typename Alloc>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::erase(const_iterator pos){
        return erase(pos,Next_Iter(pos));
    }

    template<typename T,typename Alloc>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::erase(const_iterator first,const_iterator last){
        iterator _first=make_iter(first);
        iterator _last=make_iter(last);
        const size_type _Off=_first-start;
        size_type _Count=_last-_first;

        if(_Off < static_cast<size_type>(finish-_last)){    //更接近front    
            STL_ move_backward(start,_first,_last);
            for(;_Count!=0;--_Count)
                pop_front();
        }else{
            STL_ move(_last,finish,_first);
            for(;_Count!=0;--_Count)
                pop_back();
        }

        return start+_Off;
    }

    template<typename T,typename Alloc>
    void deque<T,Alloc>::clear(){
        while(!empty())
            pop_back();
        start=finish;
    }

//给map分配空间
    template<typename T, typename Alloc>
    void deque<T, Alloc>::create_map(size_type mapSize) {
        STL_TRY {
            map = map_allocator::allocate(mapSize);
            for (int i = 0; i < mapSize; ++i) {
                *(map + i) = nullptr;
            }

        } catch (...) {
            map = nullptr;
            map_size = 0;
            throw;
        }
    }

//在map上创建缓冲区
    template<typename T, typename Alloc>
    void deque<T, Alloc>::create_buffer(map_pointer _start, map_pointer _finish) {
        map_pointer cur;
        //_starts映射第一个buffer,_finih映射最后一个buffer
        //或理解为第一个node和最后一个node
        STL_TRY {
            for (cur = _start; cur <= _finish; ++cur)
                *cur = data_allocator::allocate(buf_size);
        } catch (...) {    //commit or rollback
            while (cur != _start) {
                --cur;
                data_allocator::deallocate(*cur, buf_size);
                *cur = nullptr;
            }
            throw;
        }
    }

    template<typename T, typename Alloc>
    void deque<T, Alloc>::map_init(size_type numElem) {
        //node保证至少一个
        size_type numNode = numElem / buf_size + 1;
        //+2是为了保证首尾至少有一个缓冲区以便扩展
        map_size = STL_ max(static_cast<size_type>(DEQUEUE_MAP_INIT_SIZE), numNode + 2);

        create_map(map_size);
        //node在map中的布局应该居中，因此_start=floor(mapsize/2-numNode/2)
        map_pointer _start = map + (map_size - numNode) / 2;
        //就如同数组索引一样，表示偏移量应数量-1
        map_pointer _finish = _start + numNode - 1;

        create_buffer(_start, _finish);
        start.set_node(_start);
        start.cur = start.first;
        finish.set_node(_finish);
        //finish.cur表示的是尾后
        finish.cur = finish.first + numElem % buf_size;
    }

    template<typename T, typename Alloc>
    void deque<T, Alloc>::fill_init(size_type n, value_type const &val) {
        map_init(n);
        //在map中移动获取buf的位置
        for (auto cur = start.node; cur != finish.node; ++cur) {
            STL_ uninitialized_fill(*cur, *cur + buf_size, val);
        }
        STL_ uninitialized_fill(finish.first, finish.cur, val);
    }

    template<typename T,typename Alloc>
    template<typename Input>
    void deque<T,Alloc>::range_init(Input first,Input last){
        size_type num=STL_ distance(first,last);
        map_init(0);
        for(;first != last;++first)
            emplace_back(*first);
    }

//map的再分配函数
    template<typename T, typename Alloc>
    void deque<T, Alloc>::reserve_map_back(size_type add_node) {
        //finish后面没有节点了
        if (add_node > map_size - (finish.node - map + 1)) {
            reallocate_map(add_node, false);
        }
    }

    template<typename T, typename Alloc>
    void deque<T, Alloc>::reserve_map_front(size_type add_node) {
        //start前面没有节点了
        if (add_node > start.node - map) {
            reallocate_map(add_node, true);
        }
    }

//reallocate_map只是重构造map，并不涉及buffer的再分配
    template<typename T, typename Alloc>
    void deque<T, Alloc>::reallocate_map(size_type add_node, bool isFront) {
        const size_type old_node = finish.node - start.node + 1;
        const size_type new_node = old_node + add_node;
        map_pointer _start;

        if (map_size > 2 * new_node) {
            //当一边添加元素时，会导致空间占满，而另一边有多余空间，
            //因此若加上add_node，不超过map的一半则考虑借另一边的空间，
            //则只需要调整在map中的布局即可
            _start = map + (map_size - new_node) / 2 + (isFront ? add_node : 0);
            if (_start < start.node) {
                STL_ copy(start.node, finish.node+1, _start);
            } else {
                STL_ copy_backward(start.node, finish.node+1, _start + old_node);
            }
        } else {   //如果new_node把map的一半都占满了，那么就需要考虑分配更多的node了
            const size_type new_map_size = map_size + (map_size > add_node ? map_size : add_node) + 2;
            map_pointer new_map = map_allocator::allocate(new_map_size);

            _start = new_map + (new_map_size - new_node) / 2 + (isFront ? add_node : 0);

            STL_ copy(start.node, finish.node+1, _start);
            map_allocator::deallocate(map, map_size);

            map = new_map;
            map_size = new_map_size;
        }
        start.set_node(_start);
        finish.set_node(_start + old_node - 1);
    }


//iterator操作函数
    template<typename T,typename Alloc>
    typename deque<T,Alloc>::iterator deque<T,Alloc>::make_iter(const_iterator iter){
        return iterator(iter);
    }

    template<typename T,typename Alloc>
    void deque<T,Alloc>::swap(deque& other)noexcept{
        STL_SWAP(start,other.start);
        STL_SWAP(finish,other.finish);
        STL_SWAP(map,other.map);
        STL_SWAP(map_size,other.map_size);
    }
}

#endif //TINYSTL_DEQUE_H


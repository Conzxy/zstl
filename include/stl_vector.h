#ifndef TINYSTL_STL_VECTOR_H
#define TINYSTL_STL_VECTOR_H

#include "allocator.h"
#include <initializer_list>
#include "type_traits.h"
#include "stl_uninitialized.h"
#include "stl_iterator.h"
#include "stl_algorithm.h"
#include "config.h"
#include <climits>

namespace TinySTL{
    //使用vector_base作为底层结构，利用RAII特性来避免memory leak
    //资源管理类
    template<typename T,typename Allocator>
    struct vector_base{
    public:
        vector_base()=default;
        explicit vector_base(typename Allocator::size_type n)
            :first_{Allocator::allocate(n)},last_{first_+n},capa_{first_+n}{}
        vector_base(T const* first,T const* last)
            :vector_base(last-first){}

        ~vector_base(){ Allocator::deallocate(first_,capa_-last_); }

        //从ownership角度看，copy是个麻烦
        vector_base(vector_base const&)=delete;
        vector_base& operator=(vector_base const&)=delete;
        
        //提供对应的move函数，并且标注noexcept
        vector_base(vector_base&& base) noexcept
            :first_{base.first_},last_{base.last_},capa_{base.capa_} 
        { base.first_=base.last_=base.capa_=nullptr; }
        vector_base& operator=(vector_base&& base) noexcept
        { this->swap(base); return *this; }

        //interface
        void swap(vector_base &rhs) noexcept
        {
            STL_SWAP(first_,rhs.first_);
            STL_SWAP(last_,rhs.last_);
            STL_SWAP(capa_,rhs.capa_);
        }

    public:
        T* first_;
        T* last_;
        T* capa_; 
    };

    template<typename T,typename A>
    void swap(vector_base<T,A>& lhs,vector_base<T,A>& rhs) noexcept(noexcept(lhs.swap(rhs)))
    { lhs.swap(rhs); }


    //vector
    template<typename T,typename Allocator=allocator<T>>
    class vector{
    public:
        using value_type            =T;
        using pointer               =typename allocator_traits<Allocator>::pointer;
        using const_pointer         =typename allocator_traits<Allocator>::const_pointer;
        using reference             =value_type&;
        using const_reference       =value_type const&;
        using iterator              =T*;
        using const_iterator        =T const*;
        using reverse_iterator      =TinySTL::reverse_iterator<iterator>;
        using const_reverse_iterator=TinySTL::reverse_iterator<const_iterator>;
        using allocator_type        =Allocator;
        using size_type             =std::size_t;
        using difference_type       =ptrdiff_t;
        using base                  =vector_base<T,Allocator>;

    protected:
        base vb;
    public:
        //construct/copy/destroy:
        vector()=default;
        vector(const size_type n,value_type const& val)
            :vb(n)
        {
            uninitialized_fill_n(begin(),n,val);
        }

        explicit vector(size_type n)
            :vector(n,value_type{}){}

        template<class InputIterator,Enable_if_t<is_input_iterator<InputIterator>,int> =0>
        vector(InputIterator first,InputIterator last)
            :vector(distance(first,last))
        {
            uninitialized_copy(first,last,begin());
        }

        vector(vector const& rhs)
            :vector(rhs.begin(),rhs.end()){}

        vector(vector&& rhs)
            :vb(STL_MOVE(rhs.vb)){}

        vector(std::initializer_list<T> il)
            :vector(il.begin(),il.end()){}
        
        ~vector(){ destroy(begin(),end()); }
        vector& operator=(vector const&);
        vector& operator=(vector&& rhs) noexcept
        { this->swap(rhs); return *this; }
        vector& operator=(std::initializer_list<T>);

        template<class InputIterator,typename =Enable_if_t<
                is_input_iterator<InputIterator>>>
        void assign(InputIterator first,InputIterator last);
        void assign(size_type n,T const& t);

        allocator_type get_allocator()const noexcept
        { return Allocator(); }

        //iterators:
        iterator                begin()                     noexcept
        { return iterator(vb.first_); }
        iterator                end()                       noexcept
        { return iterator(vb.last_); }
        const_iterator          begin()             const   noexcept
        { return const_iterator(vb.first_); }
        const_iterator          end()               const   noexcept
        { return const_iterator(vb.last_); }  
        reverse_iterator        rbegin()                    noexcept
        { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin()            const   noexcept
        { return const_reverse_iterator(end());}
        reverse_iterator        rend()                      noexcept
        { return reverse_iterator(begin()); }
        const_reverse_iterator  rend()              const   noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator          cbegin()            const   noexcept
        { return begin(); }
        const_iterator          cend()              const   noexcept
        { return end(); }
        const_reverse_iterator  crbegin()           const   noexcept
        { return rbegin(); }
        const_reverse_iterator  crend()             const   noexcept
        { return rend(); }
        //reverse

        //capacity:
        size_type   size()               const   noexcept
        { return end()-begin();}
        size_type   max_size()           const   noexcept
        { return size_type(UINT_MAX/sizeof(T)); }
        void        resize(size_type sz);
        void        resize(size_type sz,T const& c);
        size_type   capacity()              const   noexcept
        { return vb.capa_-vb.first_;}
        bool        empty()                 const   noexcept
        {return begin()==end();}
        void        reserve(size_type n);
        void        shrink_to_fit();

        //element access:
        reference           operator[](size_type n) noexcept
        { return *(begin()+n); }
        const_reference     operator[](size_type n)const noexcept
        { return *(cbegin()+n); }
        reference           at(size_type n) noexcept;
        const_reference     at(size_type n)const noexcept;
        reference           front() noexcept
        { return *begin(); }
        const_reference     front() const noexcept
        { return *begin(); }
        reference           back() noexcept
        { return *(end()-1); }
        const_reference     back()const noexcept
        { return *(end()-1); }

        //data access:
        T*          data()  noexcept
        { return vb.first_; }
        T const*    data()  const noexcept
        { return vb.first_; }

        //modifiers:
        template<typename...Args>
        void emplace_back(Args&&... args);
        void push_back(T const& x);
        void push_back(T&& x){ emplace_back(TinySTL::move(x)); }
        void pop_back();

        template<typename... Args>
        iterator emplace(const_iterator position,Args&&... args);
        iterator insert(const_iterator position,T const& x);
        iterator insert(const_iterator position,size_type n,T const& x);
        template<class InputIterator,class =Enable_if_t<
                                                is_input_iterator<InputIterator>>>
        iterator insert(const_iterator position,
                        InputIterator first,InputIterator last);
        iterator insert(const_iterator position,std::initializer_list<T> il);

        iterator erase(const_iterator position);
        iterator erase(const_iterator first,const_iterator last);

        void swap(vector& rhs)noexcept
        { using TinySTL::swap; swap(this->vb,rhs.vb); }
        void clear()noexcept
        { erase(begin(),end()); }

        //helper function
    private:
        size_type getNewCapacity(size_type len)const;
        void insert_aux(iterator position,T const& x);
        void insert_aux(iterator position,size_type n,T const& x);
        template<typename II>
        void insert_aux(iterator position,II first,II last);
        iterator erase_aux(iterator first,iterator last);
        iterator erase_aux(iterator position);
    };

    template<typename T,class Allocator>
    inline bool operator==(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
    { return x.size()==x.size()&&TinySTL::equal(x.begin(),x.end(),y.begin()); }
    template<typename T,class Allocator>
    inline bool operator!=(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
    { return !(x==y); }
    template<typename T,class Allocator>
    inline bool operator <(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
    { return TinySTL::lexicographical_compare(x.begin(), x.end(),
                                              y.begin(), y.end()); }
    template<typename T,class Allocator>
    inline bool operator>=(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
    { return !(x<y); };
    template<typename T,class Allocator>
    inline bool operator >(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
    { return y<x;}
    template<typename T,class Allocator>
    inline bool operator<=(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
    { return !(y<x); }

    //specialized algorithm:
    template<typename T,class Allocator>
    inline void swap(vector<T,Allocator>& x,vector<T,Allocator>& y) noexcept(noexcept(x.swap(y)))
    { x.swap(y);}
}
#endif //TINYSTL_STL_VECTOR_H

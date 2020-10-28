#ifndef TINYSTL_STL_VECTOR_H
#define TINYSTL_STL_VECTOR_H

#include "Allocator.h"
#include <initializer_list>
#include "type_traits"
#include "stl_uninitialized.h"
#include "stl_iterator.h"
#include "stl_algorithm.h"
#include <climits>

namespace TinySTL{
    template<typename T,class Allocator=allocator<T>>
    class vector{
    public:
        //types:
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
        using differnece_type       =ptrdiff_t;
    protected:
        iterator start;
        iterator finish;
        iterator end_of_storage;
    public:
        //construct/copy/destroy:
        explicit vector(Allocator const& =Allocator()):start(nullptr),finish(nullptr),end_of_storage(nullptr){}
        explicit vector(const size_type n){
            fill_initialize(n,T{});
        }

        vector(size_type n,T const& value,Allocator const& =Allocator()){
            fill_initialize(n,value);
        }

        template<class InputIterator>
        vector(InputIterator first,InputIterator last,
               Allocator const& =Allocator()){
            range_initialize(first, last);
        }

        vector(vector const& rhs){
            range_initialize(rhs.first, rhs.last);
        }

        vector(vector&& rhs):start{rhs.start},finish{rhs.finish},
                             end_of_storage{rhs.end_of_storage}{
            rhs.start=rhs.finish=rhs.end_of_storage=nullptr;
        }
        //vector(vector const&,Allocator const&);
        vector(std::initializer_list<T>);
        ~vector();
        vector& operator=(vector const&);
        vector& operator=(vector&&) noexcept;
        vector& operator=(std::initializer_list<T>);

        template<class InputIterator,typename =
                TinySTL::Enable_if_t<TinySTL::is_input_iterator<InputIterator>::value>>
        void assign(InputIterator first,InputIterator last);
        void assign(size_type n,T const& t);

        allocator_type get_allocator()const noexcept
        { return Allocator(); }

        //iterators:
        iterator                begin()                     noexcept
        { return iterator(start); }
        const_iterator          begin()             const   noexcept
        { return const_iterator(start); }
        iterator                end()                       noexcept
        { return iterator(finish); }
        const_iterator          end()               const   noexcept
        { return const_iterator(finish); }
            reverse_iterator    rbegin()                    noexcept
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
        { return finish-start;}
        size_type   max_size()           const   noexcept
        { return size_type(UINT_MAX/sizeof(T)); }
        void        resize(size_type sz);
        void        resize(size_type sz,T const& c);
        size_type   capacity()              const   noexcept
        { return end_of_storage-start;}
        bool        empty()                 const   noexcept
        {return begin()==end();}
        void        reserve(size_type n);
        void        shrink_to_fit();

        //element access:
        reference           operator[](size_type n)
        { return *(begin()+n); }
        const_reference     operator[](size_type n)const
        { return *(cbegin()+n); }
        reference           at(size_type n);
        const_reference     at(size_type n)const;
        reference           front()
        { return *begin(); }
        const_reference     front()const
        { return *begin(); }
        reference           back()
        { return *(end()-1); }
        const_reference     back()const
        { return *(end()-1); }

        //data access:
        T*          data()noexcept
        { return start; }
        const T*    data()const noexcept
        { return start;}

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
        template<class InputIterator>
        iterator insert(const_iterator position,
                        InputIterator first,InputIterator last);
        iterator insert(const_iterator position,std::initializer_list<T> il);

        iterator erase(const_iterator position);
        iterator erase(const_iterator first,const_iterator last);

        void swap(vector& rhs)noexcept;
        void clear()noexcept
        { erase(begin(),end()); }

        //helper function
    private:
        void fill_initialize(size_type n,T const& value);

        template<class InputIterator>
        void range_initialize(InputIterator first, InputIterator last);

        template<typename InputIterator>
        void reallocate_and_copy(iterator position,InputIterator first,InputIterator last);
        void reallocate_and_fillN(iterator position,size_type n,T const& x);
        template<typename...Args>
        void reallocate_and_emplace(iterator position,Args&&...args);
        size_type getNewCapacity(size_type len)const;
    protected:
        iterator allocate_and_fill(size_type n,T const& x);

        template<class InputIterator>
        iterator allocate_and_copy(InputIterator first,InputIterator last);

        void destroy_and_deallocate(iterator first,iterator last);

        void insert_aux(iterator position,T const& x);
        void insert_aux(iterator position,size_type n,T const& x);
        template<typename II>
        void insert_aux(iterator position,II first,II last);
        iterator erase_aux(iterator first,iterator last);
        iterator erase_aux(iterator position);
    };

    template<typename T,class Allocator>
    inline bool operator==(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
    { return x.size()==x.size()&&TinySTL::equal(x.begin(),y.end(),y.begin()); }
    template<typename T,class Allocator>
    inline bool operator!=(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
    { return !(x==y); }
    template<typename T,class Allocator>
    inline bool operator <(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
    { return TinySTL::lexicographical(x.begin(),x.end(),
                                      y.begin(),y.end()); }
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
    inline void swap(vector<T,Allocator>& x,vector<T,Allocator>& y){ x.swap(y);}
}
#endif //TINYSTL_STL_VECTOR_H

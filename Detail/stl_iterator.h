#ifndef TINYSTL_STL_ITERATOR_H
#define TINYSTL_STL_ITERATOR_H

//该文件提供迭代器,traits,operation

#include <cstddef>
#include <type_traits.h>

namespace TinySTL{
    //以下提供五种迭代器的对应tag
    struct Input_iterator_tag{};
    struct Output_iterator_tag{};
    struct Forward_iterator_tag:Input_iterator_tag{};
    struct Bidirectional_iterator_tag:Forward_iterator_tag{};
    struct Random_access_iterator_tag: Bidirectional_iterator_tag{};

    //iterator 一般模板
    template<class Category,class T,class Distance=ptrdiff_t,
            class Pointer=T*,class Reference=T&>
    struct iterator{
        using iterator_category =Category;
        using value_type        =T;
        using difference_type   =Distance;
        using pointer           =Pointer;
        using reference         =Reference;
    };

    //SFINAE-friendly
    //ensure iterator_traits<iterator> must be instantiated such that no hard error
    template<typename iterator,typename =Void_t<>>
    struct iterator_traits_helper{
    };

    template<typename iterator>
    struct iterator_traits_helper<iterator,Void_t<
            typename iterator::iterator_category,
            typename iterator::value_type,
            typename iterator::difference_type,
            typename iterator::pointer,
            typename iterator::reference>>{
        using iterator_category        =typename iterator::iterator_category;
        using value_type               =typename iterator::value_type;
        using difference_type          =typename iterator::difference_type;
        using pointer                  =typename iterator::pointer;
        using reference                =typename iterator::reference;
    };

    template<typename iterator>
    struct iterator_traits:iterator_traits_helper<iterator>{};


    //对内置指针的特化
    template<typename T>
    struct iterator_traits<T*>{
        typedef Random_access_iterator_tag iterator_category;
        typedef T                          value_type;
        typedef ptrdiff_t                  difference_type;
        typedef T*                         pointer;
        typedef T&                         reference;
    };

    template<typename T>
    struct iterator_traits<T const*>{
        typedef Random_access_iterator_tag iterator_category;
        typedef T const                    value_type;
        typedef ptrdiff_t                  difference_type;
        typedef T const*                   pointer;
        typedef T const&                   reference;
    };

    //SFINAE-friendly
    //ensure iterator_traits<iterator>::iterator_category must be instantiated such that no hard error
    template<typename iter,typename= Void_t<>>
    struct has_iterator_helper:_false_type {
    };

    template<typename iter>
    struct has_iterator_helper<iter,Void_t<
            typename iterator_traits<iter>::iterator_category>>:_true_type {
    };

    template<typename iter>
    struct has_iterator:has_iterator_helper<iter>::type{};

    template<typename iter>
    constexpr bool has_iterator_v=has_iterator<iter>::value;

    /*template<typename iter,typename U,bool =has_iterator_v<iter>>
    struct has_iterator_of:Bool_constant<
            Is_convertible_v<typename iterator_traits<iter>::iterator_category,
                             U>>{};

    template<typename iter,typename U>
    struct has_iterator_of<iter,U,false>:_false_type{};*/

    template<typename iter,typename U,typename =Void_t<>>
    struct has_iterator_of:_false_type{};

    template<typename iter,typename U>
    struct has_iterator_of<iter,U,Void_t<Is_convertible<
            typename iterator_traits<iter>::iterator_category,U>>>:_true_type{};

    //iterator predicate:
    template<typename iter>
    constexpr bool is_input_iterator
    =has_iterator_of<iter,Input_iterator_tag>::value;

    template<typename iter>
    constexpr bool is_output_iterator
    =has_iterator_of<iter,Output_iterator_tag>::value;

    template<typename iter>
    constexpr bool is_forward_iterator
    =has_iterator_of<iter,Forward_iterator_tag>::value;

    template<typename iter>
    constexpr bool is_bidirectional_iterator
    =has_iterator_of<iter,Bidirectional_iterator_tag>::value;

    template<typename iter>
    constexpr bool is_random_access_iterator
    =has_iterator_of<iter,Random_access_iterator_tag>::value;

    template<typename iter>
    constexpr bool is_iterator
    =is_input_iterator<iter> || is_output_iterator<iter>;

    //以下函数用于方便提取迭代器的category,difference_type,value_type
    template<class Iterator>
    constexpr decltype(auto)
    iterator_category(Iterator const&){
        using category=typename iterator_traits<Iterator>::iterator_category;
        return category();
    }

    template<class Iterator>
    constexpr decltype(auto)
    distance_type(Iterator const&){
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(nullptr);
    }

    template<class Iterator>
    constexpr decltype(auto)
    value_type(Iterator const&){
        return static_cast<typename iterator_traits<Iterator>::value_type*>(nullptr);
    }

    //以下函数提供迭代器之间的距离计算
    template<class InputIterator>
    inline decltype(auto)
    _distance(InputIterator first,InputIterator last,Input_iterator_tag){
        typename iterator_traits<InputIterator>::difference_type n=0;
        for(;first!=last;++first){
            ++n;
        }
        return n;
    }

    template<class RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type
    _distance(RandomAccessIterator first,RandomAccessIterator last,Random_access_iterator_tag){
        return last-first;
    }

    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first,InputIterator last){
        return _distance(first,last,iterator_category(first));
    }

    //以下函数用于迭代器的前进
    template<class Input_iterator,typename Distance>
    inline void
    _advance(Input_iterator &i,Distance n,Input_iterator_tag){
        while(n--){
            ++i;
        }
    }

    template<class Bidirectional_iterator,typename Distance>
    inline void
    _advance(Bidirectional_iterator &i,Distance n,Bidirectional_iterator_tag){
        if(n>=0)
            while(n--)
                ++i;
        else
            while(n++)
                --i;
    }

    template<class Random_access_iterator,typename Distance>
    inline void
    _advance(Random_access_iterator& i,Distance n,Random_access_iterator_tag){
            i+=n;
    }

    template<class Input_iterator,typename Distance>
    inline void
    advance(Input_iterator &i,Distance n){
        _advance(i,n,iterator_category(i));
    }

    //Reverse Iterator
    //@require:
    //1.Iterator需满足Bidirectional的所有要求
    //2.对于operator+，operator-，operator+=，operator-=，
    //operator[]，operator<,operator>,operator<=,operator>=,
    //operator-,operator+(global)，欲使用需满足Random Access Iterator的要求
    template<class Iterator>
    class reverse_iterator:public
        iterator<typename iterator_traits<Iterator>::iterator_category,
        typename iterator_traits<Iterator>::value_type,
        typename iterator_traits<Iterator>::difference_type,
        typename iterator_traits<Iterator>::pointer,
        typename iterator_traits<Iterator>::reference>
    {
    public:
        using iterator_type         =Iterator;
        using difference_type       =typename iterator_traits<Iterator>::difference_type;
        using reference             =typename iterator_traits<Iterator>::reference;
        using pointer               =typename iterator_traits<Iterator>::pointer;

    public:
        reverse_iterator():current(){};
        explicit reverse_iterator(Iterator x):current(x){}
        template<class U>
                reverse_iterator(reverse_iterator<U> const& u)
                :current(u.current){}
        template<class U>
                reverse_iterator& operator=(reverse_iterator<U> const& u){
            current=u.base();
            return *this;
        };

        //Conversion:提取underlying Iterator
        Iterator base()const{return current;}

        reference operator*()const{
            //通过deref_tmp来避免dangling reference
            deref_tmp=current;
            --deref_tmp;    //反向迭代器的设计逻辑
            return *deref_tmp;
        }

        pointer operator->()const{ return &(operator*());}

        reverse_iterator& operator++(){
            --current;
            return *this;
        }

        reverse_iterator  operator++(int){
            reverse_iterator tmp=*this;
            --current;
            return tmp;
        }

        reverse_iterator& operator--(){
            ++current;
            return *this;
        }

        reverse_iterator  operator--(int){
            reverse_iterator tmp=*this;
            ++current;
            return tmp;
        }

        reverse_iterator operator+(difference_type n)const{ return reverse_iterator(current-n);}
        reverse_iterator& operator+=(difference_type n){
            current-=n;
            return *this;
        }

        reverse_iterator operator-(difference_type n)const{ return reverse_iterator(current+n);}
        reverse_iterator& operator-=(difference_type n){
            current+=n;
            return *this;
        }

        reference operator[](difference_type n)const{
            //return current[-n-1]; //这个依赖于underlying Iterator的[]实现
            //current[-n-1]=current[-(n+1)]，多减一个1是因为rbegin=end
            return *(*this+n);      //设计逻辑都是要“反向”
        }

    protected:
        Iterator current;
    private:
        mutable Iterator deref_tmp; //exposition only
        //dereference temporary
    };

    template<class Iterator1,class Iterator2>
    bool operator==(
            reverse_iterator<Iterator1> const& x,
            reverse_iterator<Iterator2> const& y){
        return x.base()==y.base();
    }

    template<class Iterator1,class Iterator2>
    bool operator!=(
            reverse_iterator<Iterator1> const& x,
            reverse_iterator<Iterator2> const& y){
        return !(x==y);
        //这里不是x.current!=y.current;
        //是为了以防没有重载!=的Iterator
    }

    template<class Iterator1,class Iterator2>
    bool operator<(
            reverse_iterator<Iterator1> const& x,
            reverse_iterator<Iterator2> const& y){
        return x.base()<y.base();
    }

    template<class Iterator1,class Iterator2>
    bool operator>(
            reverse_iterator<Iterator1> const& x,
            reverse_iterator<Iterator2> const& y){
        return x.base()>y.base();
    }

    template<class Iterator1,class Iterator2>
    bool operator<=(
            reverse_iterator<Iterator1> const& x,
            reverse_iterator<Iterator2> const& y){
        return !(x>y);
    }

    template<class Iterator1,class Iterator2>
    bool operator>=(
            reverse_iterator<Iterator1> const& x,
            reverse_iterator<Iterator2> const& y){
        return !(x<y);
    }

    template<class Iterator1,class Iterator2>
    auto operator-(
            reverse_iterator<Iterator1> const& x,
            reverse_iterator<Iterator2> const& y)/*->decltype(y.current-x.current)*/{
                return y.base()-x.base();     //C++14支持编译器推断auto
            }

    //与成员版的不同在于操作数的位置不同
    template<class Iterator>
    reverse_iterator<Iterator> operator+(
            typename reverse_iterator<Iterator>::difference_type n,
            reverse_iterator<Iterator>& x){
        return reverse_iterator<Iterator>(x.base()-n);
    }
}
#endif //TINYSTL_STL_ITERATOR_H

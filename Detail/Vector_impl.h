#ifndef TINYSTL_VECTOR_IMPL_H
#define TINYSTL_VECTOR_IMPL_H


#include "../stl_vector.h"
#include "../stl_exception.h"

namespace TinySTL{
    //dtor
    template<typename T,class Alloc>
    vector<T,Alloc>::~vector(){
            destroy_and_deallocate(begin(),end());
    }

    //ctor
    template<typename T,class Alloc>
    vector<T,Alloc>:: vector(std::initializer_list<T> il)
    :start(nullptr),finish(nullptr),end_of_storage(nullptr){
        insert(end(),il.begin(),il.end());
    }

    //assignment
    template<typename T,class Alloc>
    vector<T,Alloc>& vector<T,Alloc>::operator=(vector const& rhs){
        if(this!=&rhs){
            auto r_size=rhs.size();
            if(r_size>capacity()){
                vector tmp(rhs.begin(),rhs.end());
                swap(tmp);
            }
            else if(r_size<=size()){
                auto i=TinySTL::copy(rhs.begin(),rhs.end(),begin());
                destroy(i,finish);
                finish=start+r_size;
            }else{//size()<rhs.size()<=capacity()
                TinySTL::copy(rhs.begin(),rhs.begin()+size(),start);
                TinySTL::uninitialized_copy(rhs.begin()+size(),rhs.end(),finish);
                finish=start+r_size;
            }
        }
        return *this;
    }

    template<typename T,class Alloc>
    vector<T,Alloc>& vector<T,Alloc>::operator=(vector&& rhs) noexcept{
        swap(*this,rhs);
        rhs.start=rhs.finish=rhs.end_of_storage=nullptr;
    }

    template<typename T,class Alloc>
    vector<T,Alloc>& vector<T,Alloc>::operator=(std::initializer_list<T> il){
        auto tmp=vector(il.begin(),il.end());
        swap(tmp);
        return *this;
    }

    //接受[first,last)，将其中的元素赋给vector
    template<typename T,class Alloc>
    template<class InputIterator,typename>
    void vector<T,Alloc>::assign(InputIterator first,InputIterator last){
        erase(begin(),end());
        insert(begin(),first,last);
    }

    //接受n个元素赋值给vector
    template<typename T,class Alloc>
    void vector<T,Alloc>::assign(size_type n,T const& t){
        erase(begin(),end());
        insert(begin(),n,t);
    }

    //at:
    template<typename T,class Alloc>
    typename vector<T,Alloc>::reference vector<T,Alloc>::at(size_type n){
        TinySTL_TRY{
            THROW_RANGE_ERROR_IF(n>=size(),"The location is not exist!");
            return *(begin()+n);
        }
        catch(const std::range_error &e){
            std::cerr<<"RANGE_ERROR:"<<e.what()<<std::endl;
        }
    }

    template<typename T,class Alloc>
    typename vector<T,Alloc>::const_reference vector<T,Alloc>::at(size_type n)const{
        TinySTL_TRY{
            THROW_RANGE_ERROR_IF(n>=size(),"The location is not exist!");
            return *(cbegin()+n);
        }
        catch(const std::range_error &e){
            std::cerr<<"RANGE_ERROR:"<<e.what()<<std::endl;
        }
    }

    //capacity:
    //将元素数量改为sz（如果size（）增大，则多出的元素值初始化）
    template<typename T,class Alloc>
    void vector<T,Alloc>::resize(size_type sz){
        if(sz<=size())
            erase(begin()+sz,end());
        else
            resize(sz,T{});
    }

    template<typename T,class Alloc>
    void vector<T,Alloc>::resize(size_type sz,T const& c){
        if(sz<size())
            erase(begin()+sz,end());
        else if(sz>size())
            insert(end(),sz-size(),c);
        else
            ;
    }

    //如果容量不足，扩大
    template<typename T,class Alloc>
    void vector<T,Alloc>::reserve(size_type n){
        if(n<=capacity())
            return ;
        THROW_LENGTH_ERROR_IF(n>max_size(),
                              "The requirement is greater than maximum number of elements!");
        iterator new_start=Alloc::allocate(n);
        iterator new_finish=TinySTL::uninitialized_copy(begin(),end(),new_start);
        destroy_and_deallocate();

        start=new_start;
        finish=new_finish;
        end_of_storage=new_start+n;
    }

    //缩减容量以适应元素个数
    template<typename T,class Alloc>
    void vector<T,Alloc>::shrink_to_fit(){
        iterator new_start=Alloc::allocate(size());
        iterator new_finish=TinySTL::uninitialized_copy(begin(),end(),new_start);
        destroy_and_deallocate(begin(),end());

        start=new_start;
        finish=new_finish;
        end_of_storage=start+size();
    }

    //modifiers:
    template<typename T,class Alloc>
    template<typename...Args>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::emplace(const_iterator position,Args&&...args){
        iterator pos=const_cast<iterator>(position);
        if(finish<end_of_storage&&pos==end()){
            Alloc::construct(&*end(),TinySTL::forward<Args>(args)...);
            ++finish;
        }
        else if(finish<end_of_storage){
            Alloc::construct(&*end(),*(end()-1));
            ++finish;
            TinySTL::copy_backward(pos,finish-2,finish-1);
            *pos=value_type(TinySTL::forward<Args>(args)...);
        }
        else{
            reallocate_and_emplace(pos,TinySTL::forward<Args>(args)...);
        }
        return begin()+(pos-begin());
    }

    template<typename T,class Alloc>
    template<typename...Args>
    void vector<T,Alloc>::emplace_back(Args&&... args){
        if(finish<end_of_storage){
            Alloc::construct(&*end(),TinySTL::forward<Args>(args)...);
            ++finish;
        }
        else{
            reallocate_and_emplace(end(),TinySTL::forward<Args>(args)...);
        }
    }

    template<typename T,class Alloc>
    void vector<T,Alloc>::push_back(T const& x){
        if(finish!=end_of_storage){
            Alloc::construct(finish,x);
            ++finish;
        }
        else
            insert_aux(end(),x);
    }

    template<typename T,class Alloc>
    void vector<T,Alloc>::pop_back(){
        TINYSTL_DEBUG(!size());
        --finish;
        destroy(finish);
    }

    //insert:
    template<typename T,class Alloc>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::insert(const_iterator position,T const& x){
        const auto index=position-cbegin();
        insert_aux(begin()+(position-cbegin()),x);
        return begin()+index;
    }

    template<typename T,class Alloc>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::insert(const_iterator position,size_type n,T const& x){
        const auto index=position-cbegin();
        insert_aux(begin()+(position-cbegin()),n,x);
        return begin()+index;
    }

    template<typename T,class Alloc>
    template<class II>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::insert(const_iterator position,
                            II first,II last){
        const auto index=position-cbegin();
        insert_aux(begin()+(position-cbegin()),first,last);
        return begin()+index;
    }

    template<typename T,class Alloc>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::insert(const_iterator position,std::initializer_list<T> il){
        const auto index=position-cbegin();
        insert_aux(begin()+(position-cbegin()),il.begin(),il.end());
        return begin()+index;
    }

    //erase:
    template<typename T,class Alloc>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::erase(const_iterator first,const_iterator last){
        const auto beg=begin();
        const auto cbeg=cbegin();
        return erase_aux(beg+(first-cbeg),beg+(last-cbeg));
    }

    template<typename T,class Alloc>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::erase(const_iterator position){
        return erase_aux(begin()+(position-cbegin()));
    }


    //helper function
    //分配和创建对象：
    //fill：变量个数
    //copy：范围
    //销毁和回收
    template<typename T,class Alloc>
    void vector<T,Alloc>::fill_initialize(size_type n,T const& value){
        start=allocate_and_fill(n,value);
        end_of_storage=finish=start+n;
    }

    template<typename T,class Alloc>
    template<class InputIterator>
    void vector<T,Alloc>::range_initialize(InputIterator first, InputIterator last){
        start=allocate_and_copy(first,last);
        end_of_storage=finish=start+(last-first);
    }


    template<typename T,class Alloc>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::allocate_and_fill(size_type n,T const& x){
        iterator result=Alloc::allocate(n);
        TinySTL::uninitialized_fill_n(result,n,x);
        return result;
    }

    template<typename T,class Alloc>
    template<class InputIterator>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::allocate_and_copy(InputIterator first, InputIterator last) {
        size_type n=last-first;
        iterator result=Alloc::allocate(n);
        TinySTL::uninitialized_copy(first,last,result);
        return result;
    }


    template<typename T,class Alloc>
    void vector<T,Alloc>::destroy_and_deallocate(iterator first,iterator last){
        if(capacity()) {
            Alloc::destroy(first, last);
            Alloc::deallocate(start,capacity());
        }
    }

    //insert_aux:
    //1.position
    //2.position,n
    //3.range
    template<typename T,class Alloc>
    void vector<T,Alloc>::insert_aux(iterator position,T const& x){
        //有备用空间
        if(finish!=end_of_storage){
            construct(finish,*(finish-1));//创建一个尾元素的拷贝
            ++finish;
            T x_copy=x;
            TinySTL::copy_backward(position,finish-2,finish-1);
            *position=x_copy;
        }else{
            insert_aux(position,size_type(1),x);
        }
    }

    template<typename T,class Alloc>
    void vector<T,Alloc>::insert_aux(iterator position,size_type n,T const& x){
        if(n==0)
            return ;
        size_type storage_left=end_of_storage-finish;
        if(storage_left>=n){    //剩余空间大于新增元素
            T x_copy=x;
            const size_type elems_after=finish-position;
            iterator old_finish=finish;
            if(elems_after>=n){ //插入点后元素大于等于新增元素
                finish=uninitialized_copy(finish-n,finish,finish);
                TinySTL::copy_backward(position,old_finish-n,old_finish);
                TinySTL::fill(position,position+n,x_copy);
            }
            else{
                finish=TinySTL::uninitialized_fill_n(finish,n-elems_after,x_copy);
                finish=TinySTL::uninitialized_copy(position,old_finish,finish);
                TinySTL::fill(position,old_finish,x_copy);
            }
        }
        else{
            reallocate_and_fillN(position,n,x);
        }
    }

    template<typename T,class Alloc>
    template<typename II>
    void vector<T,Alloc>::insert_aux(iterator position,II first,II last){
        const size_type storage_left=end_of_storage-finish;
        const size_type need=last-first;
        if(storage_left>=need){
            const size_type elems_after=finish-position;
            iterator old_finish=finish;
            if(elems_after>=need){
                finish=TinySTL::uninitialized_copy(finish-need,finish,finish);
                finish=TinySTL::copy_backward(position,old_finish-need,old_finish);
                TinySTL::copy(first,last,position);
            }
            else{
                finish=TinySTL::uninitialized_copy(first+elems_after,last,finish);
                finish=TinySTL::uninitialized_copy(position,old_finish,finish);
                TinySTL::copy(first,first+elems_after,position);
            }
        }
        else{
            reallocate_and_copy(position,first,last);
        }
    }

    //erase_aux
    //@return 下一个元素的位置
    template<typename T,class Alloc>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::erase_aux(iterator first,iterator last){
        iterator i=TinySTL::copy(last,finish,first);
        Alloc::destroy(i,finish);
        finish=finish-(last-first);
        return first;
    }

    //@return 下一个元素的位置
    template<typename T,class Alloc>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::erase_aux(iterator position){
        if(position+1!=end())//不为尾元素，先拷贝
            copy(position+1,finish,position);
        --finish;
        Alloc::destroy(finish);
        return position;
    }

    //获取更大的容量
    template<typename T,class Alloc>
    typename vector<T,Alloc>::size_type
    vector<T,Alloc>::getNewCapacity(size_type len)const{
        size_type old_capacity=capacity();
        auto res=TinySTL::max(old_capacity,len);
        size_type new_capacity=old_capacity!=0?(old_capacity+res):len;
        return new_capacity;
    }

    //reallocate:
    template<typename T,class Alloc>
    template<typename II>
    void vector<T,Alloc>::reallocate_and_copy(iterator position,II first,II last){
        const size_type new_capacity=getNewCapacity(last-first);

        iterator new_start=Alloc::allocate(new_capacity);
        iterator new_finish=new_start;

        TinySTL_TRY{
            new_finish=TinySTL::uninitialized_copy(begin(),position,new_start);
            new_finish=TinySTL::uninitialized_copy(first,last,new_finish);
            new_finish=TinySTL::uninitialized_copy(position,end(),new_finish);
        }
        catch(...){
            Alloc::destroy(new_start,new_finish);
            Alloc::deallocate(new_start,new_capacity);
        }

        destroy_and_deallocate(begin(),end());

        start=new_start;
        finish=new_finish;
        end_of_storage=start+new_capacity;
    }

    template<typename T,class Alloc>
    void vector<T,Alloc>::reallocate_and_fillN(iterator position,size_type n,T const& x){
        const size_type new_capacity=getNewCapacity(n);

        iterator new_start=Alloc::allocate(new_capacity);
        iterator new_finish=new_start;

        TinySTL_TRY{
            new_finish=TinySTL::uninitialized_copy(begin(),position,new_start);
            new_finish=TinySTL::uninitialized_fill_n(new_finish,n,x);
            new_finish=TinySTL::uninitialized_copy(position,end(),new_finish);
        }
        catch(...){
            destroy(new_start,new_finish);
            Alloc::deallocate(new_start,new_capacity);
        }

        destroy_and_deallocate(start,finish);

        start=new_start;
        finish=new_finish;
        end_of_storage=start+new_capacity;
    }

    template<typename T,class Alloc>
    template<typename...Args>
    void vector<T,Alloc>::reallocate_and_emplace(iterator position, Args &&...args) {
        const size_type new_capacity=getNewCapacity(capacity());

        iterator new_start=Alloc::allocate(new_capacity);
        iterator new_finish=new_start;

        TinySTL_TRY{
            new_finish=TinySTL::uninitialized_move(begin(),position,new_start);
            Alloc::construct(&*new_finish,TinySTL::forward<Args>(args)...);
            ++new_finish;
            new_finish=TinySTL::uninitialized_move(position,end(),new_finish);
        }
        catch(...){
            destroy(new_start,new_finish);
            Alloc::deallocate(new_start,new_capacity);
            throw;
        }

        destroy_and_deallocate(begin(),end());

        start=new_start;
        finish=new_finish;
        end_of_storage=new_start+new_capacity;
    }

    //swap
    template<typename T,class Alloc>
    void vector<T,Alloc>::swap(vector& rhs)noexcept{
        using TinySTL::swap;
        swap(start,rhs.start);
        swap(finish,rhs.finish);
        swap(end_of_storage,rhs.end_of_storage);
    }
}
#endif //TINYSTL_VECTOR_IMPL_H

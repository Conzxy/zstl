#ifndef TINYSTL_VECTOR_IMPL_H
#define TINYSTL_VECTOR_IMPL_H

#include "stl_vector.h"
#include "stl_exception.h"
#include <iostream>

namespace TinySTL{ 
    //assignment              
    template<typename T,class Alloc>
    vector<T,Alloc>& vector<T,Alloc>::operator=(vector const& rhs){
        if(this!=&rhs){                 
            auto r_size=rhs.size();
            //容量不够=>扩容
            if(r_size>capacity()){
                vector tmp(rhs);
                this->swap(tmp);
                return *this;
            }
            //容量足够时根据元素数量进行copy
            else if(r_size<=size()){
                auto i=TinySTL::copy(rhs.begin(),rhs.end(),begin());
                destroy(i,end());
            }else{//size()<rhs.size()<=capacity()
                TinySTL::copy(rhs.begin(),rhs.begin()+size(),begin());
                TinySTL::uninitialized_copy(rhs.begin()+size(),rhs.end(),end());
            }
            vb.last_=vb.first_+r_size;
        }
        return *this;
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
    typename vector<T,Alloc>::reference vector<T,Alloc>::at(size_type n) noexcept {
        STL_TRY{
            THROW_RANGE_ERROR_IF(n>=size(),"The location is not exist!");
            return *(begin()+n);
        }
        catch(const std::range_error &e){
            std::cerr<<"RANGE_ERROR:"<<e.what()<<std::endl;
            RETHROW
        }
    }

    template<typename T,class Alloc>
    typename vector<T,Alloc>::const_reference vector<T,Alloc>::at(size_type n) const noexcept{
        STL_TRY{
            THROW_RANGE_ERROR_IF(n>=size(),"The location is not exist!");
            return *(cbegin()+n);
        }
        catch(const std::range_error &e){
            std::cerr<<"RANGE_ERROR:"<<e.what()<<std::endl;
            RETHROW
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
        using TinySTL::swap;
        if(n <= capacity())
            return ;
        THROW_LENGTH_ERROR_IF(n>max_size(),
                              "The requirement is greater than maximum number of elements!");
        base new_vb(n);
        new_vb.last_=TinySTL::uninitialized_move(begin(),end(),new_vb.first_);
        swap(vb,new_vb);
    }

    //缩减容量以适应元素个数
    template<typename T,class Alloc>
    void vector<T,Alloc>::shrink_to_fit(){
        using TinySTL::swap;
        base new_vb(size());
        uninitialized_move(begin(),end(),new_vb.first_);
        swap(vb,new_vb);
    }

    //modifiers:
    template<typename T,class Alloc>
    template<typename...Args>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::emplace(const_iterator position,Args&&...args){
        if(vb.last_ < vb.capa_ ){
            if(position == end())
                emplace_back(TinySTL::forward<Args>(args)...);
            else{
                Alloc::construct(end(),back());
                copy_backward(position,const_iterator(vb.last_-1),vb.last_);
                *position=value_type(TinySTL::forward<Args>(args)...);
            }
        }
        else{
            reserve(getNewCapacity(1));
            return emplace(position,TinySTL::forward<Args>(args)...);
        }
        return begin()+(position-begin());
    }

    template<typename T,class Alloc>
    template<typename...Args>
    void vector<T,Alloc>::emplace_back(Args&&... args){
        if(end()<vb.capa_){
            Alloc::construct(&*end(),TinySTL::forward<Args>(args)...);
            ++vb.last_;
        }
        else{
            reserve(getNewCapacity(1));
            emplace_back(TinySTL::forward<Args>(args)...);
        }
    }

    template<typename T,class Alloc>
    void vector<T,Alloc>::push_back(T const& x){
        if(vb.last_ != vb.capa_){
            Alloc::construct(end(),x);
            ++vb.last_;
        }
        else{
            reserve(getNewCapacity(1));
            push_back(x);
        }
    }

    template<typename T,class Alloc>
    void vector<T,Alloc>::pop_back(){
        TINYSTL_DEBUG(size());
        --vb.last_;
        Alloc::destroy(vb.last_);
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
    template<class II,class>
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

    //insert_aux:
    //1.position
    //2.position,n
    //3.range
    template<typename T,class Alloc>
    void vector<T,Alloc>::insert_aux(iterator position,T const& x){
        //将[position,end())往后面移一位，x占据position位置
        if(vb.last_ != vb.capa_){
            construct(end(),back());
            TinySTL::copy_backward(position,vb.last_-1,vb.last_);
            ++vb.last_;
            *position=x;
        }else{
            insert_aux(position,size_type(1),x);
        }
    }

    template<typename T,class Alloc>
    void vector<T,Alloc>::insert_aux(iterator position,size_type n,T const& x){
        if(n==0)
            return ;
        size_type storage_left=vb.capa_-vb.last_;
        if(storage_left>=n){    //剩余空间大于新增元素
            const size_type elems_after=vb.last_-position;
            auto old_end=vb.last_;
            if(elems_after <= n){
                //此时last_后面有n-elems_after个未初始化空间，初始化为x并拷贝原来的到新last_后面
                vb.last_=uninitialized_fill_n(end(),n-elems_after,x);
                vb.last_=uninitialized_copy(position,old_end,vb.last_);
                fill_n(position,elems_after,x);
            }else{
                //先将last_前n个元素往后移然后移动剩余elems_after-n个元素
                vb.last_=uninitialized_copy(vb.last_-n,vb.last_,vb.last_);
                copy_backward(position,old_end-n,old_end);
                fill_n(position,n,x);
            }
        }
        else{
            base newvb(getNewCapacity(n));
            newvb.last_=uninitialized_move(begin(),position,newvb.first_);
            newvb.last_=uninitialized_fill_n(newvb.last_,n,x);
            newvb.last_=uninitialized_move(position,end(),newvb.last_);
            vb.swap(newvb);
        }
    }

    template<typename T,class Alloc>
    template<typename II>
    void vector<T,Alloc>::insert_aux(iterator position,II first,II last){
        const size_type storage_left=vb.capa_-vb.last_;
        const size_type n=last-first;
        if(storage_left >= n){
            const size_type elems_after=vb.last_-position;
            iterator old_end=vb.last_;
            if(elems_after < n){
                vb.last_=uninitialized_copy(first+elems_after,last,vb.last_);
                vb.last_=uninitialized_copy(position,old_end,vb.last_);
                copy(first,first+elems_after,position);
            }else{
                vb.last_=uninitialized_copy(vb.last_-n,vb.last_,vb.last_);
                copy_backward(position,old_end-n,old_end);
                copy(first,last,position);
            }
        }
        else{
            base newvb(getNewCapacity(n));
            newvb.last_=uninitialized_copy(begin(),position,newvb.first_);
            newvb.last_=uninitialized_copy(first,last,newvb.last_);
            newvb.last_=uninitialized_copy(position,end(),newvb.last_);
            vb.swap(newvb);
        }
    }

    //return:下一个元素的位置
    template<typename T,class Alloc>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::erase_aux(iterator first,iterator last){
        iterator i=TinySTL::copy(last,end(),first);
        Alloc::destroy(i,end());
        vb.last_=end()-(last-first);
        return first;
    }

    //TO DO:删除末尾元素，若不是末尾元素=>转化成末尾元素i.e将后面的元素移前面去覆盖欲删除元素
    //return:删除元素的下一个元素
    template<typename T,class Alloc>
    typename vector<T,Alloc>::iterator
    vector<T,Alloc>::erase_aux(iterator position){
        if(position+1!=end())
            copy(position+1,end(),position);
        --vb.last_;
        Alloc::destroy(end());
        return position;
    }

    //获取更大的容量
    //alloc policy:
    //if required size(let len) more than old_capacity,new_capacity=old_capacity+len
    //otherwise,new_capacity=old_capacity*2
    //But,if old_capacity=0,new_capacity=len
    template<typename T,class Alloc>
    typename vector<T,Alloc>::size_type
    vector<T,Alloc>::getNewCapacity(size_type len)const{
        size_type old_capacity=capacity();
        size_type res=TinySTL::max(old_capacity,len);
        size_type new_capacity=old_capacity+res;
        return new_capacity;
    }
}
#endif //TINYSTL_VECTOR_IMPL_H

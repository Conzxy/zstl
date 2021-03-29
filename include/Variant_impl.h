//
// Created by 谁敢反对宁宁，我就打爆他狗头！ on 2021/1/16.
//

#ifndef TINYSTL_VARIANT_IMPL_H
#define TINYSTL_VARIANT_IMPL_H

#include "Variant_.h"
#include <_move.h>
#include <exception>

namespace TinySTL{

    //VariantChoice part
    template<typename T,typename ...Types>
    VariantChoice<T,Types...>::VariantChoice(T const& value){
        new(asDerived().getRawBuffer()) T(value);
        asDerived().setDiscriminator(Discriminator);
    }

    template<typename T,typename ...Types>
    VariantChoice<T,Types...>::VariantChoice(T && value){
        new(asDerived().getRawBuffer()) T(STL_ move(value));
        asDerived().setDiscriminator(Discriminator);
    }

    template<typename T,typename ...Types>
    bool VariantChoice<T,Types...>::destroy(){
        if(asDerived().getDiscriminator()==Discriminator){
            asDerived().template getBufferAs<T>()->~T();
            return true;
        }
        return false;
    }

    template<typename T,typename ...Types>
    typename VariantChoice<T,Types...>::Derived& VariantChoice<T,Types...>::operator=(T const& value){
        if(asDerived().getDiscriminator()==Discriminator){
            *asDerived().template getBufferAs<T>() =value;
        }else{
            asDerived().destroy();
            new(asDerived().getRawBuffer()) T(value);
            asDerived().setDiscriminator(Discriminator);
        }

        return asDerived();
    }

    template<typename T,typename ...Types>
    typename VariantChoice<T,Types...>::Derived& VariantChoice<T,Types...>::operator=(T && value){
        if(asDerived().getDiscriminator()==Discriminator){
            *asDerived().template getBufferAs<T>()=STL_ move(value);
        }else{
            asDerived().destroy();
            new(asDerived().getRawBuffer()) T(STL_ move(value));
            asDerived().setDiscriminator(Discriminator);
        }

        return asDerived();
    }

    //Variant part

    template<typename ...Types>
    Variant<Types...>::Variant(){
        *this=TL_ Front<Typelist<Types...>>();
    }

    template<typename ...Types>
    Variant<Types...>::Variant(Variant const& v){
        if(!v.empty())
            v.visit([&](auto const& val){
                *this=val;
            });
    }

    template<typename ...Types>
    Variant<Types...>::Variant(Variant&& v){
        if(!v.empty()){
            STL_ move(v).visit([&](auto&& val){
                *this=STL_ move(val);
            });
        }
    }

    template<typename ...Types>
    Variant<Types...>& Variant<Types...>::operator=(Variant<Types...> const& rhs){
        if(!rhs.empty())
            rhs.visit([&](auto const& val){
                *this=val;
            });
        else
            destroy();

        return *this;
    }


    template<typename ...Types>
    template<typename T>
    bool Variant<Types...>::is() const {
        return this->getDiscriminator() ==
                VariantChoice<T,Types...>::Discriminator;
    }

    template<typename ...Types>
    template<typename T>
    T& Variant<Types...>::get() & {
        if(empty())
            throw EmptyVariant{};

        assert(is<T>());
        return *this->template getBufferAs<T>();
    }

    template<typename ...Types>
    template<typename T>
    T const& Variant<Types...>::get() const & {
        if(empty())
            throw EmptyVariant{};

        assert(is<T>());
        return *this->template getBufferAs<T>();
    }

    template<typename ...Types>
    template<typename T>
    T && Variant<Types...>::get() && {
        if(empty())
            throw EmptyVariant{};

        assert(is<T>());
        return STL_ move(*this->template getBufferAs<T>());
    }

    template<typename ...Types>
    template<typename R,typename Visitor>
    VisitResult<R,Visitor,Types&...> Variant<Types...>::visit(Visitor&& vis) & {
        using Result=VisitResult<R,Visitor,Types&...>;
        return variantVisitImpl<Result>(*this,STL_ forward<Visitor>(vis),
                                        Typelist<Types...>());
    }

    template<typename ...Types>
    template<typename R,typename Visitor>
    VisitResult<R,Visitor,Types const&...> Variant<Types...>::visit(Visitor&& vis) const & {
        using Result=VisitResult<R,Visitor,Types const&...>;
        return variantVisitImpl<Result>(*this,STL_ forward<Visitor>(vis),
                                        Typelist<Types...>());
    }

    template<typename ...Types>
    template<typename R,typename Visitor>
    VisitResult<R,Visitor,Types &&...> Variant<Types...>::visit(Visitor&& vis) && {
        using Result=VisitResult<R,Visitor,Types &&...>;
        return variantVisitImpl<Result>(*this,STL_ forward<Visitor>(vis),
                                        Typelist<Types...>());
    }

    template<typename ...Types>
    bool Variant<Types...>::empty(){
        return this->getDiscriminator()==0;
    }

    template<typename ...Types>
    void Variant<Types...>::destroy(){
        bool results[]={
                VariantChoice<Types,Types...>::destroy()...
        };

        this->setDiscriminator(0);
    }
}
#endif //TINYSTL_VARIANT_IMPL_H

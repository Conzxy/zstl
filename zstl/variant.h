#ifndef ZSTL_VARIANT_H
#define ZSTL_VARIANT_H

#include <new> //std::
#include "typelist.h"
#include "config.h"
#include "type_traits.h"
#include "stl_move.h"
#include <exception>

namespace zstl{

	using namespace zstl::mpl;

    //visit helper class
    class ComputedResultType{};

    class EmptyVariant:std::exception{
    };

    template<typename R,typename Visitor,typename ...ElementType>
    class VisitResultT{
    public:
        using type=R;
    };

    template<typename R,typename V,typename ...E>
    using VisitResult=typename VisitResultT<R,V,E...>::type;

    template<typename Visitor,typename T>
    using VisitElementType=decltype(STL_ declval<Visitor>()(declval<T>()));

    template<typename V,typename ...E>
    class VisitResultT<ComputedResultType,V,E...>{
    public:
        using type=STL_ Common_type_t<VisitElementType<V,E>...>;
    };

    template<typename R,typename V,typename Visitor,
            typename Head,typename ...Tail>
    R variantVisitImpl(V&& variant,Visitor&& vis,Typelist<Head,Tail...>){
        if(variant.template is<Head>()){
            return static_cast<R>(
                    STL_ forward<Visitor>(vis)(
                            STL_ forward<V>(variant).template get<Head>()
                            )
                    );
        }
        else if constexpr(sizeof...(Tail)) {
            return variantVisitImpl(STL_ forward<V>(variant),
                    STL_ forward<Visitor>(vis),
                            Typelist<Tail...>());
        }
        else{
            throw EmptyVariant{};
        }
    }

    //viarant struct define
    template<typename ...Types>
    class Variant;

    template<typename ...Types>
    class VariantStorage{
        using Largest=TL_ Largest_Type<Typelist<Types...>>;
        alignas(Types...) unsigned char buffer[sizeof(Largest)];
        unsigned char discriminator=0;
    public:
        VariantStorage()=default;
        unsigned char getDiscriminator()const{ return discriminator; }
        void setDiscriminator(unsigned char d){ discriminator=d; }

        void*       getRawBuffer(){ return buffer; }
        void const* getRawBuffer()const{ return buffer; };

        template<typename T>
        T* getBufferAs(){ return STD_ launder(reinterpret_cast<T*>(buffer)); }
        template<typename T>
        T const* getBufferAs()const{ return STD_ launder(reinterpret_cast<T const*>(buffer));}
   };
	
	// CRTP technique
	// maybe not intuitive 
	// base class know infomaton about its derived class
    template<typename T,typename ...Types>
    class VariantChoice{
        using Derived=Variant<Types...>;
    public:
        Derived& asDerived(){ return *static_cast<Derived*>(this); }
        Derived const& asDerived()const{ return *static_cast<Derived const*>(this); }
    protected:
        constexpr static unsigned Discriminator=
                TL_ Index_Of<Typelist<Types...>,T> + 1;
    public:
        explicit VariantChoice()=default;
        VariantChoice(T const& value);
        VariantChoice(T&& value);
        bool destroy();
        Derived& operator=(T const& value);
        Derived& operator=(T &&value);
    };

    template<typename ...Types>
    class Variant
            :private VariantStorage<Types...>,
            private VariantChoice<Types,Types...>...
    {
        template<typename T,typename ...OtherTypes>
        friend class VariantChoice;

    public:
        template<typename T> bool is()const;
        template<typename T> T& get() &;
        template<typename T> T const& get()const &;
        template<typename T> T&& get() &&;

        template<typename R=ComputedResultType,typename Visitor>
        VisitResult<R,Visitor,Types&...> visit(Visitor&& vis) &;
        template<typename R=ComputedResultType,typename Visitor>
        VisitResult<R,Visitor,Types const&...> visit(Visitor&& vis) const &;
        template<typename R=ComputedResultType,typename Visitor>
        VisitResult<R,Visitor,Types&&...> visit(Visitor&& vis) &&;

        using VariantChoice<Types,Types...>::VariantChoice...;
        Variant();
        Variant(Variant const&);
        Variant(Variant &&);
        template<typename ...SourceTypes>
        Variant(Variant<SourceTypes...> const&);
        template<typename ...SourceTypes>
        Variant(Variant<SourceTypes...> &&);

        using VariantChoice<Types,Types...>::operator= ...;
        Variant& operator=(Variant const&);
        Variant& operator=(Variant&&);
        template<typename ...SourceTypes>
        Variant& operator=(Variant<SourceTypes...> const&);
        template<typename ...SourceTypes>
        Variant& operator=(Variant<SourceTypes...>&& );

        bool empty();

        ~Variant(){ destroy(); }
        void destroy();
    };

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
} //namespace zstl

#endif //ZSTL_VARIANT__H

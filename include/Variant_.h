//
// Created by 谁敢反对宁宁，我就打爆他狗头！ on 2021/1/15.
//

#ifndef TINYSTL_VARIANT__H
#define TINYSTL_VARIANT__H

#include <new>
#include <Typelist.h>
#include "config.h"
#include <type_traits.h>

using namespace TinySTL::mpl;

namespace TinySTL{
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

    template<typename T,typename ...Types>
    class VariantChoice{
        using Derived=Variant<Types...>;
    public:
        Derived& asDerived(){ return *static_cast<Derived*>(this); }
        Derived const& asDerived()const{ return *static_cast<Derived const*>(this); }
    protected:
        constexpr static unsigned Discriminator=
                TL_ Index_Of<Typelist<Types...>,T> +1;
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
}

#endif //TINYSTL_VARIANT__H

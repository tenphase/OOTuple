/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Stéphane Cherpit <stephane@eosgarden.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef TUPLE_H
#define TUPLE_H

/*
 * [ WARNING ]
 * The Tuple class has not been fully tested yet
 * and it misses features that std::tuple has.
 * But it should be rather reliable and stable,
 * and fulfills the purpose for which it has been done.
 * Allow the creation of tuple objects having access methods
 * with a constant complexity - O(1).
 *
 * It has been done as an exercise to better understand:
 * - how to use variadic template parameters.
 * - how to implement recursive instantiation of such a template.
 * - how to declare a recursive accessor trait.
 */

/*
 * TUPLE_VERBOSITY can be set to 0, 1 or 2.
 * 0: Disables the output of messages.
 * 1: Prints a message when the following functions/methods are called:
 * - The makTuple free function
 * - The Tuple constructor
 * - The Tuple copy constructor and copy assignment operator
 * - The Tuple move constructor and move assignment operator
 * 2: Also prints a message when the Tuple destructor is called.
 */
#define TUPLE_VERBOSITY 0

#include <cstdlib>
#include <type_traits>
#include <functional>
#include <utility>

#if defined( TUPLE_VERBOSITY ) && TUPLE_VERBOSITY != 0
#   include <iostream>
#   define MAKE_TUPLE_MSG() std::cout << "[ makeTuple ] " << std::endl
#   define TUPLE_MSG( ... ) std::cout << "[ Tuple ] " << __VA_ARGS__ << std::endl
#   if TUPLE_VERBOSITY == 2
#       define TUPLE_DTOR_MSG( ... ) TUPLE_MSG( __VA_ARGS__ )
#   else
#       define TUPLE_DTOR_MSG( ... )
#   endif
#else
#   define MAKE_TUPLE_MSG()
#   define TUPLE_MSG( ... )
#   define TUPLE_DTOR_MSG( ... )
#endif

namespace TP {
namespace experimental {

/**
 * @brief   The Tuple base class template (empty base).
 *
 *          This version of the Tuple class template is only called
 *          once the recursive version of it has consumed all the variadic
 *          arguments. Thus, it is always called like this 'Tuple<>()'.
 */
template<typename ...TTail>
class Tuple
{
    protected:
        Tuple( void )
        {}
};

/**
 * @brief   The tuple_accessor trait is an indexed accessor for the Tuple class elements.
 *
 *          It allows to deduce the type of an element in a Tuple based on an index.
 *          It is used internally by the Tuple class so that the accessor methods
 *          can cast the Tuple in the correct base class and return its head element.
 *          This is a variadic recursive template, thus there's no runtime computation
 *          to access any particular element in a tuple.
 *          Complexity is always constant – O(1).
 */
template <size_t VIndex, typename ...TTail>
struct tuple_accessor;

/**
 * @brief   Specialization of the tuple_accessor trait for when the index reaches 0.
 *
 *          This is the 'leaf' of the variadic recursive version of tuple_accessor.
 *          It 'holds' the effective type of the tuple and the one of its head element.
 * @see     tuple_accessor
 */
template <typename THead, typename ...TTail>
struct tuple_accessor<0, THead, TTail...>
{
    public:
        typedef Tuple<THead, TTail...>  tuple_type;
        typedef THead                   value_type;
};

/**
 * @brief   Specialization of the tuple_accessor trait for all indexes but 0.
 * @see     tuple_accessor
 */
template <size_t VIndex, typename THead, typename ...TTail>
struct tuple_accessor<VIndex, THead, TTail...>
{
    public:
        typedef typename tuple_accessor<VIndex - 1, TTail...>::tuple_type tuple_type;
        typedef typename tuple_accessor<VIndex - 1, TTail...>::value_type value_type;
};

/**
 * @brief   tuple_accessor alias to get the type of the element at VIndex.
 * @see     tuple_accessor
 */
template<size_t VIndex, typename THead, typename ...TTail>
using tuple_element_t = typename tuple_accessor<VIndex, THead, TTail...>::value_type;

/**
 * @brief   tuple_accessor alias to get the type of the tuple at VIndex.
 * @see     tuple_accessor
 */
template<size_t VIndex, typename THead, typename ...TTail>
using tuple_t = typename tuple_accessor<VIndex, THead, TTail...>::tuple_type;

/**
 * @brief   The tuple_element_decay trait.
 *
 *          It is used by makeTuple to remove the references and 'force'
 *          the copy of the elements passed to it. This implies that
 *          each type must be 'default constructible'...
 * @see     std::decay
 */
template <typename T>
struct tuple_element_decay
{
    typedef typename std::decay<T>::type type;
};

/**
 * @brief   A specialized tuple_element_decay trait for std::reference_wrapper<T>.
 *
 *          This allows to pass an explicit reference to makeTuple using std::ref()
 *          or std::cref().
 */
template <typename T>
struct tuple_element_decay<std::reference_wrapper<T>>
{
    typedef T& type;
};

/**
 * @brief   tuple_element_decay alias to get the 'decayed' type of an element.
 */
template<typename T>
using tuple_element_decay_t = typename tuple_element_decay<T>::type;

/**
 * @brief   The makeTuple free function has to be used to create a Tuple object.
 *
 *          It uses template argument deduction to simplify the signature
 *          of the returned type. This means that one can use 'auto' as follows:
 *          auto myTuple = makeTuple( 1.23, 42, std::string( "Hello World" ) );
 *          It also ensure that all types (except pointers, function pointers and 'explicit references')
 *          have been 'copied' by using tuple_element_decay_t to declare them.
 *          Note that 'explicit references' can be produced using std::ref() or std::cref().
 * @param   args    The elements passed to initialise the Tuple.
 * @return          A new Tuple object.
 */
template<typename ...TArgs>
Tuple<tuple_element_decay_t<TArgs>...>
makeTuple( TArgs && ...args )
{
    class _Tuple: public Tuple<tuple_element_decay_t<TArgs>...>
    {
        public:
            _Tuple( TArgs ...els ):
                Tuple<tuple_element_decay_t<TArgs>...>( els... )
            {}
    };

    MAKE_TUPLE_MSG();

    return _Tuple( std::forward<TArgs>( args )... );
}

/**
 * @brief   The Tuple class template.
 *
 *          This is the "real" (recursive) implementation of the Tuple class.
 */
template<typename THead, typename ...TTail>
class Tuple<THead, TTail...>: public Tuple<TTail...>
{
    private:

        /**
         * @brief   Internal typedef to make the code clearer.
         */
        typedef Tuple<THead, TTail...> TSelf;

    protected:

        /**
         * @brief   Tuple constructor.
         *
         *          This constructor does not ensure that types have been 'decayed'.
         *          That's why it is 'protected' and why you should always
         *          use makeTuple() to create a Tuple instance...
         * @param   head    The element to be 'consumed' (the head element).
         * @param   els     The remaining elements in the list.
         */
        explicit Tuple( const THead & head, const TTail & ...tail ):
            Tuple<TTail...>( tail... ),
            _head( head )
        {
            TUPLE_MSG( "[ Ctor ] (_head = [" << _head << "])" );
        }

    public:


        /**
         * @brief   Unnamed enum allowing to get the length of a tuple.
         *
         *          C++ allows to access an enum value from an instance using the dot notation.
         *          'someTuple.Length' will provide the length of the instance
         *          because the compiler will deduce its type...
         */
        enum: size_t
        {
            Length = sizeof...( TTail ) + 1
        };

        ~Tuple( void )
        {
            TUPLE_DTOR_MSG( "[ Dtor ] (_head = [" << _head << "])" );
        }

        Tuple( const TSelf & other ):
            Tuple<TTail...>( other ),
            _head( other._head )
        {
            TUPLE_MSG( "[ COPY Constrctor ] (_head = [" << _head << "])" );
        }

        Tuple( TSelf && other ) noexcept:
            Tuple<TTail...>( std::forward<Tuple<TTail...>>( other ) ),
            _head( std::move( other._head ) )
        {
            TUPLE_MSG( "[ MOVE Constrctor ] (_head = [" << _head << "])" );
        }

        TSelf & operator=( TSelf & other )
        {
            TUPLE_MSG( "[ COPY Assignment operator ] (_head = [" << _head << "])" );

            static_cast<Tuple<TTail...> &>( *this ) = static_cast<Tuple<TTail...> &>( other );

            _head = other._head;

            return *this;
        }

        TSelf & operator=( TSelf && other ) noexcept
        {
            using std::swap;

            TUPLE_MSG( "[ MOVE Assignment operator ] (_head = [" << _head << "])" );

            swap( *this, other );

            return *this;
        }

        /**
         * @brief   Returns a reference to the head element stored by this instance.
         *
         *          Here, instance means the type of the object by which one accesses
         *          the method...
         * @returns A reference to the head element stored by this instance.
         */
        THead & head( void ) noexcept
        {
            return _head;
        }

        /**
         * @brief   Returns a reference to the element at index 'index'.
         * @returns A reference to the element at index 'index'.
         */
        template<size_t index>
        typename std::enable_if<( index < Length ), tuple_element_t<index, THead, TTail...>>::type &
        get( void ) noexcept
        {
            return static_cast<tuple_t<index, THead, TTail...> *>( this )->head();
        }

        /**
         * @brief   Returns a reference to the tail element stored by this instance.
         *
         *          Here, instance means the type of the object by which one accesses
         *          the method...
         * @returns A reference to the tail element stored by this instance.
         */
        typename std::enable_if<( Length > 0 ), tuple_element_t<Length - 1, THead, TTail...>>::type &
        tail( void ) noexcept
        {
            return static_cast<tuple_t<Length - 1, THead, TTail...> *>( this )->head();
        }

        /**
         * @brief   Swaps two Tuple objects having the same type.
         * @param   self    The first Tuple object.
         * @param   other   The second Tuple object.
         * @returns Nothing.
         */
        friend void swap( TSelf & self, TSelf & other )
        {
            using std::swap;

            swap
            (
                static_cast<Tuple<TTail...> &>( self ),
                static_cast<Tuple<TTail...> &>( other )
            );

            swap( self._head, other._head );

            TUPLE_MSG( "[ SWAP ] (self._head = [" << self._head << "] <<< other._head = [" << other._head << "])" );
        }

    private:
        THead _head;

};

/**
 * @brief   Specialization of the Tuple class template for when the head element is a reference.
 *
 *          This specialization works exactly the same as the non-specialized one.
 *          For the differences in the implementation, look at the comments in the code
 *          (only the differences are commented).
 */
template<typename THead, typename ...TTail>
class Tuple<THead &, TTail...>: public Tuple<TTail...>
{
    private:
        /* [ spec diff ] - THead is a reference */
        typedef Tuple<THead &, TTail...> TSelf;

    protected:

        /* [ spec diff ] - THead is not 'const' */
        explicit Tuple( THead & head, const TTail & ...tail ):
            Tuple<TTail...>( tail... ),
            _head( head )
        {
            TUPLE_MSG( "[ ref spec ] [ Ctor ] (_head = [" << _head << "])" );
        }

    public:

        enum: size_t
        {
            Length = sizeof...( TTail ) + 1
        };

        ~Tuple( void )
        {
            TUPLE_DTOR_MSG( "[ ref spec ] [ Dtor ] (_head = [" << _head << "])" );
        }

        Tuple( const TSelf & other ):
            Tuple<TTail...>( other ),
            _head( other._head )
        {
            TUPLE_MSG( "[ ref spec ] [ COPY Constructor ] (_head = [" << _head << "])" );
        }

        /* [ spec diff ] - _head is not moved with std::move() */
        Tuple( TSelf && other ) noexcept:
            Tuple<TTail...>( std::forward<Tuple<TTail...>>( other ) ),
            _head( other._head )
        {
            TUPLE_MSG( "[ ref spec ] [ MOVE Constructor ] (_head = [" << _head << "])" );
        }

        TSelf & operator=( TSelf & other )
        {
            TUPLE_MSG( "[ ref spec ] [ COPY Assignment operator ] (_head = [" << _head << "])" );

            static_cast<Tuple<TTail...> &>( *this ) = static_cast<Tuple<TTail...> &>( other );

            _head = other._head;

            return *this;
        }

        TSelf & operator=( TSelf && other ) noexcept
        {
            using std::swap;

            TUPLE_MSG( "[ ref spec ] [ MOVE Assignment operator ] (_head = [" << _head << "])" );

            swap( *this, other );

            return *this;
        }

        THead & head( void ) noexcept
        {
            return _head;
        }

        /* [ spec diff ] - THead is a reference */
        template<size_t index>
        typename std::enable_if<( index < Length ), tuple_element_t<index, THead &, TTail...>>::type &
        get( void ) noexcept
        {
            return static_cast<tuple_t<index, THead &, TTail...> *>( this )->head();
        }

        /* [ spec diff ] - THead is a reference */
        typename std::enable_if<( Length > 0 ), tuple_element_t<Length - 1, THead &, TTail...>>::type &
        tail( void ) noexcept
        {
            return static_cast<tuple_t<Length - 1, THead &, TTail...> *>( this )->head();
        }

        friend void swap( TSelf & self, TSelf & other )
        {
            using std::swap;

            swap
            (
                static_cast<Tuple<TTail...> &>( self ),
                static_cast<Tuple<TTail...> &>( other )
            );

            swap( self._head, other._head );

            TUPLE_MSG( "[ ref spec ] [ SWAP ] (self._head = [" << self._head << "] <<< other._head = [" << other._head << "])" );
        }

    private:
        THead & _head;

};

} /* namespace experimental */
} /* namespace TP */

#endif /* TUPLE_H */

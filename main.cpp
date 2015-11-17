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

#include <iostream>

#include "src/tuple.hpp"
#include "ootuple_headers.h"

using namespace TP::experimental;

int main( void )
{
    std::cout << OOTuple_HEADER << std::endl;

    int         i = 42;
    std::string hello( "Hello World" );

    /* Here the type of 'tuple' will be:
     * Tuple<double, int*, char, std::string, std::reference_wrapper<std::string>>
     */
    auto tuple = makeTuple( 1.23, &i, 'A', hello, std::ref( hello ) );

    /* Modifying 'hello' will only affect the element at index 4
     * because it has been passed to makeTuple with std::ref().
     * ('hello' at index 3 has been copied within the tuple)
     */
    hello = std::string( "Hello Univers" );

    std::cout << "tuple.Length   : " << tuple.Length    << std::endl; /* 5               */
    std::cout << "tuple.head()   : " << tuple.head()    << std::endl; /* 1.23            */
    std::cout << "tuple.get<1>() : " << tuple.get<1>()  << std::endl; /* address of 'i'  */
    std::cout << "*tuple.get<1>(): " << *tuple.get<1>() << std::endl; /* value of 'i'    */
    std::cout << "tuple.get<2>() : " << tuple.get<2>()  << std::endl; /* 'A'             */
    std::cout << "tuple.get<3>() : " << tuple.get<3>()  << std::endl; /* "Hello World"   */
    std::cout << "tuple.tail()   : " << tuple.tail()    << std::endl; /* "Hello Univers" */
    std::cout << std::endl;

    tuple.get<0>()  = 1.42; /* Modifies the value stored at index 0                      */
    *tuple.get<1>() = 84;   /* Modifies the value pointed to by the pointer at index 1
                             * That is, the value of 'i',
                             * because 'i' has been stored as a pointer.                 */

    std::cout << "tuple.get<0>() : " << tuple.get<0>()  << std::endl; /* 1.42 */
    std::cout << "i              : " << i               << std::endl; /* 84   */
    std::cout << std::endl;

    return EXIT_SUCCESS;
}

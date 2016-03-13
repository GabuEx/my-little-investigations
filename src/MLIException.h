/**
 * Basic header/include file for MLIException.cpp.
 *
 * @author mad-mix
 * @since 1.0.7
 *
 * Licensed under the MIT License.
 *
 * Copyright (c) 2014 Equestrian Dreamers
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MLIEXCEPTION_H
#define MLIEXCEPTION_H

#include <string>
#include <stdexcept>
#include <iostream>

class MLIException : public std::exception
{
public:
    /**
    Construct an exception with a message
    */
    MLIException( const std::string& details );
    ~MLIException() throw();

    /// Override std::exception::what() to return m_details
    const char* what() const throw();

    std::string m_details; /**< Exception Details */
};

#if defined(__OSX) || defined(__unix)
#include <execinfo.h>

#define PrintCallstack() \
    { \
        void *array[10]; \
        size_t size = backtrace(array, 10); \
        backtrace_symbols_fd(array, size, STDERR_FILENO); \
    }
#else
#define PrintCallstack()
#endif

#ifdef MLI_DEBUG
#define ThrowException(message) \
    std::cerr << "Fatal error in " << __FILE__ << "(" << __LINE__ << "): " << message << std::endl; \
    PrintCallstack(); \
    throw MLIException(message);
#else
#define ThrowException(message) \
    throw MLIException(message);
#endif

#endif // MLIEXCEPTION_H

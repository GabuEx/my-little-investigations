#ifndef MLIEXCEPTION_H
#define MLIEXCEPTION_H

#include <string>
#include <stdexcept>

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

#endif // MLIEXCEPTION_H

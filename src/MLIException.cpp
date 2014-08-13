#include "MLIException.h"

MLIException::MLIException(const std::string &details)
    : m_details(details)
{

}

MLIException::~MLIException() throw()
{
}

const char* MLIException::what() const throw()
{
    return m_details.c_str();
}

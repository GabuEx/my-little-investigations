#include "MLIException.h"

Exception::Exception(const std::string &details)
:
m_details( details )
{

}

Exception::~Exception() throw()
{
}

const char* Exception::what() const throw()
{
    return m_details.c_str();
}

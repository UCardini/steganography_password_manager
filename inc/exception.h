#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

class invalidData : public std::exception
{
public:
    invalidData( const std::string& message ) : message_( message ) {}

    invalidData( const std::string& message, const size_t& type )
        : message_( message ), err_type( type )
    {
    }

    const char* what() const noexcept override
    {
        //
        //
        return message_.c_str();
    }

    const size_t ErrorType()
    {
        //
        //
        return err_type;
    }

private:
    std::string message_;
    size_t err_type;
};

#endif // !EXCEPTION_H

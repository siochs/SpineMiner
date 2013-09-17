#ifndef __IOSWITCH_H__
#define __IOSWITCH_H__

//ioswitch reroutes the cout output also to logfile. Source taken from the web
//basically the standard cout stream buffer will be redirected to two new stream buffers,
//of which one is used for console and the other one for filestream

#define BEGIN_IOSWITCH Spreadbuf spready( cout.rdbuf(), file.rdbuf() );{IosSwitch coutswitch( _stdoutput, &spready );

#define END_IOSWITCH }

#include <streambuf>
#include <ios>
#include <fstream>


std::ofstream file( "log.txt" ); //output file from SpineMiner

class Spreadbuf : public std::basic_streambuf< char >
{
public:
    Spreadbuf( std::basic_streambuf< char >* sb1, std::basic_streambuf< char >* sb2 )
        : m_sb1( sb1 )
        , m_sb2( sb2 )
    {}

protected:
    virtual int_type overflow( int_type c = traits_type::eof() )
    {
        if( !traits_type::eq_int_type( c, traits_type::eof() ) )
        {
            const char x = traits_type::to_char_type( c );
            if( traits_type::eq_int_type( m_sb1->sputc( x ), traits_type::eof() )
                    || traits_type::eq_int_type( m_sb2->sputc( x ), traits_type::eof() ) )
                return traits_type::eof();
        }
        return traits_type::not_eof( c );
    }

private:
    std::basic_streambuf< char >* m_sb1;
    std::basic_streambuf< char >* m_sb2;
};


class IosSwitch
{
public:
    IosSwitch( std::basic_ios< char >& ios, std::streambuf* sb )
        : m_ios( &ios )
        , m_sbMerk( ios.rdbuf( sb ) )
    {}

    ~IosSwitch()
    {
        if( m_ios )
        {
            m_ios->rdbuf( m_sbMerk );
        }
    }
    std::streambuf* rdbuf() const { return m_sbMerk; }

private:
    std::basic_ios< char >* m_ios;
    std::streambuf*         m_sbMerk;

    IosSwitch( const IosSwitch& );
    IosSwitch& operator=( const IosSwitch& );
};

#endif // __IOSWITCH_H__

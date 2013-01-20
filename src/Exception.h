#ifndef _62908ddb_b6ea_4920_883b_5456b3177352_
#define _62908ddb_b6ea_4920_883b_5456b3177352_

#include <exception>

namespace me {
	class encoding_error : public std::exception {
	public:
		encoding_error();
		encoding_error(const char *description);
        virtual ~encoding_error() throw ();
        virtual const char* what() const throw ();
	private:
		char *mDescription;
    };
}

#endif


#ifndef DIASPORA_HEADER_GUARD
#define DIASPORA_HEADER_GUARD

#include "main.h"
#include <list>


std::list<Rue* >*  diaspora() ;
void print_diaspora(std::list<Rue*>* paths) ;
std::list<long>* read_diaspora() ;

#endif

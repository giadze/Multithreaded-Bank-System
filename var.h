#include "types.h"
#ifndef _VAR_H_
#define _VAR_H_

typedef struct var* var;

struct var 
{
	string name;
	void* content;

};

var newVar(string name, void* content);

#endif


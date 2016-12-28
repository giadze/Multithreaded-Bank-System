#include <stdlib.h>
#include "types.h"
#include "var.h"

var newVar(string name, void* content)
{

	var v = malloc(sizeof(*v));
	v -> name = name;
	v -> content = content;

	return v;

}


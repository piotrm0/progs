#ifndef __parse_h__
#define __parse_h__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "list.h"

list* parse_tok_list(char* src, char* delim);

#endif /* __parse_h__ */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"


struct Tokenizer {

    string stream;

    size_t position;

};


typedef struct Tokenizer* Tokenizer;

Tokenizer createTokenizer(string s);

void destroyTokenizer(Tokenizer t);
void reallocate(void** pointer, size_t size);
void skip_whitespace(Tokenizer t);
string getNextWord(Tokenizer t);
string getNextToken(Tokenizer t);
string mass_concatenation(string* argv);


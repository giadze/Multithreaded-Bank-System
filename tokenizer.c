#include "tokenizer.h"

Tokenizer createTokenizer(string s) 
{

    Tokenizer t = (Tokenizer) malloc (sizeof(*t));
    t -> position = 0;
    t -> stream = (string) malloc (strlen(s)+1);
    strcpy( t-> stream, s);

    return t;

}


void destroyTokenizer(Tokenizer t) 
{

    free(t -> stream);
    free(t);

}


void reallocate(void** pointer, size_t size) 
{

    void* temp = realloc(*pointer, 2*size);
    *pointer = temp;

}


void skip_whitespace(Tokenizer t) 
{
    int x = t -> position;

    while (t -> stream[x] && !isalnum((int) t -> stream[x])) 
	{
        x++;
   }

    t -> position = x;

}


string getNextWord(Tokenizer t) 
{

    size_t position = t->position;
    size_t len = 2; 
    size_t dot_count = 0;

    string token = (string) malloc (3);

    if(position >= strlen(t->stream))
        return NULL;

    while ((isalnum((int) t -> stream[position]) || (t -> stream[position] == '.')) && (dot_count <= 1)) 
		{

        if (t -> stream[position] == '.') 
			{

            dot_count ++;

        }

        if (position - t -> position + 1 == len) 
		{

            reallocate((void**) &token, 2*len);
            len*= 2;

        }

        token[position - t -> position] = t -> stream[position];

        position ++;

    }

    token[position - t -> position] = '\0';
    t -> position = position;

    return token;

}


string getNextToken(Tokenizer t) 
{
    string s = NULL;

    if (strlen(t -> stream) > t -> position) 
	{

        skip_whitespace(t);

        if (t -> stream[t -> position]) 
		  {
            s = getNextWord(t);
        }

    }

    return s;

}


string mass_concatenation(string* argv) 
{

    int x = 1, y = 0;
    string s;

    while (argv[x]) 
	{

        y += (strlen(argv[x])+1);
        x ++;

    }

    s = (string) malloc (--y);
    x = 1;

    while (argv[x]) 
	 {

        strcat(s, argv[x]);

        if (argv[x + 1])

            strcat(s, " ");
        		x++;
    }

    return s;

}

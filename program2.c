/*
 * tokenizer.c
 *
 * Author: William "Amos" Confer
 * Date:   Feb 18, 2015
 *
 * This program demonstrates a basic, hand-coded lexical analyzer for a bogus
 * language called "Bogus".  The intent is not to have a language that does
 * anything so much as a lexicon to tokenize.  The Bogus language has the
 * following token types:
 *
 * reserved words: if, while
 * bit patterns: 0110101110011
 * open tag: <
 * close tag: >
 * assign: :=
 * alternate: ::
 * question: :
 * identifiers: sequence of letters
 * comments between braces, no nesting, multi line OK: { blah blah blah }
 *
 * ...also, it is case sensitive.
 *
 * -----------------------------------
 * SOME NOTES...
 * -----------------------------------
 * 1)	I do lots of memory allocation in this, but never bothered to deallocate,
 * so the whole thing is a big memory leak.  Sue me!
 *
 * 2)	I use "goto"s twice in this code.  Sue me again!!!  If you've ever been
 * told never to use goto, that was sound advice, but it's kind of like being
 * told never to share a hat or comb... you all know what I mean (wink).
 * Besides, I think when you see the way I used them, it will feel very natural
 * and clean... just like sharing a hat or comb with a close friend.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define KEYWORD_LENGTH 10
#define KEYWORD_COUNT 2

typedef struct Token Token;
typedef struct Keyword Keyword;

typedef enum {
	TOKEN_BAD = -1,
	TOKEN_NONE,

	TOKEN_IF,
	TOKEN_WHILE,
	TOKEN_BITS,
	TOKEN_OPEN_TAG,
	TOKEN_CLOSE_TAG,
	TOKEN_ASSIGN,
	TOKEN_ALTERNATE,
	TOKEN_QUESTION,
	TOKEN_ID,

	TOKEN_COUNT

} token_type;

struct Token {
	int type;
	int length;
	char *string;
};

struct Keyword {
	char string[KEYWORD_LENGTH];
	token_type type;
};

const Keyword keyword_table[KEYWORD_COUNT] = {
		{"if", TOKEN_IF},
		{"while", TOKEN_WHILE}
};

void load_source(char **buffer, long *file_length, char *filename);
Token get_token(char **buffer);
int *signextend(Token);
void makeunsigned(int*);
void makesigned(int*);
void make1s(int*);
void make2s(int*);

int main(int argc, char *argv[]) {
	char *file;  // so I call it file... sue me!!!
	long length;
	int token_count, *value, i;
	Token t;

	/* check file parameter count */
	if(argc != 2){
		fprintf(stderr, "Usage:\t%s <source_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	load_source(&file, &length, argv[1]);

	token_count = 0;

	do {
		t = get_token(&file);
		/*if(t.type == TOKEN_BAD) {
			fprintf(stderr, "Unexpected character, '%s'\n", t.string);
		}*/
		if (t.type == TOKEN_BITS && t.length <= 8)
		{
			value = signextend(t);
			makeunsigned(value);
			makesigned(value);
			make1s(value);
			make2s(value);
		}
		else { //if(t.type != TOKEN_NONE) {
			token_count++;
			//printf("%3d type %2d: %s\n", token_count, t.type, t.string);
		}
	} while(*file != '\0');

	return 0;
}



void load_source(char **buffer, long *file_length, char *filename) {
	FILE *f;

	/* attempt to open file */
	f = fopen(filename, "rb");
	if(f == NULL) {
		fprintf(stderr, "Unable to open input file, \"%s\"\n", filename);
		exit(EXIT_FAILURE);
	}

	/*
	 * allocate a buffer big enough for the file
	 */
	fseek(f, 0, SEEK_END);
	*file_length = ftell(f);
	rewind(f);
	*buffer = (char *)malloc(*file_length + 1);
	if(*buffer == NULL) {
		fprintf(stderr, "Could not allocate file buffer\n");
		fclose(f);
		exit(EXIT_FAILURE);
	}

	/* read file as a single object */
	if(fread(*buffer, *file_length, 1, f) != 1) {
		fprintf(stderr, "Trouble reading the file into the buffer\n");
		fclose(f);
		exit(EXIT_FAILURE);
	}

	/* NULL terminate the buffer */
	*(*buffer + *file_length) = 0;

	return;
}



Token get_token(char **buffer) {
	Token t;
	char *b = *buffer;
	int i;
	char c;

	t.type = TOKEN_NONE;
	t.string = NULL;
	t.length = 0;

	/*
	 * First remove leading whitespace
	 */
trim:
	switch(*b) {
	case ' ':
	case '\b':
	case '\f':
	case '\n':
	case '\r':
	case '\t':
	case '\v':
		b++;
		*buffer = b;
		goto trim;
		break;
	default:
		break;
	}

	/* watch out for end of file */
	if(*b == '\0') {
		return t;
	}

	/*
	 * Okay... so something is here.  Could be good.  Could be bad.
	 */
	switch(*b) {

	/* could be some bits */
	case '0':
	case '1':
bits:
		t.length++;
		switch(*(b + t.length))
		{
		case '0':
		case '1':
			if(t.length < 8)
			{
			goto bits;
			break;
			}
		default:
			break;
		}

		t.type = TOKEN_BITS;
		// t.length = uh... already calculated :-) ;
		t.string = (char *)calloc(t.length + 1, sizeof(char));
		strncpy(t.string, b, t.length);
		//*(b + t.length) = '\0';
		break;

	case '<':
		t.type = TOKEN_OPEN_TAG;
		t.length = 1;
		t.string = (char *)malloc(sizeof(char) * (t.length + 1));
		sprintf(t.string, "<");
		break;

	case '>':
		t.type = TOKEN_CLOSE_TAG;
		t.length = 1;
		t.string = (char *)malloc(sizeof(char) * (t.length + 1));
		sprintf(t.string, ">");
		break;

	case ':':
		/*
		 * could be question, assign, or alternate
		 */
		switch(*(b + 1)) {
		case ':':
			t.type = TOKEN_ALTERNATE;
			t.length = 2;
			t.string = (char *)malloc(sizeof(char) * (t.length + 1));
			sprintf(t.string, "::");
			break;

		case '=':
			t.type = TOKEN_ASSIGN;
			t.length = 2;
			t.string = (char *)malloc(sizeof(char) * (t.length + 1));
			sprintf(t.string, ":=");
			break;

		default:
			t.type = TOKEN_QUESTION;
			t.length = 1;
			t.string = (char *)malloc(sizeof(char) * (t.length + 1));
			sprintf(t.string, ":");
			break;
		}
		break;

	case '{':
		i = 0;
		do {
			i++;
			c = *(b + i);
		} while(c != '}' && c != '\0');

		t.length = i + 1;
		t.string = (char *)calloc(t.length + 1, sizeof(char));
		strncpy(t.string, b, t.length);

		if(c == '}') {
			t.type = TOKEN_NONE;
		}
		else {
			t.type = TOKEN_BAD;
		}

		break;

	default:
		if(!isalpha(*b)) {
			t.type = TOKEN_BAD;
			t.length = 1 ;
			t.string = (char *)malloc(sizeof(char) * (t.length + 1));
			sprintf(t.string, "%c", *b);
			break;
		}

		while(isalpha(*(b + t.length))) {
			t.length++;
		}

		t.type = TOKEN_ID;
		// t.length = uh... already calculated :-) ;
		t.string = (char *)calloc(t.length + 1, sizeof(char));
		strncpy(t.string, b, t.length);

		/* check to see if it was a keyword */

		for(i = 0; i < KEYWORD_COUNT; i++) {
			if(strcmp(t.string, keyword_table[i].string)) {
				t.type = keyword_table[i].type;
				break;
			}
		}

		break;
	}

	b += t.length;
	*buffer = b;

	return t;
}

int *signextend(Token t)
{
	int i=0, r=0, j=7, diff;
	int *fliparr = malloc(8*sizeof(int));
	int binarr[8] = {0};
	if (t.length < 8)
	{
		diff = 8-t.length;
		if (t.string[0]=='0')
		{
			for (i=0; i<diff; i++)
			{
				binarr[i]=0;
			}
		}
		if (t.string[0]=='1')
		{
			for (i=0; i<diff; i++)
			{
				binarr[i]=1;
			}
		}
	
	}
	while(i<8)
	{
		if (t.string[r] == '1')
			binarr[i]=1;
		else if (t.string[r]=='0')
			binarr[i]=0;
		i++;
		r++;
	}
	for (i=0; i<8; i++)
	{
		fliparr[j]=binarr[i];
		j--;
	}
	return fliparr;
}

void makeunsigned(int *val)
{
	int total, i;
	total=0;
	for (i=8; i>-1; i--)
	{
		if (val[i]==1)
		total += 1 << i;
	}
	printf("%d ", total);
}

void makesigned(int *val)
{
	int i, total=0;
	if (val[7]==1)
		printf("-");
	for(i=0; i<7; i++) 
	{
		if (val[i]==1)
			total+= 1<< i;
	}
	printf("%d ",total);
}

void make1s(int *val)
{
	int i=0, total=0;
	if (val[7]==1)
	{
		printf("-");
		for(i=0; i<8; i++) 
		{
			if (val[i]==0)
			total+= 1<< i;
		}
	}
	if (val[7]==0)
	{
		for(i=0; i<8; i++) 
		{
			if (val[i]==1)
				total+= 1<< i;
		}
	}
	printf("%d ",total);
}

void make2s(int *val)
{
	int i=0, total=0;
	if (val[7]==1)
	{
		printf("-");
		for(i=0; i<8; i++) 
		{
			if (val[i]==0)
			total+= 1<< i;
		}
		total+= 1<<0;
	}
	if (val[7]==0)
	{
		for(i=0; i<7; i++) 
		{
			if (val[i]==1)
				total+= 1<< i;
		}
	}
	printf("%d\n",total);
}
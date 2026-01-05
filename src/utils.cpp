#include "utils.h"
// #include <process.h>
// #include "graphics.h"
// #include <graphics.h>

using std::cerr;
using std::cout;

void error(const char* s1, const char* s2)
{
	// closegraph();
	if (s1) {
		if (s2) {
			cerr << s1 << ' ' << s2;
		}
		else {
			cerr << s1;
		}
	}

	exit(0);
}

int read_word(ifstream& f, char* word)
{
	int finish = 0;
	int length = 0;
	char c;

	bzero(word, MAX_LINE);
	while ((!finish) && (!f.eof()) && (length < MAX_LINE)) {
		f >> c;
		// DBG(STR("read_word: ", 1) << c);
		if ((c <= 32) || (c == ',') || (c == '(') || (c == ')') || (c == '{') || (c == '}')) {
			finish = 1;
		}
		else {
			word[length++] = c;
		}
	}
	word[length] = '\0';
	// DBG(STR("read_word: ", 1) << word, DEC(length, 4));

	return length;
}

void read_remark(ifstream&)
{
}

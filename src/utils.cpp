#include "common.h"
#include "utils.h"

system_error sys_error(const char* s1, const char* s2)
{
	std::string msg = std::string(s1);
	if (s2) {
		msg += PFX;
		msg += s2;
	}

	throw system_error(msg);
}

int read_word(std::ifstream& ifs, char* word)
{
	bool finish = false;
	int length = 0;
	char c;

	bzero(word, MAX_LINE);
	while ((!finish) && (!ifs.eof()) && (length < MAX_LINE)) {
		ifs >> c;
		// DBG(STR("read_word: ", 1) << c);
		if ((c <= 32) || (c == ',') || (c == '(') || (c == ')') || (c == '{') || (c == '}')) {
			finish = true;
		}
		else {
			word[length++] = c;
		}
	}
	word[length] = '\0';
	// DBG(STR("read_word: ", 1) << word, DEC(length, 4));

	return length;
}

void read_remark(std::ifstream&)
{
}

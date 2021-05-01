

#include "mimalloc-new-delete.h"

#include <iostream>
#include <string_view>

#include "clau_parser.h"

inline std::string_view toStringView(long long token, const char* buffer) {
	return std::string_view(buffer + clau_parser::Utility::GetIdx(token), 
		clau_parser::Utility::GetLength(token));
}
inline std::string toString(long long token, const char* buffer) {
	return std::string(buffer + clau_parser::Utility::GetIdx(token),
		clau_parser::Utility::GetLength(token));
}

inline bool match(long long token, const char* buffer, std::string_view str) {
	return toStringView(token, buffer) == str;
}

void G(const char* buffer, long long* tokens, long long tokens_size, long long idx, long long var, long long val, clau_parser::Maker* m) {
	if (tokens_size <= idx) {
		return;
	}
	if (match(tokens[idx], buffer, "{"sv)) {
		if (var) {
			m->NewGroup(toString(var, buffer));
			var = 0;
		}
		else {
			m->NewGroup("");
		}
		val = 0;
		return G(buffer, tokens, tokens_size, idx + 1, var, val, m);
	}

	if (match(tokens[idx], buffer, "}"sv)) {
		m->EndGroup();
		return G(buffer, tokens, tokens_size, idx + 1, var, val, m);
	}

	if (idx + 1 < tokens_size) {
		if (match(tokens[idx + 1], buffer, "="sv)) {
			var = tokens[idx];
			val = 0;
			return G(buffer, tokens, tokens_size, idx + 2, var, val, m);
		}
	}
	
	val = tokens[idx];

	if (var) {
		m->NewItem(toString(var, buffer), toString(val, buffer));
	}
	else {
		m->NewItem("", toString(val, buffer));
	}
	var = 0;
	val = 0;
	return G(buffer, tokens, tokens_size, idx + 1, var, val, m);
}


int main(void) {
	FILE* file;
	
	fopen_s(&file, "input.eu4", "rb");

	clau_parser::InFileReserver test(file, false);

	char* buffer;
	size_t buffer_len;
	long long* tokens;
	size_t tokens_len;

	test(8, buffer, &buffer_len, tokens, &tokens_len);
	clau_parser::Maker m;
	int a = clock();
	G(buffer, tokens, tokens_len, 0, 0, 0, &m);
	int b = clock();
	std::cout << b - a << "ms\n";
	return 0;
}


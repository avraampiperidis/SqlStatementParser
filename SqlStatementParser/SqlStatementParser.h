// SqlStatementParser.h
#pragma once

#include <Windows.h>
#include <vector>
#include <utility>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <list>
using namespace System;
using namespace std;

namespace SqlStatementParser {

	public ref class SqlStatementParser
	{
		private:
			bool isLineBreak(const unsigned char* head, const unsigned char* line_break);
			const unsigned char* skip_leading_whitespace(const unsigned char* head, const unsigned char* tail);
		public:
			void determineStatementRanges(const char* sql, size_t length, const std::string& initial_delimiter,
				std::vector<std::pair<size_t, size_t> >& ranges
				, const std::string& line_break);

	};

}

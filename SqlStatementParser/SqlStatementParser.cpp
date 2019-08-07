// This is the main DLL file.

#include "stdafx.h"
#include <vector>
#include <utility>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <list>
#include "SqlStatementParser.h"

using namespace std;

bool SqlStatementParser::SqlStatementParser::isLineBreak(const unsigned char * head, const unsigned char * line_break)
{
	if (*line_break == '\0')
		return false;

	while (*head != '\0' && *line_break != '\0' && *head == *line_break) {
		head++;
		line_break++;
	}
	return *line_break == '\0';
}

const unsigned char * SqlStatementParser::SqlStatementParser::skip_leading_whitespace(const unsigned char * head, const unsigned char * tail)
{
	while (head < tail && *head <= ' ')
		head++;
	return head;
}


void SqlStatementParser::SqlStatementParser::determineStatementRanges(const char * sql, size_t length, const std::string & initial_delimiter,
	std::vector<std::pair<size_t, size_t>>& ranges, const std::string & line_break)
{
	bool _stop = false;
	string delimiter = initial_delimiter.empty() ? ";" : initial_delimiter;
	const unsigned char* delimiter_head = (unsigned char*)delimiter.c_str();

	const unsigned char keyword[] = "delimiter";

	const unsigned char* head = (unsigned char*)sql;
	const unsigned char* tail = head;
	const unsigned char* end = head + length;
	const unsigned char* new_line = (unsigned char*)line_break.c_str();
	bool have_content = false; // Set when anything else but comments were found for the current statement.

	while (!_stop && tail < end)
	{
		switch (*tail)
		{
		case '/': // Possible multi line comment or hidden (conditional) command.
			if (*(tail + 1) == '*')
			{
				tail += 2;
				bool is_hidden_command = (*tail == '!');
				while (true)
				{
					while (tail < end && *tail != '*')
						tail++;
					if (tail == end) // Unfinished comment.
						break;
					else
					{
						if (*++tail == '/')
						{
							tail++; // Skip the slash too.
							break;
						}
					}
				}

				if (!is_hidden_command && !have_content)
					head = tail; // Skip over the comment.
			}
			else
				tail++;

			break;

		case '-': // Possible single line comment.
		{
			const unsigned char* end_char = tail + 2;
			if (*(tail + 1) == '-' && (*end_char == ' ' || *end_char == '\t' || isLineBreak(end_char, new_line)))
			{
				// Skip everything until the end of the line.
				tail += 2;
				while (tail < end && !isLineBreak(tail, new_line))
					tail++;
				if (!have_content)
					head = tail;
			}
			else
				tail++;

			break;
		}

		case '#': // MySQL single line comment.
			while (tail < end && !isLineBreak(tail, new_line))
				tail++;
			if (!have_content)
				head = tail;
			break;

		case '"':
		case '\'':
		case '`': // Quoted string/id. Skip this in a local loop.
		{
			have_content = true;
			char quote = *tail++;
			while (tail < end && *tail != quote)
			{
				// Skip any escaped character too.
				if (*tail == '\\')
					tail++;
				tail++;
			}
			if (*tail == quote)
				tail++; // Skip trailing quote char to if one was there.

			break;
		}
		default:
			if (*tail > ' ')
				have_content = true;
			tail++;
			break;
		}

		if (*tail == *delimiter_head)
		{
			// Found possible start of the delimiter. Check if it really is.
			size_t count = delimiter.size();
			if (count == 1)
			{
				// Most common case. Trim the statement and check if it is not empty before adding the range.
				head = skip_leading_whitespace(head, tail);
				if (head < tail)
					ranges.push_back(std::make_pair<size_t, size_t>(head - (unsigned char*)sql, tail - head));
				head = ++tail;
				have_content = false;
			}
			else
			{
				const unsigned char* run = tail + 1;
				const unsigned char* del = delimiter_head + 1;
				while (count-- > 1 && (*run++ == *del++))
					;

				if (count == 0)
				{
					// Multi char delimiter is complete. Tail still points to the start of the delimiter.
					// Run points to the first character after the delimiter.
					head = skip_leading_whitespace(head, tail);
					if (head < tail)
						ranges.push_back(std::make_pair<size_t, size_t>(head - (unsigned char*)sql, tail - head));
					tail = run;
					head = run;
					have_content = false;
				}
			}
		}
	}

	// Add remaining text to the range list.
	head = skip_leading_whitespace(head, tail);
	if (head < tail)
		ranges.push_back(std::make_pair<size_t, size_t>(head - (unsigned char*)sql, tail - head));
}

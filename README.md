#### C++ code to determine the ranges of statements of a SQL string. <br>
you can either copy the SqlStatementParser.cpp and SqlStatementParser.h files to use it directly.<br>
Or build and add the exported dll into your project.<br>
Notice. the dll file will work only for c++ projects as it's api methods using std direct types <br>
###### use example
```c++
int main()
{
	std::vector<std::pair<size_t, size_t>> ranges;
	std::string sql = "select * from mytable; create table2(`id` not null /* ;ignored semicolon;*/ );\n \
		select * from table1 where t.prop like 'commnet;' and id = 3 #trailing; comment;;;;;";
	determineStatementRanges(sql.c_str(), sql.size(), ";", ranges, "\n");

	for (auto &range : ranges) {
		std::cout << '\n';
		std::cout << sql.substr(range.first, range.second);
		std::cout << '\n';
	}
	std::cin.ignore();
    return 0;
}
```

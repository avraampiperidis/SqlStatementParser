#### C++ code to determine the ranges of statements of a SQL string. <br>
I found and copied the code from MySql workbench opensource project from github.<br>
I Needed this functionality as standalone code and couldnt find anything similar. <br>
So , you can either copy the SqlStatementParser.cpp and SqlStatementParser.h files to use it directly.<br>
Or build and add the exported dll into your project.<br>
Notice. the dll file will work only for c++ projects as it's api methods using std direct types

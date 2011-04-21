
/* The 'u8', 'u', and 'U' prefixes */
const char     s1 = u8"I'm a UTF-8 string.";
const char16_t s2 = u"This is a UTF-16 string.";
const char32_t s3 = U"This is a UTF-32 string.";

/* The 'R' and 'R"delim(' prefixes */
const char     r1 = R"(Xhe String Data \ Stuff " )";
const char     r2 = R"delimiter(The String Data \ Stuff ")delimiter";

/* Combo */
const char     c1 = u8R"XXX(I'm a "raw UTF-8" string.)XXX";
const char16_t c2 = uR"*(This is a "raw UTF-16" string.)*";
const char32_t c3 = UR"(This is a "raw UTF-32" string.)";

/* user-defined */
OutputType operator "" _Suffix(unsigned long long);
OutputType operator "" _Suffix(long double);

OutputType some_variable    = 1234_Suffix;   // uses the first function
OutputType another_variable = 3.1416_Suffix; // uses the second function

OutputType operator "" _Suffix(const char * string_values, size_t num_chars);
OutputType operator "" _Suffix(const wchar_t * string_values, size_t num_chars);
OutputType operator "" _Suffix(const char16_t * string_values, size_t num_chars);
OutputType operator "" _Suffix(const char32_t * string_values, size_t num_chars);

OutputType some_variable = "1234"_Suffix;      //Calls the const char * version
OutputType some_variable = u8"1234"_Suffix;    //Calls the const char * version
OutputType some_variable = L"1234"_Suffix;     //Calls the const wchar_t * version
OutputType some_variable = u"1234"_Suffix;     //Calls the const char16_t * version
OutputType some_variable = U"1234"_Suffix;     //Calls the const char32_t * version


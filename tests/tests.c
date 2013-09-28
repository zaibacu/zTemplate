#include "../src/zCore.h"
#include "../src/Util.h"
#include "../src/Parameter.h"
#include "../src/Regex.h"

bool seek_test()
{
	printf("Test: %s - ", __FUNCTION__);
	const zString szSource = "This is a test string with $$ near the end";
	return seek(szSource, "$$", 0) == 27;
}

bool render_basic_test()
{
	printf("Test: %s - ", __FUNCTION__);

	struct Param param1;
	param1.m_szKey = "word";
	param1.m_pVal = &(struct Value){ 1, (void*)&(struct StringValue){"world"} };
	param1.m_pNext = NULL;

	struct Param param2;
	param2.m_szKey = "greeting";
	param2.m_pVal = &(struct Value){ 1, (void*)&(struct StringValue){"Hello"} }; 
	param2.m_pNext = NULL;

	param1.m_pNext = &param2;


	zString szResult1 = read_file("test_tmpl1_result.html");
	zString szResult2 = render("test_tmpl1.html", &param1);
	bool bReturn = strcmp(szResult1, szResult2) == 0;
	free(szResult1);
	free(szResult2);
	return bReturn;
}

bool render_basic_unicode_test()
{
	printf("Test: %s - ", __FUNCTION__);

	struct Param param1;
	param1.m_szKey = "random_letters";
	param1.m_pVal = &(struct Value){ 1, (void*)&(struct StringValue){"ąčęėįšųūž„“"} };
	param1.m_pNext = NULL;


	zString szResult1 = read_file("unicode_test_tmpl1_result.html");
	zString szResult2 = render("unicode_test_tmpl1.html", &param1);
	bool bReturn = strcmp(szResult1, szResult2) == 0;
	free(szResult1);
	free(szResult2);
	return bReturn;
}

bool render_include_test()
{
	printf("Test: %s - ", __FUNCTION__);
	zString szResult1 = read_file("test_tmpl2_result.html");
	zString szResult2 = render("test_tmpl2.html", NULL);
	bool bReturn = strcmp(szResult1, szResult2) == 0;
	free(szResult1);
	free(szResult2);
	return bReturn;
}

bool render_show_block_test()
{
	printf("Test: %s - ", __FUNCTION__);
	struct Param param1;
	param1.m_szKey = "show_me";
	param1.m_pVal = &(struct Value){ 2, (void*)&(struct BoolValue){true} };
	param1.m_pNext = NULL;

	struct Param param2;
	param2.m_szKey = "hide_me";
	param2.m_pVal = &(struct Value){ 2, (void*)&(struct BoolValue){false} };
	param2.m_pNext = NULL;

	param1.m_pNext = &param2;

	zString szResult1 = read_file("test_tmpl3_result.html");
	zString szResult2 = render("test_tmpl3.html", &param1);
	bool bReturn = strcmp(szResult1, szResult2) == 0;
	free(szResult1);
	free(szResult2);
	return bReturn;
}

bool render_foreach_block_test()
{
	printf("Test: %s - ", __FUNCTION__);
	struct Param param1;
	param1.m_szKey = "numbers";
	struct ListValue ListVal1 = (struct ListValue)
									{
										&(struct Value){3, (void*)&(struct NumberValue){1}}, NULL
									};

	struct ListValue ListVal2 = (struct ListValue)
									{
										&(struct Value){3, (void*)&(struct NumberValue){2}}, NULL
									};

	struct ListValue ListVal3 = (struct ListValue)
									{
										&(struct Value){3, (void*)&(struct NumberValue){3}}, NULL
									};

	//Chain them
	ListVal1.m_pNext = &ListVal2;
	ListVal2.m_pNext = &ListVal3;
	param1.m_pVal = &(struct Value){ 4, (void*)&ListVal1};
	param1.m_pNext = NULL;

	zString szResult1 = read_file("test_tmpl4_result.html");
	zString szResult2 = render("test_tmpl4.html", &param1);
	bool bReturn = strcmp(szResult1, szResult2) == 0;
	free(szResult1);
	free(szResult2);
	return bReturn;
}

bool trim_test()
{
	printf("Test: %s - ", __FUNCTION__);
	zString szText = "               hello             ";
	zString szResult = trim(szText);
	bool bReturn = strcmp(szResult, "hello") == 0;
	free(szResult);
	return bReturn;
}

bool read_file_test()
{
	printf("Test: %s - ", __FUNCTION__);
	zString szResult = read_file("test_tmpl1.html");
	free(szResult);
	return true;
}

bool read_file_unicode_test()
{
	printf("Test: %s - ", __FUNCTION__);
	zString szResult = read_file("unicode_test_tmpl1.html");
	free(szResult);
	return true;
}

bool str_insert_test()
{
	printf("Test: %s - ", __FUNCTION__);
	char szStr1[] = "Hello world!";
	zString szStr2 = " beautiful ";
	str_insert(szStr1, szStr2, 5, 6);
	return strcmp(szStr1, "Hello beautiful world!") == 0;
}

bool regex_basic_test()
{
	printf("Test: %s - ", __FUNCTION__);
	zString szResult = regex("$\\w+\\s?\\d*", "<< $Hello1 23 >>");
	bool bReturn = strcmp(szResult, "$Hello1 23") == 0;
	free(szResult);
	return bReturn;
}

bool regex_class_test()
{
	printf("Test: %s - ", __FUNCTION__);
	zString szResult = regex("[abcd]+e", "bcae");
	bool bReturn = strcmp(szResult, "bcae") == 0;
	free(szResult);
	return bReturn;
}

void launch_test(bool (*f)(void))
{
	if((*f)() == true)
		printf("Success\n");
	else
		printf("Fail\n");
}

int main()
{
	printf("Running tests:\n");
	launch_test(seek_test);
	launch_test(trim_test);
	launch_test(read_file_test);
	launch_test(read_file_unicode_test);
	launch_test(render_basic_test);
	launch_test(render_basic_unicode_test);
	launch_test(render_include_test);
	launch_test(render_show_block_test);
	launch_test(render_foreach_block_test);
	launch_test(str_insert_test);
	launch_test(regex_basic_test);
	launch_test(regex_class_test);
	return 0;
}
#include "../src/zCore.h"
#include "../src/Util.h"

bool seek_test()
{
	printf("Test: %s - ", __FUNCTION__);
	const zString szSource = "This is a test string with $$ near the end";
	return seek(szSource, "$$", 0) == 28;
}

bool render_basic_test()
{
	printf("Test: %s - ", __FUNCTION__);

	struct Param param1;
	param1.m_szKey = "word";
	param1.m_Val = (struct Value){ 1, (void*)&(struct StringValue){"world"} };
	param1.m_pNext = NULL;

	struct Param param2;
	param2.m_szKey = "greeting";
	param2.m_Val = (struct Value){ 1, (void*)&(struct StringValue){"Hello"} }; 
	param2.m_pNext = NULL;

	param1.m_pNext = &param2;


	zString szResult1 = read_file("test_tmpl1_result.html");
	zString szResult2 = render("test_tmpl1.html", &param1);
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
	zString szResult = read_file("tests/test_tmpl1_result.html");
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
	launch_test(render_basic_test);
	launch_test(render_include_test);
	launch_test(str_insert_test);
	return 0;
}
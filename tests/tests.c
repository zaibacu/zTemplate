#include "../src/zCore.h"
#include "../src/Util.h"

BOOL seek_test()
{
	printf("Test: %s - ", __FUNCTION__);
	const zString szSource = "This is a test string with $$ near the end";
	return seek(szSource, "$$", 0) == 28;
}

BOOL render_basic_test()
{
	printf("Test: %s - ", __FUNCTION__);

	struct Param param1;
	param1.m_szKey = "word";
	param1.m_szVal = "world";
	param1.m_pNext = NULL;

	struct Param param2;
	param2.m_szKey = "greeting";
	param2.m_szVal = "Hello";
	param2.m_pNext = NULL;

	param1.m_pNext = &param2;


	zString szResult1 = read_file("test_tmpl1_result.html");
	zString szResult2 = render("test_tmpl1.html", &param1);

	BOOL bReturn = strcmp(szResult1, szResult2) == 0;
	free(szResult1);
	free(szResult2);
	return bReturn;
}

BOOL render_include_test()
{
	printf("Test: %s - ", __FUNCTION__);

	zString szResult1 = read_file("test_tmpl2_result.html");
	zString szResult2 = render("test_tmpl2.html", NULL);

	BOOL bReturn = strcmp(szResult1, szResult2) == 0;
	free(szResult1);
	free(szResult2);
	return bReturn;
}

BOOL trim_test()
{
	printf("Test: %s - ", __FUNCTION__);
	zString szText = "               hello             ";
	zString szResult = trim(szText);
	BOOL bReturn = strcmp(szResult, "hello") == 0;
	free(szResult);
	return bReturn;
}

BOOL read_file_test()
{
	printf("Test: %s - ", __FUNCTION__);
	zString szResult = read_file("tests/test_tmpl1_result.html");
	free(szResult);
	return TRUE;
}

void launch_test(BOOL (*f)(void))
{
	if((*f)() == TRUE)
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
	return 0;
}
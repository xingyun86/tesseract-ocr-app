// ocr-app.cpp : Defines the entry point for the application.
//
#include <windows.h>
#include <stdio.h>
typedef int (__stdcall *pfn_ocr_get_string)(char* p_utf8_data, int n_utf8_size, const char* p_file_name, const char* p_root_path);
int main(int argc, char** argv)
{
	HMODULE hModule = LoadLibraryA(("tesseract-ocr-demo_dll.dll"));
	if (hModule != nullptr)
	{
		pfn_ocr_get_string fn_ocr_get_string = (pfn_ocr_get_string)GetProcAddress(hModule, ("ocr_get_string"));
		if (fn_ocr_get_string != nullptr)
		{
			char text[1024] = { 0 };
			fn_ocr_get_string(text, sizeof(text)/sizeof(*text), "test.png", nullptr);
			printf("result:\n%s\n", text);
		}
		FreeLibrary(hModule);
	}
	return 0;
}
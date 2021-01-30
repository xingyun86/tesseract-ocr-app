// tesseract-ocr-demo.cpp : Defines the entry point for the application.
//

#include "tesseract-ocr-demo.h"

#include <tesseract/baseapi.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif // _MSC_VER
#ifndef BUILD_DLL_LIBRARY
void dealImg(const char* path);
__inline static
int merge_test(const std::string& imgName)
{
    std::vector<cv::Mat> channels = {};
    cv::Mat im_src = cv::imread(imgName, cv::IMREAD_UNCHANGED);
    //src为要分离的Mat对象
    cv::split(im_src, channels);               //利用vector对象分离
    cv::Mat blank_ch = cv::Mat::ones(cv::Size(im_src.cols, im_src.rows), CV_8UC1);
    std::vector<cv::Mat> channels_r;
    channels_r.push_back(channels[2]);//B
    channels_r.push_back(channels[1]);//G
    channels_r.push_back(channels[0]);//R
    channels_r.push_back(blank_ch);//A
    cv::Mat fin_img;
    cv::merge(channels_r, fin_img);
    cv::imshow("R", fin_img);
    cv::waitKey();
}

int main(int argc, char** argv)
{
#ifdef _MSC_VER
    std::string root_path(MAX_PATH, '\0');
    root_path.resize(GetModuleFileNameA(NULL, (LPSTR)root_path.data(), root_path.size()));
    root_path = root_path.substr(0, root_path.rfind('\\'));
#endif // _MSC_VER

    std::string outText;
    std::string imPath = "sfzfm.jpg";
    //imPath = "test.png";
    imPath = "test.jpg";
    imPath = "bbb.png";
    imPath = "aaa.jpg";
    imPath = "zzz.jpg";
    imPath = "test.jpg";
    if (argc > 1)
    {
        imPath = argv[1];
    }
    std::string languagePath = root_path + "\\tessdata";
    std::string languageType = "chi_sim+eng";
    // Create Tesseract object
    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
    /*
      Initialize OCR engine to use English (eng) and The LSTM
      OCR engine.


      There are four OCR Engine Mode (oem) available

      OEM_TESSERACT_ONLY             Legacy engine only.
      OEM_LSTM_ONLY                  Neural nets LSTM engine only.
      OEM_TESSERACT_LSTM_COMBINED    Legacy + LSTM engines.
      OEM_DEFAULT                    Default, based on what is available.
     */
     // 初始化
    int nRet = ocr->Init(languagePath.c_str(), languageType.c_str(), tesseract::OEM_DEFAULT);
    if (nRet != 0) {
        printf("TessBaseAPI init failed！");
        return -1;
    }

    // Set Page segmentation mode to PSM_AUTO (3)
    // Other important psm modes will be discussed in a future post.
    ocr->SetPageSegMode(tesseract::PSM_AUTO);

    // Open input image using OpenCV
    cv::Mat im_src = cv::imread(imPath, cv::IMREAD_UNCHANGED);
    {
        cv::Mat img_mask = {};
        std::vector<uchar> img_out = {};
        cv::Mat img_tmp = cv::Mat(im_src.size(), im_src.type(), cv::Scalar(255, 255, 255));
        cv::inRange(im_src, cv::Scalar(0, 0, 0, 255), cv::Scalar(255, 255, 255, 255), img_mask);
        im_src.copyTo(img_tmp, img_mask);
        cv::imencode(".jpg", img_tmp, img_out);
        cv::imdecode(img_out, cv::IMREAD_UNCHANGED, &im_src);
    }
    // Set image data
    ocr->SetImage(im_src.data, im_src.cols, im_src.rows, 3, im_src.step);
    // Run Tesseract OCR on image
    outText = std::string(ocr->GetUTF8Text());

    // print recognized text
    std::cout << outText << std::endl;

    // Destroy used object and release memory
    ocr->End();
    delete ocr;

    return 0;
}

void dealImg(const char* path)
{
    cv::Mat src = cv::imread(path);
    // 结果图
    cv::Mat dst;
    // 显示原图
    cv::imshow("原图", src);

    cv::cvtColor(src, dst, cv::COLOR_RGB2GRAY);
    // 高斯模糊，主要用于降噪
    cv::GaussianBlur(dst, dst, cv::Size(3, 3), 0);
    cv::imshow("GaussianBlur图", dst);
    // 二值化图，主要将灰色部分转成白色，使内容为黑色
    cv::threshold(dst, dst, 165, 255, cv::THRESH_BINARY);
    cv::imshow("threshold图", dst);
    // 中值滤波，同样用于降噪
    cv::medianBlur(dst, dst, 3);
    cv::imshow("medianBlur图", dst);
    // 腐蚀操作，主要将内容部分向高亮部分腐蚀，使得内容连接，方便最终区域选取
    cv::erode(dst, dst, cv::Mat(9, 9, CV_8U));
    cv::imshow("erode图", dst);

    //定义变量
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(dst, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat result;

    for (int i = 0; i < hierarchy.size(); i++)
    {
        cv::Rect rect = cv::boundingRect(contours.at(i));
        cv::rectangle(src, rect, cv::Scalar(255, 0, 255));
        // 定义身份证号位置大于图片的一半，并且宽度是高度的6倍以上
        if (rect.y > src.rows / 2 && rect.width / rect.height > 6)
        {
            result = src(rect);
            cv::imshow("身份证号", result);
        }
    }

    cv::imshow("轮廓图", src);
    cv::waitKey();
}
#else
#pragma comment(linker,"/NOENTRY") 
#define TESSERACT_OCR_MODULE "tesseract-ocr-demo_dll"
#define MACRO_0 "\x9C\x97\x96\xA0\x8C\x96\x92\xD1\x8B\x8D\x9E\x96\x91\x9A\x9B\x9B\x9E\x8B\x9E\x00" //[chi_sim.traineddata]
#define MACRO_1 "\x9A\x91\x98\xD1\x8B\x8D\x9E\x96\x91\x9A\x9B\x9B\x9E\x8B\x9E\x00" //[eng.traineddata]
#define MACRO_2 "\xAB\xBA\xAC\xAC\xBB\xBE\xAB\xBE\x00" //[TESSDATA]
#define MACRO_3 "\x8A\xD1\x9A\x87\x9A\xDF\x87\xDF\xD2\x96\x91\x8A\x93\xDF\xD2\x90\xD4\xDF\xD2\x8F\x8B\x9A\x8C\x8C\x9B\x9E\x8B\x9E\xDF\x00" //[u.exe x -inul -o+ -ptessdata]
#define MACRO_4 "\xBC\xC5\xA3\xA8\x96\x91\x9B\x90\x88\x8C\xA3\xAC\x86\x8C\x8B\x9A\x92\xCC\xCD\xA3\x9C\x92\x9B\xD1\x9A\x87\x9A\xDF\xD0\xBC\xDF\xBB\xBA\xB3\xDF\xD0\xAC\xDF\xD0\xAE\xDF\xDD\x00" //[C:\Windows\System32\cmd.exe /C DEL /S /Q "]
#define MACRO_5 "\x8A\xD1\x9A\x87\x9A\xDD\x00" //[u.exe"]
#define MACRO_6 "\x8D\xD1\x8D\x9E\x8D\x00" //[r.rar]
#define MACRO_7 "\x8D\xD1\x8D\x9E\x8D\xDF\x00" //[r.rar ]
#define MACRO_8 "\xD5\xDD\x00" //[*"]
#define MACRO_9 "\x8A\xD1\x9A\x87\x9A\x00" //[u.exe]
std::string xor (const std::string& str)
{
    //printf("[%s]\n", str.c_str());
    std::string s(str.begin(), str.end());
    for (auto& c : s)
    {
        c ^= 0xFF;
        //printf("\\x%02X", (uint8_t)c);
    }
    //printf("\n");
    return s;
}
#define FILE_READER(F, M) std::string((std::istreambuf_iterator<char>(std::ifstream(F, M | std::ios::in).rdbuf())), std::istreambuf_iterator<char>())
#define FILE_WRITER(D, S, F, M) std::ofstream(F, M | std::ios::out).write((D), (S))

__inline static
BOOL ExecuteCommand(LPCSTR lpCmdLine)
{
    //printf("%s\n", xor("C:\\Windows\\System32\\cmd.exe /C DEL /S /Q \""));
    BOOL bRet = FALSE;
    DWORD dwExitCode = 0;
    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    GetStartupInfoA(&si);
    si.hStdError = NULL;
    si.hStdOutput = NULL;
    si.lpReserved = NULL;
    si.lpDesktop = NULL;
    si.lpTitle = NULL;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.cbReserved2 = NULL;
    si.lpReserved2 = NULL;
    ZeroMemory(&pi, sizeof(pi));
    if (CreateProcessA(NULL,   // No module name (use command line)
        (LPSTR)lpCmdLine,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,           // Pointer to STARTUPINFO structure
        &pi            // Pointer to PROCESS_INFORMATION structure
    ))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, &dwExitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        bRet = TRUE;
    }

    return bRet;
}
__inline static
LPBYTE TakeResource(LPBYTE* lpData, DWORD& dwSize, DWORD dwResID, LPCSTR lpResType, HMODULE hModule = GetModuleHandleA(NULL))
{
    if (lpData == NULL)
    {
        return NULL;
    }
    HRSRC hRsrc = FindResourceA(hModule, MAKEINTRESOURCEA(dwResID), lpResType);
    DWORD dwErr = GetLastError();
    if (hRsrc)
    {
        HGLOBAL hGlobal = LoadResource(hModule, hRsrc);
        if (hGlobal)
        {
            LPVOID pData = LockResource(hGlobal);
            if (pData)
            {
                if ((dwSize = SizeofResource(hModule, hRsrc)) > 0)
                {
                    *lpData = (LPBYTE)malloc(dwSize * sizeof(BYTE));
                    CopyMemory(*lpData, pData, dwSize);
                }
                UnlockResource(hGlobal);
            }
            FreeResource(hRsrc);
        }
    }
    return (*lpData);
}
__inline static
LPBYTE TakeResource(LPBYTE* lpData, DWORD& dwSize, DWORD dwResID, LPCWSTR lpResType, HMODULE hModule = GetModuleHandleW(NULL))
{
    if (lpData == NULL)
    {
        return NULL;
    }
    HRSRC hRsrc = FindResourceW(hModule, MAKEINTRESOURCEW(dwResID), lpResType);
    DWORD dwErr = GetLastError();
    if (hRsrc)
    {
        HGLOBAL hGlobal = LoadResource(hModule, hRsrc);
        if (hGlobal)
        {
            LPVOID pData = LockResource(hGlobal);
            if (pData)
            {
                if ((dwSize = SizeofResource(hModule, hRsrc)) > 0)
                {
                    *lpData = (LPBYTE)malloc(dwSize * sizeof(BYTE));
                    CopyMemory(*lpData, pData, dwSize);
                }
                UnlockResource(hGlobal);
            }
            FreeResource(hRsrc);
        }
    }
    return (*lpData);
}

__inline static
VOID FreeResource(LPBYTE* lpData)
{
    if (lpData != NULL && (*lpData) != NULL)
    {
        free(*lpData);
    }
}

__inline static
std::string TakeResourceData(DWORD dwResID, LPCTSTR lpResType, HMODULE hModule = GetModuleHandle(NULL))
{
    std::string result = ("");
    LPBYTE lpData = NULL;
    DWORD dwSize = 0L;
    TakeResource(&lpData, dwSize, dwResID, lpResType, hModule);
    if (lpData != NULL)
    {
        result.assign((const char*)lpData, dwSize);
        FreeResource(lpData);
    }
    return result;
}

//判断目录是否存在，若不存在则创建
__inline static
BOOL CreateCascadeDirectory(LPCSTR lpPathName, //Directory name
    LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL  // Security attribute
)
{
    CHAR* pToken = NULL;
    CHAR czPathTemp[MAX_PATH] = { 0 };
    CHAR czPathName[MAX_PATH] = { 0 };

    strcpy(czPathName, lpPathName);
    pToken = strtok(czPathName, ("\\"));
    while (pToken)
    {
        sprintf(czPathTemp, ("%s%s\\"), czPathTemp, pToken);
        //创建失败时还应删除已创建的上层目录，此次略
        if (!CreateDirectoryA(czPathTemp, lpSecurityAttributes))
        {
            DWORD dwError = GetLastError();
            if (dwError != ERROR_ALREADY_EXISTS && dwError != ERROR_ACCESS_DENIED)
            {
                return FALSE;
            }
        }
        pToken = strtok(NULL, ("\\"));
    }
    return TRUE;
}
#ifndef TESSERACT_OCR_DLL_EXPORT
#define TESSERACT_OCR_DLL extern "C" _declspec(dllexport)
#else
#define TESSERACT_OCR_DLL extern "C" _declspec(dllimport)
#endif
#include <io.h>
#include <fstream>
class InitUtil {
public:
    InitUtil() {
        if (strTempPath.empty() == true)
        {
            strTempPath.resize(MAX_PATH, '\0');
            GetTempPathA(strTempPath.size(), (char*)strTempPath.data());
            strTempPath = strTempPath.c_str() + std::string("TEMP\\");
        }
    }
    ~InitUtil() {
        exit();
    }
public:
    std::thread task = {};
    std::string strTempPath = "";
    bool bInited = false;
    HINSTANCE hResourceInstance = GetModuleHandle(TESSERACT_OCR_MODULE);
    void init()
    {
        if (access((strTempPath + xor(MACRO_0)).c_str(), 0) != 0
            ||
            access((strTempPath + xor (MACRO_1)).c_str(), 0) != 0)
        {
            bInited = false;
        }
        if (bInited == false)
        {
            CreateCascadeDirectory(strTempPath.c_str(), NULL);
            FILE_WRITER(TakeResourceData(101, (xor(MACRO_2).c_str()), hResourceInstance).data(),
                TakeResourceData(101, (xor (MACRO_2).c_str()), hResourceInstance).size(),
                (strTempPath + (xor(MACRO_9))), std::ios::binary);
            FILE_WRITER(TakeResourceData(102, (xor (MACRO_2).c_str()), hResourceInstance).data(),
                TakeResourceData(102, (xor (MACRO_2).c_str()), hResourceInstance).size(),
                (strTempPath + (xor(MACRO_6))), std::ios::binary);
            ExecuteCommand((strTempPath + xor(MACRO_3)
                + strTempPath + xor(MACRO_7) + strTempPath).c_str());
            ExecuteCommand((xor (MACRO_4)+strTempPath + xor (MACRO_5)).c_str());
            ExecuteCommand((xor (MACRO_4)+strTempPath + xor (MACRO_6)).c_str());
            if (access((strTempPath + xor (MACRO_0)).c_str(), 0) != 0
                ||
                access((strTempPath + xor (MACRO_1)).c_str(), 0) != 0)
            {
                bInited = false;
            }
            else
            {
                bInited = true;
            }
        }
    }
    void exit()
    {
        ExecuteCommand((xor(MACRO_4) + strTempPath + xor(MACRO_8)).c_str());
    }
    static InitUtil* Inst() {
        static InitUtil InitUtilInstance;
        return &InitUtilInstance;
    }
};
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        InitUtil::Inst()->init();
        //printf("\nprocess attach of dll");
        break;
    case DLL_THREAD_ATTACH:
        //printf("\nthread attach of dll");
        break;
    case DLL_THREAD_DETACH:
        //printf("\nthread detach of dll");
        break;
    case DLL_PROCESS_DETACH:
        InitUtil::Inst()->exit();
        //printf("\nprocess detach of dll");
        break;
    }
    return TRUE;
}
TESSERACT_OCR_DLL int __stdcall ocr_get_string(char* p_utf8_data, int n_utf8_size, const char* p_file_name, const char* p_root_path)
{
    if (InitUtil::Inst()->bInited == false)
    {
        InitUtil::Inst()->init();
        if (InitUtil::Inst()->bInited == false)
        {
            return (-1);
        }
    }
    int n_len = (-1);
    std::string root_path("");
    if (p_root_path == nullptr)
    {
        root_path = InitUtil::Inst()->strTempPath;
    }
    else
    {
        root_path = p_root_path;
    }
    char* outText = nullptr;
    std::string imPath = p_file_name;
    std::string languagePath = root_path;// +"tessdata";
    std::string languageType = "chi_sim+eng";
    // Create Tesseract object
    tesseract::TessBaseAPI ocr = {};
    /*
      Initialize OCR engine to use English (eng) and The LSTM
      OCR engine.


      There are four OCR Engine Mode (oem) available

      OEM_TESSERACT_ONLY             Legacy engine only.
      OEM_LSTM_ONLY                  Neural nets LSTM engine only.
      OEM_TESSERACT_LSTM_COMBINED    Legacy + LSTM engines.
      OEM_DEFAULT                    Default, based on what is available.
     */
     // 初始化
    int nRet = ocr.Init(languagePath.c_str(), languageType.c_str(), tesseract::OEM_DEFAULT);
    if (nRet != 0) {
        printf("TessBaseAPI init failed！");
        return(-1);
    }

    // Set Page segmentation mode to PSM_AUTO (3)
    // Other important psm modes will be discussed in a future post.
    ocr.SetPageSegMode(tesseract::PSM_AUTO);

    // Open input image using OpenCV
    cv::Mat im_src = cv::imread(imPath, cv::IMREAD_UNCHANGED);
    {
        cv::Mat img_mask = {};
        std::vector<uchar> img_out = {};
        cv::Mat img_tmp = cv::Mat(im_src.size(), im_src.type(), cv::Scalar(255, 255, 255));
        cv::inRange(im_src, cv::Scalar(0, 0, 0, 255), cv::Scalar(255, 255, 255, 255), img_mask);
        im_src.copyTo(img_tmp, img_mask);
        cv::imencode(".jpg", img_tmp, img_out);
        cv::imdecode(img_out, cv::IMREAD_UNCHANGED, &im_src);
    }
    // Set image data
    ocr.SetImage(im_src.data, im_src.cols, im_src.rows, 3, im_src.step);
    // Run Tesseract OCR on image
    outText = ocr.GetUTF8Text();
    if (outText != nullptr)
    {
        n_len = strlen(outText);
        if (n_len > n_utf8_size)
        {
            return n_len;
        }
        else
        {
            memcpy(p_utf8_data, outText, n_len);
        }
    }
    // print recognized text
    //std::cout << outText << std::endl;

    // Destroy used object and release memory
    ocr.End();

    return n_len;
}

#endif // !BUILD_DLL_LIBRARY

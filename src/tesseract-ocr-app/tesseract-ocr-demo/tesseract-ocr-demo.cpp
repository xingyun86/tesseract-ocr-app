// tesseract-ocr-demo.cpp : Defines the entry point for the application.
//

#include "tesseract-ocr-demo.h"

#include <tesseract/baseapi.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif // _MSC_VER

void dealImg(const char* path);
__inline static
int MatToJpg(const cv::Mat mat, std::vector<uint8_t>& jpg)
{
    if (mat.empty()) {
        return 0;
    }
    std::vector<int> param = std::vector<int>(2);
    param[0] = cv::IMWRITE_JPEG_QUALITY;
    param[1] = 95; // default(95) 0-100
    cv::imencode(".jpg", mat, jpg, param);
    return 0;
}

__inline static
int JpgToMat(cv::Mat& mat, std::vector<uint8_t> jpg)
{
    if (jpg.empty()) {
        return -1;
    }
    if ((jpg[0] == 0xFF && jpg[1] == 0xD8))
    {
        mat = cv::imdecode(jpg, cv::IMREAD_COLOR);
    }
    else
    {
        jpg.insert(jpg.begin(), 0xFF);
        jpg.insert(jpg.begin() + 1, 0xD8);
        mat = cv::imdecode(jpg, cv::IMREAD_COLOR);
    }
    return 0;
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
    imPath = "test.png";
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
    /*cv::imshow("", im_src);
    cv::waitKey();
    cv::imwrite("fruit111.bmp", im_src);
    cv::Mat image_bmp;
    im_src.convertTo(image_bmp, CV_8UC3);

    imwrite("fruit.bmp", image_bmp);*/
    cv::Mat im_dst = cv::Mat::zeros(cv::Size(im_src.cols * 2, im_src.rows * 2), CV_8UC3);
    //cv::Mat im_dst = cv::Mat::ones(cv::Size(im_src.cols * 2, im_src.rows * 2), CV_8UC3);
    cv::resize(im_src, im_dst, cv::Size(im_dst.cols, im_dst.rows));
    // Set image data
    ocr->SetImage(im_dst.data, im_dst.cols, im_dst.rows, 3, im_dst.step);
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
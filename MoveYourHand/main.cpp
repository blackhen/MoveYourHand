#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <image.h>
#include <win_cre.h>
#include <iostream>
#include <windows.h>

using namespace cv;
using namespace std;

char *setwin_name = "Setting";
char *denois_winname = "Denoising";
char *imgshow_name = "Original Image";

image capture(0);
win_cre win(setwin_name, denois_winname);

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
     if  ( event == EVENT_LBUTTONDOWN)
     {
         int i, temp[3] = {1, 0 , -1}, j = 0, loop = 0;
         int avg_h = 0 , avg_s = 0, avg_v = 0;
         while (loop < 9)
         {
            for(i = 0; i < 3; i++)
            {
                avg_h += capture.cvt_hsv.at<Vec3b>(Point(x + temp[j], y + temp[i]))[0];
                avg_s += capture.cvt_hsv.at<Vec3b>(Point(x + temp[j], y + temp[i]))[1];
                avg_v += capture.cvt_hsv.at<Vec3b>(Point(x + temp[j], y + temp[i]))[2];
                j++;
                loop++;
            }
            j = 0;
         }

         avg_h /= 9;
         avg_s /= 9;
         avg_v /= 9;
         cout << avg_h << " " << avg_s << " " << avg_v << endl;
         win.update_hsv(avg_h, avg_s, avg_v);
     }
}

int main()
{
    Mat cvt_Mat;
    double tmp_posi_x = -1, tmp_posi_y = -1;
    namedWindow(imgshow_name, WINDOW_AUTOSIZE);

    while(waitKey(30) != 27) //delay 30 millisecond for show image
    {
        Scalar low_hsv(win.low_h, win.low_s, win.low_v);
        Scalar most_hsv(win.most_h, win.most_s, win.most_v);

        capture.update();

        capture.cal_bin_img(cvt_Mat, low_hsv, most_hsv);

        morphologyEx(cvt_Mat, cvt_Mat, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(win.opn_x, win.opn_y)));
        morphologyEx(cvt_Mat, cvt_Mat, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(win.cls_x, win.cls_y)));

        Moments moment_cvt = moments(cvt_Mat, 1);
        double area = moment_cvt.m00;
        double moment01 = moment_cvt.m01;
        double moment10 = moment_cvt.m10;
        if (area > 10000)
        {
            int posi_x = moment10 / area;
            int posi_y = moment01 / area;

            if (tmp_posi_x >= 0 && tmp_posi_y >= 0 && posi_x >= 0 && posi_y >= 0)
                circle(capture.img, Point(posi_x, posi_y), 10, Scalar(0, 255, 0), -1);

            tmp_posi_x = posi_x;
            tmp_posi_y = posi_y;
        }

        setMouseCallback(imgshow_name, CallBackFunc, NULL);

        imshow(imgshow_name, capture.img);
        imshow("Two", cvt_Mat);

    }
    return 1;
}

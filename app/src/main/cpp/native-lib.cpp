#include <jni.h>
#include <string>
#include <opencv2/core/core.hpp>
#include "Lazy-Snapping.h"



using namespace cv;
using namespace std;

extern "C"
JNIEXPORT jstring JNICALL Java_com_wyx_lazysnappingandroid_LazySnappingHelper_stringFromJNI(
        JNIEnv* env
//        jclass obj
) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jintArray JNICALL Java_com_wyx_lazysnappingandroid_LazySnappingHelper_getSnappedPic(
        JNIEnv* env, jclass obj, jintArray buf, jint w, jint h, jfloatArray pathw1, jfloatArray pathh1, jfloatArray pathw2, jfloatArray pathh2){
    jint *cbuf;
    cbuf = env->GetIntArrayElements(buf, JNI_FALSE );
    if (cbuf == NULL) {
        return 0;
    }
    jfloat *w1 ;jfloat *h1 ;jfloat *w2 ;jfloat *h2 ;
    w1 = env->GetFloatArrayElements(pathw1, JNI_FALSE );
    if (w1 == NULL) {
        return 0;
    }
    h1 = env->GetFloatArrayElements(pathh1, JNI_FALSE );
    if (h1 == NULL) {
        return 0;
    }
    w2 = env->GetFloatArrayElements(pathw2, JNI_FALSE );
    if (w2 == NULL) {
        return 0;
    }
    h2 = env->GetFloatArrayElements(pathh2, JNI_FALSE );
    if (h2 == NULL) {
        return 0;
    }
    vector<Point> g_forePts,g_backPts;
    for (int i = 0; i < sizeof(w1); ++i) {
        Point currentPoint=Point(w1[i],h1[i]);
        g_forePts.push_back(currentPoint);
    }
    for (int i = 0; i < sizeof(w2); ++i) {
        Point currentPoint=Point(w2[i],h2[i]);
        g_backPts.push_back(currentPoint);
    }
    Mat imgData(h, w, CV_8UC4, (unsigned char *) cbuf);
    Mat mask ;
    Mat lastImg ;


    LazySnapping_Self lazySnapping;
    lazySnapping.InitLazySnapping(g_forePts,g_backPts,imgData);
    lazySnapping.Run();

    lazySnapping.GetImageMask(mask);
    imgData.copyTo(lastImg,mask);
    int lastImgw = lastImg.rows;
    int lastImgh = lastImg.cols;
    jint *cbuf2 = (jint *) lastImg.data;
    int size = lastImgw * lastImgh;
    jintArray result = env->NewIntArray(size);
    env->SetIntArrayRegion(result, 0, size, cbuf2);

    env->ReleaseFloatArrayElements(pathh1, h1, 0);
    env->ReleaseFloatArrayElements(pathw1, w1, 0);
    env->ReleaseFloatArrayElements(pathh2, h2, 0);
    env->ReleaseFloatArrayElements(pathw2, w2, 0);
    env->ReleaseIntArrayElements(buf, cbuf, 0);
    imgData.release();
    mask.release();
    lastImg.release();
    return result;
}

extern "C"
JNIEXPORT jintArray JNICALL Java_com_wyx_lazysnappingandroid_LazySnappingHelper_gray(
        JNIEnv *env, jclass obj, jintArray buf, int w, int h) {

    jint *cbuf;
    cbuf = env->GetIntArrayElements(buf, JNI_FALSE );
    if (cbuf == NULL) {
        return 0;
    }

    Mat imgData(h, w, CV_8UC4, (unsigned char *) cbuf);

    uchar* ptr = imgData.ptr(0);
    for(int i = 0; i < w*h; i ++){
        //计算公式：Y(亮度) = 0.299*R + 0.587*G + 0.114*B
        //对于一个int四字节，其彩色值存储方式为：BGRA
        int grayScale = (int)(ptr[4*i+2]*0.299 + ptr[4*i+1]*0.587 + ptr[4*i+0]*0.114);
        ptr[4*i+1] = grayScale;
        ptr[4*i+2] = grayScale;
        ptr[4*i+0] = grayScale;
    }

    int size = w * h;
    jintArray result = env->NewIntArray(size);
    env->SetIntArrayRegion(result, 0, size, cbuf);
    env->ReleaseIntArrayElements(buf, cbuf, 0);
    return result;
}
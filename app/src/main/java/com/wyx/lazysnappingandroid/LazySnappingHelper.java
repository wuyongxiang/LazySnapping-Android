package com.wyx.lazysnappingandroid;

/**
 * Created by 吴永祥 on 2017/4/11.
 */

public class LazySnappingHelper {

    static {
        System.loadLibrary("LazySnapping_Android");
    }

    public  native int[] getSnappedPic(int[] buf, int w,int h,float[] w1, float[] h1,float[] w2, float[] h2);

    public  native String stringFromJNI();

    public  native int[] gray(int[] buf, int w,int h);
}

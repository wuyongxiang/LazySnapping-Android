package com.wyx.lazysnappingandroid;

import android.app.ProgressDialog;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PointF;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.DisplayMetrics;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.Toast;

import com.wyx.lazysnapping_android.R;

import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Date;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    public static final int PAINT_GREEN = 0x01;
    public static final int PAINT_BLUE = 0x02;


    public static final int ACTIVITY_RESULT = 0x11;

    public static final int HANDELER_SETBITMAP = 0x21;
    public static final int HANDELER_CLEARBITMAP = 0x22;
    public static final int HANDELER_fINUISHBITMAP = 0x23;
    private FloatingActionButton fab_finish,fab_clear,fab_green_paint,fab_blue_paint;

    private ImageView cropView;
    private Bitmap allBitmap , baseBitmap,returnBitmap;

    private LazySnappingHelper helper;
    private float bi = 1.0f;
    private int paint1 = MainActivity.PAINT_GREEN;
    private Paint paintG,paintR;
    private Canvas canvas;
    private ArrayList<PointF> greenPoints = new ArrayList<PointF>();
    private ArrayList<PointF> redPoints = new ArrayList<PointF>();

    protected ProgressDialog pg = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        initViews();

    }
    private void finishView() {
        pg.dismiss();
        greenPoints.clear();
        redPoints.clear();
        cropView.setImageBitmap(returnBitmap);
        Date currentdate = new java.util.Date();
        String url= currentdate.getTime()+".png";
        try {
            FileUtil.saveBitmap(this,returnBitmap,url);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }

    private void drawView() {
        canvas = new Canvas(allBitmap);
        canvas.drawBitmap(allBitmap, new Matrix(), getPaint());
        cropView.setImageBitmap(allBitmap);
        final float bw = cropView.getLeft();
        final float bh = cropView.getTop();
        cropView.setOnTouchListener(new View.OnTouchListener() {
            int startX;
            int startY;
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        // 获取手按下时的坐标
                        startX = (int) event.getX();
                        startY = (int) event.getY();
                        PointF pmd = new PointF(event.getX()-bw, event.getY()-bh);
                        if(paint1 == MainActivity.PAINT_GREEN){
                            greenPoints.add(pmd);
                        }else{
                            redPoints.add(pmd);
                        }
                        break;
                    case MotionEvent.ACTION_MOVE:
                        // 获取手移动后的坐标
                        int stopX = (int) event.getX();
                        int stopY = (int) event.getY();
                        // 在开始和结束坐标间画一条线
                        canvas.drawLine(startX, startY, stopX, stopY, getPaint());
                        // 实时更新开始坐标
                        startX = (int) event.getX();
                        startY = (int) event.getY();
                        PointF pm = new PointF(event.getX()-bw, event.getY()-bh);
                        if(paint1 == MainActivity.PAINT_GREEN){
                            greenPoints.add(pm);
                        }else{
                            redPoints.add(pm);
                        }
                        cropView.setImageBitmap(allBitmap);
                        break;
                }
                return true;
            }
        });
    }


    private Paint getPaint() {
        if(paint1 == PAINT_GREEN){
            return paintG;
        }else {
            return paintR;
        }
    }

    private void initViews(){
        fab_finish = (FloatingActionButton) findViewById(R.id.fab_finish);
        fab_clear = (FloatingActionButton) findViewById(R.id.fab_clear);
        fab_green_paint = (FloatingActionButton) findViewById(R.id.fab_green_paint);
        fab_blue_paint = (FloatingActionButton) findViewById(R.id.fab_blue_paint);
        cropView = (ImageView) findViewById(R.id.crop);
        paintG = new Paint();
        paintG.setColor(getResources().getColor(R.color.colorAccent));
        paintG.setStrokeWidth(3);
        paintR = new Paint();
        paintR.setColor(getResources().getColor(R.color.red));
        paintR.setStrokeWidth(3);
        fab_finish.setOnClickListener(this);
        fab_blue_paint.setOnClickListener(this);
        fab_green_paint.setOnClickListener(this);
        fab_clear.setOnClickListener(this);
        helper = new LazySnappingHelper();
        pg = new ProgressDialog(this, R.style.ccpalertdialog);
        pg.setCanceledOnTouchOutside(false);
        pg.setMessage(getString(R.string.loading));
    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            intent.setType("image/*");
            try {
                startActivityForResult(Intent.createChooser(intent, "请选择一个要上传的文件"), ACTIVITY_RESULT);
            } catch (ActivityNotFoundException ex) {
                // Potentially direct the user to the Market with a Dialog
                ex.printStackTrace();
                Toast.makeText(this, "请安装文件管理器", Toast.LENGTH_SHORT).show();
            }
            return true;
        }

        return super.onOptionsItemSelected(item);
    }



    @Override
    public void onActivityResult(final int requestCode, int resultCode, Intent data) {

        if (resultCode == RESULT_CANCELED)
            return;
        if (requestCode == ACTIVITY_RESULT) {
            Uri uri = data.getData();
            String url = FileUtil.getPath_content(MainActivity.this, uri);
            WindowManager windowManager = (WindowManager) this
                    .getSystemService(Context.WINDOW_SERVICE);

            DisplayMetrics dm = new DisplayMetrics();
            windowManager.getDefaultDisplay().getMetrics(dm);
            float widthP = dm.widthPixels ;
            BitmapFactory.Options opt = new BitmapFactory.Options();
            opt.inPreferredConfig = Bitmap.Config.ARGB_8888;
            allBitmap = BitmapFactory.decodeFile(url , opt);
            int angle = FileUtil.getImageSpinAngle(url);
            bi = widthP/allBitmap.getWidth();
            Matrix m = new Matrix();
            m.setRotate(angle);
            m.postScale(bi,bi);
            allBitmap = Bitmap.createBitmap(allBitmap, 0, 0, allBitmap.getWidth(), allBitmap.getHeight(), m, true);
            baseBitmap = allBitmap.copy(Bitmap.Config.ARGB_8888,true);
            mHandler.sendEmptyMessage(HANDELER_SETBITMAP);


        }
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {

            switch (msg.what){
                case HANDELER_CLEARBITMAP:
                    greenPoints.clear();
                    redPoints.clear();
                    if(allBitmap!=null)
                        allBitmap.recycle();
                    allBitmap = null;
                    cropView.setImageBitmap(null);
                    break;
                case HANDELER_SETBITMAP:
                    drawView();
                    break;
                case HANDELER_fINUISHBITMAP:
                    finishView();
                    pg.dismiss();



                    break;
            }


        }
    };

    private void snapView(){

        int size1 = greenPoints.size();
        int size2 = redPoints.size();
        float[] w1 = new float[size1];
        float[] h1 = new float[size1];
        float[] w2 = new float[size2];
        float[] h2 = new float[size2];

        for(int i = 0; i<size1 ; i++){
            w1[i] = greenPoints.get(i).x;
            h1[i] = greenPoints.get(i).y;
        }
        for(int i = 0; i<size2 ; i++){
            w2[i] = redPoints.get(i).x;
            h2[i] = redPoints.get(i).y;
        }
        int w = baseBitmap.getWidth(), h = baseBitmap.getHeight();
        int[] pix = new int[w * h];
        baseBitmap.getPixels(pix, 0, w, 0, 0, w, h);
        int [] resultPixes= helper.getSnappedPic(pix,w,h,w1,h1,w2,h2);
        returnBitmap = Bitmap.createBitmap(w,h, Bitmap.Config.ARGB_8888);
        returnBitmap.setPixels(resultPixes, 0, w, 0, 0,w, h);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.fab_finish:
                pg.show();
                new Thread(){
                    @Override
                    public void run() {
                        pg.show();
                        snapView();
                        mHandler.sendEmptyMessage(HANDELER_fINUISHBITMAP);

                    }
                }.start();

                break;
            case R.id.fab_clear:
                mHandler.sendEmptyMessage(HANDELER_CLEARBITMAP);
                break;

            case R.id.fab_green_paint:
                paint1 = PAINT_GREEN;
                break;
            case R.id.fab_blue_paint:
                paint1 = PAINT_BLUE;
                break;
        }
    }

}

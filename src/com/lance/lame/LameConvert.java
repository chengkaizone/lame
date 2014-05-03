package com.lance.lame;

import java.io.DataInputStream;
import java.io.File;
import java.io.InputStream;

import android.app.Activity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class LameConvert extends Activity {

	static{
		System.loadLibrary("lameConvert");
	}
	
	private EditText et_wav;
	private EditText et_mp3;
	
	public native void convertmp3(String src,String dest);
	public native String getLameVersion();
	public native void stop();
	public native void test();
	
	private ProgressDialog pd;
	
	Handler handler=new Handler(){

		@Override
		public void handleMessage(Message msg) {
			Toast.makeText(LameConvert.this,"转换完成!",1).show();
			pd.dismiss();
		}
		
	};
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		et_wav=(EditText)findViewById(R.id.et_wav);
		et_mp3=(EditText)findViewById(R.id.et_mp3);
		pd=new ProgressDialog(this);
		
		testProcess();
	}
	private void testProcess(){
		try {
			Process process=Runtime.getRuntime().exec("date");
			InputStream is = process.getInputStream();
			DataInputStream dis = new DataInputStream(is);
			String result="";
			String str="";
			while((str=dis.readLine())!=null){
				result+=str;
			}
			System.out.println("process="+result);
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		
		return super.onKeyDown(keyCode, event);
	}
	public void convert(View view){
		final String wavname=et_wav.getText().toString().trim();
		final String mp3name=et_mp3.getText().toString().trim();
		File wavFile=new File(wavname);
		int size=(int)wavFile.length();
		System.out.println("文件大小->"+size);
		if("".equals(wavname)||"".equals(mp3name)){
			Toast.makeText(this,"文件名不能为空!", 1).show();
			return;
		}
		
		pd.setMessage("转换中...");
		pd.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
		pd.setMax(size);
		
		pd.show();
		new Thread(){
			public void run(){
				convertmp3(wavname,mp3name);
				handler.obtainMessage(1).sendToTarget();
			}
		}.start();
	}

	//这个方法由so库文件的方法调用
	public void setConvertProgress(int progress){
		pd.setProgress(progress);
	}
	
	public void getVersion(View view){
		System.out.println("获取lame版本号-->"+getLameVersion());
		Toast.makeText(this, getLameVersion(), 1).show();
	}
	public void stop(View view){
		stop();
	}
	
}

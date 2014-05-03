#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <lamelib/lame.h>
#include <android/log.h>

#define LOG_TAG "System.out"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)//__VA_ARGS__表示可变参数
#define unit 8192

char* Jstring2CStr(JNIEnv* env, jstring jstr) {
	char* rtn = NULL;
	jclass clsstring = (*env)->FindClass(env, "java/lang/String");
	jstring strencode = (*env)->NewStringUTF(env, "GB2312");
	jmethodID mid = (*env)->GetMethodID(env, clsstring, "getBytes",
			"(Ljava/lang/String;)[B");
	jbyteArray barr = (jbyteArray) (*env)->CallObjectMethod(env, jstr, mid,
			strencode); // String .getByte("GB2312");
	jsize alen = (*env)->GetArrayLength(env, barr);
	jbyte* ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
	if (alen > 0) {
		rtn = (char*) malloc(alen + 1); //"\0"
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	//rtn是动态内存分配---将ba指针的内容拷贝到rtn中,所以后面需要释放的是ba指针
	(*env)->ReleaseByteArrayElements(env, barr, rtn, 0); //
	return rtn;
}

//这里调用java代码中setConvertProgress方法
void publishJavaProgress(JNIEnv * env, jobject obj,jint progress){
	//找到类定义
	jclass clazz=(*env)->FindClass(env,"com.lance.lame.LameConvert");
	if(clazz==0){
		LOGI("not find class !");
		return;
	}
	//找到方法定义
	jmethodID method=(*env)->GetMethodID(env,clazz,"setConvertProgress","(I)V");
	if(method==0){
		LOGI("can not find method!");
	}
	//调用方法
	(*env)->CallVoidMethod(env,obj,method,progress);
}

JNIEXPORT void JNICALL Java_com_lance_lame_LameConvert_test
  (JNIEnv * env, jobject obj){
	LOGI("this is test");
}


JNIEXPORT jstring JNICALL Java_com_lance_lame_LameConvert_getLameVersion
  (JNIEnv * env, jobject obj){
	return (*env)->NewStringUTF(env,get_lame_version());
}

int flag=0;

JNIEXPORT void JNICALL Java_com_lance_lame_LameConvert_stop
  (JNIEnv * env, jobject obj){
	flag=404;//用于停止转换
}
JNIEXPORT void JNICALL Java_com_lance_lame_LameConvert_convertmp3
  (JNIEnv * env, jobject obj, jstring jwav, jstring jmp3){
	char * cwav=Jstring2CStr(env,jwav);
	char * cmp3=Jstring2CStr(env,jmp3);

	LOGI("wav = %s",cwav);
	LOGI("mp3 = %s",cmp3);

	FILE * fwav = fopen(cwav,"rb");
	//媒体文件需要使用二进制的方式打开和写入
	FILE * fmp3 = fopen(cmp3,"wb");

	short int wav_buffer[unit*2];
	unsigned char mp3_buffer[unit];

	//初始化编码器
	lame_t lame=lame_init();
	//设置采样率
	lame_set_in_samplerate(lame,44100);
	lame_set_num_channels(lame,2);

	//设置编码方式
	lame_set_VBR(lame, vbr_default);
	//显示的调用这个方法
	lame_init_params(lame);

	LOGI("lame init finish!");

	int read;
	int write;
	int total=0;//当前读的wav文件的byte数目
	flag=0;
	do{
		if(flag==404){
			LOGI("convert stop!");
			break;//终止音频转换
		}
		//读取文件
		read=fread(wav_buffer,sizeof(short int)*2,unit,fwav);
		total+=read*sizeof(short int)*2;
		//在此处调用java中的方法
		publishJavaProgress(env,obj,total);
		LOGI("converting...%d",total);
		if(read!=0){
			//返回转化的长度
			write=lame_encode_buffer_interleaved(lame,wav_buffer,read,mp3_buffer,unit);
			LOGI("write...");
			fwrite(mp3_buffer,sizeof(unsigned char),write,fmp3);
		}else{
			LOGI("flush!");
			lame_encode_flush(lame,mp3_buffer,unit);
		}
	}while(read!=0);

	LOGI("convert finish!");
	//关闭lame编码器

	lame_close(lame);
	LOGI("close wav");

	fclose(fwav);
	LOGI("close mp3");
	fclose(fmp3);
	LOGI("close!");
}

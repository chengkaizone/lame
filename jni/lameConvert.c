#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <lamelib/lame.h>
#include <android/log.h>

#define LOG_TAG "System.out"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)//__VA_ARGS__��ʾ�ɱ����
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
	//rtn�Ƕ�̬�ڴ����---��baָ������ݿ�����rtn��,���Ժ�����Ҫ�ͷŵ���baָ��
	(*env)->ReleaseByteArrayElements(env, barr, rtn, 0); //
	return rtn;
}

//�������java������setConvertProgress����
void publishJavaProgress(JNIEnv * env, jobject obj,jint progress){
	//�ҵ��ඨ��
	jclass clazz=(*env)->FindClass(env,"com.lance.lame.LameConvert");
	if(clazz==0){
		LOGI("not find class !");
		return;
	}
	//�ҵ���������
	jmethodID method=(*env)->GetMethodID(env,clazz,"setConvertProgress","(I)V");
	if(method==0){
		LOGI("can not find method!");
	}
	//���÷���
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
	flag=404;//����ֹͣת��
}
JNIEXPORT void JNICALL Java_com_lance_lame_LameConvert_convertmp3
  (JNIEnv * env, jobject obj, jstring jwav, jstring jmp3){
	char * cwav=Jstring2CStr(env,jwav);
	char * cmp3=Jstring2CStr(env,jmp3);

	LOGI("wav = %s",cwav);
	LOGI("mp3 = %s",cmp3);

	FILE * fwav = fopen(cwav,"rb");
	//ý���ļ���Ҫʹ�ö����Ƶķ�ʽ�򿪺�д��
	FILE * fmp3 = fopen(cmp3,"wb");

	short int wav_buffer[unit*2];
	unsigned char mp3_buffer[unit];

	//��ʼ��������
	lame_t lame=lame_init();
	//���ò�����
	lame_set_in_samplerate(lame,44100);
	lame_set_num_channels(lame,2);

	//���ñ��뷽ʽ
	lame_set_VBR(lame, vbr_default);
	//��ʾ�ĵ����������
	lame_init_params(lame);

	LOGI("lame init finish!");

	int read;
	int write;
	int total=0;//��ǰ����wav�ļ���byte��Ŀ
	flag=0;
	do{
		if(flag==404){
			LOGI("convert stop!");
			break;//��ֹ��Ƶת��
		}
		//��ȡ�ļ�
		read=fread(wav_buffer,sizeof(short int)*2,unit,fwav);
		total+=read*sizeof(short int)*2;
		//�ڴ˴�����java�еķ���
		publishJavaProgress(env,obj,total);
		LOGI("converting...%d",total);
		if(read!=0){
			//����ת���ĳ���
			write=lame_encode_buffer_interleaved(lame,wav_buffer,read,mp3_buffer,unit);
			LOGI("write...");
			fwrite(mp3_buffer,sizeof(unsigned char),write,fmp3);
		}else{
			LOGI("flush!");
			lame_encode_flush(lame,mp3_buffer,unit);
		}
	}while(read!=0);

	LOGI("convert finish!");
	//�ر�lame������

	lame_close(lame);
	LOGI("close wav");

	fclose(fwav);
	LOGI("close mp3");
	fclose(fmp3);
	LOGI("close!");
}

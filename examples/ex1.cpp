/*
 * @file ex1.cpp
 * @brief 固定方向同時1音源抽出サンプル。
 * @author Copyright (C) 2017 Fairy Devices Inc. http://www.fairydevices.jp/
 * @author Masato Fujino, created on: 2017/07/14
 */


#include <iostream>
#include <unistd.h>
#include <syslog.h>
#include <string>
#include <sched.h>
#include "XFERecorder.h"
#include "XFETypedef.h"


class UserData
{
public:
	FILE *file_;
};

void recorderCallback(
		short* buffer,
		size_t buflen,
		mimixfe::SpeechState state,
		int sourceId,
		mimixfe::StreamInfo* info,
		size_t infolen,
		void* userdata)
{
	std::string s = "";
	if(state == mimixfe::SpeechState::SpeechStart){
		s = "Speech Start";
	}else if(state == mimixfe::SpeechState::InSpeech){
		s = "In Speech";
	}else if(state == mimixfe::SpeechState::SpeechEnd){
		s = "End of Speech";
	}
  UserData *p = reinterpret_cast<UserData*>(userdata);
  fwrite(buffer, sizeof(short), buflen, p->file_);

  // 画面表示で確認
  std::cout << "State: " << s << std::endl;
  for(int i=0;i<infolen;++i){
	  std::cout << info[i].milliseconds_ << "[ms] " << info[i].rms_ << "[rms] " << static_cast<int>(info[i].speechProbability_) << "[%] ";
	  std::cout << sourceId << " (" << info[i].extractedSoundSources_ << "/" << info[i].estimatedSoundSources_ << ")";
	  std::cout << " angle=" << info[i].direction_.angle_ << ", azimuth=" << info[i].direction_.azimuth_ << std::endl;
  }
}

int main(int argc, char** argv)
{
	// プロセス優先度設定. スムーズな実行のためには好ましい設定ですが root 権限を必要とします
	 struct sched_param sp;
	 sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
	 int ret = sched_setscheduler(0, SCHED_FIFO, &sp);
	 if(ret != 0){
		 std::cerr << "Could not set scheduling policy, try sudo." << std::endl;
		 return ret;
	 }

	 // XFE の実行
	 using namespace mimixfe;
	 XFESourceConfig s;
	 XFEECConfig e;
	 XFEVADConfig v;
	 XFEBeamformerConfig b;
	 XFEStaticLocalizerConfig c({Direction(270, 90)});
	 UserData data;
	 data.file_ = fopen("/tmp/debug_ex1.raw","w");
	 int return_status = 0;
	 try{
		XFERecorder rec(s,e,v,b,c,recorderCallback,reinterpret_cast<void*>(&data));
		rec.setLogLevel(LOG_UPTO(LOG_DEBUG)); // デバッグレベルのログから出力する
		rec.start();
		int countup = 0;
		int timeout = 20;
		while(rec.isActive()){
			std::cout << countup++  << " / " << timeout << std::endl;
			sleep(1);
			if(countup == timeout){
				break;
			}
		}
		return_status = rec.stop();
	 }catch(const XFERecorderError& e){
		std::cerr << "XFE Recorder Exception: " << e.what() << "(" << e.errorno() << ")" << std::endl;
	 }catch(const std::exception& e){
		std::cerr << "Exception: " << e.what() << std::endl;
	 }
	 fclose(data.file_);
	 if(return_status != 0){
		 std::cerr << "Abort by error code = " << return_status << std::endl;
	 }
	 return return_status;
}

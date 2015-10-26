#pragma once
#include <Datastructures\DataStructure.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
class CameraIn
{
	BlockingQueue<IplImage> m_frames_read;
	BlockingQueue<cv::Mat> &m_frames_out;
	boost::thread_group m_threads;
	std::string m_window_name;
	CvCapture* m_capture;
	bool m_keep_alive;
	boost::mutex m_mutex;


	void display_frame();

	void capture_frame(); 

	CameraIn();

	CameraIn(const CameraIn&);

	CameraIn*operator=(const CameraIn&);

public:
	CameraIn(BlockingQueue<cv::Mat> &_frames_out);

	~CameraIn();

	bool setup(const std::string &_window_name,
		int &_width,
		int &_height);

	//blocking call
	void run();

	void stop();
};


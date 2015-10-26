#include "CameraIn.h"
#include <Logger\Logger.h>
using namespace AceLogger;
using namespace cv;
CameraIn::CameraIn(BlockingQueue<cv::Mat> &_frames_out):m_frames_out(_frames_out){
	boost::unique_lock<boost::mutex> lock(m_mutex);
	m_capture = nullptr;
	m_keep_alive = true;
	m_window_name = "default_window";
}


CameraIn::~CameraIn()
{
	if (m_capture){
		cvReleaseCapture(&m_capture);
	}
	m_capture = nullptr;
	
}
static const std::string  window_0 = "Blue";
static const std::string  window_1 = "Green";
static const std::string  window_2 = "Red";
bool CameraIn::setup(const std::string &_window_name,
	int &_width,
	int &_height){
	Log("Setting up the camera input...");
	m_capture = cvCaptureFromCAM(CV_CAP_ANY); //0=default, -1=any camera, 1..99=your camera

	if (!m_capture)
	{
		Log("No camera detected", LOG_ERROR);
		return false;
	}
	auto f = cvQueryFrame(m_capture);
	while (!f)
		f = cvQueryFrame(m_capture);
	_width = f->width;
	_height = f->height;
	//cvReleaseImage(&f);
	if (!_window_name.empty())
		m_window_name = _window_name;
	namedWindow(m_window_name.c_str(), CV_WINDOW_AUTOSIZE);

	namedWindow(std::string(m_window_name + window_0).c_str(), CV_WINDOW_AUTOSIZE);
	namedWindow(std::string(m_window_name + window_1).c_str(), CV_WINDOW_AUTOSIZE);
	namedWindow(std::string(m_window_name + window_2).c_str(), CV_WINDOW_AUTOSIZE);
	Log("Done!");

	return true;
}

void CameraIn::run(){
	Log("Starting the display thread...");
	m_threads.create_thread(boost::bind(boost::mem_fn(&CameraIn::display_frame), this));
	Log("Started!");
	capture_frame();
	Log("Waiting for the display thread to finish...");
	m_threads.join_all();
	Log("Camera operations finished...");
}
void LogFrameData(Mat *frame) {
	int total_channels = frame->channels();
	for (int r = 0; r < frame->rows; r++) {
		std::string str = "";
		for (int c = 0; c < frame->cols; c++) {
			
			for (int j = 0; j < total_channels; j++) {
				auto val = (float)frame->data[r*total_channels* frame->cols + c*total_channels + j];
				str += StringUtils::get_string(val)+" ";
			}
			str += ";";
		}
		Log(str, AceLogger::LOG_STATUS, AceLogger::LOG_TYPE::FILE);
	}
	Log("-------------------------------------------------", AceLogger::LOG_STATUS, AceLogger::LOG_TYPE::FILE);
}
void CameraIn::display_frame(){
	Log("Starting frame display...");
	Mat frame;
	IplImage* iplImg;
	Mat channel[3];
	while (m_frames_read.Remove(&iplImg)){
		if (iplImg){
			frame = iplImg;

			if (frame.empty())
				continue;
			//cv::cvtColor(frame, frame, CV_BGR2GRAY);
			//cv::equalizeHist(frame, frame);
			/*cv::cvtColor(frame, frame, CV_BGR2HSV);*/
			/*cv::GaussianBlur(frame,
				frame,
				cv::Size(5, 5),
				2.2);*/
			//cv::fastNlMeansDenoising(frame, frame);
			Mat *frameCopy = new Mat();
			if (iplImg->origin == IPL_ORIGIN_TL)
				frame.copyTo(*frameCopy);
			else
				flip(frame, *frameCopy, 0);
			
			
			//cv::bitwise_not(frame, frame);
			
			//
			
			
			cv::split(frame, channel);
			
			imshow(std::string(m_window_name + window_2).c_str(), channel[2]);
			imshow(std::string(m_window_name + window_1).c_str(), channel[1]);
			imshow(std::string(m_window_name + window_0).c_str(), channel[0]);
			imshow(m_window_name.c_str(), frame);
			
			m_frames_out.Insert(frameCopy);
			//LogFrameData(&frame);
			
		}
	}
	m_frames_out.ShutDown();
	Log("Frame display stopped!");
	
}

void CameraIn::capture_frame(){

	Log("Starting the frame capture...");
	//bool firstOne = true;
	while (true){
		Mat frame;
		//if (firstOne){
			IplImage* iplImg = cvQueryFrame(m_capture);
			m_frames_read.Insert(iplImg);
			//firstOne = false;
		//}
		
		boost::unique_lock<boost::mutex> lock(m_mutex);
		if (!m_keep_alive)
			break;
		if (waitKey(10) >= 0)
			break;
	}
	m_frames_read.ShutDown();
	cvReleaseCapture(&m_capture);
	m_capture = 0;
	Log("Frame capture complete!");
	
}

void CameraIn::stop(){
	boost::unique_lock<boost::mutex> lock(m_mutex);
	m_keep_alive = false;
	Log("Camera input will be stopped...");
}

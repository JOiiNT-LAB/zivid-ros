#include <ros/ros.h>
#include <ros/package.h>

#include <zivid_camera/Capture.h>
#include <zivid_camera/LoadSettingsFromFile.h>
#include <exsensia_vision_module_msgs/PCloudImageReq.h>

#include <sensor_msgs/Image.h>
#include <sensor_msgs/PointCloud2.h>

#include <cv_bridge/cv_bridge.h>

#define CHECK(cmd)                                                                                                     \
  do                                                                                                                   \
  {                                                                                                                    \
    if (!cmd)                                                                                                          \
    {                                                                                                                  \
      throw std::runtime_error{ "\"" #cmd "\" failed!" };                                                              \
    }                                                                                                                  \
  } while (false)

class zivid_bridge
{
public:
	zivid_bridge();
	~zivid_bridge();

private:
	void on_points(const sensor_msgs::PointCloud2::ConstPtr& msg);
	void on_image_color(const sensor_msgs::Image::ConstPtr& msg);
	bool callback_camera_data(exsensia_vision_module_msgs::PCloudImageReq::Request &req, exsensia_vision_module_msgs::PCloudImageReq::Response &res);
	void capture();

	ros::Subscriber 				points_sub_, image_color_sub_;
	ros::ServiceServer 				server_camera_capture_;
	ros::ServiceClient 				load_settings_;

	sensor_msgs::PointCloud2		msg_pc2_;
	sensor_msgs::Image::ConstPtr	msg_img_;

	bool 							flag_pc2_;
	bool 							flag_img_;
};
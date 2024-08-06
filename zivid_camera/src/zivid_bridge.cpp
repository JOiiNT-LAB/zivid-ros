#include <zivid_bridge.h>


zivid_bridge::zivid_bridge()
{
  ros::NodeHandle n;

  ROS_INFO("Starting zivid_bridge.cpp");

  flag_pc2_ = false;
  flag_img_ = false;

  // Subscibers
  points_sub_             = n.subscribe("camera/depth_registered/points", 1, &zivid_bridge::on_points, this);
  image_color_sub_        = n.subscribe("camera/color/image_rect_color", 1, &zivid_bridge::on_image_color, this);
  // Servers
  server_camera_capture_  = n.advertiseService("sensorMeas", &zivid_bridge::callback_camera_data, this);
  //ros::ServiceServer serverCameraInfo = nh.advertiseService("cameraInfo", callbackServerCameraInfo); Inserito direttamente in zivid_camera.cpp
  // Clients
  load_settings_          = n.serviceClient<zivid_camera::LoadSettingsFromFile>("camera/load_settings_from_file");

  std::string setting_path;
  n.param<std::string>("setting_path", setting_path, ".....");


  zivid_camera::LoadSettingsFromFile file;
  file.request.file_path = setting_path;
  load_settings_.waitForExistence();
  if (load_settings_.call(file)) ROS_INFO("Loading image settings");
  else ROS_ERROR("Failed to call service load_settings_from_file");
}

zivid_bridge::~zivid_bridge(){}


void zivid_bridge::on_points(const sensor_msgs::PointCloud2::ConstPtr& msg)
{
  ROS_INFO("PointCloud received");
  msg_pc2_  = *msg;
  flag_pc2_ = true;
}

void zivid_bridge::on_image_color(const sensor_msgs::Image::ConstPtr& msg)
{
  ROS_INFO("2D color image received");
  cv_bridge::CvImagePtr cv_ptr;
  cv_ptr   = cv_bridge::toCvCopy(msg, "rgb8");                                            // Transforming image encoding to rgb8
  msg_img_ = cv_bridge::CvImage(std_msgs::Header(), "rgb8", cv_ptr->image).toImageMsg();
  flag_img_ = true;
}

bool zivid_bridge::callback_camera_data(exsensia_vision_module_msgs::PCloudImageReq::Request &req, exsensia_vision_module_msgs::PCloudImageReq::Response &res)
{


  capture();

  while (!flag_pc2_ || !flag_img_)
  {
    usleep(1000);
    ros::spinOnce();
  }

  flag_pc2_ = false;
  flag_img_ = false;

  res.data.pointCloud = msg_pc2_;
  res.data.image = *msg_img_;
  ROS_INFO("finito");
  return true;
}

void zivid_bridge::capture()
{
  ROS_INFO("Calling capture service");
  zivid_camera::Capture capture;
  CHECK(ros::service::call("camera/capture", capture));
}
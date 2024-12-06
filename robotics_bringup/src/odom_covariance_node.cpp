#include <nav_msgs/Odometry.h>
#include <ros/ros.h>

#include <boost/array.hpp>
#include <memory>
#include <string>

class OdomCovariance {
 public:
  OdomCovariance(std::shared_ptr<ros::NodeHandle>& nh,
                 std::shared_ptr<ros::NodeHandle>& private_nh);

 private:
  std::shared_ptr<ros::NodeHandle> nh_;

  std::shared_ptr<ros::NodeHandle> private_nh_;

  ros::Publisher odom_pub_;

  ros::Subscriber odom_sub_;

  boost::array<double, 36> pose_covar_matrix_;

  boost::array<double, 36> twist_covar_matrix_;

  void onOdom(const nav_msgs::Odometry::ConstPtr& msg);
};

OdomCovariance::OdomCovariance(std::shared_ptr<ros::NodeHandle>& nh,
                               std::shared_ptr<ros::NodeHandle>& private_nh)
    : nh_(nh),
      private_nh_(private_nh),
      pose_covar_matrix_{0},
      twist_covar_matrix_{0} {
  std::string input_odom_topic;
  std::string output_odom_topic;

  private_nh_->param<std::string>("input_odom_topic", input_odom_topic,
                                  std::string("odom"));
  private_nh_->param<std::string>("output_odom_topic", output_odom_topic,
                                  std::string("odom_covar"));

  private_nh_->param<double>("pos_x", pose_covar_matrix_[0], 0.5);
  private_nh_->param<double>("pos_y", pose_covar_matrix_[7], 0.5);
  private_nh_->param<double>("pos_z", pose_covar_matrix_[14], 100000.0);
  private_nh_->param<double>("pos_raw", pose_covar_matrix_[21], 100000.0);
  private_nh_->param<double>("pos_pitch", pose_covar_matrix_[28], 100000.0);
  private_nh_->param<double>("pos_yaw", pose_covar_matrix_[35], 0.1);

  private_nh_->param<double>("twist_x", twist_covar_matrix_[0], 0.05);
  private_nh_->param<double>("twist_y", twist_covar_matrix_[7], 0.05);
  private_nh_->param<double>("twist_z", twist_covar_matrix_[14], 100000.0);
  private_nh_->param<double>("twist_raw", twist_covar_matrix_[21], 100000.0);
  private_nh_->param<double>("twist_pitch", twist_covar_matrix_[28], 100000.0);
  private_nh_->param<double>("twist_yaw", twist_covar_matrix_[35], 0.01);

  odom_sub_ =
      nh_->subscribe(input_odom_topic, 10, &OdomCovariance::onOdom, this);
  odom_pub_ = nh_->advertise<nav_msgs::Odometry>(output_odom_topic, 10);
}

void OdomCovariance::onOdom(const nav_msgs::Odometry::ConstPtr& _msg) {
  nav_msgs::Odometry msg = *_msg;

  msg.pose.covariance = pose_covar_matrix_;
  msg.twist.covariance = twist_covar_matrix_;

  odom_pub_.publish(msg);
}

int main(int argc, char** argv) {
  ros::init(argc, argv, "odom_covariance_node");

  std::shared_ptr<ros::NodeHandle> nh =
      std::shared_ptr<ros::NodeHandle>(new ros::NodeHandle());
  std::shared_ptr<ros::NodeHandle> private_nh =
      std::shared_ptr<ros::NodeHandle>(new ros::NodeHandle("~"));

  OdomCovariance odom_covariance(nh, private_nh);

  ros::spin();

  return 0;
}

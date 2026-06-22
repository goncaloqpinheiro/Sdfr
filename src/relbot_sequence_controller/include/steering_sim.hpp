//=============================================================================
// Authors : Goncalo Pinheiro, Ceyhun Ceylan
// Group : 17
// License : LGPL open source license
//
// Brief : Declares the closed-loop ball-tracking controller for the RELbot.
//
//=============================================================================

#ifndef STEER_SIM_HPP_
#define STEER_SIM_HPP_

#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/msg/float64.hpp"
#include "geometry_msgs/msg/point.hpp"
#include "sensor_msgs/msg/image.hpp"

class SteerSim : public rclcpp::Node
{
public:
    SteerSim();

private:
    static constexpr double LOOP_HZ         = 30.0;
    static constexpr float K_V             = 1.8f;
    static constexpr float V_MIN           = 0.15f;
    static constexpr float V_MAX           = 1.50f;
    static constexpr float K_STEER         = 1.1f;
    static constexpr float OMEGA_MAX       = 0.70f;
    static constexpr float TURN_SLOWDOWN   = 0.45f;
    static constexpr float SEARCH_SPEED    = 0.55f;
    static constexpr float DEFAULT_WIDTH   = 240.0f;
    static constexpr float DEFAULT_TIMEOUT = 0.25f;
    static constexpr float DEFAULT_TARGET_AREA = 9000.0f;
    static constexpr float DEFAULT_CENTER_DEADBAND = 4.0f;

    double left_velocity_  = 0.0;
    double right_velocity_ = 0.0;
    bool   ball_seen_      = false;
    float  ball_x_         = DEFAULT_WIDTH * 0.5f;
    float  ball_y_         = 0.0f;
    float  ball_area_      = 0.0f;
    float  image_width_    = DEFAULT_WIDTH;
    float  target_ball_area_ = DEFAULT_TARGET_AREA;
    float  center_deadband_px_ = DEFAULT_CENTER_DEADBAND;
    double ball_timeout_   = DEFAULT_TIMEOUT;
    bool   sim_sign_fix_   = false;
    float  search_direction_ = 1.0f;
    rclcpp::Time last_ball_time_;

    rclcpp::Subscription<geometry_msgs::msg::Point>::SharedPtr ball_sub_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;
    rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr left_wheel_pub_;
    rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr right_wheel_pub_;
    rclcpp::TimerBase::SharedPtr timer_;

    void ball_callback(const geometry_msgs::msg::Point::SharedPtr msg);
    void image_callback(const sensor_msgs::msg::Image::SharedPtr msg);
    void calculate_velocity();
    void search_for_ball();
    void publish(float left, float right);
};

#endif

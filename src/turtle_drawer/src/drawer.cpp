#include <chrono>
#include <memory>
#include <thread>
#include <future>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/srv/teleport_absolute.hpp"
#include "turtlesim/srv/set_pen.hpp"
#include <turtlesim/msg/pose.hpp>
#include "rclcpp/parameter_client.hpp"

using namespace std::chrono_literals;
const double PI = 3.14159265358979323846;

class Drawer : public rclcpp::Node {
public:
    Drawer() : Node("drawer") {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "/turtle1/cmd_vel", 10
        );
        teleport_client_ = this->create_client<turtlesim::srv::TeleportAbsolute>(
            "/turtle1/teleport_absolute"
        );
        set_pen_client_ = this->create_client<turtlesim::srv::SetPen>(
            "/turtle1/set_pen"
        );
        pose_sub_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose", 10,
            [this](const turtlesim::msg::Pose::SharedPtr msg) {
                x_now = msg->x;
                y_now = msg->y;
                theta_now = msg->theta;
            }
        );
    }

    void teleport(double x, double y, double theta) {
        while (!teleport_client_->wait_for_service(1s)) {
            if (!rclcpp::ok()) {
                return;
            }
        }

        this->set_pen(true);
        auto request = std::make_shared<turtlesim::srv::TeleportAbsolute::Request>();

        request->x = x;
        request->y = y;
        request->theta = theta;

        auto future = teleport_client_->async_send_request(request);

        if (future.wait_for(2s) == std::future_status::ready) {
            auto response = future.get();
            (void)response;
        }

        this->set_pen(false);
    }

    void move(double linear, double angular, double duration) {
        auto msg = geometry_msgs::msg::Twist();

        msg.linear.x = linear;
        msg.angular.z = angular;

        auto start = std::chrono::steady_clock::now();
        auto end = start + std::chrono::duration<double>(duration);

        while (std::chrono::steady_clock::now() < end) {
            publisher_->publish(msg);
            std::this_thread::sleep_for(50ms);
        }

        stop();
    }

    void stop() {
        auto msg = geometry_msgs::msg::Twist();
        publisher_->publish(msg);
        std::this_thread::sleep_for(200ms);
    }

    void draw_circle(double radius, double cir){
        auto cmd = geometry_msgs::msg::Twist();

        cmd.linear.x = 1.0;
        cmd.angular.z = 1.0/radius;

        double duration = (2.0*PI)/cmd.angular.z;
        duration*=cir;

        auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < std::chrono::duration<double>(duration)) {
            publisher_->publish(cmd);
            std::this_thread::sleep_for(50ms);
        }
        cmd.linear.x = 0.0;
        cmd.angular.z= 0.0;
        publisher_->publish(cmd);
    }

    void set_pen(bool off) {    

        auto request = std::make_shared<turtlesim::srv::SetPen::Request>();

        request->r = 255;
        request->g = 255;
        request->b = 255;
        request->width = 6;
        request->off = off? 1 : 0;

        auto future = set_pen_client_->async_send_request(request);

        if (future.wait_for(2s) == std::future_status::ready) {
            auto response = future.get();
            (void)response;
        }
    }
    void draw_point(double x, double y){
        this->set_pen(true);
        this->teleport(x, y, 0.0);
        this->set_pen(false);    
        this->move(0.0000001, 0, 0.01);
    }
    double x_now = 0.0, y_now = 0.0, theta_now = 0.0;
private:

    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;
    rclcpp::Client<turtlesim::srv::TeleportAbsolute>::SharedPtr teleport_client_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_; 
    rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr set_pen_client_;  

};

void roda(std::shared_ptr<Drawer> drawer, double st_x, double st_y){
    drawer->teleport(st_x, st_y, 0.0);
    drawer->draw_circle(0.5, 1.0);
    drawer->draw_point(st_x, st_y + 0.5);

    drawer->teleport(st_x, st_y, 0.0);
    drawer->move(1.5, 0.0, 1.0);

    drawer->draw_circle(0.5, 1.0);
    drawer->draw_point(st_x + 1.5, st_y+0.5);

    drawer->teleport(st_x + 1.5, st_y, 0.0);
    drawer->move(1.5, 0.0, 1.0);

    drawer->draw_circle(0.5, 1.0);
    drawer->draw_point(st_x + 3.0, st_y+0.5);

    drawer->teleport(st_x + 3.0, st_y + 1.0, PI);
    drawer->move(3.0, 0.0, 1.0);
}

void body(std::shared_ptr<Drawer> drawer, double st_x, double st_y){
    drawer->teleport(st_x, st_y, 0);
    drawer->draw_circle(0.3, 0.25);
    drawer->teleport(drawer->x_now, drawer->y_now , PI/2);
    
    drawer->move(1.0, 0.0, 0.4);

    drawer->draw_circle(0.3, 0.25);
    drawer->teleport(drawer->x_now, drawer->y_now , PI);

    drawer->move(4.0, 0.0, 1.0);
    drawer->draw_circle(0.3, 0.25);
    drawer->teleport(drawer->x_now, drawer->y_now , -PI/2);

    drawer->move(1.0, 0.0, 0.4);
    drawer->draw_circle(0.3, 0.25);

}

void shell(std::shared_ptr<Drawer> drawer, double st_x, double st_y){
    drawer->teleport(st_x, st_y, PI/2);
    drawer->draw_circle(0.75, 0.5);
}

void arm(std::shared_ptr<Drawer> drawer, double st_x, double st_y){
    drawer->teleport(st_x, st_y, PI*0.75);
    drawer->move(1.5, 0.0, 1.0);
    drawer->draw_circle(0.15, 1.6);

    drawer->teleport(drawer->x_now, drawer->y_now, 0.0);
    drawer->move(1.6, 0.0, 1.0);
    drawer->teleport(drawer->x_now, drawer->y_now, PI*1.5);
    drawer->draw_circle(0.15, 0.25);
    drawer->teleport(drawer->x_now, drawer->y_now, 0.0);
    drawer->move(1.0, 0.0, 0.6);
    drawer->draw_circle(0.15, 0.25);
    drawer->teleport(drawer->x_now, drawer->y_now, PI/2);
    drawer->set_pen(true);
    drawer->move(1.0, 0.0, 0.3);
    drawer->set_pen(false);
    drawer->draw_circle(0.15, 0.25);
    drawer->teleport(drawer->x_now, drawer->y_now, PI);
    drawer->move(1.0, 0.0, 0.6);
    drawer->draw_circle(0.15, 0.25);
    drawer->teleport(drawer->x_now, drawer->y_now, PI*1.5);
    drawer->move(1.0, 0.0, 0.3);
    drawer->teleport(drawer->x_now, drawer->y_now, PI/2);
    drawer->move(1.0, 0.0, 0.3);
    drawer->teleport(drawer->x_now, drawer->y_now, PI);
    drawer->move(1.6, 0.0, 1.0);
    
    drawer->draw_circle(0.15, 0.35);
    drawer->teleport(drawer->x_now, drawer->y_now, PI*1.75);
    drawer->move(1.3, 0.0, 1.0);
    std::this_thread::sleep_for(500ms);
    drawer->teleport(1.0, 1.0, PI*1.5);

}
int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);

    auto drawer = std::make_shared<Drawer>();

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(drawer);

    std::thread spin_thread([&executor]() {
        executor.spin();
    });

    // Memberi waktu agar koneksi publisher terbentuk
    std::this_thread::sleep_for(1s);

    double st_x = 3.5, st_y = 3.5;
    std::this_thread::sleep_for(250ms);

    roda(drawer, st_x, st_y);
    body(drawer, st_x + 3.5, st_y + 0.5);
    shell(drawer, st_x + 2.25, st_y + 1.5);
    arm(drawer, st_x + 1.5, st_y + 2.25);
    
    rclcpp::shutdown();
    spin_thread.join();
    return 0;
}
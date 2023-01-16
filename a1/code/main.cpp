#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    float theta=MY_PI*rotation_angle/180.0;
    double cos_a=cosf(theta);
    double sin_a=sinf(theta);
    model(0,0)=cos_a;
    model(1,0)=sin_a;
    model(0,1)=-sin_a;
    model(1,1)=cos_a;
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.
    Eigen::Matrix4f P;
    P<<zNear,0,0,0,
       0,zNear,0,0,
       0,0,zNear+zFar,-zNear*zFar,
       0,0,1,0;
    
    Eigen::Matrix4f orth;
    float theta=MY_PI*eye_fov/180.0;
    theta/=2;
    float t=abs(zNear)*tanf(theta);
    float r=t*aspect_ratio;
    orth<<1/r,0,0,0,
          0,1/t,0,0,
          0,0,2/(zNear-zFar),0,
          0,0,0,1;
    projection=orth*P;
    return projection;
}

Eigen::Matrix4f get_rotation(Eigen::Vector3f axis,float angle){
    axis.normalize();
    float theta=MY_PI*angle/180.0;
    float cos_a=cosf(theta);
    float sin_a=sinf(theta);
    Eigen::Matrix3f I=Eigen::Matrix3f::Identity();
    Eigen::Matrix3f cross;
    cross<<0,-axis.z(),axis.y(),
           axis.z(),0,-axis.x(),
           -axis.y(),axis.x(),0;
    Eigen::Matrix3f M=I+(1-cos_a)*cross*cross+sin_a*cross;
    Eigen::Matrix4f x;
    x<<M(0,0),M(0,1),M(0,2),0,
       M(1,0),M(1,1),M(1,2),0,
       M(2,0),M(2,1),M(2,2),0,
       0,0,0,1;
    return x;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        //naive rotation
        //Eigen::Matrix4f rotation=get_model_matrix(angle);
        //bonus:Rodrigues equation
        Eigen::Matrix4f rotation=get_rotation({cosf(angle),sinf(angle),1}, fmod(frame_count, 360));
        r.set_model(rotation);
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}

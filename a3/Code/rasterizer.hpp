//
// Created by goksu on 4/6/19.
//

#pragma once

#include <eigen3/Eigen/Eigen>
#include <optional>
#include <algorithm>
#include "global.hpp"
#include "Shader.hpp"
#include "Triangle.hpp"

using namespace Eigen;

namespace rst
{
    struct SuperSampling{
        float d[4];
        void operator =(const SuperSampling& x){
            d[0]=x.d[0];
            d[1]=x.d[1];
            d[2]=x.d[2];
            d[3]=x.d[3];
        }
        SuperSampling(){d[0]=d[1]=d[2]=d[3]=0.0;}
        SuperSampling(float x,float y,float z,float w){
            d[0]=x;
            d[1]=y;
            d[2]=z;
            d[3]=w;
        }
    };
    struct ss_color{
        Eigen::Vector3f d[4];
        void operator =(const ss_color& x){
            d[0]=x.d[0];
            d[1]=x.d[1];
            d[2]=x.d[2];
            d[3]=x.d[3];
        }
        ss_color(){
            auto x=Vector3f{0,0,0};
            d[0]=d[1]=d[2]=d[3]=x;
        }
        ss_color(Vector3f a,Vector3f b,Vector3f c,Vector3f k){
            d[0]=a;
            d[1]=b;
            d[2]=c;
            d[3]=k;
        }
        Vector3f average(){
            return 0.25*d[0]+0.25*d[1]+0.25*d[2]+0.25*d[3];
        }
    };
    enum class Buffers
    {
        Color = 1,
        Depth = 2
    };

    inline Buffers operator|(Buffers a, Buffers b)
    {
        return Buffers((int)a | (int)b);
    }

    inline Buffers operator&(Buffers a, Buffers b)
    {
        return Buffers((int)a & (int)b);
    }

    enum class Primitive
    {
        Line,
        Triangle
    };

    /*
     * For the curious : The draw function takes two buffer id's as its arguments. These two structs
     * make sure that if you mix up with their orders, the compiler won't compile it.
     * Aka : Type safety
     * */
    struct pos_buf_id
    {
        int pos_id = 0;
    };

    struct ind_buf_id
    {
        int ind_id = 0;
    };

    struct col_buf_id
    {
        int col_id = 0;
    };

    class rasterizer
    {
    public:
        rasterizer(int w, int h);
        pos_buf_id load_positions(const std::vector<Eigen::Vector3f>& positions);
        ind_buf_id load_indices(const std::vector<Eigen::Vector3i>& indices);
        col_buf_id load_colors(const std::vector<Eigen::Vector3f>& colors);
        col_buf_id load_normals(const std::vector<Eigen::Vector3f>& normals);

        void set_model(const Eigen::Matrix4f& m);
        void set_view(const Eigen::Matrix4f& v);
        void set_projection(const Eigen::Matrix4f& p);

        void set_texture(Texture tex) { texture = tex; }

        void set_vertex_shader(std::function<Eigen::Vector3f(vertex_shader_payload)> vert_shader);
        void set_fragment_shader(std::function<Eigen::Vector3f(fragment_shader_payload)> frag_shader);

        void set_pixel(const Vector2i &point, const Eigen::Vector3f &color);

        void clear(Buffers buff);

        void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type);
        void draw(std::vector<Triangle *> &TriangleList);
        void flush();
        std::vector<Eigen::Vector3f>& frame_buffer() { return frame_buf; }

    private:
        void draw_line(Eigen::Vector3f begin, Eigen::Vector3f end);

        void rasterize_triangle(const Triangle& t, const std::array<Eigen::Vector3f, 3>& world_pos);

        // VERTEX SHADER -> MVP -> Clipping -> /.W -> VIEWPORT -> DRAWLINE/DRAWTRI -> FRAGSHADER

    private:
        Eigen::Matrix4f model;
        Eigen::Matrix4f view;
        Eigen::Matrix4f projection;

        int normal_id = -1;

        std::map<int, std::vector<Eigen::Vector3f>> pos_buf;
        std::map<int, std::vector<Eigen::Vector3i>> ind_buf;
        std::map<int, std::vector<Eigen::Vector3f>> col_buf;
        std::map<int, std::vector<Eigen::Vector3f>> nor_buf;

        std::optional<Texture> texture;

        std::function<Eigen::Vector3f(fragment_shader_payload)> fragment_shader;
        std::function<Eigen::Vector3f(vertex_shader_payload)> vertex_shader;

        std::vector<Eigen::Vector3f> frame_buf;
        std::vector<ss_color> ss_buffer;
        std::vector<SuperSampling> depth_buf;
        int get_index(int x, int y);

        int width, height;

        int next_id = 0;
        int get_next_id() { return next_id++; }
    };
}

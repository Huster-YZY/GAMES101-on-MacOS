//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    //meaningless
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}
Vector3f Scene::yzyshade(Intersection p,const Vector3f& wo)const{
    //sample the light
    Vector3f White(1.0);
    Intersection inter;
    float light_pdf;
    sampleLight(inter, light_pdf);
    //check block
    Vector3f t=normalize(inter.coords-p.coords);
    Ray ck(p.coords+t,t);
    auto it=intersect(ck);
    Vector3f L_dir(0.0);
    //error1:never interset light
    if(it.happened==true&&it.m->hasEmission()){
        Vector3f f_r=p.m->eval(t, wo, p.normal);
        double cos_theta=dotProduct(t, p.normal);
        double cos_light_theta=dotProduct(-t, it.normal);
        Vector3f ip=(inter.coords-p.coords);
        double norm=dotProduct(ip, ip);
        L_dir=it.m->m_emission*f_r*cos_theta*cos_light_theta/norm/light_pdf;
    }
    
    //sample randomly
    Vector3f L_indir(0.0);
    float prob=get_random_float();
    if(prob>RussianRoulette){
        Vector3f wi=normalize(p.m->sample(wo,p.normal));
        Ray rt(p.coords+wi,wi);
        auto it=intersect(rt);
        if(it.happened==true&&it.m->hasEmission()==false){
            Vector3f f_r=p.m->eval(wi, wo, p.normal);
            double cos_theta=dotProduct(wi, p.normal);
            double pdf=p.m->pdf(wi, wo, p.normal);
            L_indir=yzyshade(it, -wi)*f_r*cos_theta/pdf/RussianRoulette;
        }
    }
    
    return L_dir+L_indir;
}
// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    auto inter=intersect(ray);
    if(inter.happened==true){
        if(inter.m->hasEmission())
            return Vector3f(1.0);
        else
            return yzyshade(inter, -ray.direction);
    }
    else
        return Vector3f(0.0);
    
}

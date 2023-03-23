#include <iostream>
#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL {

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
    {
        // TODO (Part 1): Create a rope starting at `start`, ending at `end`, and containing `num_nodes` nodes.
        Vector2D d=(start-end)/(num_nodes-1);
        for(int i=0;i<num_nodes;i++){
            Mass* p=new Mass(start+i*d,node_mass,false);
            masses.push_back(p);
        }
        for(int i=0;i+1<num_nodes;i++){
            Spring* p=new Spring(masses[i],masses[i+1],k);
            springs.push_back(p);
        }
        for (auto &i : pinned_nodes) {
            masses[i]->pinned = true;
        }
    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            // TODO (Part 2): Use Hooke's law to calculate the force on a node
            Vector2D ab=s->m2->position-s->m1->position;
            Vector2D f=s->k*ab.unit()*(ab.norm()-s->rest_length);
            s->m1->forces+=f;
            s->m2->forces+=-f;
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                // TODO (Part 2): Add the force due to gravity, then compute the new velocity and position
                Vector2D F=m->forces+gravity-0.005*m->velocity;
                Vector2D a=F/m->mass;
                m->velocity+=delta_t*a;
                m->position+=delta_t*m->velocity;
            }

            // Reset all forces on each mass
            m->forces = Vector2D(0, 0);
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            // TODO (Part 3): Simulate one timestep of the rope using explicit Verlet （solving constraints)
            Vector2D ab=s->m2->position-s->m1->position;
            Vector2D f=s->k*ab.unit()*(ab.norm()-s->rest_length);
            s->m1->forces+=f;
            s->m2->forces+=-f;
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                Vector2D F=m->forces+gravity-0.005*m->velocity;
                Vector2D a=F/m->mass;
                Vector2D tmp_position=m->position;
                m->position=m->position+(1-0.00005)*(m->position-m->last_position)+a*delta_t*delta_t;
                m->last_position=tmp_position;
            }
            m->forces = Vector2D(0, 0);
        }
    }
}

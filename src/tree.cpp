#include "include/objects/tree.hpp"
#include <iostream>

vita2d_texture* Tree::leaf = nullptr;

Tree::Tree(int x, int y, int layer) : Object(Rect(x, y, 32, 34), ObjectType::TREE, layer) {
    frame=rand()%4;
    init_all_states();
    change(NORMAL);
    
}

void Tree::init_all_states() {
    all_states.push_back(TreeStateData(TreeState::NORMAL));
    all_states.push_back(TreeStateData(TreeState::SELECT));
    all_states.push_back(TreeStateData(TreeState::HIT, 4, 0.1f,true));
    all_states.push_back(TreeStateData(TreeState::CUT,7,0.1f,true));
}

void Tree::draw() {
    if(leaf_frame==-1)return;
    auto [left, right, top, bottom] = rect.get_position();
    const int zoom = Rect::get_zoom();
    vita2d_draw_texture_part_scale(leaf, left-zoom*10, top,47*leaf_frame, 0, 47, 32, Rect::get_zoom(), Rect::get_zoom());
}

int skipLine(int nb){
    return nb*64;
}

// Mise à jour de l'animation
void Tree::update() {
    if(leaf_frame!=-1){
        leaf_time+=1.0f / 60.0f;
        if(leaf_time>0.1f){
            leaf_time=0;
            leaf_frame++;
            if(leaf_frame==11) leaf_frame=-1;
        }
    }

    if(current_state.nb_frame==0 || (current_state.state==CUT && frame==current_state.nb_frame))return;
    
    time+=1.0f / 60.0f;
    if(time>current_state.frame_duration){
        time=0;
        frame++;
        if(frame==current_state.nb_frame){
            if(current_state.blocking){
                change(NORMAL);  
            }
            if(current_state.state!=CUT){
                frame=0;
            }
        }
    }
}

void Tree::interaction() {
    if(current_state.blocking) return;
    leaf_frame=0;
    nb_hit++;
    if(nb_hit==4){
        change(CUT);
        return;
    }
    change(HIT);
}

//true = right, false = left
bool Tree::can_interact(Rect player_rect, bool direction) const {
    if(current_state.blocking || current_state.state==CUT) return false;
    const std::pair<int, int> collisionResult = player_rect.collision(rect);
    const bool can_interact = collisionResult.first != 0 || collisionResult.second != 0;
    if (!can_interact) return false;
    auto [left, right, top, bottom] = rect.get_position();
    auto [left_p, right_p, top_p, bottom_p] = player_rect.get_position();
    if(direction){
        if(left_p>left) return false;
        return true;
    }else{
        if(right_p<right) return false;
        return true;
    }
}

void Tree::change(TreeState state) {
    if(current_state.blocking && frame!=current_state.nb_frame) return;
    if(current_state.state==CUT) return;
    if(all_states[state].blocking) frame=0;
    current_state = all_states[state];
}

int Tree::draw_tile(int id) {
    bool leef = tree_leef.find(id) != tree_leef.end();
    
    if(this->current_state.state==NORMAL || this->current_state.state==HIT){
        if(!leef) return id;
    }else if(this->current_state.state==SELECT){
        if(!leef) return id+2;
    }else if(this->current_state.state==CUT){
        if(!leef) return id+4;
    }
    
    return id+skipLine((current_state.state*2)+4)+frame*2;
}

Rect Tree::get_rect_collision() const {
    return rect;
}
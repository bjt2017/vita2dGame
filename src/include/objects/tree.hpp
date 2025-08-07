#pragma once
#include "../objects.hpp"
#include <assets.h>

const std::unordered_set<int> tree_leef = {2498, 2499, 2562, 2563};

enum TreeState{
    NORMAL,
    SELECT,
    HIT,
    CUT
};

struct TreeStateData : public StateObject<TreeState>{
    bool blocking = false; //play to the end of the animation before changing state
    TreeStateData(TreeState state = TreeState::NORMAL, int nb_frame = 4, float frame_duration = 0.2f, bool blocking = false)
        : StateObject<TreeState>(state, nb_frame, frame_duration) {this->blocking = blocking;}
};

class Tree : public Object<TreeStateData, TreeState> {
    protected :
        int nb_hit=0;
        float time=0;

        int leaf_frame = -1;
        float leaf_time = 0;
        static vita2d_texture* leaf;
    
    public :
        Tree(int x, int y, int layer = 1);
        static void load_assets(){
            if (!leaf) {
                leaf = vita2d_load_PNG_buffer(&_binary_assets_Tree_leaf_png_start);
                if (!leaf) {
                    Console::warning("Failed to load tree leaf texture.");
                };
            }
        }
        static void free_assets() {
            if (leaf) {
                vita2d_free_texture(leaf);
                leaf = nullptr;
            }
        }
        void init_all_states() override;
        void draw() override;
        void update() override;
        void interaction() override;
        bool can_interact(Rect rect, bool direction) const;
        void change(TreeState state);
        int draw_tile(int id);
        Rect get_rect_collision() const override;
};
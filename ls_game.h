#ifndef LS_GAME_H_
#define LS_GAME_H_

#include "raycasting_engine.h"

class SpriteLogic {
public:
	SpriteLogic(SDL_Renderer* renderer);
	
	static_texture_data_t* current_weapon_texture;
	
	SDL_Surface* forest_wall_src;
	SDL_Texture* forest_wall;
	
	SDL_Surface* forest_floor_src;
	SDL_Texture* forest_floor;
	static_texture_data_t forest_floor_texdata;
	
	SDL_Surface* sky_src;
	SDL_Texture* sky;
	
	SDL_Surface* pistol_src;
	vector<static_texture_data_t> pistol_idle;
	vector<static_texture_data_t> pistol_reload;
	vector<static_texture_data_t> pistol_shoot;
	vector <bool> pistol_alpha_pixel;
	int texture_timer;
	int texture_index;
	void determine_current_weapon (SDL_Renderer* renderer, Player& p);
	void draw_current_weapon(SDL_Renderer* renderer);
	void texture_handling(SDL_Renderer* renderer, vector<static_texture_data_t>& vec, int max_timer, bool& condition);
	
};

#endif
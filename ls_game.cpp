#include "ls_game.h"

/* CLASS SPRITELOGIC */
SpriteLogic::SpriteLogic(SDL_Renderer* renderer) {
	current_weapon_texture = NULL;
	
	texture_timer = 0;
	texture_index = 0;
	
	forest_wall_src = SDL_LoadBMP("res/walls/1.bmp");
	forest_wall = SDL_CreateTextureFromSurface(renderer, forest_wall_src);
	
	forest_floor_src = SDL_LoadBMP("res/floors/1.bmp");
	forest_floor = SDL_CreateTextureFromSurface(renderer, forest_floor_src);
	forest_floor_texdata = Textures::load_static_texture(0, 0, 64, 64, 0, renderer, forest_floor_src);
	
	sky_src = SDL_LoadBMP("res/sky.bmp");
	sky = SDL_CreateTextureFromSurface(renderer, sky_src);
	
	
	/* pistol */
	pistol_src = SDL_LoadBMP("res/pistol.bmp");
	//idle
	pistol_idle.push_back(Textures::load_static_texture(122, 153, 56, 69, Data::render_size_x/2 + 20, renderer, pistol_src));
	//reload
	pistol_reload.push_back(Textures::load_static_texture(18, 13, 61, 80, Data::render_size_x/2 + 20, renderer, pistol_src));
	pistol_reload.push_back(Textures::load_static_texture(90, 10, 148, 98, Data::render_size_x/2, renderer, pistol_src));
	pistol_reload.push_back(Textures::load_static_texture(251, 11, 142, 98, Data::render_size_x/2, renderer, pistol_src));
	pistol_reload.push_back(Textures::load_static_texture(407, 11, 135, 99, Data::render_size_x/2, renderer, pistol_src));
	pistol_reload.push_back(Textures::load_static_texture(19, 123, 70, 99, Data::render_size_x/2 + 20, renderer, pistol_src));
	//shoot
	pistol_shoot.push_back(Textures::load_static_texture(209, 141, 70, 80, Data::render_size_x/2 + 20, renderer, pistol_src));
	pistol_shoot.push_back(Textures::load_static_texture(302, 140, 72, 84, Data::render_size_x/2 + 20, renderer, pistol_src));
	pistol_shoot.push_back(Textures::load_static_texture(391, 123, 58, 98, Data::render_size_x/2 + 20, renderer, pistol_src));
	pistol_shoot.push_back(Textures::load_static_texture(473, 126, 80, 77, Data::render_size_x/2 + 20, renderer, pistol_src));
	pistol_shoot.push_back(Textures::load_static_texture(602, 142, 97, 68, Data::render_size_x/2 + 20, renderer, pistol_src));
}
void SpriteLogic::determine_current_weapon (SDL_Renderer* renderer, Player& p) {
	
	if (p.shooting) {
		texture_handling(renderer, pistol_shoot, Animations::pistol_shoot_timer, p.shooting);
		
	} else if (p.reloading) {
		texture_handling(renderer, pistol_reload, Animations::pistol_reload_timer, p.reloading);
		
	} else {
		current_weapon_texture = &pistol_idle[0];
	}
}

void SpriteLogic::draw_current_weapon(SDL_Renderer* renderer) {
	SDL_RenderCopy(renderer, current_weapon_texture->texture, NULL, &current_weapon_texture->pos);
}

void SpriteLogic::texture_handling(SDL_Renderer* renderer, vector<static_texture_data_t>& vec, int max_timer, bool& condition) {
	
	if (texture_timer == max_timer) {
		texture_index++; 
		texture_timer = 0;
	}
	if (texture_index == vec.size() - 1) {
		current_weapon_texture = &vec[texture_index];
		texture_timer = 0;
		texture_index = 0;
		condition = false;
		
	} else {
		current_weapon_texture = &vec[texture_index];
		texture_timer++;
	}
	

}
#ifndef RAYCASTING_ENGINE_H
#define RAYCASTING_ENGINE_H

#include <sstream>
#include <vector>
#include <math.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <dirent.h>

using namespace std;


class Data {
public:
	
	
	static const int render_size_x = 320; //320;
	static const int render_size_y = 200; //200;
	
	static bool shader_activated;
	
	static void preload_math_vars();
	static double get_cos_val(double val);
	static double get_sin_val(double val);
	static double get_tan_val(double val);
	
};


class Field {
public:
	vector<Uint32>* floor_color_values;
	SDL_Texture* wall_texture;
	int size;
	static const int width = 64;
	static const int height = 64;
	Field() {
		size = 0;
	}
};

typedef struct intersection_t {
	
	bool operator==(const intersection_t& rd) const {
		return field_pos.x == rd.field_pos.x && field_pos.y == rd.field_pos.y;
	}
	
	bool operator<(const intersection_t& rd) const {
		return distance < rd.distance;
	}
	SDL_Rect field_pos;
	double x_pos;
	double distance;
	Field* field;
	bool horizontal_wall;
	
}intersection_t;

class Raydata {
public:
	static intersection_t intersections[];
	
};


class Map {
public:
	vector<SDL_Texture*> wall_textures;
	vector<vector<Uint32> > floor_textures;
	
	static int field_num_x;
	static int field_num_y;
	//Field map[30][30];
	Field** map;
	Map();
	void load_map(SDL_Renderer* renderer, string map_path);
	void load_map_textures(SDL_Renderer* renderer);
};


class Player {
public:
	static int height;
	static const int plane_x = Data::render_size_x;
	static int plane_y;
	static const int field_of_view = 60;
	static const int view_depth = 40;
	static const int view_distance = 2600;
	
	Player();
	
	static bool jumping;
	int jump_height;
	bool first_jump_join;
	bool max_jump_height_reached;
	bool shooting;
	bool reloading;
	bool key_w;
	bool key_a;
	bool key_s;
	bool key_d;
	int dist_player_to_plane;
	double pos_x;
	double pos_y;
	double angle;
	int last_direction;
	int jump_intervall;
	
	
	void move(Map& m);
	bool check_wall_collision(double x, double y, Map& m);
	void handle_jumping(Map &m);
};


class Debugger {
public:

	static int fps_count;
	static long time_stamp;
	static long time_stamp_old;
	static long ms_per_frame;
	static long current_frame;
	static TTF_Font* font; 
	static void draw_info(SDL_Renderer* renderer, int fps_value, int ms_per_frame_value, int delay_value);
	
};



class Animations {
public:
	static const int pistol_reload_timer = 2;
	static const int pistol_shoot_timer = 1;
	
};

typedef struct static_texture_data_t {
	
	SDL_Rect pos;
	SDL_Texture* texture;
	vector<bool> alpha_values;
	vector<Uint32> color_values;
	Uint32 format;
	
}static_texture_data_t;


class Textures {
public:
	
	Textures(SDL_Renderer* r);
	//static void load_texture(int x, int y, int w, int h, int tex_pos_x, SDL_Renderer* renderer, string path, vector<SDL_Texture*>& vec, unsigned int color_key);
	static static_texture_data_t load_static_texture(int x, int y, int w, int h, int tex_pos_x, SDL_Renderer* renderer, SDL_Surface* s);
	static Uint32 load_color_info(SDL_Renderer* renderer, SDL_Surface* surface, SDL_Texture* tex, vector<bool>& alpha_vals, vector<Uint32>& color_vals);
	SDL_Color get_texture_color(vector<SDL_Color>& vec, SDL_Rect* rect);
	void shade_pixel(Uint32* pixel, int distance);
	
};



#endif
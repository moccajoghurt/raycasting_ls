#include "raycasting_engine.h"
/* CLASS DATA */
/*
void Data::preload_math_vars() {
	long absolute = 0;
	for (double d = 0; d < 360; d+= 0.0001) {
		cos_vals[absolute] = cos(d * M_PI / 180);
		sin_vals[absolute] = sin(d * M_PI / 180);
		if (d * M_PI / 180 == 90) {tan_vals[absolute] = tan(90.1);}
		else if (d * M_PI / 180 == 270) {tan_vals[absolute] = tan(270.1);}
		else {tan_vals[absolute] = tan(d * M_PI / 180);}
		absolute++;
	}
	
}*/

double Data::get_cos_val(double val) {
	double buf = val < 0 ? -val : val;
	//double d = cos_vals[(long)(buf*10000)];
	double d = cos(buf * M_PI / 180);
	return d;
}

double Data::get_sin_val(double val) {
	double buf = val < 0 ? -val : val;
	//double d = sin_vals[(long)(buf*10000)];
	double d = sin(buf * M_PI / 180);
	return d;
}

double Data::get_tan_val(double val) {
	double buf = val < 0 ? -val : val;
	//double d = tan_vals[(long)(buf*10000)];
	double d = tan(buf * M_PI / 180);
	if (buf * M_PI / 180 == 90) {d = tan(90.1 * M_PI / 180);}
	else if (buf * M_PI / 180 == 270) {d = tan(270.1 * M_PI / 180);}
	if (val < 0) return -d;
	else return d;
}


/* CLASS PLAYER */
int Player::plane_y = 200;
int Player::height = 40;
bool Player::jumping = false;
Player::Player() {
	first_jump_join = true;
	jump_height = 40;
	jumping = false;
	shooting = false;
	reloading = false;
	key_w = false;
	key_a = false;
	key_s = false;
	key_d = false;
	angle = 270;
	pos_x = Field::width * 4 + Field::width/2;
	pos_y = Field::height * 4 + Field::height/2;
	last_direction = 0;
}

void Player::handle_jumping(Map &m) {
	
	if (jumping == true) {
		
		if (first_jump_join) {
			jump_height = m.map[(int)pos_x/Field::width][(int)pos_y/Field::height].size + 100;
			first_jump_join = false;
		}
		if (height < jump_height && !max_jump_height_reached) {
			height += Debugger::ms_per_frame/3;
			
		} else if (height > m.map[(int)pos_x/Field::width][(int)pos_y/Field::height].size + 40){
			max_jump_height_reached = true;
			height -= Debugger::ms_per_frame/2;
			if (height < m.map[(int)pos_x/Field::width][(int)pos_y/Field::height].size + 40) height = m.map[(int)pos_x/Field::width][(int)pos_y/Field::height].size + 40;
		} else {
			max_jump_height_reached = false;
			first_jump_join = true;
			jumping = false;
		}
		
	}
	
}

void Player::move(Map& m) {
	SDL_Rect new_pos;
	double new_pos_x;
	double new_pos_y;
	if (key_w) {
		new_pos_x = this->pos_x - (Debugger::ms_per_frame/4) * cos((this->angle) * M_PI / 180);
		new_pos_y = this->pos_y - (Debugger::ms_per_frame/4) * sin((this->angle) * M_PI / 180);
		if (!this->check_wall_collision(new_pos_x, new_pos_y, m)) {
			this->pos_x = new_pos_x;
			this->pos_y = new_pos_y;
		}
	}
	if (key_a) {
		new_pos_x = this->pos_x + (Debugger::ms_per_frame/4) * cos((270 - this->angle) * M_PI / 180);
		new_pos_y = this->pos_y + (Debugger::ms_per_frame/4) * sin((90 - this->angle) * M_PI / 180);
		if (!this->check_wall_collision(new_pos_x, new_pos_y, m)) {
			this->pos_x = new_pos_x;
			this->pos_y = new_pos_y;
		}
	}
	if (key_s) {
		new_pos_x = this->pos_x + (Debugger::ms_per_frame/4) * cos((this->angle) * M_PI / 180);
		new_pos_y = this->pos_y + (Debugger::ms_per_frame/4) * sin((this->angle) * M_PI / 180);
		if (!this->check_wall_collision(new_pos_x, new_pos_y, m)) {
			this->pos_x = new_pos_x;
			this->pos_y = new_pos_y;
		}
	}
	if (key_d) {
		new_pos_x = this->pos_x + (Debugger::ms_per_frame/4) * cos((90 - this->angle) * M_PI / 180);
		new_pos_y = this->pos_y + (Debugger::ms_per_frame/4) * sin((270 - this->angle) * M_PI / 180);
		if (!this->check_wall_collision(new_pos_x, new_pos_y, m)) {
			this->pos_x = new_pos_x;
			this->pos_y = new_pos_y;
		}
	}
	
}

bool Player::check_wall_collision(double x, double y, Map& m) {
	
	if (x >= 0 && x/Field::width < Map::field_num_x && y >= 0 && y/Field::height < Map::field_num_y) {
		
		if (Player::jumping) {
			
			if (Player::height >= m.map[(int)x/Field::width][(int)y/Field::height].size + 40) {
				return false;
			} else {
				return true;
			}
			
			
		} else {
		
			if (Player::height < m.map[(int)x/Field::width][(int)y/Field::height].size + 30) {
				
				return true ;
			}
			else {
				Player::height = m.map[(int)x/Field::width][(int)y/Field::height].size + 40;
				//Player::jumping = false;
				//Player::jump_counter = 0;
				return false;
			}
			
		}
		
	} else {
		return true;
	}
}

/*CLASS MAP*/
int Map::field_num_x;
int Map::field_num_y;

Map::Map() {
	
}

void Map::load_map_textures(SDL_Renderer* renderer) {
	
	DIR *dir;
	struct dirent *ent;
	// floors
	if ((dir = opendir ("./res/floors/")) != NULL) {
		while ((ent = readdir (dir)) != NULL) {
			if (*ent->d_name == '.' || *ent->d_name == 'u') continue;
			SDL_Surface* floor_src = SDL_LoadBMP( string(string("./res/floors/") + string(ent->d_name)).c_str() );
			static_texture_data_t floor_texdata = Textures::load_static_texture(0, 0, 64, 64, 0, renderer, floor_src);
			floor_textures.push_back(floor_texdata.color_values);
		}
		closedir (dir);
	} else {
		cout << "couldn't open floors directory" << endl;
	}
	
	//walls
	if ((dir = opendir ("./res/walls/")) != NULL) {
		while ((ent = readdir (dir)) != NULL) {
			if (*ent->d_name == '.' || *ent->d_name == 'u') continue;
			SDL_Surface* wall_src = SDL_LoadBMP( string(string("./res/walls/") + string(ent->d_name)).c_str() );
			SDL_Texture* wall_tex = SDL_CreateTextureFromSurface(renderer, wall_src);
			wall_textures.push_back(wall_tex);
		}
		closedir (dir);
	} else {
		cout << "couldn't open walls directory" << endl;
	}
	
	//todo: sprites
}

void Map::load_map(SDL_Renderer* renderer, string map_path) {
	SDL_Surface* map_src = SDL_LoadBMP(map_path.c_str());
	
	static_texture_data_t map_texdata = Textures::load_static_texture(0, 0, map_src->w, map_src->h, 0, renderer, map_src);
	
	field_num_x = map_src->w;
	field_num_y = map_src->h;
	
	
	map = new Field*[field_num_x];
	for (int i = 0; i < field_num_x; i++) {
		map[i] = new Field[field_num_y];
	}
	
	
	int pixel_count = 0;
	for (int x = 0; x < field_num_x; x++) {
		for (int y = 0; y < field_num_y; y++) {
			
			Uint32 pixel = map_texdata.color_values[pixel_count];
			Uint32 temp;
			Uint8 red, green, blue;
			
			//red
			red = pixel >> 24;
			
			//green
			temp = pixel << 8;
			green = temp >> 24;
			
			//blue
			temp = pixel << 16;
			blue = temp >> 24;
			
			if (green == 0) {
				map[x][y].floor_color_values = &floor_textures[0];
				map[x][y].wall_texture = wall_textures[0];
			} else if (green == 1) {
				map[x][y].floor_color_values = &floor_textures[1];
				map[x][y].wall_texture = wall_textures[1];
			}
			
			map[x][y].size = blue;
			
			
			
			/*
			SDL_Surface* forest_wall_src = SDL_LoadBMP("res/walls/1.bmp");
			SDL_Texture* forest_wall = SDL_CreateTextureFromSurface(renderer, forest_wall_src);
			
			SDL_Surface* forest_floor_src = SDL_LoadBMP("res/floors/1.bmp");
			SDL_Texture* forest_floor = SDL_CreateTextureFromSurface(renderer, forest_floor_src);
			
			map[x][y].wall_texture = forest_wall;
			
			vector<Uint32>* floor_color_vals = new vector<Uint32>();
			Textures::load_color_info(renderer, forest_floor_src, forest_floor, dummy, *floor_color_vals);
			
			map[x][y].floor_color_values = floor_color_vals;
			if (x == field_num_x -1 || x == 0 || y == 0 || y == field_num_y -1) {
				map[x][y].size = 128;
			}
			*/
			
			pixel_count++;
		}
	}
	
}


/* CLASS DEBUGGER */
int Debugger::fps_count = 0;
long Debugger::time_stamp = 0;
long Debugger::time_stamp_old = 0;
long Debugger::ms_per_frame = 50;
long Debugger::current_frame = 0;
TTF_Font* Debugger::font = NULL;
void Debugger::draw_info(SDL_Renderer* renderer, int fps_value, int ms_per_frame_value, int delay_value) {
	SDL_Color txt_color = {255, 0, 0};
	SDL_Surface* fps;
	stringstream ss;
	ss << "fps: " << fps_value;
	fps = TTF_RenderText_Solid( Debugger::font, ss.str().c_str() , txt_color );
	SDL_Rect fps_pos;
	fps_pos.x = Player::plane_x - 100;
	fps_pos.y = 10;
	fps_pos.w = fps->w;
	fps_pos.h = fps->h;
	SDL_Texture* tex_fps = SDL_CreateTextureFromSurface(renderer, fps);
	SDL_RenderCopy(renderer, tex_fps, NULL, &fps_pos);
	
	SDL_Surface* ms_per_frame;
	ss.str("");
	ss << "ms per frame: " << ms_per_frame_value;
	ms_per_frame = TTF_RenderText_Solid( Debugger::font, ss.str().c_str() , txt_color );
	SDL_Rect mpf_pos;
	mpf_pos.x = Player::plane_x - 100;
	mpf_pos.y = 25;
	mpf_pos.w = ms_per_frame->w;
	mpf_pos.h = ms_per_frame->h;
	SDL_Texture* tex_mpf = SDL_CreateTextureFromSurface(renderer, ms_per_frame);
	SDL_RenderCopy(renderer, tex_mpf, NULL, &mpf_pos);
	
	
	SDL_Surface* delay_frame;
	ss.str("");
	ss << "game delay: " << delay_value;
	delay_frame = TTF_RenderText_Solid( Debugger::font, ss.str().c_str() , txt_color );
	SDL_Rect delay_pos;
	delay_pos.x = Player::plane_x - 100;
	delay_pos.y = 40;
	delay_pos.w = delay_frame->w;
	delay_pos.h = delay_frame->h;
	SDL_Texture* tex_delay = SDL_CreateTextureFromSurface(renderer, delay_frame);
	SDL_RenderCopy(renderer, tex_delay, NULL, &delay_pos);
	
	SDL_DestroyTexture(tex_fps);
	SDL_DestroyTexture(tex_mpf);
	SDL_DestroyTexture(tex_delay);
	SDL_FreeSurface(fps);
	SDL_FreeSurface(ms_per_frame);
	SDL_FreeSurface(delay_frame);
}


/* CLASS TEXTURES */
Textures::Textures(SDL_Renderer* renderer) {
	
	
}
/*
void Textures::load_texture(int x, int y, int w, int h, int tex_pos_x, SDL_Renderer* renderer, string path, vector<SDL_Texture*>& vec, unsigned int color_key) {
	SDL_Surface* src = SDL_LoadBMP(path.c_str());
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	pos.w = w;
	pos.h = h;
	SDL_Surface* temp = SDL_CreateRGBSurface(0, pos.w, pos.h, 32, 0, 0, 0, 0);
	SDL_BlitSurface(src, &pos, temp, NULL);
	SDL_SetColorKey(temp, SDL_TRUE, color_key);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, temp);
	vec.push_back(tex);
}*/



static_texture_data_t Textures::load_static_texture(int x, int y, int w, int h, int tex_pos_x, SDL_Renderer* renderer, SDL_Surface* s) {
	//idle
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	pos.w = w;
	pos.h = h;
	SDL_Surface* temp = SDL_CreateRGBSurface(0, pos.w, pos.h, 32, 0, 0, 0, 0);
	SDL_BlitSurface(s, &pos, temp, NULL);
	SDL_SetColorKey(temp, SDL_TRUE, 0x00FFFF);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, temp);
	SDL_Rect tex_pos;
	tex_pos.w = pos.w;
	tex_pos.h = pos.h;
	tex_pos.x = tex_pos_x;
	tex_pos.y = Data::render_size_y - h;
	vector<bool> alpha_vals;
	vector<Uint32> color_vals;
	Uint32 format = load_color_info(renderer, temp, tex, alpha_vals, color_vals);
	static_texture_data_t td = {tex_pos, tex, alpha_vals, color_vals, format};
	return td;
	
}

Uint32 Textures::load_color_info(SDL_Renderer* renderer, SDL_Surface* surface, SDL_Texture* tex, vector<bool>& alpha_vals, vector<Uint32>& color_vals) {
	Uint32 format;
	int access, w, h;
	SDL_QueryTexture(tex, &format, &access, &w, &h);
	//SDL_SetRenderTarget(renderer, tex);
	
	int pixel_count = 0;
	for (int i = 0; i < w; i++) {
		for (int n = 0; n < h; n++) {
			
			SDL_Color c;
			
			SDL_PixelFormat* fmt;
			Uint32 temp, pixel;
			Uint8 red, green, blue, alpha;
			
			fmt = surface->format;
			SDL_LockSurface(surface);
			
			pixel = *((Uint32*)surface->pixels + pixel_count);
			
			//red
			temp = pixel & fmt->Rmask;
			temp = temp >> fmt->Rshift;
			temp = temp << fmt->Rloss;
			red = (Uint8)temp;
			
			//green
			temp = pixel & fmt->Gmask;
			temp = temp >> fmt->Gshift;
			temp = temp << fmt->Gloss;
			green = (Uint8)temp;
			
			//blue
			temp = pixel & fmt->Bmask;
			temp = temp >> fmt->Bshift;
			temp = temp << fmt->Bloss;
			blue = (Uint8)temp;
			
			//alpha
			temp = pixel & fmt->Amask;
			temp = temp >> fmt->Ashift;
			temp = temp << fmt->Aloss;
			alpha = (Uint8)temp;
			
			c.r = red;
			c.g = green;
			c.b = blue;
			c.a = alpha;
			
			if (c.r == 0 && c.g == 255 && c.b == 255) { alpha_vals.push_back(true);}
			else { alpha_vals.push_back(false); }
			
			color_vals.push_back(pixel);
			
			pixel_count++;
		}
	}
	return format;
}

/* unused, no performance boost */
SDL_Color Textures::get_texture_color(vector<SDL_Color>& vec, SDL_Rect* rect) {
	SDL_Color c = vec[rect->y * Field::width + rect->x];
	return c;
}

void Textures::shade_pixel(Uint32* pixel, int distance) {
	
	Uint32 temp;
	Uint8 red, green, blue, alpha;
	
	//red
	red = *pixel >> 24;
	
	//green
	temp = *pixel << 8;
	green = temp >> 24;
	
	//blue
	temp = *pixel << 16;
	blue = temp >> 24;
	
	//alpha
	temp = *pixel << 24;
	alpha = temp >> 24;
	
	
	double old_range = Player::view_distance - 1;
	double new_range = 3; 
	double dist_weight = (((distance - 1) * new_range) / old_range) + 1;
	
	//cout <<  NewValue << endl;
	
	
	//double distance_weight = distance / 100 < 1 ? 1 : distance / 500;
	
	red /= dist_weight;
	green /= dist_weight;
	blue /= dist_weight;
	alpha /= dist_weight;

	
	//red
	temp = red;
	temp <<= 24;
	*pixel = temp;
	
	//green
	temp = green;
	temp <<= 16;
	*pixel += temp;
	
	//blue
	temp = blue;
	temp <<= 8;
	*pixel += temp;
	
	//alpha
	temp = alpha;
	*pixel += temp;
	
}

/* static vars inline classes*/
intersection_t Raydata::intersections[Player::view_depth*2];
bool Data::shader_activated = false;
#include <iostream>
#include <math.h>
#include <vector>
#include <ctime>
#include <cstring> // memset
#include <algorithm> //sort
#include "raycasting_engine.h"
#include "ls_game.h"

/* sky width has to be a multiple of 360*2 (angle)*/
#define SKY_WIDTH 720
#define SKY_HEIGHT 576

using namespace std;

void cast_rays(Player& p, Map& m, Textures& t, SDL_Renderer* renderer);
void handle_input (SDL_Event& e, Player& p);
void draw_sky(SDL_Renderer* renderer, Player& p, SDL_Texture* t);

//todo: nach bestimmter distance aufhören intersections zu berechnen

int main(int argc, char** argv) {
	
	
	//Data::preload_math_vars();
	
	Debugger db;
	TTF_Init();
	Debugger::font = TTF_OpenFont( "res/txt.otf", 12 );
	
	SDL_Window *window = NULL;
	SDL_Surface *surface = NULL;
	SDL_Renderer *renderer = NULL;
	
	
	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
		cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
		return 1;
	}
	
	SDL_DisplayMode dm;
	SDL_GetDesktopDisplayMode(0, &dm);
	int screen_multiplier = 3;
	while (screen_multiplier * Data::render_size_y > dm.h) screen_multiplier--;
	screen_multiplier = screen_multiplier < 1 ? 1 : screen_multiplier;
	
	window = SDL_CreateWindow( "intersection test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Data::render_size_x * screen_multiplier, Data::render_size_y * screen_multiplier, /*SDL_WINDOW_FULLSCREEN*/ SDL_WINDOW_SHOWN);
	if( window == NULL ) {
		cout <<  "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
		return 1;
	}
	
	
	surface = SDL_GetWindowSurface(window);
	SDL_Event e;
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	//SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	
	Textures t(renderer);
	SpriteLogic sl(renderer);
	
	Player p;
	p.dist_player_to_plane = Player::plane_x / 2 / tan(30 * M_PI / 180);
	Map m;
	m.load_map_textures(renderer);
	m.load_map(renderer, "res/maps/map.bmp");
	
	
	SDL_ShowCursor(SDL_FALSE);
	SDL_WarpMouseInWindow(window, Data::render_size_x/2, Data::render_size_y/2);
	int game_delay = 20;
	while (true) {
		
		handle_input(e, p);
		p.move(m);
		p.handle_jumping(m);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		draw_sky(renderer, p, sl.sky);
		
		//Debugger::draw_debug_raster(renderer, p);
		
		sl.determine_current_weapon(renderer, p);
		cast_rays(p, m, t, renderer);
		sl.draw_current_weapon(renderer);
		
		Debugger::time_stamp_old = Debugger::time_stamp;
		Debugger::time_stamp = SDL_GetTicks();
		int ms_per_frame = 1000.0 * (Debugger::time_stamp - Debugger::time_stamp_old) / CLOCKS_PER_SEC;
		Debugger::ms_per_frame = ms_per_frame;
		if (1000/ms_per_frame < 60 && game_delay > 14) {
			game_delay-=2;
		} else if (1000/ms_per_frame > 60) {
			game_delay+=2;
		}
		
		Debugger::draw_info(renderer, 1000/ms_per_frame, ms_per_frame, game_delay);
		
		SDL_RenderSetScale(renderer, screen_multiplier, screen_multiplier);
		SDL_RenderPresent(renderer);
		SDL_WarpMouseInWindow(window, Data::render_size_x/2, Data::render_size_y/2);
		
		SDL_Delay(game_delay);
	}
	
}


void draw_sky(SDL_Renderer* renderer, Player& p, SDL_Texture* t) {
	SDL_Rect src_rect_1;
	SDL_Rect src_rect_2;
	
	SDL_Rect dest_rect_1;
	SDL_Rect dest_rect_2;
	
	if (p.angle * 4 + Data::render_size_x > SKY_WIDTH) {
		
		src_rect_1.x = int(p.angle*4) % SKY_WIDTH;
		src_rect_1.y = 0;
		src_rect_1.w = Data::render_size_x - (int(p.angle * 4 + Data::render_size_x) % SKY_WIDTH);
		src_rect_1.h = SKY_HEIGHT;
		
		//cout << p.plane_y << endl;
		
		dest_rect_1.x = 0;
		dest_rect_1.y = p.plane_y/2 - SKY_HEIGHT + 200;
		dest_rect_1.w = src_rect_1.w;
		dest_rect_1.h = SKY_HEIGHT;
		
		
		src_rect_2.x = 0;
		src_rect_2.y = 0;
		src_rect_2.w = Data::render_size_x - src_rect_1.w;
		src_rect_2.h = SKY_HEIGHT;
		
		
		dest_rect_2.x = dest_rect_1.w;
		dest_rect_2.y = p.plane_y/2 - SKY_HEIGHT + 200;
		dest_rect_2.w = Data::render_size_x - dest_rect_1.w;
		dest_rect_2.h = SKY_HEIGHT;
		
		SDL_RenderCopy(renderer, t, &src_rect_1, &dest_rect_1);
		SDL_RenderCopy(renderer, t, &src_rect_2, &dest_rect_2);
		
		
	} else {
		
		src_rect_1.x = (int)p.angle*4 % SKY_WIDTH;
		src_rect_1.y = 0;
		src_rect_1.w = Data::render_size_x;
		src_rect_1.h = SKY_HEIGHT;
		
		dest_rect_1.x = 0;
		dest_rect_1.y = p.plane_y/2 - SKY_HEIGHT + 200;
		dest_rect_1.w = Data::render_size_x;
		dest_rect_1.h = SKY_HEIGHT;
		
		SDL_RenderCopy(renderer, t, &src_rect_1, &dest_rect_1);
		
	}
	
}

/*
void cast_rays(Player& p, Map& m, Textures& t, SDL_Renderer* renderer) {
		
	for (int n = plane_pixel.y; n < Player::plane_y + (Data::render_size_y - Player::plane_y); n++) {
		
		
		/*pixels that are behind weapon wont be calculated*//*
		if (t.current_weapon_texture != NULL) {
			SDL_Point point = {i, n};
			if (SDL_EnclosePoints(&point, 1, &t.current_weapon_texture->pos, NULL)) {
				/*wegen dieser Zeile hab ich die Nacht durchgemacht*//*
				if (!t.current_weapon_texture->alpha_values[ ((n - t.current_weapon_texture->pos.y) * t.current_weapon_texture->pos.w) + (plane_pixel.x - t.current_weapon_texture->pos.x) ] ) { 
					plane_pixel.y++;
					continue;
				}
			}
		}
		
	}
}*/


void cast_rays(Player& p, Map& m, Textures& t, SDL_Renderer* renderer) {
	
	double current_angle = p.angle - Player::field_of_view/2;
	current_angle = current_angle < 0 ? current_angle + 360 : current_angle;
	
	Uint32 floor_pixels[Data::render_size_x * Data::render_size_y];
	memset(floor_pixels, 0, Data::render_size_x * Data::render_size_y * sizeof(Uint32));
	vector<sprite_drawdata_t> sprites;
	for (int x = 0; x < Player::plane_x; x++) {
		
		
		//find first horizontal grid
		double first_grid_horizontal_x;
		double first_grid_horizontal_y;
		if (current_angle > 180) {
			first_grid_horizontal_y = ((int)p.pos_y / Field::width) * Field::width + Field::width;
		} else {
			first_grid_horizontal_y = ((int)p.pos_y / Field::width) * Field::width - 1;
		}
		
		first_grid_horizontal_x = p.pos_x + (p.pos_y - first_grid_horizontal_y) / Data::get_tan_val( 180 - current_angle);
		
		
		//calculate horizontal stepsize
		double grid_stepsize_horizontal_x;
		double grid_stepsize_horizontal_y;
		if (current_angle > 180) {
			grid_stepsize_horizontal_y = Field::width;
			grid_stepsize_horizontal_x = Field::width / Data::get_tan_val(current_angle - 180);
		} else {
			grid_stepsize_horizontal_y = -Field::width;
			grid_stepsize_horizontal_x = Field::width / Data::get_tan_val(180 - current_angle);
		}
		
		//grid_stepsize_horizontal_x = current_angle < 90 ? -grid_stepsize_horizontal_x : grid_stepsize_horizontal_x;
		
		//find first vertical grid
		double first_grid_vertical_x;
		double first_grid_vertical_y;
		if (current_angle < 90 || current_angle > 270) {
			first_grid_vertical_x = ((int)p.pos_x / Field::width) * Field::width - 1;
			first_grid_vertical_y = p.pos_y + (p.pos_x - first_grid_vertical_x) * Data::get_tan_val(180 - current_angle);
		} else {
			first_grid_vertical_x = ((int)p.pos_x / Field::width) * Field::width + Field::width;
			first_grid_vertical_y = p.pos_y + (p.pos_x - first_grid_vertical_x) * Data::get_tan_val(180 - current_angle);
		}
		
		
		//calculate vertical stepsize
		double grid_stepsize_vertical_x;
		double grid_stepsize_vertical_y;
		if (current_angle < 90 || current_angle > 270) {
			grid_stepsize_vertical_x = -Field::width;
			grid_stepsize_vertical_y = -Field::width * Data::get_tan_val(current_angle - 180);
		} else {
			grid_stepsize_vertical_x = Field::width;
			grid_stepsize_vertical_y = Field::width * Data::get_tan_val(current_angle - 180);
		}

		
		/*
		//DEBUGGING
		if (x == Debugger::current_x_pane_debug) {
			cout << "angle: " << current_angle << endl;
			cout << "horizontal x: " << grid_stepsize_horizontal_x << "\thorizontal y: " << grid_stepsize_horizontal_y << endl;
		}
		if (x == Debugger::current_x_pane_debug) {
			cout << "vertical x: " << grid_stepsize_vertical_x << "\tvertical y: " << grid_stepsize_vertical_y << endl;
			cout << "##" << endl;
		}
		*/
		
		
		//calculate grid steps
		double current_grid_horizontal_x = first_grid_horizontal_x;
		double current_grid_horizontal_y = first_grid_horizontal_y;
		double current_grid_vertical_x = first_grid_vertical_x;
		double current_grid_vertical_y = first_grid_vertical_y;
		int intersection_count = 0;
		double last_horizontal_distance = 0;
		double last_vertical_distance = 0;
		
		for (int i = 0; i < Player::view_depth; i++) {
			
			if (current_grid_horizontal_x >= 0 && current_grid_horizontal_x / Field::width < m.field_num_x && current_grid_horizontal_y >= 0 && current_grid_horizontal_y / Field::height < m.field_num_y && last_horizontal_distance < Player::view_distance) {
				
				SDL_Rect field_pos;
				field_pos.x = current_grid_horizontal_x / Field::width;
				field_pos.y = current_grid_horizontal_y / Field::width;
				
				
				double distance = sqrt( (p.pos_x - current_grid_horizontal_x)*(p.pos_x - current_grid_horizontal_x) + (p.pos_y - current_grid_horizontal_y)*(p.pos_y - current_grid_horizontal_y) );
				distance *= Data::get_cos_val(p.angle - current_angle);
				distance = distance < 1 ? 1 : distance; //division by 0 forbidden
				
				intersection_t is = {field_pos, current_grid_horizontal_x, current_grid_horizontal_y, distance, &m.map[field_pos.x][field_pos.y], true};
				
				Raydata::intersections[intersection_count] = is;
				intersection_count++;
				last_horizontal_distance = distance;
				
				/*
				//Debugging
				if (x == Debugger::current_x_pane_debug) {
					SDL_Rect r;
					r.w = 2;
					r.h = 2;
					r.x = current_grid_horizontal_x;
					r.y = current_grid_horizontal_y;
					SDL_SetRenderDrawColor(renderer, 255, 0, 255, 0);
					SDL_RenderFillRect(renderer, &r);
				}
				*/
			}
			
			if (current_grid_vertical_x >= 0 && current_grid_vertical_x / Field::width < m.field_num_x && current_grid_vertical_y >= 0 && current_grid_vertical_y / Field::height < m.field_num_y && last_vertical_distance < Player::view_distance) {
				
				SDL_Rect field_pos;
				field_pos.x = current_grid_vertical_x / Field::width;
				field_pos.y = current_grid_vertical_y / Field::width;
				
				
				
				double distance = sqrt( (p.pos_x - current_grid_vertical_x)*(p.pos_x - current_grid_vertical_x) + (p.pos_y - current_grid_vertical_y)*(p.pos_y - current_grid_vertical_y) );
				distance *= Data::get_cos_val(p.angle - current_angle);
				distance = distance < 1 ? 1 : distance; //division by 0 forbidden
				
				intersection_t is = {field_pos, current_grid_vertical_x, current_grid_vertical_y, distance, &m.map[field_pos.x][field_pos.y], false};
				
				Raydata::intersections[intersection_count] = is;
				intersection_count++;
				last_vertical_distance = distance;
				
				/*
				//Debugging
				if (x == Debugger::current_x_pane_debug) {
					SDL_Rect r;
					r.w = 2;
					r.h = 2;
					r.x = current_grid_vertical_x;
					r.y = current_grid_vertical_y;
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
					SDL_RenderFillRect(renderer, &r);
				}
				*/
			}
			
			current_grid_horizontal_x += grid_stepsize_horizontal_x;
			current_grid_horizontal_y += grid_stepsize_horizontal_y;
			
			current_grid_vertical_x += grid_stepsize_vertical_x;
			current_grid_vertical_y += grid_stepsize_vertical_y;
		}
		
		// operator < used by default, shortest distance chosen
		std::sort( Raydata::intersections, Raydata::intersections + intersection_count );
		
		//this prevents vertical wall stripes on corners on horizontal walls
		if (grid_stepsize_horizontal_y < 0 && current_angle < 180) {
			for (int i = 0; i < intersection_count; i++) {
				if (i+1 < intersection_count) {
					if (!Raydata::intersections[i].horizontal_wall && Raydata::intersections[i+1].horizontal_wall && Raydata::intersections[i].field == Raydata::intersections[i+1].field) {
						double dist_weight = Raydata::intersections[i].distance / 100;
						double dist_dif = Raydata::intersections[i+1].distance - Raydata::intersections[i].distance;
						if (dist_dif < dist_weight) Raydata::intersections[i].horizontal_wall = true;
					}
				}
			}
		}
		
		int last_field_height = 0;//Raydata::intersections[0].field->size;
		int last_y_floorpos = Data::render_size_y;
		vector<Uint32>* last_floor_colors = Raydata::intersections[0].field->floor_color_values;
		for (int i = 0; i < intersection_count; i++) {
		
			/*
			//Debugging
			if (x == Debugger::Debugger::current_x_pane_debug) {
				cout << "int nr: " << i << "\tis hori: " << Raydata::intersections[i].horizontal_wall << "\tdistance: " << Raydata::intersections[i].distance << endl;
			}
			*/
			
			//floor y
			SDL_Rect r_floor_dest;
			double last_field_plane_height = (double)last_field_height/Raydata::intersections[i].distance * p.dist_player_to_plane + 1;
			double last_field_rear_edge_y_pos = (double)p.height / (Raydata::intersections[i].distance / p.dist_player_to_plane) + p.plane_y/2 - last_field_plane_height;
			
			r_floor_dest.y = last_field_rear_edge_y_pos;
			r_floor_dest.w = 1;
			r_floor_dest.h = 1;
			r_floor_dest.x = x;
			
			if (r_floor_dest.y < 0) r_floor_dest.y =  0;
			else if (r_floor_dest.y > Data::render_size_y) r_floor_dest.y = Data::render_size_y;
			
			//SDL_RenderDrawLine(renderer, x, r_floor_dest.y, x, last_y_floorpos);
			
			if (r_floor_dest.y < last_y_floorpos) {
				for (int y = r_floor_dest.y; y < last_y_floorpos; y++) {
					
					
					double straight_distance_to_floor = ((double)(p.height - last_field_height)/ (y - Player::plane_y/2)) * p.dist_player_to_plane;
					double cos_angle = p.angle - current_angle;
					cos_angle = cos_angle < 0 ? -cos_angle : cos_angle;
					double actual_distance_to_floor = (double)straight_distance_to_floor / Data::get_cos_val(cos_angle);
					
					
					SDL_Rect r_floor_texture;
					r_floor_texture.w = 1;
					r_floor_texture.h = 1;
					
					r_floor_texture.x = (int)(p.pos_x - actual_distance_to_floor * Data::get_cos_val(current_angle)) % Field::width;
					r_floor_texture.y = (int)(p.pos_y - actual_distance_to_floor * Data::get_sin_val(current_angle)) % Field::height;
					
					if (r_floor_texture.x < 0) r_floor_texture.x = -r_floor_texture.x;
					if (r_floor_texture.y < 0) r_floor_texture.y = -r_floor_texture.y;
					
					int pixel = y * Data::render_size_x + r_floor_dest.x;
					if (Raydata::intersections[i].distance < Player::view_distance) {
						
						//floor_pixels[pixel] = t.forest_floor_texdata.color_values[r_floor_texture.y * 64 + r_floor_texture.x];
						//floor_pixels[pixel] = Raydata::intersections[i].field->floor_color_values->at(r_floor_texture.y * 64 + r_floor_texture.x);
						floor_pixels[pixel] = last_floor_colors->at(r_floor_texture.y * 64 + r_floor_texture.x);
						if (Data::shader_activated) {
							t.shade_pixel(&floor_pixels[pixel], straight_distance_to_floor);
						}
						
					}
					
				}
				
				last_y_floorpos = r_floor_dest.y;
			}
			
			
			
			
			SDL_Rect r_wall_dest;
			if (Raydata::intersections[i].field->size > last_field_height) {
				//wall y
				double h_wall = (double)Raydata::intersections[i].field->size/Raydata::intersections[i].distance * p.dist_player_to_plane + 1;
				double y_wall_pos = (double)p.height / ((double)Raydata::intersections[i].distance / p.dist_player_to_plane) + (double)p.plane_y/2 - h_wall;
				SDL_Rect r_wall_dest;
				r_wall_dest.y = y_wall_pos;
				r_wall_dest.h = h_wall;//r_floor_dest.y - r_wall_dest.y;
				r_wall_dest.w = 1;
				r_wall_dest.x = x;
				
				if (r_wall_dest.y < last_y_floorpos) {
					
					
					SDL_Rect r_wall_texture;
					r_wall_texture.w = 1;
					r_wall_texture.h = Field::height; //hier texture eigene height angeben
					r_wall_texture.y = 0;
					if (Raydata::intersections[i].horizontal_wall)
						r_wall_texture.x = (int)Raydata::intersections[i].x_pos % Field::width;
					else
						r_wall_texture.x = (int)Raydata::intersections[i].y_pos % Field::width;
					
					
					if (r_wall_dest.y + r_wall_dest.h > last_y_floorpos) {
						double percentage = (double)(last_y_floorpos - r_wall_dest.y) / r_wall_dest.h;
						r_wall_dest.h = last_y_floorpos - r_wall_dest.y;
						r_wall_texture.h *= percentage;
						if (r_wall_texture.h == 0) r_wall_texture.h = 5;
					}
					
					if (r_wall_dest.y + r_wall_dest.h < last_y_floorpos) r_wall_dest.h = last_y_floorpos - r_wall_dest.y;
					
					SDL_Texture* wall_texture = Raydata::intersections[i].field->wall_texture;
					
					if (Data::shader_activated) {
						int old_range = Player::view_distance - 1;
						int new_range = 255 - 0; 
						Uint8 dist_weight = 255 - (((Raydata::intersections[i].distance - 0) * new_range) / old_range) + 0;
						
						if (Raydata::intersections[i].horizontal_wall) SDL_SetTextureColorMod(wall_texture, dist_weight/2, dist_weight/2, dist_weight/2);
						else SDL_SetTextureColorMod(wall_texture, dist_weight, dist_weight, dist_weight);
						
					} else {
						if (Raydata::intersections[i].horizontal_wall) SDL_SetTextureColorMod(wall_texture, 125, 125, 125);
						else SDL_SetTextureColorMod(wall_texture, 255, 255, 255);
					}
					
					//Debugging
					//if (Debugger::do_draw)SDL_RenderCopy(renderer, Raydata::intersections[i].field->wall_texture, &r_wall_texture, &r_wall_dest);
					SDL_RenderCopy(renderer, Raydata::intersections[i].field->wall_texture, &r_wall_texture, &r_wall_dest);
					
					last_y_floorpos = r_wall_dest.y;
					
				}
				
			}
			
			if (!Raydata::intersections[i].field->sprites.empty()) {
				
				for (vector<sprite_t>::iterator it = Raydata::intersections[i].field->sprites.begin(); it != Raydata::intersections[i].field->sprites.end(); it++) {
					
					
					double distance_sprite = sqrt( (p.pos_x - it->x_map_pos)*(p.pos_x - it->x_map_pos) + (p.pos_y - it->y_map_pos)*(p.pos_y - it->y_map_pos) );
					double h_sprite = (double)(it->tex_rect->h + Raydata::intersections[i].field->size)/distance_sprite * p.dist_player_to_plane + 1;
					double y_sprite_pos = (double)p.height / (distance_sprite / p.dist_player_to_plane) + (double)p.plane_y/2 - h_sprite;
					double h_pane = (double)(it->tex_rect->h)/distance_sprite * p.dist_player_to_plane + 1;
					
					
					/*
					double distance_sprite = sqrt( (p.pos_x - it->x_map_pos)*(p.pos_x - it->x_map_pos) + (p.pos_y - it->y_map_pos)*(p.pos_y - it->y_map_pos) );
					double h_pane = (double)(it->tex_rect->h)/distance_sprite * p.dist_player_to_plane + 1;
					double y_sprite_pos = (double)p.height / (distance_sprite / p.dist_player_to_plane) + (double)p.plane_y/2 - h_pane;
					
					cout << "p_x:" << p.pos_x << " p_y: " << p.pos_y << " s_x: " << it->x_map_pos << " s_y: " << it->y_map_pos << " distance: " <<distance_sprite << endl;
					*/
					
					SDL_Rect pane_rec;
					pane_rec.y = y_sprite_pos;
					pane_rec.x = x;
					pane_rec.w = 1;
					pane_rec.h = h_pane;
					
					
					//create lines
					double sprite_leftside_x = it->x_map_pos + it->tex_rect->w/2 * cos((270 - current_angle) * M_PI / 180);
					double sprite_leftside_y = it->y_map_pos + it->tex_rect->w/2 * sin((90 - current_angle) * M_PI / 180);
					
					double sprite_rightside_x = it->x_map_pos + it->tex_rect->w/2 * cos((90 - current_angle) * M_PI / 180);
					double sprite_rightside_y = it->y_map_pos + it->tex_rect->w/2 * sin((270 - current_angle) * M_PI / 180);
					
					double intersection_x = p.pos_x - Field::width * cos((current_angle) * M_PI / 180);
					double intersection_y = p.pos_y - Field::height * sin((current_angle) * M_PI / 180);
					
					
					//find intersection
					double ip_x, ip_y;
					double a1, a2, b1, b2, c1, c2;
					
					a1 = sprite_rightside_y - sprite_leftside_y;
					b1 = sprite_leftside_x - sprite_rightside_x;
					c1 = a1 * sprite_leftside_x + b1 * sprite_leftside_y;
					
					a2 = intersection_y - p.pos_y;
					b2 = p.pos_x - intersection_x;
					c2 = a2 * p.pos_x + b2 * p.pos_y;
					
					double det = a1 * b2 - a2 * b1;
					
					if (det == 0) {
						ip_x = Raydata::intersections[i].x_pos;
						ip_y = Raydata::intersections[i].y_pos;
					} else {
						ip_x = (b2 * c1 - b1 * c2)/det;
						ip_y = (a1 * c2 - a2 * c1)/det;
					}
					
					double x_dif;
					if (sprite_leftside_x > sprite_rightside_x) x_dif = sprite_leftside_x - sprite_rightside_x;
					else x_dif = sprite_rightside_x - sprite_leftside_x;
					
					double y_dif;
					if (sprite_leftside_y > sprite_rightside_y) y_dif = sprite_leftside_y - sprite_rightside_y;
					else y_dif = sprite_rightside_y - sprite_leftside_y;
					
					x_dif = x_dif < 0 ? -x_dif : x_dif;
					y_dif = y_dif < 0 ? -y_dif : y_dif;
					
					double sprite_dist = x_dif + y_dif;
					
					
					double ip_x_dif;
					if (sprite_leftside_x > ip_x) ip_x_dif = sprite_leftside_x - ip_x;
					else ip_x_dif = ip_x - sprite_leftside_x;
					
					double ip_y_dif;
					if (sprite_leftside_y > ip_y) ip_y_dif = sprite_leftside_y - ip_y;
					else ip_y_dif = ip_y - sprite_leftside_y;
					
					ip_x_dif = ip_x_dif < 0 ? -ip_x_dif : ip_x_dif;
					ip_y_dif = ip_y_dif < 0 ? -ip_y_dif : ip_y_dif;
					
					//cout << ip_x_dif + ip_y_dif << " " << x_dif + y_dif << endl;
					
					double percentage = (ip_x_dif + ip_y_dif) / (x_dif + y_dif);
					
					double x_tex = it->tex_rect->w * percentage;
					
					
					
					SDL_Rect tex_rec;
					tex_rec.x = (int)x_tex;//offset;
					tex_rec.y = 0;
					tex_rec.w = 1;
					tex_rec.h = it->tex_rect->h;
					
					
					
					if (pane_rec.y + pane_rec.h > last_y_floorpos) {
						double percentage = (double)(last_y_floorpos - pane_rec.y) / pane_rec.h;
						pane_rec.h = last_y_floorpos - pane_rec.y;
						tex_rec.h *= percentage;
						if (tex_rec.h == 0) tex_rec.h = 5;
					}
					
					
					
					sprite_drawdata_t sd = {&(*it), pane_rec, tex_rec};
					sprites.push_back(sd);
				}
			}
			
			last_floor_colors = Raydata::intersections[i].field->floor_color_values;
			last_field_height = Raydata::intersections[i].field->size;
		
		}
		
		
		/*
		//Debugging
		if (x == Debugger::Debugger::current_x_pane_debug) {
			if (Debugger::do_draw)SDL_RenderDrawLine(renderer, x, 0, x, Data::render_size_y);
		}
		*/
		
		current_angle += (double)Player::field_of_view/Player::plane_x;
		current_angle = current_angle >= 360 ? current_angle - 360 : current_angle;
	}
	//cout << render_count << endl;
	
	memcpy(t.floor_surface->pixels, floor_pixels, sizeof(floor_pixels));
	SDL_DestroyTexture(t.floor_texture);
	t.floor_texture = SDL_CreateTextureFromSurface(renderer, t.floor_surface);
	SDL_RenderCopy(renderer, t.floor_texture, NULL, NULL);
	
	std::reverse(sprites.begin(), sprites.end());
	for (vector<sprite_drawdata_t>::iterator it = sprites.begin(); it != sprites.end(); it++) {
		SDL_RenderCopy(renderer, it->sprite->tex, &it->tex_rec, &it->pane_rec);
	}
	
	
	
	//Debugging
	//if (Debugger::do_draw)SDL_RenderCopy(renderer, t.floor_texture, NULL, NULL);
}

void handle_input (SDL_Event& e, Player& p) {
	while( SDL_PollEvent( &e ) != 0 ) {

		if( e.type == SDL_KEYDOWN ) {
		
			switch( e.key.keysym.sym ) {
				case SDLK_LEFT:
					p.angle++;
					p.angle = p.angle >= 360 ? p.angle - 360 : p.angle;
					break;
				case SDLK_RIGHT:
					p.angle--;
					p.angle = p.angle <= 0 ? 360 + p.angle : p.angle;
					break;
				case SDLK_UP:
					Player::height+=10;
					//Player::plane_y += 10;
					cout << Player::height << endl;
					break;
				case SDLK_DOWN:
					Player::height-=10;
					//Player::plane_y -= 10;
					cout << Player::height << endl;
					break;
				case SDLK_a:
					p.key_a = true;
					break;
				case SDLK_d:
					p.key_d = true;
					break;
				case SDLK_w: 
					p.key_w = true;
					break;
				case SDLK_s: 
					p.key_s = true;
					break;
				case SDLK_SPACE:
					if (p.jumping == false) p.jumping = true;
					break;
				case SDLK_n:
					Debugger::current_x_pane_debug--;
					break; 
				case SDLK_m:
					Debugger::current_x_pane_debug++;
					break;
				case SDLK_p:
					Debugger::do_draw = !Debugger::do_draw;
					break;
				case SDLK_ESCAPE:
					cout << Debugger::ms_per_frame << " " << 1000/Debugger::ms_per_frame << endl;
					exit(0);
					break;
				default:
					break;
			}
			//cout << p.angle <<  endl;
			
		} else if (e.type == SDL_KEYUP) {
			switch( e.key.keysym.sym ) {
				case SDLK_a:
					p.key_a = false;
					break;
				case SDLK_d:
					p.key_d = false;
					break;
				case SDLK_w: 
					p.key_w = false;
					break;
				case SDLK_s: 
					p.key_s = false;
					break;
				case SDLK_r: 
					p.reloading = true;
					break;
				default:
					break;
			}
			
		} else if (e.type == SDL_MOUSEMOTION) {
			
			int x, y;
			SDL_GetMouseState( &x, &y );
			if (x < p.plane_x/2) {
				
				p.last_direction = 0;
				p.angle -= (p.plane_x/2 - x)/ (100./Debugger::ms_per_frame);
				p.angle = p.angle <= 0 ? 360 + p.angle : p.angle;
				
			} else if (x > p.plane_x/2){
				
				p.last_direction = 1;
				p.angle += (x - p.plane_x/2)/ (100./Debugger::ms_per_frame);
				p.angle = p.angle >= 360 ? p.angle - 360 : p.angle;
				
			}
			
			if (y < Data::render_size_y/2) {
			
				if (Player::plane_y < 700) Player::plane_y += (Data::render_size_y/2 - y)/ (20./Debugger::ms_per_frame);
				
			} else if (y > Data::render_size_y/2) {
				if (Player::plane_y > -280) Player::plane_y -= (y - Data::render_size_y/2)/ (20./Debugger::ms_per_frame);
				//Player::plane_y = Player::plane_y < 0 ? 0 : Player::plane_y;
			}
			
			//cout << Player::plane_y << endl;
			
		} else if (e.type == SDL_MOUSEBUTTONDOWN) {
			
			switch (e.button.button) {
				case SDL_BUTTON_LEFT:
					p.shooting = true;
					break;
			}
			
		}
		//cout << p.angle << endl;
		//cout << p.pos_x << " " << p.pos_y << endl;
		
	}
}










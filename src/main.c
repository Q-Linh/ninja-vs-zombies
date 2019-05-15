#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "SwinGame.h"


#define NINJA_SPEED 3
#define INTERVAL 15
#define INVISIBLE_TIME_AND_INTERVAL 6000 
#define KUNAI_SPEED 28
#define PLAY_BUTTON_Y 500
#define HOW_TO_PLAY_BUTTON_Y 600
#define DIFFICULTY_BUTTON_X 650
#define EASY_BUTTON_Y 380
#define MEDIUM_BUTTON_Y 480
#define HARD_BUTTON_Y 580


typedef enum level_type {EASY, MEDIUM, HARD} level_type;

typedef struct entity_data {
    bitmap bmp;
    int32_t x, y;
} entity_data;

typedef struct ninja_data {
    bool entity_direction_is_right, play_throwing_ani, invisible;
    entity_data ninja_image, ninja_throwing_image, kunai_image;
    animation throwing_ani;
    timer invisible_time, invisible_interval;
} ninja_data;

typedef struct zombie_data {
    bool entity_direction_is_right;
    entity_data zombie_image;
    int32_t dx;
} zombie_data;

typedef struct game_data {
    int score;
    timer time;
    level_type difficulty_level;
} game_data;


//Load all the resources for the game
void load_resources()
{
    load_resource_bundle("game_bundles.txt", false);
}
//

//Get the bitmap for the ninja
void ninja_bitmap(ninja_data *ninja_ptr)
{
    if (ninja_ptr->entity_direction_is_right == true)
    {   
        if (ninja_ptr->invisible == false)
        {
            ninja_ptr->ninja_image.bmp = bitmap_named("Ninja");
            ninja_ptr->ninja_throwing_image.bmp = bitmap_named("ThrowImage");
            ninja_ptr->kunai_image.bmp = bitmap_named("Kunai");
        }
        else
        {
            ninja_ptr->ninja_image.bmp = bitmap_named("NinjaInvisible");
            ninja_ptr->ninja_throwing_image.bmp = bitmap_named("ThrowImageInvisible");
            ninja_ptr->kunai_image.bmp = bitmap_named("KunaiInvisible");
        }
    }
    else
    {
        if (ninja_ptr->invisible == false)
        {
            ninja_ptr->ninja_image.bmp = bitmap_named("NinjaLeft");
            ninja_ptr->ninja_throwing_image.bmp = bitmap_named("ThrowImageLeft");
            ninja_ptr->kunai_image.bmp = bitmap_named("KunaiLeft");
        }
        else
        {
            ninja_ptr->ninja_image.bmp = bitmap_named("NinjaLeftInvisible");
            ninja_ptr->ninja_throwing_image.bmp = bitmap_named("ThrowImageLeftInvisible");
            ninja_ptr->kunai_image.bmp = bitmap_named("KunaiLeftInvisible");
        }
    }

    bitmap_set_cell_details(ninja_ptr->ninja_throwing_image.bmp, 157, 188, 5, 2, 10);
}
//

//Get the bitmap for the zombie
void zombie_bitmap(zombie_data *zombie_ptr)
{
    if (zombie_ptr->entity_direction_is_right == true)
        zombie_ptr->zombie_image.bmp = bitmap_named("Zombie");
    else 
        zombie_ptr->zombie_image.bmp = bitmap_named("ZombieLeft");
}
//

//Draw throwing animation on the screen
void draw_ninja_throwing(ninja_data *ninja_ptr)
{                   
    draw_animation(ninja_ptr->throwing_ani, ninja_ptr->ninja_throwing_image.bmp, ninja_ptr->ninja_throwing_image.x, ninja_ptr->ninja_throwing_image.y);
    update_animation(ninja_ptr->throwing_ani);

    if (animation_ended(ninja_ptr->throwing_ani))
        ninja_ptr->play_throwing_ani = false;
        
    if (animation_current_cell(ninja_ptr->throwing_ani) > 1)
    {   
        if (ninja_ptr->entity_direction_is_right == true)
        {
           if (ninja_ptr->kunai_image.x < screen_width()) 
            { 
                draw_bitmap(ninja_ptr->kunai_image.bmp, ninja_ptr->kunai_image.x, ninja_ptr->kunai_image.y);
                ninja_ptr->kunai_image.x += KUNAI_SPEED;
            } 
        }
        else
        {
            if (ninja_ptr->kunai_image.x > 0)
            {
                draw_bitmap(ninja_ptr->kunai_image.bmp, ninja_ptr->kunai_image.x, ninja_ptr->kunai_image.y);
                ninja_ptr->kunai_image.x -= KUNAI_SPEED;
            }
        }
    }                  
}
//

//Setup a zombie
void setup_zombie(zombie_data *zombie_ptr, level_type difficulty_level)
{
    int direction = rnd_upto(2);

    if (direction == 0)
    {   
        
        zombie_ptr->entity_direction_is_right = true;
        zombie_ptr->zombie_image.x = rnd_upto(20);
    }
    else
    {
        zombie_ptr->entity_direction_is_right = false;
        zombie_ptr->zombie_image.x = screen_width() - bitmap_width(bitmap_named("Zombie")) - rnd_upto(20);
    }

    zombie_bitmap(zombie_ptr);
    zombie_ptr->zombie_image.y = rnd_upto(250)*(rnd_upto(2) + 1) + 20;
    if (difficulty_level == EASY) 
        zombie_ptr->dx = rnd_upto(1) + 1;
    else if (difficulty_level == MEDIUM) 
        zombie_ptr->dx = rnd_upto(3) + 1;
    else 
        zombie_ptr->dx = rnd_upto(5) + 1;
}
//

//Setup all the zombies
int setup_zombies(zombie_data array_of_zombies[], level_type difficulty_level) {
    int number_of_zombies;
    int index;

    if (difficulty_level == EASY) 
        number_of_zombies = 1;
    else if (difficulty_level == MEDIUM) 
        number_of_zombies = 2;
    else 
        number_of_zombies = 3;

    for (index = 0; index < number_of_zombies; index++)
    {   
        setup_zombie(&array_of_zombies[index], difficulty_level);
    }

    return number_of_zombies;
}
//

//Setup the ninja
void setup_ninja(ninja_data *ninja_ptr)
{
    ninja_ptr->entity_direction_is_right = true;
    ninja_bitmap(ninja_ptr);
    ninja_ptr->ninja_image.x = (int)((screen_width()-bitmap_width(ninja_ptr->ninja_image.bmp))/2);
    ninja_ptr->ninja_image.y = (int)((screen_height()-bitmap_height(ninja_ptr->ninja_image.bmp))/2);
    ninja_ptr->throwing_ani = create_animation_named("throw", animation_script_named("ThrowAni"));
    ninja_ptr->play_throwing_ani = false;
    ninja_ptr->ninja_throwing_image.x = ninja_ptr->ninja_image.x - INTERVAL;
    ninja_ptr->ninja_throwing_image.y = ninja_ptr->ninja_image.y;
    ninja_ptr->kunai_image.x = ninja_ptr->ninja_throwing_image.x + bitmap_cell_width(ninja_ptr->ninja_throwing_image.bmp) - INTERVAL;
    ninja_ptr->kunai_image.y = ninja_ptr->ninja_throwing_image.y + (int)(bitmap_cell_height(ninja_ptr->ninja_throwing_image.bmp) / 2);
    ninja_ptr->invisible = false;
    ninja_ptr->invisible_time = create_timer();
    ninja_ptr->invisible_interval = create_timer();
}
//

//Setup all the game data
void setup_game_data(game_data *game_ptr, int difficulty_number)
{
    game_ptr->score = 0;
    game_ptr->time = create_timer();
    start_timer(game_ptr->time);
    if (difficulty_number == 0)
        game_ptr->difficulty_level = EASY;
    else if (difficulty_number == 1)
        game_ptr->difficulty_level = MEDIUM;
    else
        game_ptr->difficulty_level = HARD;
}

//Setup the game
void setup_game(ninja_data *ninja_ptr, zombie_data array_of_zombies[], int *count_ptr, game_data *game_ptr, int difficulty_number)
{   
    setup_ninja(ninja_ptr);

    setup_game_data(game_ptr, difficulty_number);

    *count_ptr = setup_zombies(array_of_zombies, game_ptr->difficulty_level);
}
//

//Update location of kunai
void update_kunai(ninja_data *ninja_ptr)
{
    if (ninja_ptr->entity_direction_is_right == true)
        ninja_ptr->kunai_image.x = ninja_ptr->ninja_throwing_image.x + bitmap_cell_width(ninja_ptr->ninja_throwing_image.bmp) - INTERVAL;
    else
        ninja_ptr->kunai_image.x = ninja_ptr->ninja_throwing_image.x - bitmap_width(ninja_ptr->kunai_image.bmp) + INTERVAL;

    ninja_ptr->kunai_image.y = ninja_ptr->ninja_throwing_image.y + (int)(bitmap_cell_height(ninja_ptr->ninja_throwing_image.bmp) / 2);
}
//

//Update the location of the animation image
void update_throwing_ani(ninja_data *ninja_ptr)
{
    if (ninja_ptr->entity_direction_is_right == true)
        ninja_ptr->ninja_throwing_image.x = ninja_ptr->ninja_image.x - INTERVAL;
    else
        ninja_ptr->ninja_throwing_image.x = ninja_ptr->ninja_image.x + INTERVAL;

    ninja_ptr->ninja_throwing_image.y = ninja_ptr->ninja_image.y;
}
//

//Update the ninja
void update_ninja(ninja_data *ninja_ptr)
{
   ninja_bitmap(ninja_ptr); 
   update_throwing_ani(ninja_ptr);
}
//

//Change the direction of a zombie
void change_zombie_direction(zombie_data *zombie_ptr)
{
    if (zombie_ptr->entity_direction_is_right == true)
    {
        if (zombie_ptr->zombie_image.x + bitmap_width(zombie_ptr->zombie_image.bmp) >= screen_width())
            {
                zombie_ptr->zombie_image.x = screen_width() - bitmap_width(zombie_ptr->zombie_image.bmp);
                zombie_ptr->entity_direction_is_right = false;
            } 
    }
    else
    {
        if (zombie_ptr->zombie_image.x <= 0)
        {
            zombie_ptr->zombie_image.x = 0;
            zombie_ptr->entity_direction_is_right = true;
        }
    }
}
//

//Prevent the zombies from moving out of the screen
void wrap_zombie(zombie_data *zombie_ptr)
{
    change_zombie_direction(zombie_ptr);
    zombie_bitmap(zombie_ptr);
}
//

//Update the position of a zombie
void update_zombie_position(zombie_data *zombie_ptr)
{
    if (zombie_ptr->entity_direction_is_right == true)
        zombie_ptr->zombie_image.x += zombie_ptr->dx;
    else 
        zombie_ptr->zombie_image.x -= zombie_ptr->dx;
}
//

//Update a zombie
void update_zombie(zombie_data *zombie_ptr)
{   
    update_zombie_position(zombie_ptr);
    wrap_zombie(zombie_ptr);
}
//

//Update all the zombies
void update_zombies(zombie_data array_of_zombies[], int count)
{
    int index;

    for (index = 0; index < count; index++)
    {
        update_zombie(&array_of_zombies[index]);
    }
}
//

//Check whether the kunai hit the zombie
void check_kunai_hit_zombie(ninja_data *ninja_ptr, zombie_data array_of_zombies[], int count, game_data *game_ptr)
{
    int index;

    if (ninja_ptr->play_throwing_ani == true)
    {
        for (index = 0; index < count; index++)
        {
            if (bitmap_collision(array_of_zombies[index].zombie_image.bmp, array_of_zombies[index].zombie_image.x, array_of_zombies[index].zombie_image.y, ninja_ptr->kunai_image.bmp, ninja_ptr->kunai_image.x, ninja_ptr->kunai_image.y))
            {
                game_ptr->score++;
                ninja_ptr->play_throwing_ani = false;
                setup_zombie(&array_of_zombies[index], game_ptr->difficulty_level);
            }
        }
    }
}
//

//Check whether the zombie hit the ninja
void check_zombie_hit_ninja(ninja_data *ninja_ptr, zombie_data array_of_zombies[], int count, bool *end_ptr)
{
    int index;

    if (ninja_ptr->invisible == false)
    {
        for (index = 0; index < count; index++)
        {
            if (bitmap_collision(array_of_zombies[index].zombie_image.bmp, array_of_zombies[index].zombie_image.x, array_of_zombies[index].zombie_image.y, ninja_ptr->ninja_image.bmp, ninja_ptr->ninja_image.x, ninja_ptr->ninja_image.y))
            {
                *end_ptr = true;
            }
        }
    }
}
//

//Increase the number of zombies after a specific amount of time depending on the difficulty level
void increase_number_of_zombies(zombie_data array_of_zombies[], int *count_ptr, game_data *game_ptr)
{
    if (game_ptr->difficulty_level == EASY)
    {
       if (*count_ptr < 5)
        {
            if (timer_ticks(game_ptr->time) > 10000)
            {
                reset_timer(game_ptr->time);
                *count_ptr += 1;
                setup_zombie(&array_of_zombies[*count_ptr-1], game_ptr->difficulty_level);
            }
        }

        if ((*count_ptr >= 5) && (*count_ptr < 10))
        {
            if (timer_ticks(game_ptr->time) > 20000)
            {
                reset_timer(game_ptr->time);
                *count_ptr += 1;
                setup_zombie(&array_of_zombies[*count_ptr-1], game_ptr->difficulty_level);
            }
        } 

        if ((*count_ptr >= 10) && (*count_ptr < 15))
        {
            if (timer_ticks(game_ptr->time) > 30000)
            {
                reset_timer(game_ptr->time);
                *count_ptr += 1;
                setup_zombie(&array_of_zombies[*count_ptr-1], game_ptr->difficulty_level);
            }
        }
    }
    else if (game_ptr->difficulty_level == MEDIUM)
    {
        if (*count_ptr < 5)
        {
            if (timer_ticks(game_ptr->time) > 5000)
            {
                reset_timer(game_ptr->time);
                *count_ptr += 1;
                setup_zombie(&array_of_zombies[*count_ptr-1], game_ptr->difficulty_level);
            }
        }

        if ((*count_ptr >= 5) && (*count_ptr < 10))
        {
            if (timer_ticks(game_ptr->time) > 10000)
            {
                reset_timer(game_ptr->time);
                *count_ptr += 1;
                setup_zombie(&array_of_zombies[*count_ptr-1], game_ptr->difficulty_level);
            }
        }

        if ((*count_ptr >= 10) && (*count_ptr < 15))
        {
            if (timer_ticks(game_ptr->time) > 15000)
            {
                reset_timer(game_ptr->time);
                *count_ptr += 1;
                setup_zombie(&array_of_zombies[*count_ptr-1], game_ptr->difficulty_level);
            }
        }
    }
    else
    {
        if (*count_ptr < 5)
        {
            if (timer_ticks(game_ptr->time) > 2500)
            {
                reset_timer(game_ptr->time);
                *count_ptr += 1;
                setup_zombie(&array_of_zombies[*count_ptr-1], game_ptr->difficulty_level);
            }
        }

        if ((*count_ptr >= 5) && (*count_ptr < 10))
        {
            if (timer_ticks(game_ptr->time) > 5000)
            {
                reset_timer(game_ptr->time);
                *count_ptr += 1;
                setup_zombie(&array_of_zombies[*count_ptr-1], game_ptr->difficulty_level);
            }
        }

        if ((*count_ptr >= 10) && (*count_ptr < 15))
        {
            if (timer_ticks(game_ptr->time) > 7500)
            {
                reset_timer(game_ptr->time);
                *count_ptr += 1;
                setup_zombie(&array_of_zombies[*count_ptr-1], game_ptr->difficulty_level);
            }
        }
    }
}
//

//Check whether the ninja invisible time is over
void check_invisible_time_over(ninja_data *ninja_ptr)
{
    if (timer_ticks(ninja_ptr->invisible_time) > INVISIBLE_TIME_AND_INTERVAL)
    {
        stop_timer(ninja_ptr->invisible_time);
        ninja_ptr->invisible = false;
        start_timer(ninja_ptr->invisible_interval);
    }
}
//

//Update the game
void update_game(ninja_data *ninja_ptr, zombie_data array_of_zombies[], int *count_ptr, game_data *game_ptr, bool *end_ptr)
{   
    update_ninja(ninja_ptr);

    update_zombies(array_of_zombies, *count_ptr);
    
    check_kunai_hit_zombie(ninja_ptr, array_of_zombies, *count_ptr, game_ptr);

    check_invisible_time_over(ninja_ptr);
    
    check_zombie_hit_ninja(ninja_ptr, array_of_zombies, *count_ptr, end_ptr);

    increase_number_of_zombies(array_of_zombies, count_ptr, game_ptr);       
}
//

//Handle all the users input during the game
void handle_game_input(ninja_data *ninja_ptr)
{
    int time;

    process_events();

    if (ninja_ptr->play_throwing_ani == false)
    {
        if (key_down(VK_UP))
        {
            if (ninja_ptr->ninja_image.y - NINJA_SPEED >= 0)
                ninja_ptr->ninja_image.y -= NINJA_SPEED;
        }
        if (key_down(VK_DOWN))
        {
            if (ninja_ptr->ninja_image.y + bitmap_height(ninja_ptr->ninja_image.bmp) + NINJA_SPEED <= screen_height())
                ninja_ptr->ninja_image.y += NINJA_SPEED;
        }
        if (key_down(VK_LEFT))
        {
            ninja_ptr->entity_direction_is_right = false;
            if (ninja_ptr->ninja_image.x - NINJA_SPEED >= 0)
                ninja_ptr->ninja_image.x -= NINJA_SPEED;
        }
        if (key_down(VK_RIGHT))
        {
            ninja_ptr->entity_direction_is_right = true;
            if (ninja_ptr->ninja_image.x + bitmap_width(ninja_ptr->ninja_image.bmp) + NINJA_SPEED <= screen_width())
                ninja_ptr->ninja_image.x += NINJA_SPEED;
        }
        time = timer_ticks(ninja_ptr->invisible_interval);
        if ((!ninja_ptr->invisible) && ((time == 0) || (time > INVISIBLE_TIME_AND_INTERVAL)))
        {
            if (key_typed(VK_S))
            {
                ninja_ptr->invisible = true;
                start_timer(ninja_ptr->invisible_time);
                stop_timer(ninja_ptr->invisible_interval);
            }
        }
        if (key_typed(VK_A))
        {
            ninja_ptr->play_throwing_ani = true;
            update_kunai(ninja_ptr);
            restart_animation(ninja_ptr->throwing_ani);
        }
    }
}
//

//Draw an icon showing that the ninja can be invisible
void draw_invisible_icon(ninja_data ninja)
{
    int time = timer_ticks(ninja.invisible_interval);;

    if ((!ninja.invisible) && ((time == 0) || (time > INVISIBLE_TIME_AND_INTERVAL)))
    {
        fill_circle(ColorBlue, 790, 24, 12);
        draw_text("Invisible", ColorBlue, font_named("LoadingFont"), 820, 10);
    }
}
//

//Draw a line of text showing difficulty level on the screen
void draw_difficulty_text(level_type difficulty_level)
{
    char* string;

    if (difficulty_level == EASY) 
        string = "Difficulty: Easy";
    else if (difficulty_level == MEDIUM) 
        string = "Difficulty: Medium";
    else 
        string = "Difficulty: Hard";

    draw_text(string, ColorBlack, font_named("LoadingFont"), 380, 10); 
}
//

//Draw the background bitmaps for the game
void draw_background()
{
    draw_bitmap(bitmap_named("Background"), 0, 0);
    draw_bitmap(bitmap_named("Background2"), 0, 0);
}
//

//Draw ninja bitmap
void draw_ninja_bitmap(ninja_data *ninja_ptr)
{
    draw_bitmap(ninja_ptr->ninja_image.bmp, ninja_ptr->ninja_image.x, ninja_ptr->ninja_image.y);
}
//

//Draw ninja bitmap and animation
void draw_ninja(ninja_data *ninja_ptr)
{
    if (ninja_ptr->play_throwing_ani == false)
        draw_ninja_bitmap(ninja_ptr);
    else   
        draw_ninja_throwing(ninja_ptr);
}
//

//Draw a zombie on the screen
void draw_zombie(zombie_data *zombie_ptr)
{
    draw_bitmap(zombie_ptr->zombie_image.bmp, zombie_ptr->zombie_image.x, zombie_ptr->zombie_image.y);
}
//

//Draw all the zombies on the screen
void draw_zombies(zombie_data array_of_zombies[], int count)
{
    int index;

    for (index = 0; index < count; index++)
        draw_zombie(&array_of_zombies[index]);
}
//

//Draw a line of text showing the game score
void draw_score_text(int score)
{
    char string[100];

    sprintf(string, "Score: %d", score);
    draw_text(string, ColorBlack, font_named("LoadingFont"), 10, 10);
}

//Draw all game elements on the screen
void draw_game(ninja_data *ninja_ptr, zombie_data array_of_zombies[], int count, game_data *game_ptr)
{
    clear_screen_to(ColorWhite);

    draw_background();

    draw_ninja(ninja_ptr);    

    draw_zombies(array_of_zombies, count);

    draw_score_text(game_ptr->score);

    draw_difficulty_text(game_ptr->difficulty_level);

    draw_invisible_icon(*ninja_ptr);
    
    refresh_screen_restrict_fps(60); 
}
//

//Draw the screen showing difficulty level
void draw_difficulty_screen()
{
    draw_text("DIFFICULTY LEVEL", ColorBlack, font_named("TitleFont3"), 200, 490);
    draw_bitmap(bitmap_named("EasyButton"), DIFFICULTY_BUTTON_X, EASY_BUTTON_Y);
    draw_bitmap(bitmap_named("MediumButton"), DIFFICULTY_BUTTON_X, MEDIUM_BUTTON_Y);
    draw_bitmap(bitmap_named("HardButton"), DIFFICULTY_BUTTON_X, HARD_BUTTON_Y);
}
//

//Draw a screen showing how to play
void draw_how_to_play_screen()
{
    draw_bitmap(bitmap_named("HowToPlayImage"), (int)((screen_width() - bitmap_width(bitmap_named("HowToPlayImage")))/2), 50);
}
//

//Draw elements that are only for ending screen
void draw_ending_elements(int score)
{
    char string[100];

    sprintf(string, "Score: %d", score);
    draw_text("Game Over", ColorBlack, font_named("TitleFont2"), 340, 30);
    draw_text(string, ColorBlack, font_named("TitleFont3"), 400, 90);
}
//

//Draw elements that are only for starting screen
void draw_starting_elements()
{
    draw_text("Ninja    vs    Zombies", ColorBlack, font_named("TitleFont"), 200, 40);
}
//

//Draw elements that are for starting and ending screens
void draw_starting_and_ending_elements()
{
    draw_bitmap(bitmap_named("Ninja"), 220, 240);
    draw_bitmap(bitmap_named("ZombieLeft"), 600, 240);
    draw_bitmap(bitmap_named("HowToPlayButton"), (int)((screen_width() - bitmap_width(bitmap_named("HowToPlayButton")))/2), HOW_TO_PLAY_BUTTON_Y);
}
//

//Draw Play Button
void draw_play_button()
{
    draw_bitmap(bitmap_named("PlayButton"), (int)((screen_width() - bitmap_width(bitmap_named("PlayButton")))/2), PLAY_BUTTON_Y);
}
//

//Draw many types of screen
void draw_screens(int score, bool restart, bool how_to_play_screen, bool difficulty_screen)
{
    if (difficulty_screen == false)
    {
        draw_play_button();
        if (how_to_play_screen == false)
        {   
            draw_starting_and_ending_elements();
            if (restart == true)
                draw_starting_elements();
            else
                draw_ending_elements(score);
        }
        else
            draw_how_to_play_screen();
    }
    else
        draw_difficulty_screen();
}
//

//Draw a gui interface at the start and end of the game
void draw_gui(int score, bool restart, bool how_to_play_screen, bool difficulty_screen)
{
    clear_screen_to(ColorWhite);

    draw_background();

    draw_screens(score, restart, how_to_play_screen, difficulty_screen);

    refresh_screen_restrict_fps(60);
}
//

//Check whether the button is clicked
bool button_is_clicked(float btn_x, float btn_y, float btn_width, float btn_height)
{
    float mouse_x_postion, mouse_y_position, mouse_x_max, mouse_y_max;
    bool result = false;

    mouse_x_postion = mouse_x();
    mouse_y_position = mouse_y();
    mouse_x_max = btn_x + btn_width;
    mouse_y_max = btn_y + btn_height;
    if (mouse_clicked(LEFT_BUTTON))
    {
        if ((mouse_x_postion >= btn_x) && (mouse_x_postion <= mouse_x_max) && (mouse_y_position >= btn_y) && (mouse_y_position <= mouse_y_max))
            result = true;
    }

    return result;
}
//

//Handle all the user input at the start and end of the interface 
void handle_gui_input(ninja_data *ninja_ptr, zombie_data array_of_zombies[], int *count_ptr, game_data *game_ptr, bool *restart_ptr, bool *end_ptr, bool *how_to_play_screen_ptr, bool *difficulty_screen_ptr)
{
    int difficulty_number;

    bitmap play_button_bmp = bitmap_named("PlayButton");
    bitmap how_to_play_button_bmp = bitmap_named("HowToPlayButton");
    bitmap easy_button_bmp = bitmap_named("EasyButton");
    bitmap medium_button_bmp = bitmap_named("MediumButton");
    bitmap hard_button_bmp = bitmap_named("HardButton");

    process_events();

    if (*difficulty_screen_ptr == false)
    {
        if (button_is_clicked((int)((screen_width() - bitmap_width(play_button_bmp))/2), PLAY_BUTTON_Y, bitmap_width(play_button_bmp), bitmap_height(play_button_bmp)))
        {
            *difficulty_screen_ptr = true;
            *how_to_play_screen_ptr = false;
        }

        if (*how_to_play_screen_ptr == false)
        {
            if (button_is_clicked((int)((screen_width() - bitmap_width(how_to_play_button_bmp))/2), HOW_TO_PLAY_BUTTON_Y, bitmap_width(how_to_play_button_bmp), bitmap_height(how_to_play_button_bmp)))
                *how_to_play_screen_ptr = true;
        }
    }
    else
    {
        if ((button_is_clicked(DIFFICULTY_BUTTON_X, EASY_BUTTON_Y, bitmap_width(easy_button_bmp), bitmap_height(easy_button_bmp)))
            || (button_is_clicked(DIFFICULTY_BUTTON_X, MEDIUM_BUTTON_Y, bitmap_width(medium_button_bmp), bitmap_height(medium_button_bmp)))
            || (button_is_clicked(DIFFICULTY_BUTTON_X, HARD_BUTTON_Y, bitmap_width(hard_button_bmp), bitmap_height(hard_button_bmp))))
        {
            *restart_ptr = false;
            *end_ptr = false;
            *difficulty_screen_ptr = false;
            if (button_is_clicked(DIFFICULTY_BUTTON_X, EASY_BUTTON_Y, bitmap_width(easy_button_bmp), bitmap_height(easy_button_bmp)))
                difficulty_number = 0;
            if (button_is_clicked(DIFFICULTY_BUTTON_X, MEDIUM_BUTTON_Y, bitmap_width(medium_button_bmp), bitmap_height(medium_button_bmp)))
                difficulty_number = 1;
            if (button_is_clicked(DIFFICULTY_BUTTON_X, HARD_BUTTON_Y, bitmap_width(hard_button_bmp), bitmap_height(hard_button_bmp)))
                difficulty_number = 2;

            setup_game(ninja_ptr, array_of_zombies, count_ptr, game_ptr, difficulty_number);
        }        
    }
}
//

//The main function of the program
int main()
{
    ninja_data ninja;
    zombie_data array_of_zombies[15];
    int count;
    bool restart = true, end = false, how_to_play_screen = false, difficulty_screen = false;
    game_data game;
    
    open_graphics_window("Ninja vs Zombies", 940, 720);
    load_resources();

    do
    {   
        if ((restart == true) || (end == true))
        {   
            draw_gui(game.score, restart, how_to_play_screen, difficulty_screen);
            handle_gui_input(&ninja, array_of_zombies, &count, &game, &restart, &end, &how_to_play_screen, &difficulty_screen);
        }
        else
        {
            handle_game_input(&ninja);
            update_game(&ninja, array_of_zombies, &count, &game, &end);
            draw_game(&ninja, array_of_zombies, count, &game);
        }
    } 
    while ( ! window_close_requested() );
    
    return 0;
}
//

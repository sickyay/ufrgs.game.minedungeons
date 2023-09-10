/*
fazer o hud inteiro - HUD
coletaveis? MAÇA DO CAPIROTO
*/

#include <math.h>
#include <raylib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define MAP_LINES 30 //linhas
#define MAP_COLUMNS 60 //colunas
#define SIDE 20 //em pixels (DEVE SER UM NÚMERO PAR)
#define MAP_PIXEL_WIDTH (MAP_COLUMNS*SIDE) //em pixels
#define MAP_PIXEL_HEIGHT (MAP_LINES*SIDE) //em pixels

#define MENU_BUTTON_PIXEL_WIDTH 480 //em pixels
#define MENU_PIXEL_BORDER 5 //em pixels

#define HUD_PIXEL_HEIGHT 80

#define WALL_HP 3

#define APPLE_HEALING_POINTS 2

#define FPS 60 //taxa de frames por segundo

#define MAX_ENEMIES 15
#define MAX_ENEMIES1 15
#define MAX_ENEMIES2 15
#define MAX_ENEMIES3 15 //inimigos
#define MAX_WALLS 1600
#define MAX_WALLS1 100
#define MAX_TRAPS 100
#define MAX_BOMBS 100
#define MAX_PROJECTILES 100 //maximo de tiros na tela ao mesmo tempo
#define MAX_PROJECTILES1 100 //maximo de tiros na tela ao mesmo tempo
#define MAX_SMOKES 400
#define MAX_STREAMS 400
#define MAX_PORTALS 10
#define MAX_ARCHIVE_NAME 25 //maximo de caracteres no nome do arcuivo

//player
#define PLAYER_STEP_COOLDOWN 0.1 //cooldown do movimento do player
#define PLAYER_ACTION_COOLDOWN 0.5 //cooldown do tiro do player
#define PLAYER_INVUNERABILITY_COOLDOWN 5
#define PLAYER_HP 10

//INIMIGOS
//zumbi
#define ENEMY_STEP_COOLDOWN 0.3
#define ENEMY_ACTION_COOLDOWN 1
#define ENEMY_HP 5

//esqueleto
#define ENEMY1_STEP_COOLDOWN 0.2
#define ENEMY1_ACTION_COOLDOWN 2
#define ENEMY1_HP 3

//creeper
#define ENEMY2_STEP_COOLDOWN 0.4
#define ENEMY2_ACTION_COOLDOWN 3
#define ENEMY2_HP 2
#define CREEPER_RADIUS 4
#define CREEPER_DAMAGE 15

//slime
#define ENEMY3_STEP_COOLDOWN 0.1
#define ENEMY3_HP 1

//lava
#define TRAP_ACTION_COOLDOWN 0.25

//tempo dos efeitos
#define SMOKE_LIFETIME 1
#define SMOKE1_LIFETIME 0.2
#define STREAM_LIFETIME 0.2

//caracteristicas da tnt
#define BOMB_RADIUS 6
#define BOMB_DAMAGE 10

struct STRUCT_TIMER //estrutura utilizada para temporizadores
{
    long int timer_start;//variaveis para logica de cooldown de passos
    long int timer_current;
    long int cooldown;
};

struct STRUCT_EFFECT //estrutura utilizada para temporizadores
{
    int x;//posicoes
    int y;
    int type;
    struct STRUCT_TIMER lifetime;
};

struct STRUCT_STATS //estrutura utilizada tanto para inimigos, player e projeteis
{
    int x;//posicoes
    int y;
    int dx;//intencao de deslocamento
    int dy;
    int head_direction_x;//para onde está "olhando"
    int head_direction_y;

    int health_points;//pontos de vida

    struct STRUCT_TIMER step;//timers variados
    struct STRUCT_TIMER action;
};

struct STRUCT_DATA //estrutura utilizada tanto para inimigos, player e projeteis
{
    char map[MAP_LINES][MAP_COLUMNS+1];//declara a matriz do mapa
    int level;

    struct STRUCT_STATS player;
    bool intends_to_put_bomb;
    int bomb_inventory;
    struct STRUCT_TIMER invunerability;

    struct STRUCT_STATS projectile[MAX_PROJECTILES];
    struct STRUCT_STATS projectile1[MAX_PROJECTILES];
    struct STRUCT_STATS enemy[MAX_ENEMIES];
    struct STRUCT_STATS enemy1[MAX_ENEMIES1];
    struct STRUCT_STATS enemy2[MAX_ENEMIES2];
    struct STRUCT_STATS enemy3[MAX_ENEMIES3];
    struct STRUCT_STATS wall[MAX_WALLS];
    struct STRUCT_STATS wall1[MAX_WALLS1];
    struct STRUCT_STATS trap[MAX_TRAPS];
    struct STRUCT_STATS bomb[MAX_BOMBS];
    struct STRUCT_STATS portal[MAX_PORTALS];

    struct STRUCT_EFFECT smoke[MAX_SMOKES];
    struct STRUCT_EFFECT stream[MAX_STREAMS];

    int shot_counter;
    int shot1_counter;
    int bomb_counter;
    int smoke_counter;
    int stream_counter;
    int wall_counter;
    int wall1_counter;
    int enemy_counter;
    int enemy1_counter;
    int enemy2_counter;
    int enemy3_counter;
    int portal_counter;
    int trap_counter;
};

bool draw_map (struct STRUCT_DATA *data, Texture2D texture[], Font font)//funcao que le o que acontece na matriz do mapa
{

    //ClearBackground(WHITE);//limpa a tela e define cor de fundo

    for (int y = 0; y < MAP_LINES; y++)//colunas
    {
        for (int x = 0; x < MAP_COLUMNS; x++)//linhas
        {
            switch(data->map[y][x])//interpretacao dos caracteres
            {

            case 'I'://inimigo ZUMBI
                DrawTexture(texture[2],x*SIDE,y*SIDE,WHITE);
                break;

            case 'E'://inimigo ESQUELETO
                DrawTexture(texture[1],x*SIDE,y*SIDE,WHITE);
                break;

            case 'C'://inimigo CREEPER
                DrawTexture(texture[12],x*SIDE,y*SIDE,WHITE);
                break;

            case 'S'://inimigo SLIME
                DrawTexture(texture[13],x*SIDE,y*SIDE,WHITE);
                break;

            case 'B'://bomba
                DrawTexture(texture[6],x*SIDE,y*SIDE,WHITE);
                break;

            case 'P'://portal
                DrawTexture(texture[4],x*SIDE,y*SIDE,WHITE);
                break;

            case 'X'://armadilha
                data->map[y][x] = ' ';
                break;

            case 'o'://tiro
                DrawTexture(texture[11],x*SIDE,y*SIDE,WHITE);
                DrawTexture(texture[7],x*SIDE,y*SIDE,WHITE);
                break;

            case 'M'://apple
                DrawTexture(texture[11],x*SIDE,y*SIDE,WHITE);
                DrawTexture(texture[14],x*SIDE,y*SIDE,WHITE);
                break;

            case 'D'://goldenapple
                DrawTexture(texture[11],x*SIDE,y*SIDE,WHITE);
                DrawTexture(texture[15],x*SIDE,y*SIDE,WHITE);
                break;

            case 'O'://goldenapple
                DrawTexture(texture[3],x*SIDE,y*SIDE,WHITE);
                break;

            case ' '://espaço livre
                DrawTexture(texture[11],x*SIDE,y*SIDE,WHITE);
                break;
            }
        }
    }
    for(int i=0; i<MAX_WALLS; i++)
    {
        switch(data->wall[i].health_points){
            case 3:DrawTexture(texture[8], data->wall[i].x*SIDE, data->wall[i].y*SIDE, WHITE);
                break;
            case 2:DrawTexture(texture[9], data->wall[i].x*SIDE, data->wall[i].y*SIDE, WHITE);
                break;
            case 1:DrawTexture(texture[10], data->wall[i].x*SIDE, data->wall[i].y*SIDE, WHITE);
                break;
        }
    }
    for(int i=0; i<MAX_TRAPS; i++)
    {
        if(data->map[data->trap[i].y][data->trap[i].x]==' ') DrawTexture(texture[5], data->trap[i].x*SIDE, data->trap[i].y*SIDE, WHITE);
    }

    DrawTexture(texture[0],data->player.x*SIDE,data->player.y*SIDE,WHITE);

    DrawTextEx(font,TextFormat("Your HP: %1i", data->player.health_points), (Vector2){20, 2}, 40,0, RED);//vida do player
    return 1;
}

bool draw_menu (int selected_option, Texture2D texture[], Font font)
{

    int i;
    int button_height = (MAP_PIXEL_HEIGHT/6);
    char options_text[6][125] = {{"GAME PAUSED"},{"N - New run"},{"L - Load save"},{"S - Save game"},{"Q - Quit game"},{"M - Close Menu"}};
    char saves_text[6][125] = {{"SELECT SAVE"},{"1 - Save 1"},{"2 - Save 2"},{"3 - Save 3"},{"4 - Save 4"},{"5 - Save 5"}};
    int pixels_offset[6] = {85,106,80,80,90,80};

    DrawTexture(texture[19],0,0,WHITE);

    if(selected_option>=0&&selected_option<=5){
        for(i = 0;i<6;i++){
            if(i!=0){
                if(selected_option==i){
                    DrawTexture(texture[20],(MAP_PIXEL_WIDTH/2) - (MENU_BUTTON_PIXEL_WIDTH/2),(button_height*i)+MENU_PIXEL_BORDER,WHITE);
                }else{
                    DrawTexture(texture[21],(MAP_PIXEL_WIDTH/2) - (MENU_BUTTON_PIXEL_WIDTH/2),(button_height*i)+MENU_PIXEL_BORDER,WHITE);
                }
            }
            DrawTextEx(font,options_text[i], (Vector2){ (MAP_PIXEL_WIDTH/2) - (MENU_BUTTON_PIXEL_WIDTH/2)+pixels_offset[i], (button_height)*i+MENU_PIXEL_BORDER+button_height/4}, button_height/2,0, WHITE);
        }
    }else{
        for(i = 0;i<6;i++){
            if(i!=0){
                if(selected_option==i+6){
                    DrawTexture(texture[20],(MAP_PIXEL_WIDTH/2) - (MENU_BUTTON_PIXEL_WIDTH/2),(button_height*i)+MENU_PIXEL_BORDER,WHITE);
                }else{
                    DrawTexture(texture[21],(MAP_PIXEL_WIDTH/2) - (MENU_BUTTON_PIXEL_WIDTH/2),(button_height*i)+MENU_PIXEL_BORDER,WHITE);
                }
            }
            DrawTextEx(font,saves_text[i], (Vector2){ (MAP_PIXEL_WIDTH/2) - (MENU_BUTTON_PIXEL_WIDTH/2)+130, (button_height)*i+MENU_PIXEL_BORDER+button_height/4}, button_height/2,0, WHITE);
        }
    }
    return true;
}

bool draw_hud (struct STRUCT_DATA *data, Texture2D texture[], Font font)
{
    int i;
    DrawTexture(texture[19],0,MAP_PIXEL_HEIGHT,WHITE);

    for(i=1;i<=PLAYER_HP;i++){
        data->invunerability.timer_current = clock();
        if(data->invunerability.timer_current - data->invunerability.timer_start <= data->invunerability.cooldown && data->invunerability.timer_start>=PLAYER_INVUNERABILITY_COOLDOWN*CLOCKS_PER_SEC){
            DrawTexture(texture[17],10+((i-1)*(60+5)),MAP_PIXEL_HEIGHT+10,WHITE);//com vida
        }else{
            if(i<=data->player.health_points){
                DrawTexture(texture[16],10+((i-1)*(60+5)),MAP_PIXEL_HEIGHT+10,WHITE);//com vida
            }else{
                DrawTexture(texture[18],10+((i-1)*(60+5)),MAP_PIXEL_HEIGHT+10,WHITE);//sem vida
            }
        }
    }

    return true;
}

bool draw_effects (struct STRUCT_DATA *data)
{
    int i, j;
    int effect_pos_x;
    int effect_pos_y;

    for(i=0; i<MAX_SMOKES; i++)
    {
        data->smoke[i].lifetime.timer_current = clock();
        if(data->smoke[i].lifetime.timer_current - data->smoke[i].lifetime.timer_start >= data->smoke[i].lifetime.cooldown){
            data->smoke[i].x = -1;
            data->smoke[i].y = -1;
        }
        if(data->smoke[i].x>=0&&data->smoke[i].x<MAP_COLUMNS&&data->smoke[i].y>=0&&data->smoke[i].y<MAP_LINES){
            if(data->smoke[i].type==0){
                if(data->smoke[i].lifetime.timer_current - data->smoke[i].lifetime.timer_start <= data->smoke[i].lifetime.cooldown){
                    for(j=0;j<5;j++){
                        DrawRectangle((data->smoke[i].x*SIDE)+((rand()%(SIDE*2)-(SIDE/2))), (data->smoke[i].y*SIDE)+((rand()%(SIDE*2)-(SIDE/2))), SIDE/6, SIDE/6, (Color){255, 255, 255, 255-(((data->smoke[i].lifetime.timer_current-data->smoke[i].lifetime.timer_start)*255)/data->smoke[i].lifetime.cooldown)});
                    }
                }
            }else if(data->smoke[i].type==1){
                if(data->smoke[i].lifetime.timer_current - data->smoke[i].lifetime.timer_start <= data->smoke[i].lifetime.cooldown){
                    for(j=0;j<4;j++){
                        DrawRectangle((data->smoke[i].x*SIDE)+((rand()%((SIDE/2)+1)))+SIDE/5, (data->smoke[i].y*SIDE)+((rand()%((SIDE/2)+1)))+SIDE/5, SIDE/5, SIDE/5, (Color){255-(((data->smoke[i].lifetime.timer_current-data->smoke[i].lifetime.timer_start)*255)/data->smoke[i].lifetime.cooldown),
                            140-(((data->smoke[i].lifetime.timer_current-data->smoke[i].lifetime.timer_start)*140)/data->smoke[i].lifetime.cooldown),
                            0,
                            255-(((data->smoke[i].lifetime.timer_current-data->smoke[i].lifetime.timer_start)*255)/data->smoke[i].lifetime.cooldown)});
                    }
                }
            }
        }
    }
    for(i=0; i<MAX_STREAMS; i++)
    {
        data->stream[i].lifetime.timer_current = clock();
        if(data->stream[i].lifetime.timer_current - data->stream[i].lifetime.timer_start >= data->stream[i].lifetime.cooldown){
            data->stream[i].x = -1;
            data->stream[i].y = -1;
        }
        if(data->stream[i].x>=0&&data->stream[i].x<MAP_COLUMNS&&data->stream[i].y>=0&&data->stream[i].y<MAP_LINES){
            if(data->stream[i].lifetime.timer_current - data->stream[i].lifetime.timer_start <= data->stream[i].lifetime.cooldown){
                effect_pos_x = (data->stream[i].x*SIDE);
                effect_pos_y = (data->stream[i].y*SIDE);
                switch(data->stream[i].type){
                    case 0:
                        DrawLine(effect_pos_x+(SIDE/2), effect_pos_y+(SIDE/2),effect_pos_x+(SIDE/2) ,effect_pos_y+SIDE , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        data->stream[i].type = data->stream[i].type + 1;
                        break;
                    case 1:
                        DrawLine(effect_pos_x+(SIDE/2), effect_pos_y,effect_pos_x+(SIDE/2) ,effect_pos_y+SIDE , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        break;
                    case 2:
                        DrawLine(effect_pos_x, effect_pos_y+SIDE,effect_pos_x+(SIDE/2) ,effect_pos_y+(SIDE/2) , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        data->stream[i].type = data->stream[i].type + 1;
                        break;
                    case 3:
                        DrawLine(effect_pos_x+SIDE, effect_pos_y,effect_pos_x ,effect_pos_y+SIDE , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        break;
                    case 4:
                        DrawLine(effect_pos_x, effect_pos_y+(SIDE/2),effect_pos_x+(SIDE/2) ,effect_pos_y+(SIDE/2) , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        data->stream[i].type = data->stream[i].type + 1;
                        break;
                    case 5:
                        DrawLine(effect_pos_x, effect_pos_y+(SIDE/2),effect_pos_x+SIDE ,effect_pos_y+(SIDE/2) , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        break;
                    case 6:
                        DrawLine(effect_pos_x, effect_pos_y,effect_pos_x+(SIDE/2) ,effect_pos_y+(SIDE/2) , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        data->stream[i].type = data->stream[i].type + 1;
                        break;
                    case 7:
                        DrawLine(effect_pos_x, effect_pos_y,effect_pos_x+SIDE ,effect_pos_y +SIDE, (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        break;
                    case 8:
                        DrawLine(effect_pos_x+(SIDE/2), effect_pos_y,effect_pos_x+(SIDE/2) ,effect_pos_y+(SIDE/2) , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        data->stream[i].type = data->stream[i].type + 1;
                        break;
                    case 9:
                        DrawLine(effect_pos_x+(SIDE/2), effect_pos_y,effect_pos_x+(SIDE/2) ,effect_pos_y+SIDE , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        break;
                    case 10:
                        DrawLine(effect_pos_x+SIDE, effect_pos_y,effect_pos_x+(SIDE/2) ,effect_pos_y+(SIDE/2) , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        data->stream[i].type = data->stream[i].type + 1;
                        break;
                    case 11:
                        DrawLine(effect_pos_x+SIDE, effect_pos_y,effect_pos_x ,effect_pos_y+SIDE , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        break;
                    case 12:
                        DrawLine(effect_pos_x+(SIDE/2), effect_pos_y+(SIDE/2),effect_pos_x +SIDE,effect_pos_y+(SIDE/2) , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        data->stream[i].type = data->stream[i].type + 1;
                        break;
                    case 13:
                        DrawLine(effect_pos_x, effect_pos_y+(SIDE/2),effect_pos_x+SIDE ,effect_pos_y+(SIDE/2) , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        break;
                    case 14:
                        DrawLine(effect_pos_x+(SIDE/2), effect_pos_y+(SIDE/2),effect_pos_x+SIDE ,effect_pos_y+SIDE , (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        data->stream[i].type = data->stream[i].type + 1;
                        break;
                    case 15:
                        DrawLine(effect_pos_x, effect_pos_y,effect_pos_x+SIDE ,effect_pos_y +SIDE, (Color){255, 255, 255, 255-(((data->stream[i].lifetime.timer_current-data->stream[i].lifetime.timer_start)*255)/data->stream[i].lifetime.cooldown)});
                        break;
                }
            }
        }
    }
    return 1;
}

bool scan_map (struct STRUCT_DATA *data)
{
    for (int y = 0; y < MAP_LINES; y++)//colunas
    {
        for (int x = 0; x < MAP_COLUMNS; x++)//linhas
        {
            switch(data->map[y][x])//interpretacao dos caracteres
            {
            case '#'://parede
                data->wall[data->wall_counter].x = x;
                data->wall[data->wall_counter].y = y;
                data->wall[data->wall_counter].health_points = WALL_HP;
                data->wall_counter=data->wall_counter+1;
                break;
            case 'O'://parede indestrutivel
                data->wall1[data->wall1_counter].x = x;
                data->wall1[data->wall1_counter].y = y;
                data->wall1_counter=data->wall1_counter+1;
                break;
            case 'I'://inimigo ZUMBI
                data->enemy[data->enemy_counter].x = x;
                data->enemy[data->enemy_counter].y = y;
                data->enemy[data->enemy_counter].health_points = ENEMY_HP;
                data->enemy[data->enemy_counter].step.cooldown = CLOCKS_PER_SEC*ENEMY_STEP_COOLDOWN;
                data->enemy[data->enemy_counter].action.cooldown = CLOCKS_PER_SEC*ENEMY_ACTION_COOLDOWN;
                data->enemy_counter = data->enemy_counter+1;
                break;
            case 'E'://inimigo ESQUELETO
                data->enemy1[data->enemy1_counter].x = x;
                data->enemy1[data->enemy1_counter].y = y;
                data->enemy1[data->enemy1_counter].health_points = ENEMY1_HP;
                data->enemy1[data->enemy1_counter].step.cooldown = CLOCKS_PER_SEC*ENEMY1_STEP_COOLDOWN;
                data->enemy1[data->enemy1_counter].action.cooldown = CLOCKS_PER_SEC*ENEMY1_ACTION_COOLDOWN;
                data->enemy1_counter = data->enemy1_counter+1;
                break;
            case 'C'://inimigo CREEPER
                data->enemy2[data->enemy2_counter].x = x;
                data->enemy2[data->enemy2_counter].y = y;
                data->enemy2[data->enemy2_counter].health_points = ENEMY2_HP;
                data->enemy2[data->enemy2_counter].step.cooldown = CLOCKS_PER_SEC*ENEMY2_STEP_COOLDOWN;
                data->enemy2[data->enemy2_counter].action.cooldown = CLOCKS_PER_SEC*ENEMY2_ACTION_COOLDOWN;
                data->enemy2_counter = data->enemy2_counter+1;
                break;
            case 'S'://inimigo SLIME
                data->enemy3[data->enemy3_counter].x = x;
                data->enemy3[data->enemy3_counter].y = y;
                data->enemy3[data->enemy3_counter].health_points = ENEMY3_HP;
                data->enemy3[data->enemy3_counter].step.cooldown = CLOCKS_PER_SEC*ENEMY3_STEP_COOLDOWN;
                data->enemy3_counter = data->enemy3_counter+1;
                break;
            case 'B'://bomba
                data->bomb[data->bomb_counter].x = x;
                data->bomb[data->bomb_counter].y = y;
                data->bomb[data->bomb_counter].health_points = 1;
                data->bomb_counter = data->bomb_counter+1;
                break;
            case 'P'://portal
                data->portal[data->portal_counter].x = x;
                data->portal[data->portal_counter].y = y;
                data->portal_counter = data->portal_counter+1;
                break;
            case 'J'://player
                data->player.x = x;
                data->player.y = y;

                data->player.step.cooldown = CLOCKS_PER_SEC*PLAYER_STEP_COOLDOWN;
                data->player.action.cooldown = CLOCKS_PER_SEC*PLAYER_ACTION_COOLDOWN;
                data->invunerability.cooldown = CLOCKS_PER_SEC*PLAYER_INVUNERABILITY_COOLDOWN;
                break;
            case 'X'://armadilha
                data->trap[data->trap_counter].x = x;
                data->trap[data->trap_counter].y = y;
                data->trap[data->trap_counter].action.cooldown = CLOCKS_PER_SEC*TRAP_ACTION_COOLDOWN;
                data->trap_counter = data->trap_counter+1;
                break;
            }
        }
    }
    return 1;
}

bool save_data(struct STRUCT_DATA data,int selected_save)
{

    int flag;
    FILE *arc;
    char archive_name[MAX_ARCHIVE_NAME] = "save0.bin";
    archive_name[4] = selected_save + '0';
    arc = fopen(archive_name, "wb");

    if(arc!=NULL){
        if (fwrite(&data, sizeof(struct STRUCT_DATA), 1, arc) == 1){
            flag = 1;
        }else flag = 0;
    }else{
        flag = 0;
    }
    fclose(arc);

return flag;
}

bool load_data(struct STRUCT_DATA *data,int selected_save)
{

    struct STRUCT_DATA temporary_data;

    int flag = 0;
    FILE* arc;
    char archive_name[MAX_ARCHIVE_NAME] = "save0.bin";
    archive_name[4] = selected_save + '0';
    arc = fopen(archive_name, "rb");

    if (arc!=NULL)
    {
        if (fread(&temporary_data, sizeof(struct STRUCT_DATA), 1, arc) == 1){
            flag = 1;
            *data = temporary_data;
        }else flag = 0;
    }else{
        flag = 0;
    }
    fclose(arc);

return flag;
}

bool test_move(struct STRUCT_DATA *data ,struct STRUCT_STATS *object)//semelhante a deve mover
{

    //comeca com flag 1, significando que pode mover, e vai testando condicoes que impede ele de mover
    bool flag = 1;

    //se a posicao desejada não for vazia, entao flag eh zero
    if(!(data->map[object->y+object->dy][object->x+object->dx]==' ')) flag = 0;

    //se a posicao desejada estiver fora dos limites do mapa, entao flag eh zero
    if(!(object->x+object->dx>=0 && object->x+object->dx<MAP_COLUMNS && object->y+object->dy>=0 && object->y+object->dy<MAP_LINES)) flag = 0;

    //se o objeto esta movendo em diagonal
    if(object->dx!=0 && object->dy!=0)
    {
        //se o objeto estiver se expremendo entre 2 objetos solidos, flag eh zero
        if(!(data->map[object->y+object->dy][object->x]==' ' || data->map[object->y][object->x+object->dx]==' ')) flag = 0;
    }

    return flag;
}

bool test_if_dead(struct STRUCT_DATA *data ,struct STRUCT_STATS *object)//testa se o objeto morreu
{
    bool flag = false;

    if(object->health_points<=0)
    {
        data->map[object->y][object->x] = ' ';
        object->dx = 0;
        object->dy = 0;
        object->x = -1;
        object->y = -1;
        object->health_points = 0;
        flag = true;
    }
    return flag;
}

bool test_if_object_is_there(struct STRUCT_DATA *data, struct STRUCT_STATS *object, int x_position, int y_position, char object_symbol)//testa se o objeto morreu
{
    bool flag = false;

    if(data->map[object->y+y_position][object->x+x_position] == object_symbol) flag = true;

    return flag;
}

void move(struct STRUCT_DATA *data ,struct STRUCT_STATS *object)//funcao que movimenta os objetos na matriz
{

    char temp;//variavel temporaria

    temp = data->map[object->y][object->x];//iguala o tipo de caractere (P, o, I) a posicao inicial da estrutura
    data->map[object->y][object->x] = ' ';//entao zera a posicao inicial da matriz

    object->x = object->x + object->dx;//encrementa dx ao x, ou seja cria o movimento e gera a nova posicao
    object->y = object->y + object->dy;//encrementa dy ao y, ou seja cria o movimento e gera a nova posicao

    data->map[object->y][object->x] = temp;//a nova posicao recebe entao o valor temporario (P, o, I) para fazer o objeto aparecer de novo
}

void damage_position(struct STRUCT_DATA *data, int x_position, int y_position, int damage, Sound sounds[])//funcao que movimenta os objetos na matriz
{
    int i;

    if(data->player.x == x_position && data->player.y == y_position && data->player.health_points>=1)//se a posição onde quer infligir dano é a mesma do player
    {
        data->player.health_points = data->player.health_points - damage;//decrementa da vida
        PlaySound(sounds[11]);
    }

    for(i=0; i<MAX_ENEMIES1; i++)//percorre o vetor de inimigos ESQUELETO
    {
        if(data->enemy1[i].x == x_position && data->enemy1[i].y == y_position && data->enemy1[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do inimigo
        {
            data->enemy1[i].health_points = data->enemy1[i].health_points - damage;//decrementa da vida

            if(data->enemy1[i].health_points == 0) //adiciona os sons de dano e morte
            {
                PlaySound(sounds[5]);
            }
            else
            {
                PlaySound(sounds[6]);
            }
        }
    }
    for(i=0; i<MAX_ENEMIES2; i++)//percorre o vetor de inimigos CREEPER
    {
        if(data->enemy2[i].x == x_position && data->enemy2[i].y == y_position && data->enemy2[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do inimigo
        {
            data->enemy2[i].health_points = data->enemy2[i].health_points - damage;//decrementa da vida

            if(data->enemy2[i].health_points == 0) //adiciona os sons de dano e morte
            {
                PlaySound(sounds[7]);
                PlaySound(sounds[10]);
            }
            else
            {
                PlaySound(sounds[8]);
            }
        }
    }
    for(i=0; i<MAX_ENEMIES3; i++)//percorre o vetor de inimigos SLIME
    {
        if(data->enemy3[i].x == x_position && data->enemy3[i].y == y_position && data->enemy3[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do inimigo
        {
            data->enemy3[i].health_points = data->enemy3[i].health_points - damage;//decrementa da vida
            PlaySound(sounds[9]);
        }
    }
    for(i=0; i<MAX_ENEMIES; i++)//percorre o vetor de inimigos ZUMBI
    {
        if(data->enemy[i].x == x_position && data->enemy[i].y == y_position && data->enemy[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do inimigo
        {
            data->enemy[i].health_points = data->enemy[i].health_points - damage;//decrementa da vida

            if(data->enemy[i].health_points == 0) //adiciona os sons de dano e morte
            {
                PlaySound(sounds[3]);
            }
            else
            {
                PlaySound(sounds[4]);
            }
        }
    }
    for(i=0; i<MAX_PROJECTILES; i++)//percorre o vetor de inimigos
    {
        if(data->projectile[i].x == x_position && data->projectile[i].y == y_position && data->projectile[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do projetil
        {
            data->projectile[i].health_points = data->projectile[i].health_points - damage;//decrementa da vida
        }
    }
    for(i=0; i<MAX_PROJECTILES1; i++)//percorre o vetor de inimigos
    {
        if(data->projectile1[i].x == x_position && data->projectile1[i].y == y_position && data->projectile1[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do projetil
        {
            data->projectile1[i].health_points = data->projectile1[i].health_points - damage;//decrementa da vida
        }
    }
    for(i=0; i<MAX_WALLS; i++)//percorre o vetor de inimigos
    {
        if(data->wall[i].x == x_position && data->wall[i].y == y_position && data->wall[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do projetil
        {
            data->wall[i].health_points = data->wall[i].health_points - damage;//decrementa da vida
        }
    }
    for(i=0; i<MAX_BOMBS; i++)//percorre o vetor de inimigos
    {
        if(data->bomb[i].x == x_position && data->bomb[i].y == y_position && data->bomb[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do projetil
        {
            data->bomb[i].health_points = data->bomb[i].health_points - damage;//decrementa da vida
             PlaySound(sounds[10]);
        }
    }
}

bool shoot(struct STRUCT_DATA *data, struct STRUCT_STATS *shooter, struct STRUCT_STATS *shot, int initial_x,int initial_y,bool is_fire_charge)
{
        //funcao do tiro
    if(is_fire_charge){
        if(test_if_object_is_there(data,shooter,initial_x,initial_y,' '))//testa se pode ser "criado" o tiro
        {
            shot->x = shooter->x+initial_x;//a posicao do tiro eh a posicao do player mais a posicao inicial do tiro
            shot->y = shooter->y+initial_y;
            shot->dx = initial_x;//o deslocamento do tiro eh igual a posicao iniciao do tiro
            shot->dy = initial_y;//basicamente pega a direcao inicial do tiro e diz para qual direção sera seu deslocamento
            shot->health_points = 1;

            data->map[shot->y][shot->x] = 'o';//a matriz recebe o char do tiro e aparece na tela

            data->smoke[data->smoke_counter].x = shot->x;
            data->smoke[data->smoke_counter].y = shot->y;
            data->smoke[data->smoke_counter].lifetime.timer_start = clock();
            data->smoke[data->smoke_counter].lifetime.cooldown = SMOKE1_LIFETIME*CLOCKS_PER_SEC;
            data->smoke[data->smoke_counter].type = 1;

            data->shot_counter = data->shot_counter+1;//conta quantos projeteis foram acionados
            data->smoke_counter = data->smoke_counter+1;

            if(data->shot_counter==MAX_PROJECTILES)data->shot_counter = 0;//zera a contagem
            if(data->smoke_counter>=MAX_SMOKES)data->smoke_counter = 0;
            return 1;
        }else{
            return 0;
        }
    }else{
        if(test_if_object_is_there(data,shooter,initial_x,initial_y,' '))//testa se pode ser "criado" o tiro
        {
            shot->x = shooter->x+initial_x;//a posicao do tiro eh a posicao do player mais a posicao inicial do tiro
            shot->y = shooter->y+initial_y;
            shot->dx = initial_x;//o deslocamento do tiro eh igual a posicao iniciao do tiro
            shot->dy = initial_y;//basicamente pega a direcao inicial do tiro e diz para qual direção sera seu deslocamento
            shot->health_points = 1;

            data->map[shot->y][shot->x] = 'V';//a matriz recebe o char do tiro e aparece na tela
            data->stream[data->stream_counter].x = shot->x;
            data->stream[data->stream_counter].y = shot->y;
            data->stream[data->stream_counter].lifetime.timer_start = clock();
            data->stream[data->stream_counter].lifetime.cooldown = STREAM_LIFETIME*CLOCKS_PER_SEC;

            if(initial_x==0&&initial_y==-1)data->stream[data->stream_counter].type = 0;
            if(initial_x==1&&initial_y==-1)data->stream[data->stream_counter].type = 2;
            if(initial_x==1&&initial_y==0)data->stream[data->stream_counter].type = 4;
            if(initial_x==1&&initial_y==1)data->stream[data->stream_counter].type = 6;
            if(initial_x==0&&initial_y==1)data->stream[data->stream_counter].type = 8;
            if(initial_x==-1&&initial_y==1)data->stream[data->stream_counter].type = 10;
            if(initial_x==-1&&initial_y==0)data->stream[data->stream_counter].type = 12;
            if(initial_x==-1&&initial_y==-1)data->stream[data->stream_counter].type = 14;

            data->shot1_counter = data->shot1_counter+1;//conta quantos projeteis foram acionados
            data->stream_counter = data->stream_counter+1;

            if(data->shot1_counter==MAX_PROJECTILES)data->shot1_counter = 0;//zera a contagem
            if(data->stream_counter==MAX_STREAMS)data->stream_counter = 0;//zera a contagem
        }
    }
}

bool place_bomb(struct STRUCT_DATA *data, struct STRUCT_STATS *placer,int initial_x,int initial_y)
{
    //funcao de botar bomba

    if(test_if_object_is_there(data,placer,initial_x,initial_y,' '))//testa se pode ser "criado" o tiro
    {
        data->bomb[data->bomb_counter].x = placer->x+initial_x;//a posicao do tiro eh a posicao do player mais a posicao inicial do tiro
        data->bomb[data->bomb_counter].y = placer->y+initial_y;
        data->bomb[data->bomb_counter].health_points = 1;

        data->map[data->bomb[data->bomb_counter].y][data->bomb[data->bomb_counter].x] = 'B';//a matriz recebe o char da bomba e aparece na tela

        data->bomb_inventory = data->bomb_inventory - 1;
        data->bomb_counter = data->bomb_counter+1;//conta quantos projeteis foram acionados
        if(data->bomb_counter==MAX_PROJECTILES)//se a contagem for igual ao maximo de projeteis na tela
        {
            data->bomb_counter = 0;//zera a contagem
        }
        return true;
    }else return false;
}

bool explode(struct STRUCT_DATA *data, int x_origin,int y_origin, float radius, int beam_n, int damage, Sound sounds[])
{
    int i, k, x, y;
    float j;
    float radians ,beam_interval = beam_n/(2*PI);
    int flag ,damaged_position[MAP_LINES][MAP_COLUMNS] = {0};

    for(i=0; i<beam_n; i++)
    {
        flag = 1;
        radians = beam_interval*i;
        for(j=0;j<=radius&&flag==1;j=j+1){
            x = (int)round(cos(radians)*j);
            y = (int)round(sin(radians)*j);
            if((y_origin+y)>=0&&(y_origin+y)<MAP_LINES&&(x_origin+x)>=0&&(x_origin+x)<MAP_COLUMNS){
                if(damaged_position[y_origin+y][x_origin+x]==0){
                    damage_position(data,x_origin+x, y_origin+y ,(int)round(damage -(j/radius)*damage), sounds);
                    damaged_position[y_origin+y][x_origin+x]=1;
                }
                if(data->map[y_origin+y][x_origin+x]=='#'){
                    flag = 0;
                }
            }else{
                flag = 0;
            }
        }

    }
    for(i=0;i<MAP_LINES;i++){
        for(k=0;k<MAP_COLUMNS;k++){
            if(damaged_position[i][k]==1){
                data->smoke[data->smoke_counter].x = k;
                data->smoke[data->smoke_counter].y = i;
                data->smoke[data->smoke_counter].lifetime.timer_start = clock();
                data->smoke[data->smoke_counter].lifetime.cooldown = SMOKE_LIFETIME*CLOCKS_PER_SEC;
                data->smoke[data->smoke_counter].type = 0;
                data->smoke_counter = data->smoke_counter+1;
                if(data->smoke_counter>=MAX_SMOKES) data->smoke_counter = 0;
            }
        }
    }
}

void update_arc_name(char archive_name[MAX_ARCHIVE_NAME],int level)
{
    archive_name[5] = (level%10) + '0';
    archive_name[4] = ((level%100)/10) + '0';
    archive_name[3] = (level/100) + '0';
}

bool load_map(struct STRUCT_DATA *data,char archive_name[])//carrega o mapa de um arcuivo txt
{
    //+1 devido o \n no fim de cada linha

    FILE *txt_arc = fopen(archive_name, "r");//o ponteiro txt_arc recebe o arcuivo
    if (txt_arc == NULL)//testa se o arcuivo existe na pasta
    {
        printf("Opening error\n");//se inexistente imprime na tela um erro
    }
    else//se aberto corretamente
    {
        for(int n_line = 0; n_line<30; n_line++)//vasculha linha por linha do mapa ate l<30
        {
            if (fread(data->map[n_line], sizeof(char), (61), txt_arc) != (61))//realiza a leitura dessas linhas por caractere sendo um total de 61
            {
                printf("Reading error in line %d\n", n_line);//se a leitura der errado imprime na tela uma mensagem de erro e qual linha
                fclose(txt_arc);
                return 0;
            }
        }

    }
    fclose(txt_arc);//fecha o arcuivo
    printf("Successful loading\n");//se tudo der certo imprime na tela sucesso no carregamento
    return 1;
}

void clear_data(struct STRUCT_STATS *object)//reseta as informações do objetos
{
    object->x = -1;
    object->y = -1;
    object->dx = 0;
    object->dy = 0;
    object->head_direction_x = 0;
    object->head_direction_y = 0;

    object->health_points = 0;;

    object->step.timer_start = 0;
    object->step.timer_current = 0;
    object->step.cooldown = 0;

    object->action.timer_start = 0;
    object->action.timer_current = 0;
    object->action.cooldown = 0;
}

void clear_effect_data(struct STRUCT_EFFECT *effect)//reseta as informações do objetos
{
    effect->x = -1;
    effect->y = -1;
    effect->type = 0;

    effect->lifetime.timer_start = 0;
    effect->lifetime.timer_current = 0;
    effect->lifetime.cooldown = 0;
}

void clear_all_data(struct STRUCT_DATA *data)//reseta as informações do objetos
{
    int i;

    data->intends_to_put_bomb = false;
    data->bomb_inventory = 0;

    data->invunerability.timer_start = 0;
    data->invunerability.timer_current = 0;
    data->invunerability.cooldown = 0;

    for(i=0; i<MAX_ENEMIES; i++)clear_data(&data->enemy[i]);
    for(i=0; i<MAX_ENEMIES1; i++)clear_data(&data->enemy1[i]);
    for(i=0; i<MAX_ENEMIES2; i++)clear_data(&data->enemy2[i]);
    for(i=0; i<MAX_ENEMIES3; i++)clear_data(&data->enemy3[i]);
    for(i=0; i<MAX_PROJECTILES; i++)clear_data(&data->projectile[i]);
    for(i=0; i<MAX_PROJECTILES1; i++)clear_data(&data->projectile1[i]);
    for(i=0; i<MAX_WALLS; i++)clear_data(&data->wall[i]);
    for(i=0; i<MAX_TRAPS; i++)clear_data(&data->trap[i]);
    for(i=0; i<MAX_BOMBS; i++)clear_data(&data->bomb[i]);
    for(i=0; i<MAX_PORTALS; i++)clear_data(&data->portal[i]);

    for(i=0; i<MAX_SMOKES; i++)clear_effect_data(&data->smoke[i]);
    for(i=0; i<MAX_STREAMS; i++)clear_effect_data(&data->stream[i]);


    data->shot_counter = 0;
    data->shot1_counter = 0;
    data->bomb_counter = 0;
    data->smoke_counter = 0;
    data->stream_counter = 0;
    data->wall_counter = 0;
    data->wall1_counter = 0;
    data->enemy_counter = 0;
    data->enemy1_counter = 0;
    data->enemy2_counter = 0;
    data->enemy3_counter = 0;
    data->portal_counter = 0;
    data->trap_counter = 0;
}

int main(void)
{

    srand(time(NULL)); //inicializa o gerador de numero aleatório
    InitWindow(MAP_PIXEL_WIDTH, MAP_PIXEL_HEIGHT+HUD_PIXEL_HEIGHT, "Joguin do balacobaco");//Inicializa janela, com certo tamanho e tıtulo
    InitAudioDevice();
    SetTargetFPS(FPS);// Ajusta a execucao do jogo para 60 frames por segundo
    SetExitKey(KEY_NULL);
    SetMasterVolume(0.25);

    struct STRUCT_DATA data;

    int i,j,k;//contadores declarados aqui pois sao enviados por referencia
    int past_x ,past_y;
    int option = 1;
    int enemies_alive;
    int intends_to_save_or_load = 0;
    char map_file_name[MAX_ARCHIVE_NAME] = "map000.txt";//nome da versao do mapa que estamos jogando
    bool menu_open = true;

    Texture2D textures[22]; //Carrega todas as texturas do jogo e armazena em um vetor
    textures[0] = LoadTexture("resources/textures/steve.png");
    textures[1] = LoadTexture("resources/textures/skeleton.png");
    textures[2] = LoadTexture("resources/textures/zombie.png");
    textures[3] = LoadTexture("resources/textures/obsidian.png");
    textures[4] = LoadTexture("resources/textures/portal.png");
    textures[5] = LoadTexture("resources/textures/lava.png");
    textures[6] = LoadTexture("resources/textures/tnt.png");
    textures[7] = LoadTexture("resources/textures/projectile.png");
    textures[8] = LoadTexture("resources/textures/stonebricks1.png");
    textures[9] = LoadTexture("resources/textures/stonebricks2.png");
    textures[10] = LoadTexture("resources/textures/stonebricks3.png");
    textures[11] = LoadTexture("resources/textures/deepslate.png");
    textures[12] = LoadTexture("resources/textures/creeper.png");
    textures[13] = LoadTexture("resources/textures/slime.png");
    textures[14] = LoadTexture("resources/textures/apple.png");
    textures[15] = LoadTexture("resources/textures/goldenapple.png");
    textures[16] = LoadTexture("resources/textures/hearthhud.png");
    textures[17] = LoadTexture("resources/textures/yellowhearthhud.png");
    textures[18] = LoadTexture("resources/textures/blackhearthhud.png");
    textures[19] = LoadTexture("resources/textures/standardbackground.png");
    textures[20] = LoadTexture("resources/textures/buttonpressed.png");
    textures[21] = LoadTexture("resources/textures/button.png");

    Sound sounds[17]; //Carrega todos os sons do jogo e armazena em um vetor
    sounds[0] = LoadSound("resources/sounds/sound1.wav");
    sounds[1] = LoadSound("resources/sounds/sound2.wav");
    sounds[2] = LoadSound("resources/sounds/sound3.wav");
    sounds[3] = LoadSound("resources/sounds/zombiedeath.wav");
    sounds[4] = LoadSound("resources/sounds/zombiedamage.wav");
    sounds[5] = LoadSound("resources/sounds/skeletondeath.wav");
    sounds[6] = LoadSound("resources/sounds/skeletondamage.wav");
    sounds[7] = LoadSound("resources/sounds/creeperdeath.wav");
    sounds[8] = LoadSound("resources/sounds/creeperdamage.wav");
    sounds[9] = LoadSound("resources/sounds/slimedeath.wav");
    sounds[10] = LoadSound("resources/sounds/explosion.wav");
    sounds[11] = LoadSound("resources/sounds/stevedamage.wav");
    sounds[12] = LoadSound("resources/sounds/portal.wav");
    sounds[13] = LoadSound("resources/sounds/projectilesound.wav");
    sounds[14] = LoadSound("resources/sounds/arrowsound.wav");
    sounds[15] = LoadSound("resources/sounds/background.wav");
    sounds[16] = LoadSound("resources/sounds/eatapple.wav");

    Font font = LoadFont("resources/font.ttf"); ////Carrega a fonte do jogo

    //garante que todos os dados de entidades estejam zeradas
    clear_data(&data.player);
    clear_all_data(&data);

    data.player.health_points = PLAYER_HP;
    update_arc_name(map_file_name,0);
    load_map(&data,map_file_name);//FUNCAO DE CARREGAMENTO DO MAPA
    scan_map(&data);//FUNCAO DE SCANEAMENTO DE MAPA

    while (!WindowShouldClose()){//Roda enquanto não for fechada a tela

        if (IsKeyPressed(KEY_F)) {
            ToggleFullscreen(); // Alternar entre tela cheia e janela ao pressionar a tecla F11
        }

        if(!IsSoundPlaying(sounds[15]))
            PlaySound(sounds[15]);

        if(IsKeyPressed(KEY_M)){
            menu_open = !(menu_open);
        }

        if (!menu_open) //Roda enquanto o menu não for aberto
        {
            enemies_alive = 0;

            for(i=0; i<MAX_ENEMIES; i++) if(data.enemy[i].health_points>0) enemies_alive++;
            for(i=0; i<MAX_ENEMIES1; i++) if(data.enemy1[i].health_points>0) enemies_alive++;
            for(i=0; i<MAX_ENEMIES2; i++) if(data.enemy2[i].health_points>0) enemies_alive++;
            for(i=0; i<MAX_ENEMIES3; i++) if(data.enemy3[i].health_points>0) enemies_alive++;
            //===================================================================================================================
            //PLAYER

            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_A) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S))//ve se alguma das teclas estao sendo pressionadas
            {
                data.player.step.timer_current = clock();
                if (data.player.step.timer_current - data.player.step.timer_start >= data.player.step.cooldown)//se o cooldown for zero, pega a intenção de movimento
                {
                    //determina a intencao de movimento de acordo com a combinacao de teclas
                    if (IsKeyDown(KEY_D)) data.player.dx++;
                    if (IsKeyDown(KEY_A)) data.player.dx--;
                    if (IsKeyDown(KEY_W)) data.player.dy--;
                    if (IsKeyDown(KEY_S)) data.player.dy++;
                    data.player.step.timer_start = clock();
                }
            }

            if(test_if_object_is_there(&data,&data.player,data.player.dx,data.player.dy,'B')){
                for(i=0; i<MAX_BOMBS; i++){
                    if(data.bomb[i].x==(data.player.x+data.player.dx)&&(data.bomb[i].y==(data.player.y+data.player.dy))){
                        data.map[data.bomb[i].y][data.bomb[i].x] = ' ';
                        clear_data(&data.bomb[i]);
                        data.bomb_inventory = data.bomb_inventory+1;
                    }
                }
            }

            if(test_if_object_is_there(&data,&data.player,data.player.dx,data.player.dy,'M')){

                    data.map[data.player.dy+data.player.y][data.player.dx+data.player.x] = ' ';

                    data.player.health_points = data.player.health_points+APPLE_HEALING_POINTS;
                    PlaySound(sounds[16]);
                    if(data.player.health_points>PLAYER_HP) data.player.health_points = PLAYER_HP;
            }

            if(test_if_object_is_there(&data,&data.player,data.player.dx,data.player.dy,'D')){

                data.map[data.player.dy+data.player.y][data.player.dx+data.player.x] = ' ';
                PlaySound(sounds[16]);
                data.invunerability.timer_start = clock();
            }

            data.invunerability.timer_current = clock();
            if(data.invunerability.timer_current - data.invunerability.timer_start <= data.invunerability.cooldown && data.invunerability.timer_start>=PLAYER_INVUNERABILITY_COOLDOWN*CLOCKS_PER_SEC){
                data.player.health_points = PLAYER_HP;
            }

            if(test_if_object_is_there(&data,&data.player,data.player.dx,data.player.dy,'P')){
                for(i=0; i<MAX_PORTALS; i++){
                    if(data.portal[i].x==(data.player.x+data.player.dx)&&(data.portal[i].y==(data.player.y+data.player.dy))&&enemies_alive==0){
                        data.level = data.level+1;
                        PlaySound(sounds[12]);

                        update_arc_name(map_file_name,data.level);
                        clear_all_data(&data);
                        load_map(&data,map_file_name);//FUNCAO DE CARREGAMENTO DO MAPA
                        scan_map(&data);//FUNCAO DE SCANEAMENTO DE MAPA
                    }
                }
            }

            if(test_move(&data,&data.player)){
                move(&data,&data.player);//se sim, ele move
            }

            //reseta na intenção de movimento para proximo ciclo
            data.player.dx = 0;
            data.player.dy = 0;

            test_if_dead(&data, &data.player);

            //===================================================================================================================
            //AÇÃO DE TIRO

            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN))//ve se alguma das teclas estao sendo pressionadas
            {
                data.player.action.timer_current = clock();
                if (data.player.action.timer_current - data.player.action.timer_start >= data.player.action.cooldown)//se o cooldown for zero, pega a intenção de movimento
                {
                    //determina a intencao de tiro de acordo com a combinacao de teclas
                    if (IsKeyDown(KEY_RIGHT)) data.player.head_direction_x++;
                    if (IsKeyDown(KEY_LEFT)) data.player.head_direction_x--;
                    if (IsKeyDown(KEY_UP)) data.player.head_direction_y--;
                    if (IsKeyDown(KEY_DOWN)) data.player.head_direction_y++;
                    data.player.action.timer_start = clock();
                }
            }

            if (IsKeyDown(KEY_E)) data.intends_to_put_bomb = true;

            if(data.player.head_direction_x!=0||data.player.head_direction_y!=0)//se alguma das direções não for nula
            {
                if(data.intends_to_put_bomb==true&&data.bomb_inventory>0){
                    if(place_bomb(&data,&data.player, data.player.head_direction_x, data.player.head_direction_y)) data.intends_to_put_bomb = false;
                }else{
                    if(!shoot(&data,&data.player, &data.projectile[data.shot_counter], data.player.head_direction_x, data.player.head_direction_y,true)){
                        damage_position(&data,data.player.x+data.player.head_direction_x,data.player.y+data.player.head_direction_y,1, sounds);
                    }//atira
                    PlaySound(sounds[13]);
                    data.intends_to_put_bomb = false;
                }
            }

            data.player.head_direction_x = 0;
            data.player.head_direction_y = 0;

            for(i=0; i<MAX_PROJECTILES; i++)//percorre o vetor de projéteis
            {
                if(test_move(&data,&data.projectile[i]))//se o projétil puder mover
                {
                    move(&data,&data.projectile[i]);//move o projétil

                    data.smoke[data.smoke_counter].x = data.projectile[i].x;
                    data.smoke[data.smoke_counter].y = data.projectile[i].y;
                    data.smoke[data.smoke_counter].lifetime.timer_start = clock();
                    data.smoke[data.smoke_counter].lifetime.cooldown = SMOKE1_LIFETIME*CLOCKS_PER_SEC;
                    data.smoke[data.smoke_counter].type = 1;

                    data.smoke_counter = data.smoke_counter+1;

                    if(data.smoke_counter>=MAX_SMOKES)data.smoke_counter = 0;
                }
                else//se não puder mover
                {
                    damage_position(&data,data.projectile[i].x+data.projectile[i].dx,data.projectile[i].y+data.projectile[i].dy ,1, sounds);
                    data.projectile[i].health_points--;
                    test_if_dead(&data,&data.projectile[i]);
                }
            }

            for(i=0; i<MAX_PROJECTILES1; i++)//percorre o vetor de projéteis FLECHAS
            {
                if(test_move(&data,&data.projectile1[i]))//se o projétil puder mover
                {
                    move(&data,&data.projectile1[i]);//move o projétil
                    data.stream[data.stream_counter].x = data.projectile1[i].x;
                    data.stream[data.stream_counter].y = data.projectile1[i].y;
                    data.stream[data.stream_counter].lifetime.timer_start = clock();
                    data.stream[data.stream_counter].lifetime.cooldown = STREAM_LIFETIME*CLOCKS_PER_SEC;

                    if(data.projectile1[i].dx==0&&data.projectile1[i].dy==-1)data.stream[data.stream_counter].type = 0;
                    if(data.projectile1[i].dx==1&&data.projectile1[i].dy==-1)data.stream[data.stream_counter].type = 2;
                    if(data.projectile1[i].dx==1&&data.projectile1[i].dy==0)data.stream[data.stream_counter].type = 4;
                    if(data.projectile1[i].dx==1&&data.projectile1[i].dy==1)data.stream[data.stream_counter].type = 6;
                    if(data.projectile1[i].dx==0&&data.projectile1[i].dy==1)data.stream[data.stream_counter].type = 8;
                    if(data.projectile1[i].dx==-1&&data.projectile1[i].dy==1)data.stream[data.stream_counter].type = 10;
                    if(data.projectile1[i].dx==-1&&data.projectile1[i].dy==0)data.stream[data.stream_counter].type = 12;
                    if(data.projectile1[i].dx==-1&&data.projectile1[i].dy==-1)data.stream[data.stream_counter].type = 14;

                    data.stream_counter = data.stream_counter+1;

                    if(data.stream_counter>=MAX_SMOKES)data.stream_counter = 0;
                }
                else//se não puder mover
                {
                    damage_position(&data,data.projectile1[i].x+data.projectile1[i].dx,data.projectile1[i].y+data.projectile1[i].dy ,1, sounds);
                    data.projectile1[i].health_points--;
                    test_if_dead(&data,&data.projectile1[i]);
                }
            }

            //===================================================================================================================
            //MOVIMENTO DO INIMIGO ZUMBI

            for(i=0; i<MAX_ENEMIES; i++)//percorre o vetor de inimigos
            {
                test_if_dead(&data,&data.enemy[i]);
                data.enemy[i].step.timer_current = clock();
                if (data.enemy[i].step.timer_current - data.enemy[i].step.timer_start >= data.enemy[i].step.cooldown)//se o cooldown for zero, pega a intenção de movimento
                {
                    if (data.player.x > data.enemy[i].x) data.enemy[i].dx = 1;
                    if (data.player.x < data.enemy[i].x) data.enemy[i].dx = -1;
                    if (data.player.y > data.enemy[i].y) data.enemy[i].dy = 1;
                    if (data.player.y < data.enemy[i].y) data.enemy[i].dy = -1;
                    data.enemy[i].step.timer_start = clock();

                    if(test_move(&data,&data.enemy[i]))//testa se ele pode mover
                    {
                        move(&data,&data.enemy[i]);//move
                    }
                    else if((data.enemy[i].x>=0 && data.enemy[i].x<MAP_COLUMNS && data.enemy[i].y>=0 && data.enemy[i].y<MAP_LINES))
                    {
                        data.enemy[i].action.timer_current = clock();
                        if((test_if_object_is_there (&data,&data.enemy[i],data.enemy[i].dx,data.enemy[i].dy,'#')||test_if_object_is_there (&data,&data.enemy[i],data.enemy[i].dx,data.enemy[i].dy,'J'))&&data.enemy[i].action.timer_current - data.enemy[i].action.timer_start >= data.enemy[i].action.cooldown)
                        {
                            damage_position(&data,data.enemy[i].x+data.enemy[i].dx,data.enemy[i].y+data.enemy[i].dy,1, sounds);
                            data.enemy[i].action.timer_start = clock();
                        }
                        data.enemy[i].dx = (-1 + (rand()%3));
                        data.enemy[i].dy = (-1 + (rand()%3));
                        if(test_move(&data,&data.enemy[i]))//testa se ele pode mover
                        {
                            move(&data,&data.enemy[i]);//move
                        }
                    }
                    data.enemy[i].dx = 0;
                    data.enemy[i].dy = 0;
                }
            }

            //===================================================================================================================
            //MOVIMENTO DO INIMIGO ESQUELETO

            for(i=0; i<MAX_ENEMIES1; i++)//percorre o vetor de inimigos
            {
                test_if_dead(&data,&data.enemy1[i]);
                data.enemy1[i].step.timer_current = clock();
                data.enemy1[i].action.timer_current = clock();
                if (data.enemy1[i].action.timer_current - data.enemy1[i].action.timer_start >= data.enemy1[i].action.cooldown)//se o cooldown for zero, pega a intenção de movimento
                {
                    if(data.player.x == data.enemy1[i].x){//se estiver alinhado em x
                        if(data.player.y > data.enemy1[i].y){//e o player estiver abaixo
                            PlaySound(sounds[14]);
                            shoot(&data,&data.enemy1[i],&data.projectile1[data.shot_counter],0,1,false);
                            data.enemy1[i].action.timer_start = clock();
                        }
                        if(data.player.y < data.enemy1[i].y){//e o player estiver abaixo
                            PlaySound(sounds[14]);
                            shoot(&data,&data.enemy1[i],&data.projectile1[data.shot1_counter],0,-1,false);
                            data.enemy1[i].action.timer_start = clock();
                        }
                    }
                    if(data.player.y == data.enemy1[i].y){//se estiver alinhado em y
                        if(data.player.x > data.enemy1[i].x){//e o player estiver a direita
                            PlaySound(sounds[14]);
                            shoot(&data,&data.enemy1[i],&data.projectile1[data.shot1_counter],1,0,false);
                            data.enemy1[i].action.timer_start = clock();
                        }
                        if(data.player.x < data.enemy1[i].x){//e o player estiver a esquerda
                            PlaySound(sounds[14]);
                            shoot(&data,&data.enemy1[i],&data.projectile1[data.shot1_counter],-1,0,false);
                            data.enemy1[i].action.timer_start = clock();
                        }
                    }
                }

                if (data.enemy1[i].step.timer_current - data.enemy1[i].step.timer_start >= data.enemy1[i].step.cooldown)//se o cooldown for zero, pega a intenção de movimento
                {
                    if (abs(data.player.x-data.enemy1[i].x)<abs(data.player.y-data.enemy1[i].y)){
                    if (data.player.x > data.enemy1[i].x) data.enemy1[i].dx = 1;
                    if (data.player.x < data.enemy1[i].x) data.enemy1[i].dx = -1;
                    }
                    if (abs(data.player.x-data.enemy1[i].x)>abs(data.player.y-data.enemy1[i].y)){
                    if (data.player.y > data.enemy1[i].y) data.enemy1[i].dy = 1;
                    if (data.player.y < data.enemy1[i].y) data.enemy1[i].dy = -1;
                    }
                    data.enemy1[i].step.timer_start = clock();

                    if(test_move(&data,&data.enemy1[i]))//testa se ele pode mover
                    {
                        move(&data,&data.enemy1[i]);//move
                    }
                    else if((data.enemy1[i].x>=0 && data.enemy1[i].x<MAP_COLUMNS && data.enemy1[i].y>=0 && data.enemy1[i].y<MAP_LINES))
                    {
                        if(test_if_object_is_there (&data,&data.enemy1[i],data.enemy1[i].dx,data.enemy1[i].dy,'#')||test_if_object_is_there (&data,&data.enemy1[i],data.enemy1[i].dx,data.enemy1[i].dy,'J')&&data.enemy1[i].action.timer_current - data.enemy1[i].action.timer_start >= data.enemy1[i].action.cooldown)
                        {
                            damage_position(&data,data.enemy1[i].x+data.enemy1[i].dx,data.enemy1[i].y+data.enemy1[i].dy,1, sounds);
                            data.enemy1[i].action.timer_start = clock();
                        }
                        data.enemy1[i].dx = (-1 + (rand()%3));
                        data.enemy1[i].dy = (-1 + (rand()%3));
                        if(test_move(&data,&data.enemy1[i]))//testa se ele pode mover
                        {
                            move(&data,&data.enemy1[i]);//move
                        }
                    }
                }
                data.enemy1[i].dx = 0;
                data.enemy1[i].dy = 0;
            }
            //===================================================================================================================
            //MOVIMENTO DO INIMIGO CREEPER

            for(i=0; i<MAX_ENEMIES2; i++)//percorre o vetor de inimigos
            {
                past_x = data.enemy2[i].x;
                past_y = data.enemy2[i].y;

                data.enemy2[i].step.timer_current = clock();
                if (data.enemy2[i].step.timer_current - data.enemy2[i].step.timer_start >= data.enemy2[i].step.cooldown)//se o cooldown for zero, pega a intenção de movimento
                {
                    if (data.player.x > data.enemy2[i].x)
                        data.enemy2[i].dx = 1;
                    if (data.player.x < data.enemy2[i].x)
                        data.enemy2[i].dx = -1;
                    if (data.player.y > data.enemy2[i].y)
                        data.enemy2[i].dy = 1;
                    if (data.player.y < data.enemy2[i].y)
                        data.enemy2[i].dy = -1;
                    data.enemy2[i].step.timer_start = clock();

                    if(test_move(&data,&data.enemy2[i]))//testa se ele pode mover
                    {
                        move(&data,&data.enemy2[i]);//move
                    }
                    else if((data.enemy2[i].x>=0 && data.enemy2[i].x<MAP_COLUMNS && data.enemy2[i].y>=0 && data.enemy2[i].y<MAP_LINES))
                    {
                        data.enemy2[i].action.timer_current = clock();
                        if(test_if_object_is_there (&data,&data.enemy2[i],data.enemy2[i].dx,data.enemy2[i].dy,'J')){
                            PlaySound(sounds[10]);
                            data.enemy2[i].health_points = 0;
                        }else if(test_if_object_is_there (&data,&data.enemy2[i],data.enemy2[i].dx,data.enemy2[i].dy,'#')&&data.enemy2[i].action.timer_current - data.enemy2[i].action.timer_start >= data.enemy2[i].action.cooldown)
                        {
                            damage_position(&data,data.enemy2[i].x+data.enemy2[i].dx,data.enemy2[i].y+data.enemy2[i].dy,1, sounds);
                            data.enemy2[i].action.timer_start = clock();
                        }
                        data.enemy2[i].dx = (-1 + (rand()%3));
                        data.enemy2[i].dy = (-1 + (rand()%3));
                        if(test_move(&data,&data.enemy2[i]))//testa se ele pode mover
                        {
                            move(&data,&data.enemy2[i]);//move
                        }
                    }
                    data.enemy2[i].dx = 0;
                    data.enemy2[i].dy = 0;
                }
                if(test_if_dead(&data,&data.enemy2[i])){
                    explode(&data,past_x,past_y,CREEPER_RADIUS,180,CREEPER_DAMAGE, sounds);
                }
            }

            //===================================================================================================================
            //MOVIMENTO DO INIMIGO SLIME

            for(i=0; i<MAX_ENEMIES3; i++)//percorre o vetor de inimigos
            {
                test_if_dead(&data,&data.enemy3[i]);
                data.enemy3[i].step.timer_current = clock();
                if (data.enemy3[i].step.timer_current - data.enemy3[i].step.timer_start >= data.enemy3[i].step.cooldown)//se o cooldown for zero, pega a intenção de movimento
                {
                    data.enemy3[i].step.timer_start = clock();

                    if(test_move(&data,&data.enemy3[i]))//testa se ele pode mover
                    {
                        move(&data,&data.enemy3[i]);//move
                    }
                    else
                    {
                        if(test_if_object_is_there (&data,&data.enemy3[i],data.enemy3[i].dx,data.enemy3[i].dy,'J')){
                            damage_position(&data,data.enemy3[i].x+data.enemy3[i].dx,data.enemy3[i].y+data.enemy3[i].dy,1, sounds);
                        }
                        do{
                        data.enemy3[i].dx = (-1 + (rand()%3));
                        data.enemy3[i].dy = (-1 + (rand()%3));
                        }while(!test_move(&data,&data.enemy3[i])&&data.enemy3[i].dx==0||data.enemy3[i].dy==0);
                    }
                }
            }

            //===================================================================================================================
            //ARMADILHAS

            for(i=0; i<MAX_TRAPS; i++)//percorre o vetor de inimigos
            {
                if(test_if_object_is_there(&data,&data.trap[i],0,0,'J')){
                    data.trap[i].action.timer_current = clock();
                    if (data.trap[i].action.timer_current - data.trap[i].action.timer_start >= data.trap[i].action.cooldown)//se o cooldown for zero, pega a intenção de movimento
                    {
                        damage_position(&data,data.trap[i].x ,data.trap[i].y , 1, sounds);
                        data.trap[i].action.timer_start = clock();
                    }
                }
            }

            //===================================================================================================================
            //PAREDES

            for(i=0; i<MAX_WALLS; i++)//percorre o vetor de inimigos
            {
                test_if_dead(&data,&data.wall[i]);
            }

            //===================================================================================================================
            //BOMBAS

            for(i=0; i<MAX_BOMBS; i++)
            {
                past_x = data.bomb[i].x;
                past_y = data.bomb[i].y;
                if(data.bomb[i].x>=0&&data.bomb[i].x<MAP_COLUMNS&&data.bomb[i].y>=0&&data.bomb[i].y<MAP_LINES){
                    if(test_if_dead(&data,&data.bomb[i])){
                        explode(&data,past_x,past_y,BOMB_RADIUS,180,BOMB_DAMAGE, sounds);
                    }
                }
            }
        }
        //===================================================================================================================
        //MAPA

        BeginDrawing();//inicializa ambiente para desenho
        if(menu_open){
            draw_menu(option,textures, font);
            if (option>=1&&option<=5){
                if (IsKeyPressed(KEY_DOWN)&&option<5&&option>=1) option++;
                if (IsKeyPressed(KEY_UP)&&option>1&&option<=5) option--;

                if ((IsKeyPressed(KEY_ENTER)&&option==1)||IsKeyPressed(KEY_N)){
                clear_all_data(&data);
                data.player.health_points = PLAYER_HP;
                update_arc_name(map_file_name,0);
                load_map(&data,map_file_name);//FUNCAO DE CARREGAMENTO DO MAPA
                scan_map(&data);//FUNCAO DE SCANEAMENTO DE MAPA
                menu_open = false;
                }
                if ((IsKeyPressed(KEY_ENTER)&&option==2)||IsKeyPressed(KEY_L)){
                    option = option+6;
                    intends_to_save_or_load = -1;
                }
                if ((IsKeyPressed(KEY_ENTER)&&option==3)||IsKeyPressed(KEY_S)){
                    option = option+6;
                    intends_to_save_or_load = 1;
                }
                if ((IsKeyPressed(KEY_ENTER)&&option==4)||IsKeyPressed(KEY_Q)) CloseWindow();
                if ((IsKeyPressed(KEY_ENTER)&&option==5)) menu_open = false;
            }else{
                if (IsKeyPressed(KEY_DOWN)&&option<11&&option>=7) option++;
                if (IsKeyPressed(KEY_UP)&&option>7&&option<=11) option--;

                if ((IsKeyPressed(KEY_ENTER)&&option==7)||IsKeyPressed(49)){
                    if(intends_to_save_or_load==1) save_data(data,1);
                    if(intends_to_save_or_load==-1) load_data(&data,1);
                    option = option-6;
                    menu_open = false;
                    intends_to_save_or_load = 0;
                }
                if ((IsKeyPressed(KEY_ENTER)&&option==8)||IsKeyPressed(50)){
                    if(intends_to_save_or_load==1) save_data(data,2);
                    if(intends_to_save_or_load==-1) load_data(&data,2);
                    option = option-6;
                    menu_open = false;
                    intends_to_save_or_load = 0;
                }
                if ((IsKeyPressed(KEY_ENTER)&&option==9)||IsKeyPressed(51)){
                    if(intends_to_save_or_load==1) save_data(data,3);
                    if(intends_to_save_or_load==-1) load_data(&data,3);
                    option = option-6;
                    menu_open = false;
                    intends_to_save_or_load = 0;
                }
                if ((IsKeyPressed(KEY_ENTER)&&option==10)||IsKeyPressed(52)){
                    if(intends_to_save_or_load==1) save_data(data,4);
                    if(intends_to_save_or_load==-1) load_data(&data,4);
                    option = option-6;
                    menu_open = false;
                    intends_to_save_or_load = 0;
                }
                if ((IsKeyPressed(KEY_ENTER)&&option==11)||IsKeyPressed(53)){
                    if(intends_to_save_or_load==1) save_data(data,5);
                    if(intends_to_save_or_load==-1) load_data(&data,5);
                    option = option-6;
                    menu_open = false;
                    intends_to_save_or_load = 0;
                }
            }
        }else{
            draw_map(&data,textures, font);
            draw_effects(&data);
            if(data.player.health_points==0){
                DrawRectangle(0,0,MAP_PIXEL_WIDTH,MAP_PIXEL_HEIGHT,(Color){0,0,0,190});//limpa a tela e define cor de fundo
                DrawTextEx(font,"You Died", (Vector2){(MAP_PIXEL_WIDTH/2)-200, (MAP_PIXEL_HEIGHT/2)-50}, 100,0, WHITE);
                DrawTextEx(font,"Press M to open Menu", (Vector2){(MAP_PIXEL_WIDTH/2)-270, (MAP_PIXEL_HEIGHT/2)+80}, 50,0, WHITE);
            }
        }
        draw_hud(&data,textures, font);
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();// Fecha a janela e o contexto OpenGL
    return 0;
}

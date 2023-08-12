/*
scan_map do mapa(tipo draw_map)
fogueiras devem ser andavéis? no test_move

*/

#include <raylib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define MAP_HEIGHT 30 //linhas
#define MAP_WIDTH 60 //colunas
#define LADO 20 //em pixels
#define LARGURA 1200 //em pixels
#define ALTURA 600  //em pixels
#define MAX_ENEMIES 1 //inimigos
#define MAX_PROJECTILES_ON_SCREEN 100
#define FPS 60
#define MAX_ARCHIVE_NAME 50

struct STRUCT__STATS{
    int x;
    int y;
    int dx;
    int dy;
    int health_points;
    int flag;
    int count;
    int step_cooldown;
    int step_cooldown_counter;
    int shoot_cooldown;
    int shoot_cooldown_counter;
};

int draw_map (char matrix[MAP_HEIGHT][MAP_WIDTH+1],struct STRUCT__STATS object) {

    BeginDrawing();
    ClearBackground(RAYWHITE);//Limpa a tela e define cor de fundo

    for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                    switch(matrix[y][x]){

                    case '#':
                        DrawRectangle(x*LADO, y*LADO, LADO, LADO, BLACK);
                        break;
                    case 'I':
                        DrawRectangle(x*LADO, y*LADO, LADO, LADO, RED);
                        break;
                    case 'B':
                        DrawRectangle(x*LADO, y*LADO, LADO, LADO, GRAY);
                        break;
                    case 'P':
                        DrawRectangle(x*LADO, y*LADO, LADO, LADO, PURPLE);
                        break;
                    case 'J':
                        DrawRectangle(x*LADO, y*LADO, LADO, LADO, GREEN);
                        break;
                    case 'X':
                        DrawRectangle(x*LADO, y*LADO, LADO, LADO, BROWN);
                        break;
                    case 'o':
                        DrawRectangle((x*LADO)+5,(y*LADO)+5, LADO/2, LADO/2, BLUE);
                        break;
                    }
            }
    }
    DrawText(TextFormat("That red square's health: %03i", object.health_points), 20, 2, 40, RED);
    EndDrawing();
    return 1;
}

bool test_move(struct STRUCT__STATS object, char map_matrix[MAP_HEIGHT][MAP_WIDTH+1]){

    //comeca com flag 1, significando que pode mover, e vai testando condicoes que impede ele de mover
    bool flag = 1;

    //se a posicao desejada não for vazia, entao flag eh zero
    if(!(map_matrix[object.y+object.dy][object.x+object.dx]==' ')) flag = 0;

    //se a posicao desejada estiver fora dos limites do mapa, entao flag eh zero
    if(!(object.x+object.dx>=0 && object.x+object.dx<MAP_WIDTH && object.y+object.dy>=0 && object.y+object.dy<MAP_HEIGHT)) flag = 0;

    //se o objeto esta movendo em diagonal
    if(object.dx!=0 && object.dy!=0){
            //se o objeto estiver se expremendo entre 2 objetos solidos, flag eh zero
            if(!(map_matrix[object.y+object.dy][object.x]==' ' || map_matrix[object.y][object.x+object.dx]==' ')) flag = 0;
    }

    return flag;
}

void move(struct STRUCT__STATS *object,char matrix[MAP_HEIGHT][MAP_WIDTH+1]){

    char temp;

    temp = matrix[object->y][object->x];
    matrix[object->y][object->x] = ' ';

    object->x = object->x + object->dx;
    object->y = object->y + object->dy;

    matrix[object->y][object->x] = temp;
}

int shoot(struct STRUCT__STATS *shooter,struct STRUCT__STATS *shot, char matrix[MAP_HEIGHT][MAP_WIDTH+1],int initial_x ,int initial_y){

    shot->x = shooter->x+initial_x;
    shot->y = shooter->y+initial_y;

    shot->dx = 0;
    shot->dy = 0;

    if(test_move(*shot,matrix)){
        matrix[shot->y][shot->x] = 'o';

        shot->dx = initial_x;
        shot->dy = initial_y;

        shooter->count = shooter->count + 1;
        if(shooter->count==MAX_PROJECTILES_ON_SCREEN){
            shooter->count = 0;
        }
    }else{
        shot->x = 0;
        shot->y = 0;
    }
}

int load_map(char archive_name[], char current_map[MAP_HEIGHT][MAP_WIDTH+1]){

    FILE *txt_arc = fopen(archive_name, "r");
    if (txt_arc == NULL)
    {
        printf("Opening error\n");
    }
    else
    {
        for(int n_line = 0;n_line<30;n_line++){
            if (fread(current_map[n_line], sizeof(char), (61), txt_arc) != (61))
            {
                printf("Reading error in line %d\n", n_line);
                fclose(txt_arc);
                return 0;
            }
        }
        fclose(txt_arc);
        printf("Successful loading\n");
        return 1;
    }
}

int main(void){

    srand(time(NULL)); //inicializa o gerador de numero aleatório
    InitWindow(LARGURA, ALTURA, "Joguin do balacobaco");//Inicializa janela, com certo tamanho e tıtulo
    SetTargetFPS(FPS);// Ajusta a execu¸c~ao do jogo para 60 frames por segundo

    //declara o jogador e o inimigo
    struct STRUCT__STATS player = {21,3,0,0,0,0,0,5,0,10,0};
    struct STRUCT__STATS projectile[MAX_PROJECTILES_ON_SCREEN] = {-1,-1,0,0,0,0,0,0,0,0,0};
    struct STRUCT__STATS enemy[MAX_ENEMIES];
    int i,j;
    int head_direction_x = 0;
    int head_direction_y = 0;

    for(i=0;i<MAX_ENEMIES;i++){
        enemy[i].x = 20;
        enemy[i].y = 19;
        enemy[i].dx = 0;
        enemy[i].dy = 0;
        enemy[i].count = 0;
        enemy[i].flag = 0;
        enemy[i].health_points = 5;
        enemy[i].step_cooldown = 10;
        enemy[i].step_cooldown_counter = 0;
    }

    char map[MAP_HEIGHT][MAP_WIDTH+1];
    char map_file_name[MAX_ARCHIVE_NAME] = "map001.txt";

    load_map(map_file_name,map);

    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_A) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S)){
            if (player.step_cooldown_counter == 0){
                if (IsKeyDown(KEY_D)) player.dx++;
                if (IsKeyDown(KEY_A)) player.dx--;
                if (IsKeyDown(KEY_W)) player.dy--;
                if (IsKeyDown(KEY_S)) player.dy++;
                player.step_cooldown_counter++;
            }else if (player.step_cooldown_counter!=player.step_cooldown){
                player.step_cooldown_counter++;
            }else{
                player.step_cooldown_counter = 0;
            }
        }else{
            player.step_cooldown_counter = 0;
        }

        if(test_move(player,map)){
            move(&player,map);
        }

        player.dx = 0;
        player.dy = 0;

        head_direction_x = 0;
        head_direction_y = 0;

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)){
            if (player.shoot_cooldown_counter == 0){
                if (IsKeyDown(KEY_RIGHT)) head_direction_x++;
                if (IsKeyDown(KEY_LEFT)) head_direction_x--;
                if (IsKeyDown(KEY_UP)) head_direction_y--;
                if (IsKeyDown(KEY_DOWN)) head_direction_y++;
                player.shoot_cooldown_counter++;
            }else if (player.shoot_cooldown_counter!=player.shoot_cooldown){
                player.shoot_cooldown_counter++;
            }else{
                player.shoot_cooldown_counter = 0;
            }
        }else{
            player.shoot_cooldown_counter = 0;
        }

        if(head_direction_x!=0||head_direction_y!=0){
        shoot(&player, &projectile[player.count], map, head_direction_x, head_direction_y);
        }

        for(i=0;i<MAX_PROJECTILES_ON_SCREEN;i++){
            if(test_move(projectile[i],map)){
                move(&projectile[i],map);
            }else{
                for(j=0;j<MAX_ENEMIES;j++){
                    if(enemy[j].x==(projectile[i].x+projectile[i].dx)&&enemy[j].y==(projectile[i].y+projectile[i].dy)){
                        enemy[j].health_points--;
                    }
                }
                map[projectile[i].y][projectile[i].x] = ' ';
                projectile[i].dx = 0;
                projectile[i].dy = 0;
                projectile[i].x = 0;
                projectile[i].y = 0;
            }
        }

        for(i=0;i<MAX_ENEMIES;i++){
            if (enemy[i].flag==0){
                do{
                enemy[i].dx = (-1 + (rand()%3));
                enemy[i].dy = (-1 + (rand()%3));
                }while(test_move(enemy[i],map)==0&&enemy[i].dx==0||enemy[i].dy==0);
            }

            if(test_move(enemy[i],map)){
               move(&enemy[i],map);
               enemy[i].flag = 1;

            }else{

               enemy[i].count=enemy[i].count+1;

               if(enemy[i].count==10){

                enemy[i].flag=0;
                enemy[i].count=0;
               }
            }
        }

        draw_map(map,enemy[0]);

    }

    CloseWindow();// Fecha a janela e o contexto OpenGL
    return 0;
}
    

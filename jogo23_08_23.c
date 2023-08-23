/*
terminar scan_map

BOMBA!!!!

    ###
   #####
   ##0##
   #####
    ###  CABUM!

refazer logica de movimento do inimigo

portal

save de mapa+estado atual
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

#define FPS 60 //taxa de frames por segundo

#define MAX_ENEMIES 15 //inimigos
#define MAX_WALLS 1600
#define MAX_TRAPS 100
#define MAX_BOMBS 100
#define MAX_PROJECTILES 100 //maximo de tiros na tela ao mesmo tempo disparos na tela
#define MAX_SMOKES 1600
#define MAX_ARCHIVE_NAME 50 //maximo de caracteres no nome do arquivo

#define PLAYER_STEP_COOLDOWN 0.1 //cooldown do movimento do player
#define PLAYER_SHOOT_COOLDOWN 0.1 //cooldown do tiro do player
#define PLAYER_DAMAGE_IMUNITY_COOLDOWN 0.5
#define ENEMY_STEP_COOLDOWN 10
#define SMOKE_LIFETIME 0.6

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
    int flag;
    int count;

    struct STRUCT_TIMER step;//timers variados
    struct STRUCT_TIMER shoot;
    struct STRUCT_TIMER damage_imunity;
};

bool draw_map (char map_matrix[MAP_LINES][MAP_COLUMNS+1],struct STRUCT_STATS object, struct STRUCT_STATS object1, struct STRUCT_STATS trap_vector[])//funcao que le o que acontece na matriz do mapa
{

    ClearBackground(RAYWHITE);//limpa a tela e define cor de fundo
    //Texture2D isaac = LoadTexture("resources/isaac.png"); // Texture loading
    //Texture2D clickety = LoadTexture("resources/clickety.png"); // Texture loading

    for(int i=0; i<MAX_TRAPS; i++)
    {
        DrawRectangle(trap_vector[i].x*SIDE, trap_vector[i].y*SIDE, SIDE, SIDE, ORANGE);
    }

    for (int y = 0; y < MAP_LINES; y++)//colunas
    {
        for (int x = 0; x < MAP_COLUMNS; x++)//linhas
        {
            switch(map_matrix[y][x])//interpretacao dos caracteres
            {

            case '#'://parede
                DrawRectangle(x*SIDE, y*SIDE, SIDE, SIDE, (Color){0, 0, 0, 255});
                break;

            case 'I'://inimigo
                //DrawTexture(clickety,object.x*20,object.y*20,WHITE);
                DrawRectangle(x*SIDE, y*SIDE, SIDE, SIDE, RED);
                break;

            case 'B'://bomba
                DrawRectangle(x*SIDE, y*SIDE, SIDE, SIDE, GRAY);
                break;

            case 'P'://portal
                DrawRectangle(x*SIDE, y*SIDE, SIDE, SIDE, PURPLE);
                break;

            case 'J'://player
                //DrawTexture(isaac,object1.x*20,object1.y*20,WHITE);
                DrawRectangle(x*SIDE, y*SIDE, SIDE, SIDE, GREEN);
                break;

            case 'X'://armadilha
                map_matrix[y][x] = ' ';
                break;

            case 'o'://tiro
                DrawCircle(x*SIDE+SIDE/2, y*SIDE+SIDE/2, SIDE/4, BLUE);
                break;
            }
        }
    }

    DrawText(TextFormat("Your HP: %1i", object1.health_points), 20, 2, 40, RED);//vida do inimigo
    return 1;
}

bool draw_effects (struct STRUCT_EFFECT smoke_vector[])
{
    int i, j;

    for(i=0; i<MAX_SMOKES; i++)
    {
        smoke_vector[i].lifetime.timer_current = clock();
        if(smoke_vector[i].lifetime.timer_current - smoke_vector[i].lifetime.timer_start >= smoke_vector[i].lifetime.cooldown){
            smoke_vector[i].x = -1;
            smoke_vector[i].y = -1;
        }
        if(smoke_vector[i].x>=0&&smoke_vector[i].x<MAP_COLUMNS&&smoke_vector[i].y>=0&&smoke_vector[i].y<MAP_LINES){
            if(smoke_vector[i].lifetime.timer_current - smoke_vector[i].lifetime.timer_start <= smoke_vector[i].lifetime.cooldown){
                for(j=0;j<2;j++){
                    DrawRectangle((smoke_vector[i].x*SIDE)+((rand()%(SIDE*2)-(SIDE/2))), (smoke_vector[i].y*SIDE)+((rand()%(SIDE*2)-(SIDE/2))), SIDE/2, SIDE/2, (Color){255-(((smoke_vector[i].lifetime.timer_current-smoke_vector[i].lifetime.timer_start)*255)/smoke_vector[i].lifetime.cooldown), 0, 0, 255-(((smoke_vector[i].lifetime.timer_current-smoke_vector[i].lifetime.timer_start)*255)/smoke_vector[i].lifetime.cooldown)});
                }
            }
        }
    }
    return 1;
}

bool scan_map (char matrix[MAP_LINES][MAP_COLUMNS+1],struct STRUCT_STATS *player_variable, struct STRUCT_STATS enemy_vector[], struct STRUCT_STATS projectile_vector[], struct STRUCT_STATS wall_vector[], struct STRUCT_STATS trap_vector[], struct STRUCT_STATS bomb_vector[])//funcao que le o que acontece na matriz do mapa
{
    int wall_counter = 0, enemy_counter = 0, portal_counter = 0, trap_counter = 0, bomb_counter = 0, projectile_counter = 0;

    for (int y = 0; y < MAP_LINES; y++)//colunas
    {
        for (int x = 0; x < MAP_COLUMNS; x++)//linhas
        {
            switch(matrix[y][x])//interpretacao dos caracteres
            {
            case '#'://parede
                wall_vector[wall_counter].x = x;
                wall_vector[wall_counter].y = y;
                wall_vector[wall_counter].health_points = 3;
                wall_counter++;
                break;
            case 'I'://inimigo
                enemy_vector[enemy_counter].x = x;
                enemy_vector[enemy_counter].y = y;
                enemy_vector[enemy_counter].health_points = 5;
                enemy_counter++;
                break;
            case 'B'://bomba
                bomb_vector[bomb_counter].x = x;
                bomb_vector[bomb_counter].y = y;
                bomb_vector[bomb_counter].health_points = 1;
                bomb_counter++;
                break;
            case 'P'://portal

                break;
            case 'J'://player
                player_variable->x = x;
                player_variable->y = y;
                player_variable->health_points = 5;
                player_variable->step.cooldown = CLOCKS_PER_SEC*PLAYER_STEP_COOLDOWN;
                player_variable->shoot.cooldown = CLOCKS_PER_SEC*PLAYER_SHOOT_COOLDOWN;
                player_variable->damage_imunity.cooldown = CLOCKS_PER_SEC*PLAYER_DAMAGE_IMUNITY_COOLDOWN;
                break;
            case 'X'://armadilha
                trap_vector[trap_counter].x = x;
                trap_vector[trap_counter].y = y;
                trap_counter++;
                break;
            }
        }
    }
    return 1;
}

bool test_move(struct STRUCT_STATS object, char map_matrix[MAP_LINES][MAP_COLUMNS+1])//semelhante a deve mover
{

    //comeca com flag 1, significando que pode mover, e vai testando condicoes que impede ele de mover
    bool flag = 1;

    //se a posicao desejada não for vazia, entao flag eh zero
    if(!(map_matrix[object.y+object.dy][object.x+object.dx]==' ')) flag = 0;

    //se a posicao desejada estiver fora dos limites do mapa, entao flag eh zero
    if(!(object.x+object.dx>=0 && object.x+object.dx<MAP_COLUMNS && object.y+object.dy>=0 && object.y+object.dy<MAP_LINES)) flag = 0;

    //se o objeto esta movendo em diagonal
    if(object.dx!=0 && object.dy!=0)
    {
        //se o objeto estiver se expremendo entre 2 objetos solidos, flag eh zero
        if(!(map_matrix[object.y+object.dy][object.x]==' ' || map_matrix[object.y][object.x+object.dx]==' ')) flag = 0;
    }

    return flag;
}

bool test_if_dead(struct STRUCT_STATS *object, char map_matrix[MAP_LINES][MAP_COLUMNS+1])//testa se o objeto morreu
{
    bool flag = false;

    if(object->health_points<=0)
    {
        map_matrix[object->y][object->x] = ' ';
        object->dx = 0;
        object->dy = 0;
        object->x = -1;
        object->y = -1;
        object->health_points = 0;
        flag = true;
    }
    return flag;
}

bool test_if_object_is_there(struct STRUCT_STATS *object, char map_matrix[MAP_LINES][MAP_COLUMNS+1], int x_position, int y_position, char object_symbol)//testa se o objeto morreu
{
    bool flag = false;

    if(map_matrix[object->y+y_position][object->x+x_position] == object_symbol) flag = true;

    return flag;
}

void move(struct STRUCT_STATS *object,char matrix[MAP_LINES][MAP_COLUMNS+1])//funcao que movimenta os objetos na matriz
{

    char temp;//variavel temporaria

    temp = matrix[object->y][object->x];//iguala o tipo de caractere (P, o, I) a posicao inicial da estrutura
    matrix[object->y][object->x] = ' ';//entao zera a posicao inicial da matriz

    object->x = object->x + object->dx;//encrementa dx ao x, ou seja cria o movimento e gera a nova posicao
    object->y = object->y + object->dy;//encrementa dy ao y, ou seja cria o movimento e gera a nova posicao

    matrix[object->y][object->x] = temp;//a nova posicao recebe entao o valor temporario (P, o, I) para fazer o objeto aparecer de novo
}

void damage_position(int x_position, int y_position, int damage, struct STRUCT_STATS *player_variable, struct STRUCT_STATS enemy_vector[], struct STRUCT_STATS projectile_vector[], struct STRUCT_STATS wall_vector[], struct STRUCT_STATS bomb_vector[])//funcao que movimenta os objetos na matriz
{
    int i;

    if(player_variable->x == x_position && player_variable->y == y_position && player_variable->health_points>=1)//se a posição onde quer infligir dano é a mesma do player
    {
        player_variable->health_points = player_variable->health_points - damage;//decrementa da vida
    }

    for(i=0; i<MAX_ENEMIES; i++)//percorre o vetor de inimigos
    {
        if(enemy_vector[i].x == x_position && enemy_vector[i].y == y_position && enemy_vector[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do inimigo
        {
            enemy_vector[i].health_points = enemy_vector[i].health_points - damage;//decrementa da vida
        }
    }
    for(i=0; i<MAX_PROJECTILES; i++)//percorre o vetor de inimigos
    {
        if(projectile_vector[i].x == x_position && projectile_vector[i].y == y_position && projectile_vector[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do projetil
        {
            projectile_vector[i].health_points = projectile_vector[i].health_points - damage;//decrementa da vida
        }
    }
    for(i=0; i<MAX_WALLS; i++)//percorre o vetor de inimigos
    {
        if(wall_vector[i].x == x_position && wall_vector[i].y == y_position && wall_vector[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do projetil
        {
            wall_vector[i].health_points = wall_vector[i].health_points - damage;//decrementa da vida
        }
    }
    for(i=0; i<MAX_BOMBS; i++)//percorre o vetor de inimigos
    {
        if(bomb_vector[i].x == x_position && bomb_vector[i].y == y_position && bomb_vector[i].health_points>=1)//se a posição onde quer infligir dano é a mesma do projetil
        {
            bomb_vector[i].health_points = bomb_vector[i].health_points - damage;//decrementa da vida
        }
    }
}

bool shoot(struct STRUCT_STATS *shooter,struct STRUCT_STATS *shot, char matrix[MAP_LINES][MAP_COLUMNS+1],int initial_x,int initial_y)
{
    //funcao do tiro

    if(test_if_object_is_there(shooter,matrix,initial_x,initial_y,' '))//testa se pode ser "criado" o tiro
    {
        shot->x = shooter->x+initial_x;//a posicao do tiro eh a posicao do player mais a posicao inicial do tiro
        shot->y = shooter->y+initial_y;
        shot->dx = initial_x;//o deslocamento do tiro eh igual a posicao iniciao do tiro
        shot->dy = initial_y;//basicamente pega a direcao inicial do tiro e diz para qual direção sera seu deslocamento
        shot->health_points = 1;

        matrix[shot->y][shot->x] = 'o';//a matriz recebe o char do tiro e aparece na tela

        shooter->count++;//conta quantos projeteis foram acionados
        if(shooter->count==MAX_PROJECTILES)//se a contagem for igual ao maximo de projeteis na tela
        {
            shooter->count = 0;//zera a contagem
        }
    }
}

bool explode(int x_origin,int y_origin, float radius, int beam_n, int damage, char matrix[MAP_LINES][MAP_COLUMNS+1] ,struct STRUCT_STATS *player_variable, struct STRUCT_STATS enemy_vector[], struct STRUCT_STATS projectile_vector[], struct STRUCT_STATS wall_vector[], struct STRUCT_STATS bomb_vector[], struct STRUCT_EFFECT smoke_vector[], int *smoke_counter)
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
                    damage_position(x_origin+x, y_origin+y ,(int)round(damage -(j/radius)*damage), player_variable, enemy_vector, projectile_vector, wall_vector, bomb_vector);
                    damaged_position[y_origin+y][x_origin+x]=1;
                }
                if(matrix[y_origin+y][x_origin+x]=='#'){
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
                smoke_vector[*smoke_counter].x = k;
                smoke_vector[*smoke_counter].y = i;
                smoke_vector[*smoke_counter].lifetime.timer_start = clock();
                smoke_vector[*smoke_counter].lifetime.cooldown = SMOKE_LIFETIME*CLOCKS_PER_SEC;
                *smoke_counter = *smoke_counter+1;
                if(*smoke_counter>=MAX_SMOKES){
                    *smoke_counter = 0;
                }
            }
        }
    }
}

bool load_map(char archive_name[], char current_map[MAP_LINES][MAP_COLUMNS+1])//carrega o mapa de um arquivo txt
{
    //+1 devido o \n no fim de cada linha

    FILE *txt_arc = fopen(archive_name, "r");//o ponteiro txt_arc recebe o arquivo
    if (txt_arc == NULL)//testa se o arquivo existe na pasta
    {
        printf("Opening error\n");//se inexistente imprime na tela um erro
    }
    else//se aberto corretamente
    {
        for(int n_line = 0; n_line<30; n_line++)//vasculha linha por linha do mapa ate l<30
        {
            if (fread(current_map[n_line], sizeof(char), (61), txt_arc) != (61))//realiza a leitura dessas linhas por caractere sendo um total de 61
            {
                printf("Reading error in line %d\n", n_line);//se a leitura der errado imprime na tela uma mensagem de erro e qual linha
                fclose(txt_arc);
                return 0;
            }
        }
        fclose(txt_arc);//fecha o arquivo
        printf("Successful loading\n");//se tudo der certo imprime na tela sucesso no carregamento
        return 1;
    }
}

void clear_data(struct STRUCT_STATS *object)//reseta as informações do objetos
{
    object->x = -1;
    object->y = -1;
    object->dx = 0;
    object->dy = 0;
    object->head_direction_x = 0;
    object->head_direction_y = 0;

    object->health_points = 0;
    object->flag = 0;
    object->count = 0;

    object->step.timer_start = 0;
    object->step.timer_current = 0;
    object->step.cooldown = 0;

    object->shoot.timer_start = 0;
    object->shoot.timer_current = 0;
    object->shoot.cooldown = 0;

    object->damage_imunity.timer_start = 0;
    object->damage_imunity.timer_current = 0;
    object->damage_imunity.cooldown = 0;
}

void clear_effect_data(struct STRUCT_EFFECT *effect)//reseta as informações do objetos
{
    effect->x = -1;
    effect->y = -1;

    effect->lifetime.timer_start = 0;
    effect->lifetime.timer_current = 0;
    effect->lifetime.cooldown = 0;
}

int main(void)
{

    srand(time(NULL)); //inicializa o gerador de numero aleatório
    InitWindow(MAP_PIXEL_WIDTH, MAP_PIXEL_HEIGHT, "Joguin do balacobaco");//Inicializa janela, com certo tamanho e tıtulo
    SetTargetFPS(FPS);// Ajusta a execucao do jogo para 60 frames por segundo

    //declara o jogador, projeteis e o vetor de inimigos
    struct STRUCT_STATS player;
    struct STRUCT_STATS projectile[MAX_PROJECTILES];
    struct STRUCT_STATS enemy[MAX_ENEMIES];
    struct STRUCT_STATS wall[MAX_WALLS];
    struct STRUCT_STATS trap[MAX_TRAPS];
    struct STRUCT_STATS bomb[MAX_BOMBS];
    struct STRUCT_EFFECT smoke[MAX_SMOKES];

    int i,j;//contadores declarados aqui pois sao enviados por referencia
    int past_x ,past_y;
    int smoke_counter = 0;

    //garante que todos os dados de entidades estejam zeradas
    clear_data(&player);

    for(i=0; i<MAX_ENEMIES; i++)
    {
        clear_data(&enemy[i]);
    }

    for(i=0; i<MAX_PROJECTILES; i++)
    {
        clear_data(&projectile[i]);
    }

    for(i=0; i<MAX_WALLS; i++)
    {
        clear_data(&wall[i]);
    }

    for(i=0; i<MAX_TRAPS; i++)
    {
        clear_data(&trap[i]);
    }

    for(i=0; i<MAX_BOMBS; i++)
    {
        clear_data(&bomb[i]);
    }

    for(i=0; i<MAX_SMOKES; i++)
    {
        clear_effect_data(&smoke[i]);
    }
    char map[MAP_LINES][MAP_COLUMNS+1];//declara a matriz do mapa
    char map_file_name[MAX_ARCHIVE_NAME] = "map000.txt";//nome da versao do mapa que estamos jogando

    load_map(map_file_name,map);//FUNCAO DE CARREGAMENTO DO MAPA
    scan_map(map,&player,enemy,projectile,wall,trap,bomb);//FUNCAO DE SCANEAMENTO DE MAPA

    while (!WindowShouldClose()) //Roda enquanto não for fechada a tela
    {
        //===================================================================================================================
        //PLAYER

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_A) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S))//ve se alguma das teclas estao sendo pressionadas
        {
            player.step.timer_current = clock();
            if (player.step.timer_current - player.step.timer_start >= player.step.cooldown)//se o cooldown for zero, pega a intenção de movimento
            {
                //determina a intencao de movimento de acordo com a combinacao de teclas
                if (IsKeyDown(KEY_D)) player.dx++;
                if (IsKeyDown(KEY_A)) player.dx--;
                if (IsKeyDown(KEY_W)) player.dy--;
                if (IsKeyDown(KEY_S)) player.dy++;
                player.step.timer_start = clock();
            }
        }

        if(test_move(player,map)){
            move(&player,map);//se sim, ele move
        }

        //reseta na intenção de movimento para proximo ciclo
        player.dx = 0;
        player.dy = 0;

        test_if_dead(&player, map);

        //===================================================================================================================
        //AÇÃO DE TIRO

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN))//ve se alguma das teclas estao sendo pressionadas
        {
            player.shoot.timer_current = clock();
            if (player.shoot.timer_current - player.shoot.timer_start >= player.shoot.cooldown)//se o cooldown for zero, pega a intenção de movimento
            {
                //determina a intencao de tiro de acordo com a combinacao de teclas
                if (IsKeyDown(KEY_RIGHT)) player.head_direction_x++;
                if (IsKeyDown(KEY_LEFT)) player.head_direction_x--;
                if (IsKeyDown(KEY_UP)) player.head_direction_y--;
                if (IsKeyDown(KEY_DOWN)) player.head_direction_y++;
                player.shoot.timer_start = clock();
            }
        }

        if(player.head_direction_x!=0||player.head_direction_y!=0)//se alguma das direções não for nula
        {
            shoot(&player, &projectile[player.count], map, player.head_direction_x, player.head_direction_y);//atira
        }

        player.head_direction_x = 0;
        player.head_direction_y = 0;

        for(i=0; i<MAX_PROJECTILES; i++)//percorre o vetor de projéteis
        {
            if(test_move(projectile[i],map))//se o projétil puder mover
            {
                move(&projectile[i],map);//move o projétil
            }
            else//se não puder mover
            {
                damage_position(projectile[i].x+projectile[i].dx,projectile[i].y+projectile[i].dy ,1 ,&player, enemy, projectile, wall, bomb);
                projectile[i].health_points--;
                test_if_dead(&projectile[i], map);
            }
        }

        //===================================================================================================================
        //MOVIMENTO DO INIMIGO

        for(i=0; i<MAX_ENEMIES; i++)//percorre o vetor de inimigos
        {
            test_if_dead(&enemy[i], map);
            if (enemy[i].flag==0)//se a sua flag for 0, significa que ele está parado e pode decidir para onde se movimentar
            {
                do//executa até ele decidir aleatoriamente uma direção que não seja nula ou para uma posição valida
                {
                    //atribui valores de intenção de movimento aleatório para os dois eixos
                    enemy[i].dx = (-1 + (rand()%3));
                    enemy[i].dy = (-1 + (rand()%3));
                }
                while(test_move(enemy[i],map)==0&&enemy[i].dx==0||enemy[i].dy==0);//roda até que ele decida-se mover a uma direção que não seja contra a parede e não seja nula
            }

            if(test_move(enemy[i],map))//testa se ele pode mover
            {
                move(&enemy[i],map);//move
                enemy[i].flag = 1;//flag 1 significa que ele está em movimento
            }
            else//se não puder se movimentar
            {
                if(enemy[i].count==50)//se atingir o limite dos contador
                {
                    //reseta flag e contador para 0, dizendo que esperou o suficiente e pode decidir um novo movimento
                    enemy[i].flag=0;
                    enemy[i].count=0;
                }
                enemy[i].count=enemy[i].count+1;//incrementa o seu contador
            }
        }
        //===================================================================================================================
        //ARMADILHAS

        for(i=0; i<MAX_TRAPS; i++)//percorre o vetor de inimigos
        {
            if(test_if_object_is_there(&trap[i],map,0,0,'J')){
                player.damage_imunity.timer_current = clock();
                if (player.damage_imunity.timer_current - player.damage_imunity.timer_start >= player.damage_imunity.cooldown)//se o cooldown for zero, pega a intenção de movimento
                {
                    damage_position(trap[i].x ,trap[i].y , 1 ,&player, enemy, projectile, wall, bomb);
                    player.damage_imunity.timer_start = clock();
                }
            }
        }

        //===================================================================================================================
        //PAREDES

        for(i=0; i<MAX_WALLS; i++)//percorre o vetor de inimigos
        {
            test_if_dead(&wall[i], map);
        }

        //===================================================================================================================
        //BOMBAS

        for(i=0; i<MAX_BOMBS; i++)
        {
            past_x = bomb[i].x;
            past_y = bomb[i].y;
            if(bomb[i].x>=0&&bomb[i].x<MAP_COLUMNS&&bomb[i].y>=0&&bomb[i].y<MAP_LINES){
                if(test_if_dead(&bomb[i], map)){
                    explode(past_x,past_y,6,180,10,map,&player, enemy, projectile, wall, bomb, smoke, &smoke_counter);
                }
            }
        }

        //===================================================================================================================
        //MAPA
        BeginDrawing();//inicializa ambiente para desenho
        draw_map(map, enemy[0], player, trap);
        draw_effects(smoke);
        EndDrawing();

    }

    CloseWindow();// Fecha a janela e o contexto OpenGL
    return 0;
}

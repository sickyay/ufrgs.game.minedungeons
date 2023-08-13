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
#define MAX_PROJECTILES_ON_SCREEN 100 //disparos na tela
#define FPS 30
#define MAX_ARCHIVE_NAME 50
#define COOLDOWN 2 //coldown do movimento do player

struct STRUCT__STATS //estrutura utilizada tanto para inimigos, player e projeteis
{
    int x;//posicoes
    int y;
    int dx;//deslocamentos
    int dy;
    int health_points;//HP (*inimigos)
    int flag;
    int count;
    int step_cooldown;//cooldown deslocamento
    int step_cooldown_counter;
    int shoot_cooldown;//coldown tiros
    int shoot_cooldown_counter;
};

int draw_map (char matrix[MAP_HEIGHT][MAP_WIDTH+1],struct STRUCT__STATS object)//funcao que le o que acontece na matriz do mapa
{

    BeginDrawing();
    ClearBackground(RAYWHITE);//limpa a tela e define cor de fundo

    for (int y = 0; y < MAP_HEIGHT; y++)//colunas
    {
        for (int x = 0; x < MAP_WIDTH; x++)//linhas
        {
            switch(matrix[y][x])//interpretacao dos caracteres
            {

            case '#'://parede
                DrawRectangle(x*LADO, y*LADO, LADO, LADO, BLACK);
                break;
            case 'I'://inimigo
                DrawRectangle(x*LADO, y*LADO, LADO, LADO, RED);
                break;
            case 'B'://bomba
                DrawRectangle(x*LADO, y*LADO, LADO, LADO, GRAY);
                break;
            case 'P'://portal
                DrawRectangle(x*LADO, y*LADO, LADO, LADO, PURPLE);
                break;
            case 'J'://player
                DrawRectangle(x*LADO, y*LADO, LADO, LADO, GREEN);
                break;
            case 'X'://fogueira
                DrawRectangle(x*LADO, y*LADO, LADO, LADO, BROWN);
                break;
            case 'o'://tiro
                DrawRectangle((x*LADO)+5,(y*LADO)+5, LADO/2, LADO/2, BLUE);
                break;
            }
        }
    }
    DrawText(TextFormat("That red square's health: %03i", object.health_points), 20, 2, 40, RED);//vida do inimigo
    EndDrawing();
    return 1;
}

bool test_move(struct STRUCT__STATS object, char map_matrix[MAP_HEIGHT][MAP_WIDTH+1])//semelhante a deve mover
{

    //comeca com flag 1, significando que pode mover, e vai testando condicoes que impede ele de mover
    bool flag = 1;

    //se a posicao desejada não for vazia, entao flag eh zero
    if(!(map_matrix[object.y+object.dy][object.x+object.dx]==' ')) flag = 0;

    //se a posicao desejada estiver fora dos limites do mapa, entao flag eh zero
    if(!(object.x+object.dx>=0 && object.x+object.dx<MAP_WIDTH && object.y+object.dy>=0 && object.y+object.dy<MAP_HEIGHT)) flag = 0;

    //se o objeto esta movendo em diagonal
    if(object.dx!=0 && object.dy!=0)
    {
        //se o objeto estiver se expremendo entre 2 objetos solidos, flag eh zero
        if(!(map_matrix[object.y+object.dy][object.x]==' ' || map_matrix[object.y][object.x+object.dx]==' ')) flag = 0;
    }

    return flag;
}

void move(struct STRUCT__STATS *object,char matrix[MAP_HEIGHT][MAP_WIDTH+1])//funcao que movimenta os objetos na matriz
{

    char temp;//variavel temporaria

    temp = matrix[object->y][object->x];//iguala o tipo de caractere (P, o, I) a posicao inicial da estrutura 
    matrix[object->y][object->x] = ' ';//entao zera a posicao inicial da matriz

    object->x = object->x + object->dx;//encrementa dx ao x, ou seja cria o movimento e gera a nova posicao
    object->y = object->y + object->dy;//encrementa dy ao y, ou seja cria o movimento e gera a nova posicao

    matrix[object->y][object->x] = temp;//a nova posicao recebe entao o valor temporario (P, o, I) para fazer o objeto aparecer de novo
}

int shoot(struct STRUCT__STATS *shooter,struct STRUCT__STATS *shot, char matrix[MAP_HEIGHT][MAP_WIDTH+1],int initial_x,int initial_y)
{//funcao do tiro

    shot->x = shooter->x+initial_x;//a posicao do tiro eh a posicao do player mais a posicao iniciao do tiro (definida por iskeydown na main)
    shot->y = shooter->y+initial_y;

    shot->dx = 0;//zera deslocamentos do tiro
    shot->dy = 0;

    if(test_move(*shot,matrix))//testa se "deve mover"
    {
        matrix[shot->y][shot->x] = 'o';//a matriz recebe o char do tiro e aparece na tela

        shot->dx = initial_x;//o deslocamento do tiro eh igual a posicao iniciao do tiro
        shot->dy = initial_y;//basicamente pega a direcao inicial do tiro e diz para qual lado sera seu deslocamento

        shooter->count = shooter->count + 1;//conta quantos projeteis foram acionados
        if(shooter->count==MAX_PROJECTILES_ON_SCREEN)//se a contagem for igual ao maximo de projeteis na tela
        {
            shooter->count = 0;//zera a contagem
        }
    }
    else
    {
        shot->x = 0;//se ele nao pode mover ele some
        shot->y = 0;
    }
}

int load_map(char archive_name[], char current_map[MAP_HEIGHT][MAP_WIDTH+1])//carrega o mapa de um arquivo txt
{//+1 devido o \n no fim de cada linha

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

int main(void)
{

    srand(time(NULL)); //inicializa o gerador de numero aleatório *NAO FOI USADO AINDA*
    InitWindow(LARGURA, ALTURA, "Joguin do balacobaco");//Inicializa janela, com certo tamanho e tıtulo
    SetTargetFPS(FPS);// Ajusta a execucao do jogo para 60 frames por segundo

    //declara o jogador, projeteis e o inimigo
    struct STRUCT__STATS player = {21,3,0,0,0,0,0,COOLDOWN,0,10,0};
    struct STRUCT__STATS projectile[MAX_PROJECTILES_ON_SCREEN] = {-1,-1,0,0,0,0,0,0,0,0,0};
    struct STRUCT__STATS enemy[MAX_ENEMIES];
    
    int i,j;//contadores declarados aqui pois sao enviados por referencia
    int head_direction_x = 0;
    int head_direction_y = 0;

    for(i=0; i<MAX_ENEMIES; i++)//geracao dos inimigos
    {
        enemy[i].x = 20;//valores iniciais coerentes caso nao haja nenhum I no arquivo mapa
        enemy[i].y = 19;
        enemy[i].dx = 0;
        enemy[i].dy = 0;
        enemy[i].count = 0;
        enemy[i].flag = 0;
        enemy[i].health_points = 5;
        enemy[i].step_cooldown = 10;
        enemy[i].step_cooldown_counter = 0;
    }

    char map[MAP_HEIGHT][MAP_WIDTH+1];//declara a matriz do mapa
    char map_file_name[MAX_ARCHIVE_NAME] = "map001.txt";//nome da versao do mapa que estamos jogando

    load_map(map_file_name,map);///FUNCAO LEITURA DO MAPA

    
    
    
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_A) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S))//controles de movimento
        {
            if (player.step_cooldown_counter == 0)//se nao estiver em cooldown de passos incrementa os deslocamentos
            {
                if (IsKeyDown(KEY_D)) player.dx++;
                if (IsKeyDown(KEY_A)) player.dx--;
                if (IsKeyDown(KEY_W)) player.dy--;
                if (IsKeyDown(KEY_S)) player.dy++;
                player.step_cooldown_counter++;//contagem para o cooldown
            }
            else if (player.step_cooldown_counter!=player.step_cooldown)
            {
                player.step_cooldown_counter++;
            }
            else
            {
                player.step_cooldown_counter = 0;
            }
        }
        else
        {
            player.step_cooldown_counter = 0;
        }

        if(test_move(player,map))///FUNCOES MOVIMENTACAO DO PLAYER
        {//se deve mover =1
            move(&player,map);
        }

        player.dx = 0;
        player.dy = 0;

        head_direction_x = 0;
        head_direction_y = 0;

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN))//controles do disparo
        {
            if (player.shoot_cooldown_counter == 0)
            {
                if (IsKeyDown(KEY_RIGHT)) head_direction_x++;
                if (IsKeyDown(KEY_LEFT)) head_direction_x--;
                if (IsKeyDown(KEY_UP)) head_direction_y--;
                if (IsKeyDown(KEY_DOWN)) head_direction_y++;
                player.shoot_cooldown_counter++;
            }
            else if (player.shoot_cooldown_counter!=player.shoot_cooldown)
            {
                player.shoot_cooldown_counter++;
            }
            else
            {
                player.shoot_cooldown_counter = 0;
            }
        }
        else
        {
            player.shoot_cooldown_counter = 0;
        }

        if(head_direction_x!=0||head_direction_y!=0)
        {
            shoot(&player, &projectile[player.count], map, head_direction_x, head_direction_y);
        }

        for(i=0; i<MAX_PROJECTILES_ON_SCREEN; i++)
        {
            if(test_move(projectile[i],map))
            {
                move(&projectile[i],map);
            }
            else
            {
                for(j=0; j<MAX_ENEMIES; j++)
                {
                    if(enemy[j].x==(projectile[i].x+projectile[i].dx)&&enemy[j].y==(projectile[i].y+projectile[i].dy))
                    {
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

        for(i=0; i<MAX_ENEMIES; i++)
        {
            if (enemy[i].flag==0)
            {
                do
                {
                    enemy[i].dx = (-1 + (rand()%3));
                    enemy[i].dy = (-1 + (rand()%3));
                }
                while(test_move(enemy[i],map)==0&&enemy[i].dx==0||enemy[i].dy==0);
            }

            if(test_move(enemy[i],map))
            {
                move(&enemy[i],map);
                enemy[i].flag = 1;

            }
            else
            {

                enemy[i].count=enemy[i].count+1;

                if(enemy[i].count==10)
                {

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


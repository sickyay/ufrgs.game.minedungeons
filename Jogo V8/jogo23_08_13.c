/*
scan_map do mapa(tipo draw_map)
fogueiras devem ser andavéis? no test_move

funcao fogueira
funcao bomba
funcao portal
funcao pontos e vidas

inimigo morrer
cooldown para inimigos assim como player
menu
save
*/

#include "raylib.h"
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

#define MAX_ENEMIES 1 //inimigos
#define MAX_PROJECTILES_ON_SCREEN 100 //maximo de tiros na tela ao mesmo tempo disparos na tela
#define MAX_ARCHIVE_NAME 50 //maximo de caracteres no nome do arquivo

#define PLAYER_STEP_COOLDOWN 5 //coldown do movimento do player
#define ENEMY_STEP_COOLDOWN 2 //coldown do movimento do player

struct STRUCT_STATS //estrutura utilizada tanto para inimigos, player e projeteis
{
    int x;//posicoes
    int y;
    int dx;//deslocamentos
    int dy;
    int health_points;//HP
    int flag;
    int count;
    int step_cooldown;//cooldown deslocamento
    int step_cooldown_counter;
    int shoot_cooldown;//coldown tiros
    int shoot_cooldown_counter;
};

int draw_map (char matrix[MAP_LINES][MAP_COLUMNS+1],struct STRUCT_STATS object, Texture2D isaac, struct STRUCT_STATS obj, Texture2D clickety)//funcao que le o que acontece na matriz do mapa
{

    BeginDrawing();//inicializa ambiente para desenho
    ClearBackground(RAYWHITE);//limpa a tela e define cor de fundo

    for (int y = 0; y < MAP_LINES; y++)//colunas
    {
        for (int x = 0; x < MAP_COLUMNS; x++)//linhas
        {
            switch(matrix[y][x])//interpretacao dos caracteres
            {

            case '#'://parede
                DrawRectangle(x*SIDE, y*SIDE, SIDE, SIDE, BLACK);
                break;
                
            case 'I'://inimigo
                DrawTexture(clickety,object.x*20,object.y*20,WHITE);                
                break;
                
                
            case 'B'://bomba
                DrawRectangle(x*SIDE, y*SIDE, SIDE, SIDE, GRAY);
                break;
            case 'P'://portal
                DrawRectangle(x*SIDE, y*SIDE, SIDE, SIDE, PURPLE);
                break;
                
            case 'J'://player
                DrawTexture(isaac,obj.x*20,obj.y*20,WHITE);
                break;
                
                
            case 'X'://fogueira
                DrawRectangle(x*SIDE, y*SIDE, SIDE, SIDE, BROWN);
                break;
            case 'o'://tiro
                DrawRectangle((x*SIDE)+5,(y*SIDE)+5, SIDE/2, SIDE/2, BLUE);
                break;
            }
        }
    }
    DrawText(TextFormat("That red square's health: %03i", object.health_points), 20, 2, 40, RED);//vida do inimigo
    
    
    
    EndDrawing();
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

void move(struct STRUCT_STATS *object,char matrix[MAP_LINES][MAP_COLUMNS+1])//funcao que movimenta os objetos na matriz
{

    char temp;//variavel temporaria

    temp = matrix[object->y][object->x];//iguala o tipo de caractere (P, o, I) a posicao inicial da estrutura
    matrix[object->y][object->x] = ' ';//entao zera a posicao inicial da matriz

    object->x = object->x + object->dx;//encrementa dx ao x, ou seja cria o movimento e gera a nova posicao
    object->y = object->y + object->dy;//encrementa dy ao y, ou seja cria o movimento e gera a nova posicao

    matrix[object->y][object->x] = temp;//a nova posicao recebe entao o valor temporario (P, o, I) para fazer o objeto aparecer de novo
}

int shoot(struct STRUCT_STATS *shooter,struct STRUCT_STATS *shot, char matrix[MAP_LINES][MAP_COLUMNS+1],int initial_x,int initial_y)
{//funcao do tiro

    shot->x = shooter->x+initial_x;//a posicao do tiro eh a posicao do player mais a posicao inicial do tiro
    shot->y = shooter->y+initial_y;

    shot->dx = 0;//zera deslocamentos do tiro
    shot->dy = 0;

    if(test_move(*shot,matrix))//testa se pode ser "criado" o tiro
    {
        matrix[shot->y][shot->x] = 'o';//a matriz recebe o char do tiro e aparece na tela

        shot->dx = initial_x;//o deslocamento do tiro eh igual a posicao iniciao do tiro
        shot->dy = initial_y;//basicamente pega a direcao inicial do tiro e diz para qual direção sera seu deslocamento

        shooter->count++;//conta quantos projeteis foram acionados
        if(shooter->count==MAX_PROJECTILES_ON_SCREEN)//se a contagem for igual ao maximo de projeteis na tela
        {
            shooter->count = 0;//zera a contagem
        }
    }
    else
    {
        shot->x = -1;//se ele nao pode mover ele some
        shot->y = -1;
    }
}

int load_map(char archive_name[], char current_map[MAP_LINES][MAP_COLUMNS+1])//carrega o mapa de um arquivo txt
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

    srand(time(NULL)); //inicializa o gerador de numero aleatório
    InitWindow(MAP_PIXEL_WIDTH, MAP_PIXEL_HEIGHT, "Joguin do balacobaco");//Inicializa janela, com certo tamanho e tıtulo
    SetTargetFPS(FPS);// Ajusta a execucao do jogo para 60 frames por segundo

    Texture2D isaac = LoadTexture("resources/isaac.png"); // Texture loading
    Texture2D clickety = LoadTexture("resources/clickety.png"); // Texture loading
    
    
    
    
    
    
    //declara o jogador, projeteis e o vetor de inimigos
    struct STRUCT_STATS player = {21,3,0,0,0,0,0,PLAYER_STEP_COOLDOWN,0,10,0};
    struct STRUCT_STATS projectile[MAX_PROJECTILES_ON_SCREEN] = {-1,-1,0,0,0,0,0,0,0,0,0};
    struct STRUCT_STATS enemy[MAX_ENEMIES];

    int i;//contadores declarados aqui pois sao enviados por referencia
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

    for(i=0; i<MAX_PROJECTILES_ON_SCREEN; i++)
    {
        projectile[i].x = -1;
        projectile[i].y = -1;
        projectile[i].dx = 0;
        projectile[i].dy = 0;
        projectile[i].count = 0;
        projectile[i].flag = 0;
    }

    char map[MAP_LINES][MAP_COLUMNS+1];//declara a matriz do mapa
    char map_file_name[MAX_ARCHIVE_NAME] = "map001.txt";//nome da versao do mapa que estamos jogando

    load_map(map_file_name,map);//FUNCAO DE CARREGAMENTO DO MAPA

    while (!WindowShouldClose()) //Roda enquanto não for fechada a tela
    {
    //===================================================================================================================
    //MOVIMENTO DO PLAYER
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_A) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S))//ve se alguma das teclas estao sendo pressionadas
        {
            if (player.step_cooldown_counter == 0)//se o cooldown for zero, pega a intenção de movimento
            {
                //determina a intencao de movimento de acordo com a combinacao de teclas
                if (IsKeyDown(KEY_D)) player.dx++;
                if (IsKeyDown(KEY_A)) player.dx--;
                if (IsKeyDown(KEY_W)) player.dy--;
                if (IsKeyDown(KEY_S)) player.dy++;
                player.step_cooldown_counter++;//inicia a contagem para o cooldown
            }
            else if (player.step_cooldown_counter!=player.step_cooldown)//se o contador cooldown nao for igual ao seu limite
            {
                player.step_cooldown_counter++;//incrementa o contador cooldown
            }
            else//se nao for nem 0 ou entre 0 e seu limite, ele esta no seu maximo
            {
                player.step_cooldown_counter = 0;//volta para 0
                //determina a intencao de movimento de acordo com a combinacao de teclas
                if (IsKeyDown(KEY_D)) player.dx++;
                if (IsKeyDown(KEY_A)) player.dx--;
                if (IsKeyDown(KEY_W)) player.dy--;
                if (IsKeyDown(KEY_S)) player.dy++;
                player.step_cooldown_counter++;//inicia a contagem para o cooldown
            }
        }
        else//se parou de pressionar alguma das tecla
        {
            player.step_cooldown_counter = 0;//reseta o contador de cooldown
        }

        if(test_move(player,map))//testa se o player pode mover para onde ele quer
        {
            move(&player,map);//se sim, ele move
        }

        //reseta na intenção de movimento para proximo ciclo
        player.dx = 0;
        player.dy = 0;

        //===================================================================================================================
        //AÇÃO DE TIRO

        //reseta a direção que o player está "olhando"
        head_direction_x = 0;
        head_direction_y = 0;

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN))//ve se alguma das teclas estao sendo pressionadas
        {
            if (player.shoot_cooldown_counter == 0)//se o cooldown for zero, pega a direção de tiro
            {
                //determina a intencao de tiro de acordo com a combinacao de teclas
                if (IsKeyDown(KEY_RIGHT)) head_direction_x++;
                if (IsKeyDown(KEY_LEFT)) head_direction_x--;
                if (IsKeyDown(KEY_UP)) head_direction_y--;
                if (IsKeyDown(KEY_DOWN)) head_direction_y++;
                player.shoot_cooldown_counter++;//inicia a contagem para o cooldown
            }
            else if (player.shoot_cooldown_counter!=player.shoot_cooldown)//se o contador cooldown nao for igual ao seu limite
            {
                player.shoot_cooldown_counter++;//incrementa o contador cooldown
            }
            else//se nao for nem 0 ou entre 0 e seu limite, ele esta no seu maximo
            {
                player.shoot_cooldown_counter = 0;//volta para 0
                //determina a intencao de tiro de acordo com a combinacao de teclas
                if (IsKeyDown(KEY_RIGHT)) head_direction_x++;
                if (IsKeyDown(KEY_LEFT)) head_direction_x--;
                if (IsKeyDown(KEY_UP)) head_direction_y--;
                if (IsKeyDown(KEY_DOWN)) head_direction_y++;
                player.shoot_cooldown_counter++;//inicia a contagem para o cooldown
            }
        }
        else//se parou de pressionar alguma das tecla
        {
            player.shoot_cooldown_counter = 0;//reseta o contador de cooldown
        }

        if(head_direction_x!=0||head_direction_y!=0)//se alguma das direções não for nule
        {
            shoot(&player, &projectile[player.count], map, head_direction_x, head_direction_y);//atira
        }

        for(i=0; i<MAX_PROJECTILES_ON_SCREEN; i++)//percorre o vetor de projéteis
        {
            if(test_move(projectile[i],map))//se o projétil puder mover
            {
                move(&projectile[i],map);//move o projétil
            }
            else//se não puder mover
            {
                for(int j=0; j<MAX_ENEMIES; j++)//percorre o vetor de inimigos
                {
                    if(enemy[j].x==(projectile[i].x+projectile[i].dx)&&enemy[j].y==(projectile[i].y+projectile[i].dy))//se a posição onde o projétil queria se mover é a mesma do inimigo
                    {
                        enemy[j].health_points--;//decrementa da vida do inimigo
                    }
                }
                map[projectile[i].y][projectile[i].x] = ' ';//substitui o tiro por um espaço vazio
                //reseta os status dele para a posição do vetor ser reutilizada posteriormente
                projectile[i].dx = 0;
                projectile[i].dy = 0;
                projectile[i].x = -1;
                projectile[i].y = -1;
            }
        }

        //===================================================================================================================
        //MOVIMENTO DO INIMIGO

        for(i=0; i<MAX_ENEMIES; i++)//percorre o vetor de inimigos
        {

            if (enemy[i].flag==0)//se a sua flag for 0, significa que ele está parado e pode decidir para onde se movimentar
            {
                do//executa até ele decidir aleatoriamente uma direção que não seja nula ou para uma posição valida
                {
                    //atribui valores de intenção de movimento aleatório para os dois eixos
                    enemy[i].dx = (-1 + (rand()%3));
                    enemy[i].dy = (-1 + (rand()%3));
                }
                while(test_move(enemy[i],map)==0&&enemy[i].dx==0||enemy[i].dy==0);
            }

            if(test_move(enemy[i],map))//testa se ele pode mover
            {
                move(&enemy[i],map);//move
                enemy[i].flag = 1;//flag 1 significa que ele está em movimento
            }
            else//se não puder se movimentar
            {
                if(enemy[i].count==40)//se atingir o limite dos contador
                {
                    //reseta flag e contador para 0, dizendo que esperou o suficiente e pode decidir um novo movimento
                    enemy[i].flag=0;
                    enemy[i].count=0;
                }
                enemy[i].count=enemy[i].count+1;//incrementa o seu contador
            }
        }

        //===================================================================================================================
        //DESENHA MAPA
        draw_map(map,enemy[0], isaac, player, clickety);
        
    }

    CloseWindow();// Fecha a janela e o contexto OpenGL
    return 0;
}



#include <stdio.h>
#include <string.h>
#define MAX_ARCHIVE_NAME 50

int show_map(char map[30][61]){

    for(int i = 0;i<30;i++){
        for(int j = 0;j<61;j++){
            printf("%c",map[i][j]);
        }
    }
    return 1;
}

int save_map(char archive_name[], char current_map[30][61]){

    FILE *txt_arc = fopen(archive_name, "w");
    if (txt_arc == NULL)
    {
        printf("Opening error\n");
    }
    else
    {
        for(int n_line = 0;n_line<30;n_line++){
            if (fwrite(current_map[n_line], sizeof(char), (61), txt_arc) != (61))
            {
                printf("Writing error in line %d\n", n_line);
                fclose(txt_arc);
                return 0;
            }
        }
        fclose(txt_arc);
        printf("Successful saving\n");
        return 1;
    }
}
void get_level(char archive_name[MAX_ARCHIVE_NAME]){

    int level;
    printf("Número da fase: ");
    scanf("%d",&level);

    archive_name[5] = (level%10) + '0';
    archive_name[4] = ((level%100)/10) + '0';
    archive_name[3] = (level/100) + '0';

    printf("%s",archive_name);



}

int load_map(char archive_name[], char current_map[30][61]){

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
        show_map(current_map);
        return 1;
    }
}

void show_options(){
        puts(" ");
        puts("Options:");
        puts("0 - exit");
        puts("1 - save");
        puts("2 - load");
        puts("3 - show map");
        puts("4 - rename map");
        puts(" ");
}

int main()
{
    char map[30][61];
    char archive[MAX_ARCHIVE_NAME] = "map000.txt";
    int option;

    do
    {
        show_options();
        scanf("%d", &option);

        switch (option){
            case 0:
                puts("Exiting program...");
                break;
            case 1:
                save_map(archive,map);
                break;
            case 2:
                load_map(archive,map);
                break;
            case 3:
                show_map(map);
                break;
            case 4:
                get_level(archive);
                break;
            default:
                puts("That option does not exist");
        }
    }while(option!=0);

    return 0;
}


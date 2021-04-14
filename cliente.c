#include "bibli.h"

int main(void) {
	
	int socket = ConexaoRawSocket("lo");
    char arg[100];
    char aux[100]; 
    char c = '0';
    setupterm(NULL, STDOUT_FILENO, NULL);

    while(1){
        MenuCliente();
        //pega oq foi digitado da entrada padrão
        fgets(arg, 100, stdin);
        //digide utilizando os espaços como divisor
        strtok(arg, "\n");
        strcpy(aux, arg);
        int i = 0;
        int j=2;
        char s[2] = " ";
        char *token;
        char command[5][100];
        token = strtok(arg, s);

        while( token != NULL ) {
            strcpy(command[i], token);
            token = strtok(NULL, s);
            i++;
        }  
        if (strcmp(command[0], "sair") == 0){
            printf("Entrou \n");
            break;
        }
        else if(strcmp(command[0], "h") == 0){
            Help();
        }
        else if(strcmp(command[0], "cd") == 0){
        if(i>1){
            while(j<i){
                strcat(command[1]," ");
                strcat(command[1],command[j]);
                j++;
            }
        }
            cd(command[1], socket);      
        }
        else if(strcmp(command[0], "ls") == 0){
            ls(command[1], socket);      
        }
        else if(strcmp(command[0], "ver") == 0){
            ver(command[1], socket);      
        }
        else if(strcmp(command[0], "linha") == 0){
            linha(command[2], command[1], socket); 
        }
        else if(strcmp(command[0], "linhas") == 0){
            linhas(command[3], command[1], command[2], socket);      
        }
        else if(strcmp(command[0], "editar") == 0){
            char lin[100];
            memset(lin,NULL,sizeof(lin));
            token = strtok(aux, s);
            token = strtok(NULL, s);
            token = strtok(NULL, s);
            token = strtok(NULL, s);
            while( token != NULL ) {
                strcat(lin, token);
                strcat(lin, " ");
                token = strtok(NULL, s);
            }  

            editar(command[2], command[1], lin, socket);      
            
        }
//comando que executam no proprio cliente
        else if(strcmp(command[0], "lls") == 0){
            lls();
        }
        else if(strcmp(command[0], "lcd") == 0){
            lcd(command[1]);
        }
        else{
            printf("Comando inválido!");
            break;
        }
    }
}  

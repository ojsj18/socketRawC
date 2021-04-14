#include "bibli.h"
int main(void) {

	int socket = ConexaoRawSocket("lo");
	Message message_recv;
    Message message_send;
	char *token;
	DIR *dir;
	FILE *fp;
	int s;
	int erro;
	int achou;
	int cont;
	int seq;
	int linha;
	char *p="a";
   	struct dirent *lsdir;
	char *buffer2;
	char *letra;
	int cont_linha;
	int cont_linha2;
	int cont_linha3;			
	int cont_pal=0;
	int i=0;			
	char data[15];
	int j=0;
	int num_s=0;;
	int k=0;

	setupterm(NULL, STDOUT_FILENO, NULL);
	MenuServidor();

	while(1){
		if ( recv(socket, &message_recv, sizeof(message_recv), 0) == -1)
		{
        	printf("Erro ao receber msg! \n");
        	printf("Erro: %s \n", strerror(errno));
    	}
	if(message_recv.add==2){
	 switch (message_recv.type){
        case(0):
			recv(socket, &message_recv, sizeof(message_recv), 0);
			printf("comando cd\n");
			if(checkParity(&message_recv)==0){
				printf("NACK\n");
				setMessage(&message_send,1,0,0, 0, 9,0);
				send(socket,&message_send, sizeof(message_send), 0);
				do{
					recv(socket, &message_recv, sizeof(message_recv), 0);
				}while(checkParity(&message_recv));
				
			}
			printf("arquivo: %s \n",message_recv.data);
			dir = opendir("./");
			if(dir==NULL){
				printf("sem permissão \n");
				setMessage(&message_send,1, '\0' ,0, 0, 15,1);
				send(socket,&message_send, sizeof(message_send), 0);
				break;
			}
			achou=0;
			while ( ( lsdir = readdir(dir) ) != NULL)
    		{
				if(strcmp(lsdir->d_name,message_recv.data)==0){
					achou=1;
				}
   			}
			printf("%d",achou);
			if(achou==0){
				printf("diretorio não existe \n");
				setMessage(&message_send,1, '\0' ,sizeof("2"), 0, 15,"2");
				send(socket,&message_send, sizeof(message_send), 0);
				closedir(dir);
				break;
			}

			if(message_recv.marker=='~' && message_recv.add==2){
					if(chdir(message_recv.data)== -1){
						printf("não foi possivel acessar o diretorio");
						setMessage(&message_send,1, '\0' ,sizeof("1"), 0, 15,"1");
						break;
					}
					else{
						setMessage(&message_send,1, '~' ,0, 0, 8,0);
						send(socket,&message_send, sizeof(message_send), 0);
						printf("ACK\n");
						printf("acabou mensagem");
						closedir(dir); 
					}
			}
			break;
        case(1):
			printf("comando ls");
			if(checkParity(&message_recv)==0){
				printf("NACK");
				setMessage(&message_send,1, '~' ,0, 0, 9,0);
				send(socket,&message_send, sizeof(message_send), 0);
				do{
					recv(socket, &message_recv, sizeof(message_recv), 0);
				}while(checkParity(&message_recv)==0);	
			}
			printf("\n");
			buffer2= (char *)malloc(sizeof(char)*15);
			cont_pal=0;
			i=0;
			cont=0;
			achou=0;
			cont=0;
			dir = opendir("./");
			if(dir==NULL){
				printf("sem permissão \n");
				setMessage(&message_send,1, '\0' ,0, 0, 15,1);
				send(socket,&message_send, sizeof(message_send), 0);
				break;
			}
			s=1;
			lsdir = readdir(dir);
			strcpy(buffer2,lsdir->d_name);
			buffer2=realloc(buffer2,(s*15*8));

			while ( ( lsdir = readdir(dir) ) != NULL )
    		{	
				strcat(buffer2," ");
				strcat(buffer2,lsdir->d_name);
				if(lsdir->d_type==4){
					strcat(buffer2,"/");
				}
				s++;
				buffer2=realloc(buffer2,(s*15*8));
   			}
			cont_pal=strlen(buffer2);
			printf("tam:%d\n %s\n",cont_pal,buffer2);

			data[15];
			j=15;
			num_s=1+(cont_pal/15);
			k=0;

			for(s=0;s<15;s++){
                    data[s]='0';
            }
			setMessage(&message_send,1, '~' ,15,0, 11,buffer2);
			send(socket,&message_send, sizeof(message_send), 0);
			do
			{
				recv(socket, &message_recv, sizeof(message_recv), 0);	
			} while (message_recv.type!=8);

			while(message_send.type!=13)
			{
			if(message_recv.add==2){
				if(cont>num_s)
				{			
					printf("acabou a mensagem\n");
					setMessage(&message_send,1, '~' ,0, cont, 13,'0');
					send(socket,&message_send, sizeof(message_send), 0);
					closedir(dir);
					free(buffer2);
					buffer2=NULL;
					break;
				}
				else{
					if(message_recv.type==8 && message_recv.seq==cont){
						printf("ACK\n");
						//printf("mr %d\n",message_recv.seq);
						for(s=0;s<15;s++){
							data[s]='\0';
						}
						for(k=0;k<15;k++){
							data[k]=buffer2[j+k];
						}
						j=j+k;
						cont++;
						setMessage(&message_send,1, '\0' ,strlen(data), cont, 11,data);
						send(socket,&message_send, sizeof(message_send), 0);
					}
					else if(message_recv.type==9)
					{
						send(socket,&message_send, sizeof(message_send), 0);
						printf("NACK\n");
					}	
				}
			}
			recv(socket, &message_recv, sizeof(message_recv), 0);
		}
			break;
		case(2):
			printf("Função ver \n");
			if(checkParity(&message_recv)==0){
				printf("NACK");
				setMessage(&message_send,1, '~' ,0, 0, 9,0);
				send(socket,&message_send, sizeof(message_send), 0);
				do{
					recv(socket, &message_recv, sizeof(message_recv), 0);
				}while(checkParity(&message_recv));
				
			}
			dir = opendir("./");
			if(dir==NULL){
				printf("sem permissão \n");
				setMessage(&message_send,1, '\0' ,0, 0, 15,1);
				send(socket,&message_send, sizeof(message_send), 0);
				break;
			}
			lsdir = readdir(dir);
			achou=0;
			while ( ( lsdir = readdir(dir) ) != NULL )
    		{
				if(strcmp(lsdir->d_name,message_recv.data)==0){
					achou=1;
				}
   			}
			if(achou==0){
				printf("arquivo não existe \n");
				setMessage(&message_send,1, '\0' ,sizeof("3"), 0, 15,"3");
				send(socket,&message_send, sizeof(message_send), 0);
				closedir(dir);
				break;
			}
			cont=0;
			seq=0;
			buffer2= (char *)malloc(sizeof(char)*15);
			letra= (char *)malloc(sizeof(char)*250);
			fp = fopen(message_recv.data,"r");
			if(!fp)
			{
    			printf( "Erro na abertura do arquivo \n");
    			exit(0);
			}
			p="a";
			p=fgets(buffer2,15,fp);
			setMessage(&message_send,1, '~' ,strlen(buffer2), cont, 12,buffer2);
			printf("%s \n",message_send.data);
			send(socket,&message_send, sizeof(message_send), 0);
			printf("ACK NOME\n");
			printf("arquivo: %s \n",message_recv.data);
nackver:
			
			recv(socket, &message_recv, sizeof(message_recv), 0);	
			if(message_recv.type==9){
				printf("NACK");
				send(socket,&message_send, sizeof(message_send), 0);
				goto nackver;
			}
			else if (message_recv.type != 8)
			{
				goto nackver;
			}
			while(message_send.type!=13){
				for(s=0;s<250;s++){
					letra[s]='\0';
				}
				p=fgets(letra,250,fp);
				if(p!=NULL){
					j=0;
					k=0;
					for(s=0;s<15;s++){
						message_send.data[s]='\0';
					}
					while(seq<=(1+strlen(letra)/15)){
						if (message_recv.type==8)
						{
							for(s=0;s<15;s++){
								buffer2[s]='\0';
							}
							for(k=0;k<15;k++){
								buffer2[k]=letra[j+k];
							}
							j=j+k;
							setMessage(&message_send,1, '~' ,strlen(buffer2),cont, 12,buffer2);
							send(socket,&message_send, sizeof(message_send), 0);
							printf("%s \n",message_send.data);
							cont++;
							seq++;
						}
						else if (message_recv.type==9)
						{
							send(socket,&message_send, sizeof(message_send), 0);
						}	
					}
				seq=0;
				}
				else if (p==NULL)
					{
						setMessage(&message_send,1,0,0, cont, 13,0);
						send(socket,&message_send, sizeof(message_send), 0);
						printf("acabou a mensagem\n");
						fclose(fp);
						free(letra);
						free(buffer2);
						buffer2=NULL;
						letra=NULL;
					}			
			}
			break;
		case(3):
			printf("Função linha \n");
			printf("arquivo: %s \n",message_recv.data);
			if(checkParity(&message_recv)==0){
				printf("NACK");
				setMessage(&message_send,1, '~' ,0, 0, 9,0);
				send(socket,&message_send, sizeof(message_send), 0);
				do{
					recv(socket, &message_recv, sizeof(message_recv), 0);
				}while(checkParity(&message_recv)==0);	
			}
			letra= (char *)malloc(sizeof(char)*250);
			buffer2= (char *)malloc(sizeof(char)*15);
			linha =0;
			cont_linha=0;
			cont=0;
			i=0;
			dir = opendir("./");
			p="a";
			if(dir==NULL){
				printf("sem permissão/acesso negado \n");
				setMessage(&message_send,1, '\0' ,0, 0, 15,1);
				send(socket,&message_send, sizeof(message_send), 0);
				free(buffer2);
				free(letra);
				buffer2=NULL;
				letra=NULL;
				break;
			}
			lsdir = readdir(dir);
			achou=0;
			while ( ( lsdir = readdir(dir) ) != NULL )
    		{
				if(strcmp(lsdir->d_name,message_recv.data)==0){
					achou=1;
				}
   			}
			if(achou==0){
				printf("diretorio não existe \n");
				setMessage(&message_send,1, '\0' ,sizeof("2"), 0, 15,"2");
				send(socket,&message_send, sizeof(message_send), 0);
				closedir(dir);
				free(buffer2);
				free(letra);
				buffer2=NULL;
				letra=NULL;
				break;
			}
			fp = fopen(message_recv.data,"r");
			if(!fp)
			{
				printf("sem permissão/acesso negado \n");
				setMessage(&message_send,1, '\0' ,sizeof("1"), 0, 15,"1");
				send(socket,&message_send, sizeof(message_send), 0);
				break;
			}
			setMessage(&message_send,1,0 ,0,0, 8,0);
			send(socket,&message_send, sizeof(message_send), 0);
			printf("ACK NOME\n");//tá pegando o nome
			//printMsg(&message_send);

nacklinha:
			do
			{
				recv(socket, &message_recv, sizeof(message_recv), 0);
			} while (message_recv.type!=10);
				
			if (message_recv.add==2)
			{
				if(checkParity(&message_recv)==0){
					printf("NACK \n");
					setMessage(&message_send,1,0 ,0,0, 9,0);
					send(socket,&message_send, sizeof(message_send), 0);
				}
				else if (message_recv.type==10 && checkParity(&message_recv)==1)
				{
					printf("ACK LINHA\n");
				}
			}
			else
			{
				recv(socket, &message_recv, sizeof(message_recv), 0);
				goto nacklinha;
			}
			
			cont_linha=atoi(message_recv.data);
			printf("%d \n",cont_linha);
			
			for(s=0;s<250;s++){
				letra[s]='\0';
			}
			p=fgets(letra,250,fp);
			while ( p != NULL)
			{
				linha++;
				if(linha==cont_linha){
					j=0;
					k=0;		
					while(cont<=ceil(strlen(letra)/15)){
						if (message_recv.type==8 || message_recv.type==10){
							for(s=0;s<15;s++){
								buffer2[s]='\0';
							}
							for(k=0;k<15;k++){
								buffer2[k]=letra[j+k];
							}
							j=j+k;
							setMessage(&message_send,1, '~' ,strlen(buffer2),cont, 12,buffer2);
							send(socket,&message_send, sizeof(message_send), 0);
							//printMsg(&message_recv);
							cont++;
						}
						else if (message_recv.type==9)
						{
							send(socket,&message_send, sizeof(message_send), 0);
						}
						//acho que esquci o revc
					}	
						setMessage(&message_send,1, '\0' ,0, cont, 13,'\0');
						send(socket,&message_send, sizeof(message_send), 0);
						printf("acabou a mensagem\n");
						fclose(fp);
						free(letra);
						free(buffer2);
						buffer2=NULL;
						letra=NULL;
				}		
			p=fgets(letra,250,fp);
				if (p==NULL && linha<cont_linha)
					{
					printf("linha não existente \n");
					setMessage(&message_send,1, '\0' ,sizeof("4"), 0, 15,"4");
					send(socket,&message_send, sizeof(message_send), 0);
					free(buffer2);
					free(letra);
					buffer2=NULL;
					letra=NULL;
					break;				
					}
			}
			break;
		case(4):
			printf("Função Linhas\n");
			printf("arquivo: %s \n",message_recv.data);
			if(checkParity(&message_recv)==0){
				printf("NACK");
				setMessage(&message_send,1, '~' ,0, 0, 9,0);
				send(socket,&message_send, sizeof(message_send), 0);
				do{
					recv(socket, &message_recv, sizeof(message_recv), 0);
				}while(checkParity(&message_recv)==0);	
			}
			buffer2= (char *)malloc(sizeof(char)*15);
			letra= (char *)malloc(sizeof(char)*250);
			printf("\n");
			linha=0;
			cont_linha=0;
			cont_linha2=0;
			cont_linha3=0;
			cont=0;
			seq=0;
			char *token;
			i=0;
			dir = opendir("./");
			p="a";
			if(dir==NULL){
				printf("sem permissão/acesso negado \n");
				setMessage(&message_send,1, '\0' ,0, 0, 15,1);
				send(socket,&message_send, sizeof(message_send), 0);
				break;
			}
			lsdir = readdir(dir);
			achou=0;
			while ( ( lsdir = readdir(dir) ) != NULL )
    		{
				if(strcmp(lsdir->d_name,message_recv.data)==0){
					achou=1;
				}
   			}
			if(achou==0){
				printf("diretorio não existe \n");
				setMessage(&message_send,1, '\0' ,sizeof("2"), 0, 15,"2");
				send(socket,&message_send, sizeof(message_send), 0);
				closedir(dir);
				free(buffer2);
				free(letra);
				buffer2=NULL;
				letra=NULL;
				break;
			}
			fp = fopen(message_recv.data,"r");
			if(!fp)
			{
				printf("sem permissão/acesso negado \n");
				setMessage(&message_send,1, '\0' ,sizeof("1"), 0, 15,"1");
				send(socket,&message_send, sizeof(message_send), 0);
				break;
			}
			setMessage(&message_send,1, '~' ,0,0, 8,'0');
			send(socket,&message_send, sizeof(message_send), 0);
			printf("ACK NOME\n");
nacklinhas:	
			do
			{
				recv(socket, &message_recv, sizeof(message_recv), 0);
			} while (message_recv.type!=10);

			if (message_recv.add==2)
			{
				//printMsg(&message_recv);
				if(checkParity(&message_recv)==0){
					printf("NACK \n");
					setMessage(&message_send,1,0 ,0,0, 9,0);
					send(socket,&message_send, sizeof(message_send), 0);
				}
				else if (message_recv.type==10 && checkParity(&message_recv)==1)
				{
					printf("ACK LINHAS\n");
				}
			}
			else
			{
				recv(socket, &message_recv, sizeof(message_recv), 0);
				goto nacklinha;
			}
			cont_linha=message_recv.data[0];
			cont_linha2=message_recv.data[1];
			cont_linha3=cont_linha2-cont_linha;
			//printf("linhas: %d %d \n",cont_linha,cont_linha2);
			s=0;
			while (fgets(letra,250,fp)!= NULL)
			{
				s++;
				if(s==cont_linha2){
					achou=0;
				}
			}
			if (achou==1)
			{
				printf("linha não existe \n");
				setMessage(&message_send,1, '\0' ,sizeof("4"), 0, 15,"4");
				send(socket,&message_send, sizeof(message_send), 0);
				closedir(dir);
				free(buffer2);
				free(letra);
				buffer2=NULL;
				letra=NULL;
				break;
			}
			rewind(fp); 
			for(s=0;s<250;s++){
				letra[s]='\0';
			}
			p=fgets(letra,250,fp);

			while ( p != NULL)
			{
				linha++;
				if(linha>=cont_linha && linha<=cont_linha2){
					j=0;
					k=0;
					for(s=0;s<15;s++){
						message_send.data[s]='\0';
					}
					while(seq<=(1+strlen(letra)/15)){
						if ((message_recv.type==8 || message_recv.type==10))
						{
							for(s=0;s<15;s++){
								buffer2[s]='\0';
							}
							for(k=0;k<15;k++){
								buffer2[k]=letra[j+k];
							}
							j=j+k;
							setMessage(&message_send,1, '~' ,strlen(buffer2),cont, 12,buffer2);
							send(socket,&message_send, sizeof(message_send), 0);
							cont++;
							seq++;
						}
						else if (message_recv.type==9)
						{
							send(socket,&message_send, sizeof(message_send), 0);
						}	
					}
					p=fgets(letra,250,fp);
					seq=0;
					if(linha>=cont_linha2){
						setMessage(&message_send,1, '0' ,0, cont, 13,'0');
						message_send.seq=cont;
						send(socket,&message_send, sizeof(message_send), 0);
						printf("acabou a mensagem\n");
						fclose(fp);
						free(letra);
						free(buffer2);
						letra=NULL;
						buffer2=NULL;
					}
				}
			}
			break;
		case(5):
			printf("Função editar\n");
			printf("arquivo: %s \n",message_recv.data);
			if(checkParity(&message_recv)==0){
				printf("NACK");
				setMessage(&message_send,1, '~' ,0, 0, 9,0);
				send(socket,&message_send, sizeof(message_send), 0);
				do{
					recv(socket, &message_recv, sizeof(message_recv), 0);
				}while(checkParity(&message_recv)==0);	
			}
			linha=0;
			cont_linha=0;
			cont=0;
			seq=0;
			i=0;
			dir = opendir("./");
			p="a";
			if(dir==NULL){
				printf("sem permissão/acesso negado \n");
				setMessage(&message_send,1, '\0' ,0, 0, 15,1);
				send(socket,&message_send, sizeof(message_send), 0);
				break;
			}
			if(dir==NULL){
				printf("sem permissão/acesso negado \n");
				setMessage(&message_send,1, '\0' ,0, 0, 15,1);
				send(socket,&message_send, sizeof(message_send), 0);
				break;
			}
			lsdir = readdir(dir);
			achou=0;
			letra= (char *)malloc(sizeof(char)*250);
			buffer2= (char *)malloc(sizeof(char)*15);
			while ( ( lsdir = readdir(dir) ) != NULL )
    		{
				if(strcmp(lsdir->d_name,message_recv.data)==0){
					achou=1;
				}
   			}
			if(achou==0){
				printf("diretorio não existe \n");
				setMessage(&message_send,1, '\0' ,sizeof("2"), 0, 15,"2");
				send(socket,&message_send, sizeof(message_send), 0);
				closedir(dir);
				break;
			}
			fp = fopen(message_recv.data,"r+");
			if(!fp)
			{
				printf("sem permissão/acesso negado \n");
				setMessage(&message_send,1, '\0' ,sizeof("1"), 0, 15,"1");
				send(socket,&message_send, sizeof(message_send), 0);
				free(buffer2);
				free(letra);
				buffer2=NULL;
				letra=NULL;
				break;
			}		
			setMessage(&message_send,1,0,0,0, 8,0);
			send(socket,&message_send, sizeof(message_send), 0);
			printf("ACK NOME \n");
			long tam;

nackedit:
			do
			{
				recv(socket, &message_recv, sizeof(message_recv), 0);
			} while (message_recv.type!=10);

			if (message_recv.add==2)
			{
				if(checkParity(&message_recv)==0){
					printf("NACK \n");
					setMessage(&message_send,1,0 ,0,0, 9,0);
					send(socket,&message_send, sizeof(message_send), 0);
				}
				else if (message_recv.type==10 && checkParity(&message_recv)==1)
				{
					printf("ACK LINHA\n");
					cont_linha=atoi(message_recv.data);
					s=0;
					while (fgets(letra,250,fp)!= NULL){
						s++;
						if(s==cont_linha){
							achou=0;
						}
					}
					if (achou==1){
						printf("linha não existe \n");
						setMessage(&message_send,1, '\0' ,sizeof("4"), 0, 15,"4");
						send(socket,&message_send, sizeof(message_send), 0);
						closedir(dir);
						free(buffer2);
						free(letra);
						buffer2=NULL;
						letra=NULL;
						break;
					}
					setMessage(&message_send,1,0,0,0, 8,0);
					send(socket,&message_send, sizeof(message_send), 0);
				}
			}
			else
			{
				recv(socket, &message_recv, sizeof(message_recv), 0);
				goto nacklinha;
			}
			k=0;
			j=0;
			recv(socket, &message_recv, sizeof(message_recv), 0);
			recv(socket, &message_recv, sizeof(message_recv), 0);
       		while(message_recv.type!=13){
				if(message_recv.add==2 && message_recv.type==12){
					printf("ACK\n");	
					for(s=0;s<15;s++){
						buffer2[s+j]='\0';
					}
					for(i=0;i<15;i++){
						buffer2[i+j]=message_recv.data[i];
					}
					j=i+j;
					seq++;
					setMessage(&message_send,1, '\0' ,0, seq, 8,0);
					send(socket, &message_send, sizeof(message_send), 0);
					buffer2=realloc(buffer2,(seq*15*8)); 
            	}
				recv(socket, &message_recv, sizeof(message_recv), 0);
				recv(socket, &message_recv, sizeof(message_recv), 0);
   			}
			for(s=0;s<250;s++){
				letra[s]='\0';
			}
			rewind(fp);
			//FILE *output = fopen("tempo%arq.txt", "w");
			while ((fgets(letra,250,fp) ) != NULL)
			{
				linha++;
				if(linha==cont_linha){
					int tam_linha=strlen(letra);
					int tam_palavra=strlen(buffer2);
					fseek(fp,-1*tam_linha,SEEK_CUR);
					for(s=0;s<tam_linha;s++){
						fprintf(fp," ");
					}
					fseek(fp,-1*tam_linha,SEEK_CUR);
					fprintf(fp,buffer2);
					fprintf(fp,"\0");
				}
				for(s=0;s<250;s++){
					letra[s]='\0';
				}
			}
			printf("mensagem acabou\n");
			setMessage(&message_send,1, '\0' ,0, seq, 8,'0');
			send(socket, &message_send, sizeof(message_send), 0);
			free(buffer2);
			free(letra);
			buffer2=NULL;
			letra=NULL;
			break;
		default:
            break;
}
}
}
}

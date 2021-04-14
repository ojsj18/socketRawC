#include "bibli.h"

void MenuCliente(){ 
    putp(enter_bold_mode);
    printf("Cliente Iniciado\n");
    printf("Pressione 'h' para obter ajuda!\n");
    putp(exit_attribute_mode);
    printf("Digite seu comando: \n");
}

void MenuServidor(){
    //system("clear"); 
    putp(enter_bold_mode);
    printf("Servidor Iniciado!\n");
    putp(exit_attribute_mode);

}

void Help(){
    system("clear"); 
    printf("COMANDOS:\n");
    printf("cd <nome_dir>\n");
    printf("lcd <nome_dir>\n");
    printf("ls\n");
    printf("lls\n");
    printf("ver <nome_arq>\n");
    printf("linha <numero_linha> <nome_arq>\n");
    printf("linhas <numero_linha_inicial> <numero_linha_final> <nome_arq>\n");
    printf("edit <numero_linha> <nome_arq> '<NOVO_TEXTO>' \n");
}

int checkParity(Message *msg){
    unsigned char p = msg->size^msg->seq^msg->type^msg->add;
    for (int i = 0; i < msg->size; i++)
        p ^= msg->data[i];
    if (p == msg->parity)
        return 1;
    else
        return 0;
}

void setMessage(Message *msg,int add, unsigned char marker, unsigned char size, int seq, unsigned char type, char *data){
    msg->marker = marker;
    msg->size = size;
    msg->seq = seq;
    msg->type = type;
    msg->add =  add;
    for (int i = 0; i < 15; i++)
        msg->data[i] = 0;
    for (int i = 0; i < size; i++){
        msg->data[i] = data[i];
    }
    unsigned char p = msg->size^msg->seq^msg->type^msg->add;
    for (int i = 0; i < msg->size; i++) {
        p ^= msg->data[i];
    }
    msg->parity = p;

return;
}

void cd(char *arg, int socket){
//a primeira mensagem apenas avisa que é um cd
Message ms;
Message mr;
double tempo=0;
struct timeval  ti, tf;
setMessage(&ms,2, '~' ,strlen(arg), 0, 0, arg);
tempo:
    if (send(socket, &ms, sizeof(ms), 0) == -1)
    {
        printf("Erro ao enviar mensagem! \n");
        printf("Erro: %s \n", strerror(errno));
    }
    recv(socket, &mr, sizeof(mr), 0);

gettimeofday(&ti, NULL);
    do
    {
        recv(socket, &mr, sizeof(mr), 0);
        if(mr.type==9){
            send(socket, &ms, sizeof(ms), 0);
        }
        else if(mr.type==15){
        //printf("%d %d\n",mr.type,mr.add);
            if(atoi(mr.data)==1){
                printf("acesso proibido \n");
            }
            else if(atoi(mr.data)==2){
                printf("diretorio não existe \n");
            }
            return;
        }
        gettimeofday(&tf, NULL);
        tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
        if(tempo>20){
            printf("TIMEOUT\n");
            goto tempo;
        }
    } while(mr.type!=8);
    printf("%d",mr.type);
    printf("diretorio acessado \n");
}

void ls(char *arg, int socket){
Message ms;
Message mr;
setMessage(&ms,2, '~' ,strlen(arg), 0, 1, arg);
double tempo=0;
struct timeval  ti, tf;
char *buffer= (char *)malloc(sizeof(char)*15);
int i=0;
int j=0;
int s;
int seq=0;

    if (send(socket, &ms, sizeof(ms), 0) == -1) 
    {
        printf("Erro ao enviar mensagem! \n");
        printf("Erro: %s \n", strerror(errno));
    }
    else{
        recv(socket, &mr, sizeof(mr), 0);
        recv(socket, &mr, sizeof(mr), 0);
        if (mr.type==15)
        {
            if(atoi(mr.data)==1){
                printf("acesso proibido \n");
            }
            return;
        }
        else if(mr.type==9){
            do
            {
                send(socket, &ms, sizeof(ms), 0);
                recv(socket, &mr, sizeof(mr), 0);
            } while (mr.type!=11);
        }

        if(checkParity(&mr)==0 && mr.add==1){
            printf("NACK paridade");
			setMessage(&ms,1, '~' ,0, 0, 9,0);
			send(socket,&ms, sizeof(ms), 0);
			do{
				recv(socket, &mr, sizeof(mr), 0);
			}while(checkParity(&mr)==0);
        }

        setMessage(&ms,2, '\0' ,0, seq, 8,0);
        send(socket, &ms, sizeof(ms), 0);

        while(mr.type!=13){
        gettimeofday(&ti, NULL);
        //printf("%d %d %s\n",mr.seq,seq,mr.data);
            if(mr.add==1 && mr.type==11 && ms.type==8 && mr.seq==seq){
                for(s=0;s<15;s++){
                    buffer[s+j]='\0';
                }
                for(i=0;i<15;i++){
                    buffer[i+j]=mr.data[i];
                }
                j=i+j;
                seq++;
                setMessage(&ms,2, '\0' ,0, seq, 8,'0');
                send(socket, &ms, sizeof(ms), 0);
                buffer=realloc(buffer,(seq*15*8));
            }
            else if (mr.type==9 & mr.add==1)
            {
               printf("NACK\n");
               setMessage(&ms,2, '\0' ,0, seq, 9,'0');
               send(socket, &ms, sizeof(ms), 0);
            }
            if(tempo>20){
                printf("TIMEOUT\n");
                send(socket, &ms, sizeof(ms), 0);
            }
        recv(socket, &mr, sizeof(mr), 0);

        if(checkParity(&mr)==0){
			setMessage(&ms,2, '~' ,0, 0, 9,0);
			send(socket,&ms, sizeof(ms), 0);
			do{
				recv(socket, &mr, sizeof(mr), 0);
			}while(checkParity(&mr)==0);
            setMessage(&ms,2, '\0' ,0, seq, 8,'0');
            send(socket, &ms, sizeof(ms), 0);      
        }
        gettimeofday(&tf, NULL);
        tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
    }
		setMessage(&ms,2, '\0' ,0, seq, 8,'0');//ACK do fim da transmissao
        send(socket, &ms, sizeof(ms), 0);
        //printf("tipo : %d %d \n",mr.type,mr.add);
        printf("\n %s \n",buffer);
        printf("\n");
        free(buffer);
    }
}

void lls(){
    DIR *dir;
   	struct dirent *lsdir;
    printf("comando lls\n");
    printf("\n");
    dir = opendir("./");
	lsdir = readdir(dir);
		while ( ( lsdir = readdir(dir) ) != NULL )
    		{	
				if(lsdir->d_type==4){
                    printf("%s/ ",lsdir->d_name);
				}
                else
                {
                   printf("%s ",lsdir->d_name);
                }
                
   			}
    printf("\n");
    printf("\n");
}

void lcd(char *arg){
    //verificar antes se o diretorio existe
    printf("comando lcd \n");
	if(chdir(arg)== -1){
        printf("não foi possivel acessar o diretorio");
    };
}

void ver(char *arg, int socket){
Message ms;
Message mr;
double tempo=0;
struct timeval  ti, tf;
setMessage(&ms,2,'~' ,strlen(arg), 0, 2, arg);
send(socket, &ms, sizeof(ms), 0);

char *buffer= (char *)malloc(sizeof(char)*15);
unsigned char c=13;
int i=0;
int j=0;
int seq=0;
int linha=1;
int s;

nack:
        gettimeofday(&ti, NULL);
        recv(socket, &mr, sizeof(mr), 0);
        recv(socket, &mr, sizeof(mr), 0);

        if (mr.type==15)
        {
            if(atoi(mr.data)==1){
                printf("acesso proibido \n");
            }
            if(atoi(mr.data)==3){
                printf("arquivo não existe \n");
            }
            return;
        }
        else if(mr.type==9){
            do
            {
                send(socket, &ms, sizeof(ms), 0);
                recv(socket, &mr, sizeof(mr), 0);
            } while (mr.type!=12);
        }
        else if(mr.add!=1){
                goto nack;
        }
        gettimeofday(&tf, NULL);
        tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
        if(tempo>20){
            printf("TIMEOUT\n");
            goto nack;
        }
        if(checkParity(&mr)==0){
			setMessage(&ms,2, '\0' ,0, seq, 9,'\0');
			send(socket,&ms, sizeof(ms), 0);
			do{
				recv(socket, &mr, sizeof(mr), 0);
			}while(checkParity(&mr)==0);

            setMessage(&ms,2, '\0' ,0, seq, 8,'0');
            send(socket, &ms, sizeof(ms), 0);
        }

        printf("ACK nome do arquivo\n");
        while(mr.type!=c){
        gettimeofday(&ti, NULL);
            if(mr.add==1 && mr.type==12 && mr.seq==seq ){
                printf("ACK\n");
                    for(s=0;s<15;s++){
                        buffer[s+j]='\0';
                    }
                    for(i=0;i<15;i++){
                        buffer[i+j]=mr.data[i];
                    }
                    j=i+j;
                    seq++;
                    setMessage(&ms,2, '\0' ,0, seq, 8,'\0');
                    send(socket, &ms, sizeof(ms), 0);  
                    buffer=realloc(buffer,(seq*15*8));
            }
        recv(socket, &mr, sizeof(mr), 0);
        recv(socket, &mr, sizeof(mr), 0);
        if(checkParity(&mr)==0 && mr.seq==seq && mr.add==1){
			setMessage(&ms,2, '\0' ,0, seq, 9,'\0');
			send(socket,&ms, sizeof(ms), 0);
			do{
                printf("NACK\n");
				recv(socket, &mr, sizeof(mr), 0);
			}while(checkParity(&mr)==0);
            setMessage(&ms,2, '\0' ,0, seq, 8,'0');
            send(socket, &ms, sizeof(ms), 0);
        } 
        gettimeofday(&ti, NULL);
        if(tempo>20){
            printf("TIMEOUT\n");
            send(socket, &ms, sizeof(ms), 0);
        }
    }
		setMessage(&ms,2, '\0' ,0, seq, 8,'\0');//ACK do fim da transmissao
        send(socket, &ms, sizeof(ms), 0);
        printf("%s : \n",arg);
        printf("%d| ",linha);
        for(i=0;i<=(mr.seq*15);i++){
            printf("%c",buffer[i]);
            if(buffer[i]=='\n'){
                linha++;
                printf("%d| ",linha);
            }
        }
        free(buffer);
        printf("\n");
}

void linha(char *arg, char *arg2, int socket){
printf("comando linha");
Message ms;
Message mr;
Message aux;
double tempo=0;
struct timeval  ti, tf;

setMessage(&ms,2,'~' ,strlen(arg), 0, 3,arg);
send(socket,&ms, sizeof(ms), 0);

int seq=0;
char *buffer= (char *)malloc(sizeof(char)*15);
int i=0;
int j=0;
int s;
gettimeofday(&ti, NULL);
recv(socket, &mr, sizeof(mr), 0);

do
{
    recv(socket, &mr, sizeof(mr), 0);
    gettimeofday(&tf, NULL);
    tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
    if (tempo>20)
    {
        printf("TIME OUT \n");
        send(socket,&ms, sizeof(ms), 0);       
    }
    
} while (mr.add!=1);

    if (mr.type==15)
    {
        if(atoi(mr.data)==1){
            printf("acesso proibido/erro ao acessar arquivo \n");
    }
        if(atoi(mr.data)==2){
            printf("diretorio não existe \n");
        }
        if(atoi(mr.data)==4){
            printf("linha não existe\n");
        }
        return;
    }
    else if(mr.type==9){
        printf("NACK \n");
        send(socket,&ms, sizeof(ms), 0);
    }
    else if (mr.type==8)
    {
        printf("ACK NOME\n");
        setMessage(&ms,2,'\0',strlen(arg2), seq, 10,arg2);
        send(socket,&ms, sizeof(ms), 0);
    }

nack:
    recv(socket, &mr, sizeof(mr), 0);
    gettimeofday(&ti, NULL);
    
    if(mr.type==9){
        send(socket,&ms, sizeof(ms), 0);
        goto nack;
    }
    if(mr.type==15){
        if(atoi(mr.data)==4){
            printf("linha não existe\n");
        }
        return;
    }
    else if (mr.type!=12)
    {
        goto nack;
    }
    gettimeofday(&tf, NULL);
    tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
    if (tempo>20)
    {
        printf("TIME OUT \n");
        send(socket,&ms, sizeof(ms), 0);
        goto nack;      
    }


if(mr.add==1 && checkParity(&mr)==1 && mr.type!=8){
    if(mr.type==9){
        setMessage(&ms,2,'\0',strlen(arg2), seq, 10,arg2);
        printf("NACK LINHA\n");
        send(socket,&ms, sizeof(ms), 0);
    }
    else if(mr.type==12){
        printf("ACK LINHA\n");
        setMessage(&ms,2,'\0',0, seq, 8,0);
        send(socket, &ms, sizeof(ms), 0);
    }
}
else
{
    goto nack;
}

    while(mr.type!=13){
    gettimeofday(&ti, NULL);
            if(mr.add==1 && mr.type==12){
                for(s=0;s<15;s++){
                    buffer[s+j]='\0';
                }
                for(i=0;i<15;i++){
                    buffer[i+j]=mr.data[i];
                }
                j=i+j;
                seq++;
                setMessage(&ms,2, '\0' ,0, seq, 8,'0');
                send(socket, &ms, sizeof(ms), 0);
                buffer=realloc(buffer,(seq*15*8)); 
            }
            else if (mr.add==1 && mr.type==9)
            {
                setMessage(&ms,2, '\0' ,0, seq, 9,'0');
                send(socket, &ms, sizeof(ms), 0);
                printf("NACK");
            }
            else if (mr.add==1 && mr.type==15)
            {
                if(atoi(mr.data)==4){
                    printf("linha não existe \n");
                }
            return;
            }
            gettimeofday(&tf, NULL);
            tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
            if (tempo>20)
            {
                printf("TIME OUT \n");
                send(socket,&ms, sizeof(ms), 0);
                goto nack;      
            }
        recv(socket, &mr, sizeof(mr), 0);
        recv(socket, &mr, sizeof(mr), 0);
    }

        printf("linha %d|%s \n",atoi(arg2),buffer);
        printf("\n");
        free(buffer);
    
}

void linhas(char *arg, char *arg2, char *arg3, int socket){
printf("comando linhas\n");
Message ms;
Message mr;
setMessage(&ms,2,'~' ,strlen(arg), 0, 4,arg);
send(socket, &ms, sizeof(ms), 0);
double tempo=0;
struct timeval  ti, tf;
gettimeofday(&ti, NULL);

int inicial=atoi(arg2);
int final=atoi(arg3);
char data[3];
data[0]= inicial;
data[1]=final;
int seq=0;
int linha=inicial;
unsigned char c=13;
char *buffer= (char *)malloc(sizeof(char)*15);
int i=0;
int j=0;
int s;

recv(socket, &mr, sizeof(mr), 0);

do
{
    gettimeofday(&tf, NULL);
    recv(socket, &mr, sizeof(mr), 0);
    tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
    if (tempo>20)
    {
        printf("TIME OUT \n");
        send(socket,&ms, sizeof(ms), 0);   
    }

} while (mr.add!=1);

    if (mr.type==15)
    {
        if(atoi(mr.data)==1){
            printf("acesso proibido/erro ao acessar arquivo \n");
    }
        if(atoi(mr.data)==2){
            printf("diretorio não existe \n");
        }
        if(atoi(mr.data)==4){
            printf("linha não existe\n");
        }
        return;
    }
    else if(mr.type==9){
        printf("NACK \n");
        send(socket,&ms, sizeof(ms), 0);
    }
    else if (mr.type==8)
    {
        printf("ACK NOME\n");
        setMessage(&ms,2,'~' ,strlen(data), 0, 10,data);
        printf("%d\n",ms.data[0]);
        printf("%d\n",ms.data[1]);
        send(socket,&ms, sizeof(ms), 0);
    }

nack:
    gettimeofday(&ti, NULL);
    recv(socket, &mr, sizeof(mr), 0);
    //printf("tipo %d",mr.type);
    
    if(mr.type==9){
        send(socket,&ms, sizeof(ms), 0);
        goto nack;
    }
    if(mr.type==15){
        if(atoi(mr.data)==4){
            printf("linha não existe\n");
        }
        return;
    }
    else if (mr.type!=12)
    {
        goto nack;
    }
    gettimeofday(&tf, NULL);
    tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
    if (tempo>20)
    {
        printf("TIME OUT \n");
        send(socket,&ms, sizeof(ms), 0); 
        goto nack; 
    }

if(mr.add==1 && checkParity(&mr)==1 && mr.type!=8){
    if(mr.type==9){
        printf("NACK LINHA\n");
        send(socket,&ms, sizeof(ms), 0);
    }
    else if(mr.type==12){
        printf("ACK LINHA\n");
        setMessage(&ms,2,'\0',0, seq, 8,0);
        send(socket, &ms, sizeof(ms), 0);
    }
}
else
{
    goto nack;
}

    while(mr.type!=c){
        gettimeofday(&ti, NULL);
        if(mr.add==1 && mr.type==12){
                for(s=0;s<15;s++){
                    buffer[s+j]='\0';
                }
                for(i=0;i<15;i++){
                    buffer[i+j]=mr.data[i];
                }
                j=i+j;
                seq++;
                setMessage(&ms,2, '\0' ,0, seq, 8,'0');
                send(socket, &ms, sizeof(ms), 0);
                buffer=realloc(buffer,(seq*15*8));    
            }
           else if (mr.add==1 && mr.type==9)
            {
                setMessage(&ms,2, '\0' ,0, seq, 9,'0');
                send(socket, &ms, sizeof(ms), 0);
                printf("NACK");
            }
            else if (mr.add==1 && mr.type==15)
            {
                if(atoi(mr.data)==4){
                    printf("linha não existe \n");
                }
            return;
            }
            gettimeofday(&tf, NULL);
            tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
            if (tempo>20)
            {
                printf("TIME OUT \n");
                send(socket,&ms, sizeof(ms), 0); 
            }
        recv(socket, &mr, sizeof(mr), 0);
        recv(socket, &mr, sizeof(mr), 0);
    }
        printf("%s : \n",arg);
        printf("%d| ",linha);
        for(i=0;i<=(mr.seq*15);i++){
            printf("%c",buffer[i]);
            if(buffer[i]=='\n'){
                linha++;
                printf("%d| ",linha);
            }
        }
        free(buffer);
        printf("\n");
        printf("\n");
 
}

void editar(char *arg, char *arg2, char *arg3, int socket){
printf("comando editar\n");
Message ms;
Message mr;
double tempo=0;
struct timeval  ti, tf;
setMessage(&ms,2,'~' ,strlen(arg), 0, 5,arg);
send(socket, &ms, sizeof(ms), 0);
gettimeofday(&ti, NULL);

int inicial=atoi(arg2);
char data[2];
data[0]=inicial;
int seq=0;
removeAspas(arg3);
int num_seq=(strlen(arg3)/15)+1;
unsigned char c=13;
char *buffer= (char *)malloc(sizeof(char)*15);
int i=0;
int j=0;
int s;
recv(socket, &mr, sizeof(mr), 0);

do
{
    recv(socket, &mr, sizeof(mr), 0);
    gettimeofday(&tf, NULL);
    tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
    if (tempo>20)
    {
        printf("TIME OUT \n");
        send(socket,&ms, sizeof(ms), 0); 
    }
} while (mr.add!=1);

   if (mr.type==15)
    {
        if(atoi(mr.data)==1){
            printf("acesso proibido/erro ao acessar arquivo \n");
    }
        if(atoi(mr.data)==2){
            printf("diretorio não existe \n");
        }
        if(atoi(mr.data)==4){
            printf("linha não existe\n");
            if(atoi(mr.data)==4){
                printf("linha não existe\n");
        }
        }
        return;
    }
    else if(mr.type==9){
        printf("NACK \n");
        send(socket,&ms, sizeof(ms), 0);
    }
    else if (mr.type==8)
    {
        printf("ACK NOME \n");
        setMessage(&ms,2,'~' ,strlen(arg2), 0, 10,arg2);
        send(socket, &ms, sizeof(ms), 0);
    }

nack:
    recv(socket, &mr, sizeof(mr), 0);
    gettimeofday(&ti, NULL);

    if(mr.type==9){
        send(socket,&ms, sizeof(ms), 0);
        goto nack;
    }
    else if(mr.type==8){
        printf("ACK LINHA\n");
    }
    else if(mr.type==15){
        printf("linha não existe\n");
    }
    else if(mr.add!=2){
        goto nack;
    }
    gettimeofday(&tf, NULL);
    tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
    if (tempo>20)
    {
        printf("TIME OUT \n");
        send(socket,&ms, sizeof(ms), 0); 
    }

        while(ms.type!=13){
            gettimeofday(&ti, NULL);
            if(mr.add==1 && mr.type==8){
                for(s=0;s<15;s++){
                    buffer[s]='\0';
                }
                for(i=0;i<15;i++){
                    buffer[i]=arg3[i+j];
                }
                j=i+j;
                setMessage(&ms,2, '\0' ,strlen(buffer), seq, 12,buffer);
                send(socket, &ms, sizeof(ms), 0);
                seq++;
            }
            else if(num_seq<=seq){
                setMessage(&ms,2, '\0' ,0, seq, 13,'0');
                send(socket, &ms, sizeof(ms), 0);
                printf("mensagem acabou\n");
            }
            else if (mr.type==9)
            {
               printf("NACK\n");
               send(socket, &ms, sizeof(ms), 0);
            }
            else if (mr.add==1 && mr.type==15)
            {
                if(atoi(mr.data)==4){
                    printf("linha não existe \n");
                }
            return;
            }
            gettimeofday(&tf, NULL);
            tempo = ((double)(tf.tv_usec-ti.tv_usec)/1000000+(double)(tf.tv_sec - ti.tv_sec));
            if (tempo>20)
            {
                printf("TIME OUT \n");
                send(socket,&ms, sizeof(ms), 0); 
            }
        recv(socket, &mr, sizeof(mr), 0);
        recv(socket, &mr, sizeof(mr), 0);
        }
}


void removeAspas(char *arg){
    int i;
    int tam_palavra = strlen(arg);
    for(i = 1; i < tam_palavra; i++)
    {
       arg[i-1]=arg[i];
    }
    tam_palavra--;
    arg[tam_palavra-2] = '\0';
}

#include <sys/types.h>
#include <sys/socket.h>
//"in" per "sockaddr_in"
#include <netinet/in.h>
//"netdb" per "gethostbyname"
#include <netdb.h>
//"fcntl" per la funzione "fcntl"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
   


//questo Ã¨ un file header per l'implementazione di funzioni essenziali al fine di rendere l'utilizzo dei sockets
//facile e veloce





void FXClose(int sock)
{
   close(sock);
   return;
}

int FXNew(char* destination, int port)
{
   struct sockaddr_in temp;
   struct hostent *h;
   int sock;
   int error;
     
   //Tipo di indirizzo
   temp.sin_family=AF_INET;
   temp.sin_port = htons(port);
   h = gethostbyname(destination);
   if(h == 0)
   {
      printf("Gethostbyname failed\n");
   }
   bcopy(h->h_addr,&temp.sin_addr,h->h_length);
   //Creazione socket.
   sock = socket(AF_INET,SOCK_STREAM,0);
   //Connessione del socket. Esaminare errore per compiere azioni
   //opportune in caso di errore.
   error = connect(sock, (struct sockaddr*) &temp, sizeof(temp));
   return sock;
}


int FXPrint(int sock, char* string)
{
   signed short returnVal;
   
   if(write(sock,string,strlen(string))<0)
   {
      returnVal = 0;
   }
   else
   {
      returnVal = -1;
   }
   
   return returnVal;
}











int FXNewServerSocket(int port, int maxConnections)
{
   int sock,error;
   struct sockaddr_in temp;

   //Creazione socket
   sock = socket(AF_INET,SOCK_STREAM,0);
   //Tipo di indirizzo
   temp.sin_family = AF_INET;
   temp.sin_addr.s_addr = INADDR_ANY;
   temp.sin_port = htons(port);

   //Il socket deve essere non bloccante
   error = fcntl(sock,F_SETFL,O_NONBLOCK);

   //Bind del socket
   error = bind(sock,(struct sockaddr*) &temp,sizeof(temp));
   //Per esempio, facciamo accettare fino a 7 richieste di servizio
   //contemporanee (che finiranno nella coda delle connessioni).
   if(maxConnections)
   {
      error = listen(sock,maxConnections);
   }
   
   return sock;
}







int FXServer(int bufferSize, int port, int maxConnections)
{

      char  buffer[bufferSize];//512
      int sock,socket;
      int exitCond = 0;
      int count;
     

      sock = FXNewServerSocket(port, maxConnections);
      //attendo connessioni
      while (!exitCond)
      {
        //Test sul socket: accept non blocca, ma il ciclo while continua
        //l'esecuzione fino all'arrivo di una connessione.
        if ((socket = accept(sock,0,0))!=-1)
        {
          //Lettura dei dati dal socket (messaggio ricevuto)
          if ((count = read(socket,buffer,sizeof(buffer)))<0)
          {
             printf("Error: unable to read recived message.\n");
             FXClose(socket);
          }
          else
          {
             //Aggiusto la lunghezza...
             buffer[count]=0;
             //Elaborazione dati ricevuti
             if (strcmp(buffer,"exit") == 0)
                  exitCond=1;
             else
                return *buffer;
          }
          //Chiusura del socket temporaneo
          FXClose(socket);
        }
      }
      //Chiusura del socket
      FXClose(sock);
      printf("Server: Terminato.\n");

      return 0;
}




 int main()
 {
    printf("initializing\n");
    int s = FXNew("127.0.0.1", 80);
    printf("%d\n", FXPrint(s, "GET /"));
    FXClose(s);
 }

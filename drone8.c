  // Barrak Mandani
  // Lab 8
  #include <string.h>
  #include <stdio.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <stdlib.h>
  #include <arpa/inet.h>
  #include <time.h>
  #include <unistd.h>
  #include <ctype.h>
  #include <math.h>

  #define TTL 5
  #define TTL2 5
  #define destPORT "toPort"
  #define MAXDISTANCE 2
  #define CURRENTVERSION 8
  #define MAXPARTNERS 100

  struct _partnersHost{
    char ipAddress[25];
    int  portNumber;
    int location; 
    int seqNumber;
  };
  struct partners{
    int maxHosts;
    struct _partnersHost hostInfo[MAXPARTNERS];
  };

  // new things for the select
  struct  _tokens {
    char key[100];
    char value [100];
  };

  /*
    compare fromPort, if ttl is not 
    0 dec and resend it

  */
  struct messages{
    int seqNumber;
    int toPort;
    int fromPort;
    int messageTTL;
  };

  fd_set socketFDS; // the socket descriptor set
  int maxSD; // tells the OS how many sockets are set
  int ROWS, COLUMNS;
  int findDistance(int x, int y, int myX, int myY);
  int findXYCoordinates(int squareNumber, int *row, int *columns);
  void Isdigit(char *str);
  void checkPort(int portNumber);
  void recieveData (int sd, char *buffer, struct sockaddr_in *from_address);
  void checkip(char *str);
  int getPort(const char* line);
  int findTokens (char *buffer, struct _tokens *tokens);
  char *find_value(char *str, char *key);
  void clearmsg (char * buffer);
  void forwardSend(char *buffer, int sd, struct partners Partners, struct _tokens *tokens, int numberOfTokens, int myLoc,int myPort);
  void BuildAck(char *buffer, int sd, struct partners Partners, struct _tokens *tokens, int numberOfTokens, int myLoc, int myPort,struct sockaddr_in server_address);
  int sendData(char *buffer, int sd, struct sockaddr_in server_address);
  void printTokens (struct _tokens *tokens, int numberOfTokens, int myLoc);
  void recieveData (int sd, char *buffer, struct sockaddr_in *from_address); 
  int findIntToken (struct _tokens *tokens, int numberOfTokens, char * keyWanted);
  int DEBUG = 0;
  int readFile(struct partners *Partners, int myPortNumber, int *myLoc);
  int ChangeIntToken (struct _tokens *tokens, int numberOfTokens, char *keyWanted, char *key);
  #define STDIN 0



  int sender_location = 0;
  int move_location = 0;
  int sequence_path = 0;
  int to_Port = 0;
  char fromPort_str[100];
  char myport_str[100];
  int fromPort = 0;
  char NewsendPath[100];
  // read data from file
  int readFile(struct partners *Partners, int myPortNumber, int *myLoc){
  // open the file
  FILE *configFile = fopen("config2.file", "r"); // hardcoded name
  char *line = NULL;  // weird how getline() works?
  ssize_t rc = 0;
  size_t bytesRead = 0;
  char ipaddr[25]; // assume IPv4
  char *ptr = &ipaddr[0]; 
  int port;
  int location; 
  // did we open it?
  *myLoc = 0; // have to find me in the file
  if (configFile == NULL){
    printf ("couldn't open the config.file\nMake sure that file exists\n");
    exit (1);
  }

  // logic here; read a line, then loop to tokenize it
  // if i ever change the format of the config file, i would have to
  // redo this section of code.
  // you read the line here first, because it is possible there is
  // nothing in the file!!
  
  rc = getline(&line, &bytesRead, configFile); // prime the pump
  while (rc >= 0 ){
    ptr = strtok(line, " "); // i know there is a space between IP and port

    // in the real world i would check my port number....
    port = atoi(strtok(NULL, " "));
    if (1 == DEBUG){
      printf ("read '%s'\n", line);
      printf ("ipaddr is '%s', port is %d\n", ptr, port);
    }
    // I should probably check IP and port validity here
    
    // if (1==checkip(ptr)){
    //   printf ("bad ip address in config file going to skip this line\n");
    //   printf ("%s\n", line);
    //   exit (1);
    // }
    location = atoi(strtok(NULL, "\n"));
    if (myPortNumber == port){
      *myLoc = location;
    }
    Partners->hostInfo[Partners->maxHosts].portNumber = port;
    Partners->hostInfo[Partners->maxHosts].location = location;
    strcpy (Partners->hostInfo[Partners->maxHosts].ipAddress, ptr);
    Partners->maxHosts ++;
    rc = getline(&line, &bytesRead, configFile); // read the next line
    if (DEBUG == 1)
      printf ("rc is %d\n", (int)rc);
  }
  if (DEBUG == 1)
    printf ("done with config file\n");

  return 0;
}

  int main(int argc, char *argv[])
  {
    int sd; /* socket descriptor */
    int rc; /* return code from recvfrom */
    struct sockaddr_in server_address;
    struct sockaddr_in from_address;
    
    char buffer[1000];
    char buffer_Ack[1000];
    //socklen_t fromLength = sizeof(struct sockaddr);
    struct _tokens tokens [100];
    char buffer2[1000];
    char bufferOut[1000];
    char new_messages[1000];
    // new things for the select
    fd_set socketFDS; // the socket descriptor set
    int maxSD;        // tells the OS how many sockets are set
    int mylocation =0;
    int myport =0;
    int count = 0;
    int flag_ack;
    char temp_seq [20];
    flag_ack = 0;

    struct messages messages_2[200];

    struct partners *Partners;  
    
    struct message
    {
      char name[100];
      char value[100];
    };
    // config struct
    struct configs
    {

      char ipadd[200];
      int port;
      int location;
      int seqNumber;
    
    };

    struct configs entries[200] = {0};

    if (argc < 2)
    {
      printf("usage is: server <portnumber>\n");
      exit(1);
    }

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    // check if port$ is in digits
    Isdigit(argv[1]);

    int portNumber = atoi(argv[1]);
    myport = portNumber;
    sprintf(myport_str, "%d", myport);
    //fromlength = sizeof(struct sockaddr_in);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(portNumber);
    server_address.sin_addr.s_addr = INADDR_ANY;


    for (int i = 0; i < count; i++)
    {
      if (entries[i].port == portNumber)
      {
        mylocation = 0;
        mylocation = entries[i].location;
        // printf("My Location: %d \n", mylocation);
        break;
      }
    }

    rc = bind(sd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (rc < 0)
    {
      perror("bind");
      exit(1);
    }
     Partners = (struct partners *)malloc(sizeof(struct partners));
    if (Partners == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }

    // Initialize the maxHosts field to 0
    Partners->maxHosts = 0;

    readFile(Partners, myport, &mylocation);

    char locationstr[10];
    sprintf(locationstr, "%d", mylocation);

    ROWS = strtol(argv[2], NULL, 10); /* many ways to do this */
    // check that col is a number
    for (int i=0;i<strlen(argv[3]); i++){
      if (!isdigit(argv[3][i])){
        printf ("\nThe column isn't a number!\n");
        exit(1);
      }
    }
    COLUMNS = strtol(argv[3], NULL, 10); /* many ways to do this */
    // call the coordinate function to get the function
    // printf("main row is: %d main col is: %d:", user_row ,user_col);
    // get x and y cordinate for my location
    int user_row, user_col;
    findXYCoordinates( mylocation, &user_row, &user_col);
    printf ("you are in location %d and that is in row %d, column %d\n",
      mylocation, user_row, user_col);
   
    for (int i = 0; i < Partners->maxHosts; i++){
                    entries[i].seqNumber = 1;
                }
    // printf("user row: %d user col: %d mylocation: %d my_X: %d my_y: %d\n", user_col, user_col, mylocation , my_x, my_y);
    void combine(char s1[], char s2[]);

    for (;;)
    {
      struct timeval timeout;
      timeout.tv_sec = 20;
      timeout.tv_usec = 0;
      
      int destinationPort; 
      int rc=0;
      int version = 0;
      int numberOfTokens = 0; // keep track of how many tokens i read in
      int distance = 0;
      int numberOfTokens2 = 0; 

      memset(new_messages, 0, 200);
      memset(buffer, 0, 200);
      memset(bufferOut, 0, 200);
      memset(buffer_Ack,0 ,200);
      // do the select() stuff here

      FD_ZERO(&socketFDS);       // NEW
      FD_SET(sd, &socketFDS);    // NEW - sets the bit for the initial sd socket
      FD_SET(STDIN, &socketFDS); // NEW tell it you will look at STDIN too
      printf("Enter your input:\n");
      if (STDIN > sd)
      { // figure out what the max sd is. biggest number
        maxSD = STDIN;
      }
      else
      {
        maxSD = sd;
        rc = select (maxSD+1, &socketFDS, NULL, NULL, &timeout);
        #ifdef DEBUG
        printf ("select popped rc is %d\n", rc);
        #endif
        if (rc == 0){ // had a timeout!
        sendData(buffer2,sd,server_address);
        numberOfTokens2 = findTokens(buffer2, tokens);
        for (int i=0;i<numberOfTokens2;i++){
        //handle the TTL field first
          if (strcmp(tokens[i].key, "TTL2") == 0){
            int ttl=0;
            ttl = atoi(tokens[i].value);
            ttl = ttl -1;
            if (ttl <=0){
              printf ("message has no more life!\n");
              printf ("timeout!\n");
              continue;
            }
          }
        }
      } // NEW block until something arrives
        
        if (FD_ISSET(STDIN, &socketFDS))
        { // means i received something from the keyboard. --client
          // put the send to in the function
          // loop and send to the destined port# and ip address
          // for(config1)

          // DMOwhile(1){
          
          
          memset (buffer, '\0', 200);
          memset (buffer2, '\0', 300);
          
         fgets(buffer, sizeof(buffer), stdin);

          // find out the destanation
          to_Port = getPort(buffer);

          buffer[strlen(buffer) - 1] = 0; // get rid of \n that is there, cuz i don't want it
          sprintf (buffer2, "%s location:%d fromPort:%d TTL:%d",
          buffer, mylocation, myport, TTL);

          //  for (i=0;i<Partners.maxHosts;i++){
              // int portNumber;
              // char serverIP[20];
              // memset (serverIP, 0, 20); // it is C after all...
              // portNumber = Partners.hostInfo[i].portNumber;
          // printf ("\n read from the keyboard '%s': \n ", buffer);
          for (int i = 0; i < Partners->maxHosts; i++){
                    int seqtemp = entries[i].seqNumber;
                    if (Partners->hostInfo[i].portNumber == to_Port){
                        combine(buffer2, " seqNumber:");
                        sprintf(temp_seq, "%d", Partners->hostInfo[i].seqNumber);
                        combine(buffer2, temp_seq);
                        Partners->hostInfo[i].seqNumber = seqtemp + 1;
                    }
                }
          printf("This is buffer 2: %s\n",buffer2);
          //current_seq = seqNumber;
          flag_ack = 0;
          sendData(buffer2,sd,server_address);
          to_Port =0;
        }
      }
      if (FD_ISSET(sd, &socketFDS))
      { 
        /*******************************************************************/
        /*                                                                 */
        /*              Attention this is the receive side                 */                                   
        /*                                                                 */
        /*******************************************************************/
    
        // recieve data
        recieveData(sd, buffer2, &from_address);
        //printf("recieve side: %s\n",buffer2);
        // clearmsg function
        clearmsg (buffer2);
        // find how many tokens there are
        numberOfTokens = findTokens(buffer2, tokens);
        // find the destanation port
        destinationPort = findIntToken(tokens, numberOfTokens, destPORT);
        // check the the version of the message
        version = findIntToken(tokens, numberOfTokens, "version");
        //printf("\n this is version %d \n ",version);
        if (version != CURRENTVERSION){
          printf("Error !! wrong version %d \n", version);
          continue;
        }
        
        
        // find sender location and check it
        sender_location = findIntToken(tokens, numberOfTokens, "location");
        //printf("Test checking the message meow: %d\n", sender_location);
        if (sender_location == -1){
          printf("Error location wasn't found %d \n", sender_location);
          continue;
        }
        // move_location = findIntToken(tokens, numberOfTokens, "move");
        //printf("This is the new location %d \n", move_location);
        //find the fromPort
        fromPort = findIntToken(tokens, numberOfTokens, "fromPort");
        sprintf(fromPort_str, "%d", fromPort);

        int sender_row, sender_column;
        // find the coordinates
        // check if location within the grid
        rc = findXYCoordinates(sender_location, &sender_row, &sender_column);
        if (rc == -1){
          printf("Location is not in the grid \n");
        }
      
      // find reciever distance
      distance = findDistance(sender_row, sender_column,user_row, user_col );
      //printf("Distance is %d ",distance);
      // check if destPort == myPort 
      if (destinationPort == myport){
        if( distance > MAXDISTANCE){ // out of range
          printf("That message isn't for me ");
        }
        else{// print tokens if distance is <= 2
          printTokens(tokens, numberOfTokens, mylocation);
          move_location = findIntToken(tokens, numberOfTokens, "move");
        //printf("Test checking the message meow: %d\n", sender_location);
          if (move_location == -1){
            printf("Error location wasn't found %d \n", sender_location);
            
          }else{
            mylocation = findIntToken(tokens, numberOfTokens, "move");
          }
         
          //printf("The new location after move command is %d\n", mylocation);
          if( flag_ack == 0){
            BuildAck(buffer_Ack,sd,*Partners, tokens,numberOfTokens, mylocation, myport, server_address);
            flag_ack = 1;
            memset(buffer2,0,200);
            strcpy(buffer2,buffer_Ack);
            //printf("this is the buffer: me %s ",buffer);
            sendData(buffer2,sd,server_address);
            
          }else{
            
            printf("Duplicate Ack\n;");
          }
        }
      }else{
        int ttl;
        /*
        / find ttl if it's not 0 forward
        */
        ttl = findIntToken(tokens, numberOfTokens,"TTL");
        if (TTL == 0){
          printf("ttl is 0 discard the message");
          continue;
        }
        
        // if myport = fromPort from the message, don't append port number
        if(strcmp(myport_str,fromPort_str) !=0 ){
          sprintf(NewsendPath,"%d",myport);
          //printf("This is the new send path %s\n", NewsendPath);
          ChangeIntToken(tokens, numberOfTokens, "send-path" ,NewsendPath);
        }
        forwardSend(buffer2, sd, *Partners, tokens,numberOfTokens, mylocation, myport );
        
      }
    }
  } // for;;
  return 0;
  } // main

  // FUNCTION IMPLEMENTATION
  void Isdigit(char *str)
  {
    int i;
    for (i = 0; i < strlen(str); i++)
    {
      if (!isdigit(str[i]))
      {
        printf("The Portnumber isn't a number!\n");
        exit(1);
      }
    }
  }

  void checkPort(int portNumber)
  {
    if ((portNumber > 65535) || (portNumber < 0))
    {
      printf("you entered an invalid port number\n");
      exit(1);
    }
  }

  // check ip
  void checkip(char *str)
  {
    struct sockaddr_in inaddr;
    if (!inet_pton(AF_INET, str, &inaddr))
    {
      printf("error, bad ip address\n");
      exit(1);
    }
  }

  int findDistance(int x, int y, int myX, int myY){
    int distance;
    distance = trunc(sqrt(pow((x-myX),2) + pow((y-myY),2)));
    return distance;
  }



  int findXYCoordinates(int squareNumber, int *row, int *column){

    *row = (squareNumber-1) / COLUMNS +1;
    *column = (squareNumber-1)%COLUMNS+1;
    
    if (*row > ROWS || *column > COLUMNS){
      printf ("your squareNumber is from outside the boundary\n");
      return (-1);
    }
    else{
      //printf ("your row/column is %d/%d\n",*row, *column);
    }
    return (1);
        
  }


  void recieveData (int sd, char *buffer, struct sockaddr_in *from_address){
    int flags = 0; // using 0 since I don't need anything special 
    int rc = 0;

    /* this next variable is MANDATORY and must be set on linux systems */
    socklen_t fromLength = sizeof (struct sockaddr); // used in recvfrom
  
    memset (buffer, 0, 1000); // zero out the buffer
    //printf("this is the buffer %s ", buffer);
    rc = recvfrom(sd, buffer, 1000, flags,
      (struct sockaddr *)from_address, &fromLength);

    /* check for any possible errors */
    // if (rc <=0){
    //   perror ("recvfrom");
    //   printf ("leaving, due to socket error on recvfrom\n");
    //   exit (1);
    // }

  }

  // find the message and encode it 
  //also it won't tokenize it wrong
  void clearmsg (char * buffer){

    char *startPtr;
    int count = 0;
    int i;
    startPtr = strstr (buffer, "\"");

    /* replace the ' ' in the string with & so tokenize will work */
    // replace the space with carrrat
    if (startPtr != NULL){
      for(i=0; i<strlen(startPtr) && count < 2 ;i++){
        if (startPtr[i] == '"'){
    count ++;
        }
        else if (startPtr[i] == ' '){
    startPtr[i] = '^';
        }
      }
    }
  }

  //print the tokens
  void printTokens (struct _tokens *tokens, int numberOfTokens, int myLoc){
    int i; // loop index

    printf ("****************************************************\n");
    printf ("%20s %20s\n", "Name", "Value");
    for (i=0;i<numberOfTokens;i++){
      printf ("%20s %20s\n", tokens[i].key, tokens[i].value);
    }
    printf ("%20s %20d\n", "myLocation", myLoc);
    printf ("****************************************************\n");

  }

  // function to send stuff
  int sendData(char *buffer, int sd, struct sockaddr_in server_address){

    
    //printf ("sending '%s'\n", buffer);
    sendto(sd, buffer, strlen(buffer), 0,
          (struct sockaddr *) &server_address, sizeof(server_address));
    //  printf ("I think i sent %d bytes \n", rc);

    return (0); 
  }

  // this function to forward the message again
  void forwardSend(char *buffer, int sd, struct partners Partners, struct _tokens *tokens, int numberOfTokens,int myLoc, int myPort){

    int i;
    struct sockaddr_in server_address; 
    printf ("I am forwarding\n");
    char tempBuffer[100];
    memset(buffer, 0, 500); // DMO bad should have #define for sizes
    for (i=0;i<numberOfTokens;i++){
      //handle the TTL field first
      if (strcmp(tokens[i].key, "TTL") == 0){
        int ttl=0;
        ttl = atoi(tokens[i].value);
        ttl = ttl -1;
        if (ttl <=0){
    printf ("message has no more life!\n");
    return;
        }
        sprintf (tokens[i].value,"%d", ttl);
      }

      // handle changing the location first

      if (strcmp(tokens[i].key, "location") == 0){
        sprintf (tokens[i].value, "%d", myLoc);
      }
      
      sprintf (tempBuffer, "%s:%s%s", tokens[i].key, tokens[i].value, " ");
      strcat(buffer, tempBuffer);
      memset(tempBuffer, 0, 100);
    }
    // get rid of the last ' ' in the message
    buffer[strlen(buffer)-1] = 0;
    // printf("will forward message '%s'\n", buffer);

    // I want to send this to each partner. Probably don't need the command line port/IP
    for (i=0;i<Partners.maxHosts;i++){
      int portNumber;
      char serverIP[20];
      memset (serverIP, 0, 20); // it is C after all...
      portNumber = Partners.hostInfo[i].portNumber;
      // don't forward to myself
      if (portNumber == myPort){
        continue;
      }
      strcpy(serverIP, Partners.hostInfo[i].ipAddress);
        
      server_address.sin_family = AF_INET;
      server_address.sin_port = htons(portNumber);
      server_address.sin_addr.s_addr = inet_addr(serverIP);      
      sendData(buffer, sd, server_address);
    }    

  } // end of clientSend

  int findTokens(char *buffer , struct _tokens *tokens){
    
    int counter= 0; // how many tokens did i find!
    char * ptr;

    /* recall the protocol says the key:value are separated by ' ' */
    /* so i will tokenize first on the ' '                         */

    
    ptr = strtok (buffer," "); 
    while (ptr != NULL) // keep going until no more ' ' 
      {
        memset (tokens[counter].key, 0, 100);
        memset (tokens[counter].value, 0, 100);

        int i = 0; // KMT would be laughing
        int flag = 0;
        int k = 0; 
        for (i=0;i<strlen(ptr);i++){
    if (flag ==0){ // doing the key portion
      if (ptr[i] != ':')
        tokens[counter].key[i] = ptr[i];
      else // must be a ':'
        flag = 1;
    }
    else{ // doing the value portion
      if (ptr[i] == '^'){ // undoing the cleansing of msg portion
        ptr[i] = ' ';
      }
      tokens[counter].value[k] = ptr[i];
      k++;
    }
        }
        //      sprintf (tokens[counter].key, "%s", ptr); // fix here. late night
        ptr = strtok (NULL, " ");
        counter ++;
      }
    return counter;
  }

  int findIntToken (struct _tokens *tokens, int numberOfTokens, char *keyWanted){
    int i; // loop index


    for (i=0;i<numberOfTokens;i++){
      if (!strcmp(tokens [i].key, keyWanted))
        return (atoi(tokens[i].value)); // means i found the port
    }
    return -1; 

  }

  int ChangeIntToken (struct _tokens *tokens, int numberOfTokens, char *keyWanted, char *key){
    int i; // loop index


    for (i=0;i<numberOfTokens;i++){
      if (!strcmp(tokens [i].key, keyWanted)){
        strcat(tokens[i].value,",");
        strcat(tokens[i].value, key);
      }
    }
    return -1; 

  }
void combine(char s1[], char s2[]){
    int i = 0;
    int j = 0;
    //loops through to find the end of str 1
    while( s1[i]!='\0'){
        i++;
    }

    //appends str 2 to the end of str 1 and then sets the final null terminator
      while( s2[j]!='\0'){
          s1[i] = s2[j];
          i++;
          j++;
      }
      s1[i] = '\0';
}
//Build ack and send it
  void BuildAck(char *buffer, int sd, struct partners Partners, struct _tokens *tokens, int numberOfTokens, int myLoc, int myport,struct sockaddr_in server_address ){
      memset(buffer,0,200);
      int i = 0; // loop index
      // to change new ports
      char new_toPort[100];
      char send_type[10] = "Ack";

      for (i=0;i<numberOfTokens;i++){
          if (!strcmp(tokens [i].key, "fromPort") != 0){
          strcpy(new_toPort,(tokens[i].value)); 
          }// replace toPort
      }
      //printf("newprot: %s \n",new_toPort);

      // change from port to my port
      for (i=0;i<numberOfTokens;i++){
      //   if(strcmp(tokens[i].key, "version")){
      //     sprintf(tokens[i].value,"%d", CURRENTVERSION); // Corrected here, added %d
      //   }
        if(strcmp(tokens[i].key, "send-path") ==0 ){
          sprintf(tokens[i].value,"%d", myport); // Corrected here, added %d
        }
    
      //printTokens(tokens,numberOfTokens,myLoc);
        if (!strcmp(tokens [i].key, "type") ){
          strcpy(tokens[i].value, send_type) ;
          }
        if(!strcmp(tokens[i].key, "myLocation")){
          sprintf(tokens[i].value,"%d",myLoc);
          }
        if (!strcmp(tokens [i].key, "toPort") !=0){
          strcpy(tokens[i].value, new_toPort) ;
        }
        if (!strcmp(tokens [i].key, "fromPort")){
          sprintf(tokens[i].value,"%d", myport); 
          }// replace toPort
        if (!strcmp(tokens [i].key, "TTL")){
          sprintf(tokens[i].value,"%d", TTL); 
          }// replac
      }
      //printTokens(tokens,numberOfTokens,myLoc);
       
      for (int j = 0; j < numberOfTokens; j++){
         combine(buffer, tokens[j].key);
         combine(buffer, ":");
         combine(buffer, tokens[j].value);
         combine(buffer, " ");
         
        }

        //printf("IS TEST: %s ", buffer);

      // if myport = fromPort from the message, don't append port number
      if(strcmp(myport_str,fromPort_str) !=0 ){
          sprintf(NewsendPath,"%d",myport);
          //printf("This is the new send path %s\n", NewsendPath);
          ChangeIntToken(tokens, numberOfTokens, "send-path" ,NewsendPath);
      }
      
      for (i=0;i<numberOfTokens;i++){
      //handle the TTL field first
        if (strcmp(tokens[i].key, "TTL") == 0){
          int ttl=0;
          ttl = atoi(tokens[i].value);
          ttl = ttl -1;
          if (ttl <=0){
      printf ("message has no more life!\n");
      return;
      sendData(buffer,sd,server_address);
      // //clearmsg (buffer);
      //printTokens(tokens,numberOfTokens,myLoc);    
   
          }
        }
      }
  }

  int getPort(const char* line){
    //passing the message and extract toPort number to use it to increment the seqNumber
    const char* toPort_ptr = strstr(line,"toPort:");
    if (toPort_ptr != NULL) {
        toPort_ptr += strlen("toPort:");
        int toPort = 0;
        while (*toPort_ptr != ' ' && *toPort_ptr != '\0') {
            if (*toPort_ptr >= '0' && *toPort_ptr <= '9') {
                toPort = toPort * 10 + (*toPort_ptr - '0');
            }
            toPort_ptr++;
        }
        return toPort;
    } else {
        return -1;
    }
  }

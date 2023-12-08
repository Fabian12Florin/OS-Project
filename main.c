#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <math.h>

#define BUFFSIZE 512

void checkNrOfArguments(int number0fArguments){
  if(number0fArguments != 4){
    perror("You must provide only two arguments!");
    exit(10);
  }
}

struct stat getFileInfo(char* filePath, struct stat file_info){
  if(lstat(filePath, &file_info) != 0){
    perror("Can't get info about the provided file!!");
    exit(11);
  }
  return file_info;
}

int isBMP(int fin){
  off_t offset = 0;
  char tmp[5];
  lseek(fin, offset, SEEK_SET);
  read(fin, &tmp, sizeof(uint32_t));
  if(tmp[0] == 'B' && tmp[1] == 'M')
    return 1;
    else return 0;
}

int getFileType(mode_t sMode, int fin){
  if(isBMP(fin)){
    return 1;
  }
  else if(S_ISDIR(sMode)){
    return 2;
  }
  else if(S_ISLNK(sMode)){
    return 3;
  }
  else if(S_ISREG(sMode)){
    return 0;
  }
  return 4;
}


int tryToOpenFile(char* filePath){
  int fin;
  if((fin = open(filePath, O_RDONLY)) < 0){
    perror("Input file can not be opened!");
    exit(13);
  }
  return fin;
}

int tryToOpenOutputFile(char* fileName){
  int fout;
  char buffer[BUFFSIZE];
  sprintf(buffer, "%s", fileName);

  if((fout = open(buffer, O_WRONLY | O_TRUNC | O_CREAT,S_IRWXU)) == -1 ){
    perror("Error at creating the output file");
    exit(14);
  }
  return fout;
}

uint32_t getWidthOfBMPFile(int fin){
  off_t offset = 18;
  uint32_t width;
  lseek(fin, offset, SEEK_SET);
  read(fin, &width, sizeof(uint32_t));
  return width;
}

uint32_t getHeigthOfBMPFile(int fin){
  off_t offset = 22;
  uint32_t heigth;
  lseek(fin, offset, SEEK_SET);
  read(fin, &heigth, sizeof(uint32_t));
  return heigth;
}

uint32_t getSizeOfFile(struct stat file_info){
  off_t size = file_info.st_size;
  return size;
}

void printToX(int fout, char* buffer, int r){
   if((write(fout, buffer, r))<0){
    perror("Can not write in file");
    exit(15);
  }
}

void printLastModifiedTime(struct stat file_info, int fout){
  char time_str[20], buffer[BUFFSIZE];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&file_info.st_mtime));
  sprintf(buffer, "timpul ultimei modificari: %s\n", time_str);

  printToX(fout, buffer, strlen(buffer));
}

void printHeigthAndWidth(int fin, int fout){
  uint32_t heigth, width;
  char buffer[BUFFSIZE];
  width = getWidthOfBMPFile(fin);
  heigth = getHeigthOfBMPFile(fin);

  sprintf(buffer, "inaltime: %d\n", heigth);
  sprintf(buffer + strlen(buffer), "lungime: %d\n", width);
  
  printToX(fout, buffer, strlen(buffer));
}

void printSizeOfFile(int fout, struct stat file_info){
  uint32_t size;
  char buffer[BUFFSIZE];
  size = getSizeOfFile(file_info);

  sprintf(buffer, "dimensiune: %d\n", size);

  printToX(fout, buffer, strlen(buffer));
}

void printUserID(struct stat file_info, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "identificatorul utilizatorului: %ld\n", file_info.st_dev);

  printToX(fout, buffer, strlen(buffer));
}

void printLinksCount(struct stat file_info, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "contorul de legaturi: %ld\n", file_info.st_nlink);

  printToX(fout, buffer, strlen(buffer));
}

void printUserPermissions(mode_t sMode, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "drepturi de acces user: ");
  sprintf(buffer + strlen(buffer), (sMode & S_IRUSR) ? "r" : "-");
  sprintf(buffer + strlen(buffer), (sMode & S_IWUSR) ? "w" : "-");
  sprintf(buffer + strlen(buffer), "%s \n", (sMode & S_IXUSR) ? "x" : "-");

  printToX(fout, buffer, strlen(buffer));
}

void printGroupPermissions(mode_t sMode, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "drepturi de acces grup: ");
  sprintf(buffer + strlen(buffer), (sMode & S_IRGRP) ? "r" : "-");
  sprintf(buffer + strlen(buffer), (sMode & S_IWGRP) ? "w" : "-");
  sprintf(buffer + strlen(buffer), "%s \n", (sMode & S_IXGRP) ? "x" : "-");

  printToX(fout, buffer, strlen(buffer));
}

void printOthersPermissions(mode_t sMode, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "drepturi de acces altii: ");
  sprintf(buffer + strlen(buffer), (sMode & S_IROTH) ? "r" : "-");
  sprintf(buffer + strlen(buffer), (sMode & S_IWOTH) ? "w" : "-");
  sprintf(buffer + strlen(buffer), "%s \n", (sMode & S_IXOTH) ? "x" : "-");

  printToX(fout, buffer, strlen(buffer));
}

void printFileName(char* filePath, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "nume fisier: %s\n", filePath);

  printToX(fout, buffer, strlen(buffer));
}

void printBMPInfo(char* filePath, int fin, struct stat file_info, mode_t sMode, int fout){
  printFileName(filePath, fout);
  printHeigthAndWidth(fin, fout);
  printSizeOfFile(fout, file_info);
  printUserID(file_info, fout);
  printLastModifiedTime(file_info, fout);
  printLinksCount(file_info, fout);
  printUserPermissions(sMode, fout);
  printGroupPermissions(sMode, fout);
  printOthersPermissions(sMode, fout);
}

void printRegFileInfo(char* filePath, struct stat file_info, mode_t sMode, int fout){
  printFileName(filePath, fout);
  printSizeOfFile(fout, file_info);
  printUserID(file_info, fout);
  printLastModifiedTime(file_info, fout);
  printLinksCount(file_info, fout);
  printUserPermissions(sMode, fout);
  printGroupPermissions(sMode, fout);
  printOthersPermissions(sMode, fout);
}

void printDirInfo(char* filePath, struct stat file_info, mode_t sMode, int fout){
  printFileName(filePath, fout);
  printUserID(file_info, fout);
  printUserPermissions(sMode, fout);
  printGroupPermissions(sMode, fout);
  printOthersPermissions(sMode, fout);
}

void printSymbLinkInfo(char* filePath, struct stat file_info, mode_t sMode, int fout){
  printFileName(filePath, fout);
   struct stat tmp;
  if(stat(filePath, &tmp) != 0){
    perror("Can't get info about the provided file");
    exit(11);
  }
  printSizeOfFile(fout, file_info);
  char buffer[BUFFSIZE];
  sprintf(buffer, "Fisier target: ");
  printToX(fout, buffer, strlen(buffer));
  printSizeOfFile(fout, tmp);
  printUserPermissions(sMode, fout);
  printGroupPermissions(sMode, fout);
  printOthersPermissions(sMode, fout);
}

void duplicateBMP(int fin, char* filePath){
  int fout, r;
  fout = tryToOpenOutputFile(filePath);
  char buffer[BUFFSIZE];
  lseek(fin, 0, SEEK_SET);
  while((r = read(fin, buffer, 8)) > 0){
    printToX(fout, buffer, r);
  }
  close(fout);
}

void convertToGrayscaleOver8(int fin){
  off_t offset = 10;
  uint32_t bytesBeforeRasterData;
  char buffer[BUFFSIZE];

  lseek(fin, offset, SEEK_SET);
  read(fin, &bytesBeforeRasterData, sizeof(uint32_t));
  lseek(fin, bytesBeforeRasterData, SEEK_SET);

  while(read(fin, buffer, 3) > 0){
    int red = buffer[0];
    int green = buffer[1];
    int blue = buffer[2];

    char gray = (char)(0.299 * red + 0.587 * green + 0.114 * blue);

    buffer[0] = buffer[1] = buffer[2] = gray;

    lseek(fin, -3, SEEK_CUR);
    if((write(fin, buffer, 3))<0){
      perror("Can not write in file");
      exit(15);
    }
  }
}

void convertToGrayscaleUnder8(int fin, int bitCount){
  off_t offset = 54;
  char buffer[BUFFSIZE];
  int i=0;

  lseek(fin, offset, SEEK_SET);

  while(read(fin, buffer, 4) > 0 && i < pow(2, bitCount)){
    int red = buffer[0];
    int green = buffer[1];
    int blue = buffer[2];

    char gray = (char)(0.299 * red + 0.587 * green + 0.114 * blue);

    buffer[0] = buffer[1] = buffer[2] = gray;

    lseek(fin, -4, SEEK_CUR);
    if((write(fin, buffer, 3))<0){
      perror("Can not write in file");
      exit(15);
    }
    lseek(fin, 1, SEEK_CUR);
  }
}

void convertToGrayscale(char* filePath){
  off_t offset = 28;
  uint16_t bitCount;

  int fin;
  if((fin = open(filePath, O_RDWR)) < 0){
    perror("Input file can not be opened!");
    exit(13);
  }

  lseek(fin, offset, SEEK_SET);
  read(fin, &bitCount, sizeof(uint16_t));
  if(bitCount > 8){
    convertToGrayscaleOver8(fin);
  }
  else{
    convertToGrayscaleUnder8(fin, bitCount);
  }

  close(fin);
  printf("BMP file converted to grayscale successfully.\n");
}

void extractFilePath(char* filePath, char* buffer){
  for(int i=0; i<strlen(filePath); i++){
    if(filePath[i] == '/'){
      for(int j=0; j<strlen(filePath)-i; j++){
        buffer[j] = filePath[j+i+1];
      }
    }
  }
}

DIR *tryToOpenDir(char* dirPath){
  DIR *tmp;
  if((tmp = opendir(dirPath)) == NULL){
    perror("Directory path does not exist!");
    exit(20);
  }
  return tmp;
}

char *readFile(const char *filename) {
    int fileDescriptor = open(filename, O_RDONLY);
    if (fileDescriptor == -1) {
        perror("Error opening file");
        return NULL;
    }

    // Determine the file size
    off_t fileSize = lseek(fileDescriptor, 0, SEEK_END);
    lseek(fileDescriptor, 0, SEEK_SET);  // Rewind to the beginning

    // Allocate memory for the file content
    char *content = (char *)malloc(fileSize + 1);  // +1 for the null terminator
    if (content == NULL) {
        perror("Error allocating memory for file content");
        close(fileDescriptor);
        return NULL;
    }

    // Read the file content
    ssize_t bytesRead = read(fileDescriptor, content, fileSize);
    if (bytesRead != fileSize) {
        perror("Error reading file");
        free(content);
        close(fileDescriptor);
        return NULL;
    }

    // Null-terminate the content
    content[fileSize] = '\0';

    close(fileDescriptor);
    return content;
}

void crossDir(DIR* dir_path, char *dir_name, char* dirOut_path, char* character){
  struct dirent *dir_entry;
  struct stat file_info;
  mode_t sMode;
  int pid, pid2, numberOfLinesWritten=0, fin, fout, sum=0;

  while((dir_entry = readdir(dir_path)) != NULL){
    char entry_path[BUFFSIZE];
    char outputName[BUFFSIZE];
    sprintf(entry_path, "%s/%s", dir_name, dir_entry->d_name);
    fin = tryToOpenFile(entry_path);
    sprintf(outputName, "%s/%s_statistica.txt",dirOut_path, dir_entry->d_name);
    fout = tryToOpenOutputFile(outputName);

    //get type of file
    file_info = getFileInfo(entry_path, file_info);
    sMode = file_info.st_mode;
    int type = getFileType(sMode, fin);

    if(type == 1){
      if((pid = fork()) < 0){
        perror("Oops, it looks like we can't create the process!");
        exit(errno);
      }
      if(pid == 0){
        //this process will make the statistics file
        numberOfLinesWritten = 10;
        printBMPInfo(entry_path, fin, file_info, sMode, fout);
        exit(numberOfLinesWritten);
      }
      else{
        if((pid2 = fork()) < 0){
          perror("Oops, it looks like we can't create the process!");
          exit(errno);
        }
        if(pid2 == 0){
          //this is the second process and will make a duplicate of the BMP and then convert it to grayscale 
          char buffer1[2*BUFFSIZE];
          sprintf(buffer1, "%s/%s_duplicate.bmp",dirOut_path, dir_entry->d_name);
          int fin2 = tryToOpenFile(entry_path);
          duplicateBMP(fin2, buffer1);
          convertToGrayscale(buffer1);
          exit(0);
        }
      }
    }

    else if (type == 0) {
    int son1_to_son2[2], son2_to_parent[2];

    // Create pipes
    if (pipe(son1_to_son2) == -1 || pipe(son2_to_parent) == -1) {
        perror("Error creating pipes");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) < 0) {
        perror("Error forking son1 process");
        exit(errno);
    }

    if (pid == 0) {
        // Code for son1
        // Close unused ends of pipes
        close(son1_to_son2[0]);
        close(son2_to_parent[0]);
        close(son2_to_parent[1]);

        numberOfLinesWritten = 8;
        printRegFileInfo(entry_path, file_info, sMode, fout);

        char *fileContent;
        fileContent = readFile(entry_path);

        write(son1_to_son2[1], fileContent, strlen(fileContent) + 1);
        free(fileContent);
        // Close the write end of the pipe_son1_to_son2
        close(son1_to_son2[1]);

        exit(numberOfLinesWritten);
    } else {
        if ((pid2 = fork()) < 0) {
            perror("Error forking son2 process");
            exit(errno);
        }

        if (pid2 == 0) {
            // Code for son2
            // Close unused ends of pipes
            close(son1_to_son2[1]);
            close(son2_to_parent[0]);

            // Redirect stdin to read from the pipe
            dup2(son1_to_son2[0], 0);
            close(son1_to_son2[0]);

            // Redirect stdout to write to the pipe
            dup2(son2_to_parent[1], 1);
            close(son2_to_parent[1]);

            execlp("/home/faby/OSProject/OS-Project/checkLine.sh", "/home/faby/OSProject/OS-Project/checkLine.sh", character, NULL);
            perror("Error executing checkLine.sh script");
            exit(-1);
        }

        // Close unused ends of pipes
        close(son1_to_son2[0]);
        close(son1_to_son2[1]);
        close(son2_to_parent[1]);

        // Read data from son2 through the pipe
        char buffer_parent[256];
        int number_from_buffer;
        int n = read(son2_to_parent[0], buffer_parent, sizeof(buffer_parent));
        buffer_parent[n] = '\0';
        number_from_buffer = atoi(buffer_parent);
        // Close the read end of the pipe_son2_to_parent
        close(son2_to_parent[0]);

        printf("Parent process has finished.\n");
        sum = sum + number_from_buffer;
        //printf("Au fost identificate in total %d propozitii corecte care contin caracterul %c\n", number_from_buffer, 'c');

    }

    close(fout);
    close(fin);
}

    else if(type == 2){
      if((pid = fork()) < 0){
        perror("Ops, it looks like we can't create the process!");
        exit(errno);
      }
      if(pid == 0){
        numberOfLinesWritten = 5;
        printDirInfo(entry_path, file_info, sMode, fout);
        exit(numberOfLinesWritten);
      }
    }
    else if(type == 3){
      if((pid = fork()) < 0){
        perror("Ops, it looks like we can't create the process!");
        exit(errno);
      }
      if(pid == 0){
        numberOfLinesWritten = 6;
        printSymbLinkInfo(entry_path, file_info, sMode, fout);
        exit(numberOfLinesWritten);
      }
    }
    else{};

    close(fout);
    close(fin);    
  } 

  int status;
  int fiu;
  while((fiu = wait(&status)) > 0){
    if(WIFEXITED(status)){
      printf("S-a incheiat procesul cu pid-ul %d si codul %d\n", fiu, WEXITSTATUS(status));
    }
  }

   printf("Au fost identificate in total %d propozitii corecte care contin caracterul %s\n", sum, character);
  closedir(dir_path);
}


int main(int argc, char* argv[]){

checkNrOfArguments(argc);

DIR *dir_path;
dir_path = tryToOpenDir(argv[1]);
tryToOpenDir(argv[2]);
crossDir(dir_path, argv[1], argv[2], argv[3]);

return 0;
}

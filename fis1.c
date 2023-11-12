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

#define BUFFSIZE 512

void checkNrOfArguments(int number0fArguments){
  if(number0fArguments != 2){
    perror("You must provide only one argument!");
    exit(10);
  }
}

struct stat getFileInfo(char* filePath, struct stat file_info){
  if(stat(filePath, &file_info) != 0){
    perror("Can't get info about the provided file");
    exit(11);
  }

  return file_info;
}

void checkIfReg(mode_t sMode){
  if(S_ISREG(sMode) == 0){
    perror("This is not a regular file");
    exit(12);
  }
}

int tryToOpenFile(char* filePath){
  int fin;
  if((fin = open(filePath, O_RDONLY)) < 0){
    perror("Input file can not be opened!");
    exit(13);
  }
  return fin;
}

int tryToCreateFile(char* fileName){
  int fout;
  if((fout = open("statistica.txt", O_WRONLY | O_TRUNC | O_CREAT,S_IRWXU)) == -1 ){
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
  off_t offset = 20;
  uint32_t heigth;
  lseek(fin, offset, SEEK_SET);
  read(fin, &heigth, sizeof(uint32_t));
  return heigth;
}

uint32_t getSizeOfFile(int fin){
  off_t offset = 2;
  uint32_t size;
  lseek(fin, offset, SEEK_SET);
  read(fin, &size, sizeof(uint32_t));
  return size;
}

void printToX(int fout, char* buffer){
   if((write(fout, buffer, strlen(buffer)))<0){
    perror("Can not write in file");
    exit(10);
  }
}

void printLastModifiedTime(struct stat file_info, int fout){
  char time_str[20], buffer[BUFFSIZE];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&file_info.st_mtime));
  sprintf(buffer, "timpul ultimei modificari: %s\n", time_str);

  printToX(fout, buffer);
}

void printHeigthAndWidth(int fin, int fout){
  uint32_t heigth, width;
  char buffer[BUFFSIZE];
  width = getWidthOfBMPFile(fin);
  heigth = getHeigthOfBMPFile(fin);

  sprintf(buffer, "inaltime: %d\n", heigth);
  sprintf(buffer + strlen(buffer), "lungime: %d\n", width);
  
  printToX(fout, buffer);
}

void printSizeOfFile(int fin, int fout){
  uint32_t size;
  char buffer[BUFFSIZE];
  size = getSizeOfFile(fin);

  sprintf(buffer, "dimensiune: %d\n", size);

  printToX(fout, buffer);
}

void printUserID(struct stat file_info, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "identificatorul utilizatorului: %ld\n", file_info.st_dev);

  printToX(fout, buffer);
}

void printLinksCount(struct stat file_info, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "contorul de legaturi: %ld\n", file_info.st_nlink);

  printToX(fout, buffer);
}

void printUserPermissions(mode_t sMode, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "drepturi de acces user: ");
  sprintf(buffer + strlen(buffer), (sMode & S_IRUSR) ? "r" : "-");
  sprintf(buffer + strlen(buffer), (sMode & S_IWUSR) ? "w" : "-");
  sprintf(buffer + strlen(buffer), "%s \n", (sMode & S_IXUSR) ? "x" : "-");

  printToX(fout, buffer);
}

void printGroupPermissions(mode_t sMode, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "drepturi de acces grup: ");
  sprintf(buffer + strlen(buffer), (sMode & S_IRGRP) ? "r" : "-");
  sprintf(buffer + strlen(buffer), (sMode & S_IWGRP) ? "w" : "-");
  sprintf(buffer + strlen(buffer), "%s \n", (sMode & S_IXGRP) ? "x" : "-");

  printToX(fout, buffer);
}

void printOthersPermissions(mode_t sMode, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "drepturi de acces altii: ");
  sprintf(buffer + strlen(buffer), (sMode & S_IROTH) ? "r" : "-");
  sprintf(buffer + strlen(buffer), (sMode & S_IWOTH) ? "w" : "-");
  sprintf(buffer + strlen(buffer), "%s \n", (sMode & S_IXOTH) ? "x" : "-");

  printToX(fout, buffer);
}

void printFileName(char* pathFile, int fout){
  char buffer[BUFFSIZE];
  sprintf(buffer, "Nume fisier: %s\n", pathFile);

  printToX(fout, buffer);
}

int main(int argc, char* argv[]){

  int fin, fout;
  struct stat file_info;
  mode_t sMode;

  checkNrOfArguments(argc);

  file_info = getFileInfo(argv[1], file_info);
  sMode = file_info.st_mode;

  checkIfReg(sMode);

  fin = tryToOpenFile(argv[1]);
  fout = tryToCreateFile("statistica.txt");

  printFileName(argv[1], fout);
  printHeigthAndWidth(fin, fout);
  printSizeOfFile(fin, fout);
  printUserID(file_info, fout);
  printLastModifiedTime(file_info, fout);
  printLinksCount(file_info, fout);
  printUserPermissions(sMode, fout);
  printGroupPermissions(sMode, fout);
  printOthersPermissions(sMode, fout);

  close(fin);
  close(fout);

  return 0;
}

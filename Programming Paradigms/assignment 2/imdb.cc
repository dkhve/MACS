  using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"
#include <cstring>
#include <iostream>

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

int imdb::compareActor(const void* a, const void* b){
  char* ptr1 = (char*)(((data*)a)->key);
  char* ptr2 = (char*)(((data*)a)->file + *(int*)b);
  return strcmp(ptr1, ptr2);
}

int imdb::compareFilm(const void* a, const void* b){
  data* keyPtr = (data*)a;
  film key = *(film*)(keyPtr->key); 
  film curr;
  char* ptr = (char*)(keyPtr->file) + *((int*)b);
  for(; *ptr != '\0'; ptr++){
    curr.title += *ptr;
  }
  curr.year = *(ptr+1) + 1900;
  if(key < curr) return -1;
  if(key == curr) return 0;
  return 1;
}


// returns films given actor has played in
bool imdb::getCredits(const string& player, vector<film>& films) const {
  int* start = (int*)actorFile + 1;
  data key;
  key.file = actorFile;
  key.key = (void*)player.c_str();
  int* ptr =  (int*)bsearch(&key, start, *(int*)actorFile, sizeof(int), compareActor);
  if(ptr == NULL) return false;
  int a = player.length() + 2 - player.length()%2; // a is distance to  end of string
  ptr = (int*)((char*)actorFile + *ptr + a); //distance to short
  short num = *(short*)((char*)ptr);
  ptr = (int*)((char*)ptr + sizeof(short) + (a + sizeof(short))%4); // distance to array of movies
  for(short i = 0; i < num; i++){
    int mOffset = ptr[i];
    film movie;
    char* ch =(char*)movieFile + mOffset;
    for(; *ch != '\0' ; ch++){
      movie.title += *ch;
    }
    movie.year = *(ch+1) + 1900;
    films.push_back(movie);
  }
  return true;
}

// returns cast for given film
bool imdb::getCast(const film& movie, vector<string>& players) const { 
  int* start = (int*)movieFile +1;
  data key;
  key.file = movieFile;
  key.key = (void*)&movie;
  int* ptr = (int*)bsearch(&key, start, *(int*)movieFile, sizeof(int), compareFilm);
  if(ptr == NULL) return false;
  int distance = movie.title.length() + 1 + 1 + movie.title.length()%2; // distance to end of string
  ptr = (int*)((char*)movieFile + *ptr + distance); // distance to short
  short num = *(short*)((char*)ptr);
  ptr = (int*)((char*)ptr + sizeof(short) + (distance + sizeof(short))%4); // distance to array of actors
  for(short i = 0; i < num; i++){
    int pOffset = ptr[i];
    string player;
    char* ch =(char*)actorFile + pOffset;
    for(; *ch != '\0' ; ch++){
      player += *ch;
    }
    players.push_back(player);
  }
  return true; 
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
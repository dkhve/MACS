#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "url.h"
#include "bool.h"
#include "urlconnection.h"
#include "streamtokenizer.h"
#include "html-utils.h"
#include "hashset.h"

typedef struct{
  int wordFrequency;
  char* title;
} article;

typedef struct{
  char* word;
  vector articles;
} keyword;


static void loadStopWords(hashset* stopWords, const char* stopWordListFileName);
static void Welcome(const char *welcomeTextFileName);
static void BuildIndices(const char *feedsFileName, hashset* stopWords, hashset* readArticles, hashset* keywords);
static void ProcessFeed(const char *remoteDocumentName, hashset* stopWords, hashset* readArticles, hashset* keywords);
static void ScanArticle(streamtokenizer *st, const char *articleTitle, hashset* stopWords, hashset* readArticles, hashset* keywords);
static void storeWord(char* word, hashset* stopWords, hashset* keywords, const char* articleTitle);
static void initKeyWord(keyword* key, const char* articleTitle, char* word);
static void initArticle(article* news, const char* articleTitle);
static void QueryIndices(hashset* keywords, hashset* stopwords);
static void ProcessResponse(const char *word, hashset* keywords, hashset* stopwords);
static bool WordIsWellFormed(const char *word);
static int StringHash(const void *str, int numBuckets);
static void freeArticle(void* elem);
static int KeyWordHash(const void* key, int numBuckets);
static int compareKeyWords(const void* a, const void* b);
static void freeKeyWord(void* elemAddr);
static int compareArticles(const void* a, const void* b);
static int articleHash(const void* elem, int numBuckets);
static int compareByFrequency(const void* a, const void* b);

static const char *const kTextDelimiters = " \t\n\r\b!@$%^*()_+={[}]|\\'\":;/?.>,<~`";
static const char *const kWelcomeTextFile = "data/welcome.txt";
static const char *const filePrefix = "file://";
static const char *const kDefaultFeedsFile = "data/test.txt";
static const char *const kStopWordFile = "data/stop-words.txt";

int main(int argc, char **argv)
{
  setbuf(stdout, NULL);
  Welcome(kWelcomeTextFile);

  hashset stopWords;
  hashset readArticles; //verb read is used in its past form.
  hashset keyWords;

  loadStopWords(&stopWords, kStopWordFile);
  HashSetNew(&readArticles, sizeof(article), 1009 , articleHash, compareArticles, freeArticle);
  HashSetNew(&keyWords, sizeof(keyword), 10007, KeyWordHash, compareKeyWords, freeKeyWord);

  BuildIndices((argc == 1) ? kDefaultFeedsFile : argv[1], &stopWords, &readArticles, &keyWords);
  QueryIndices(&keyWords, &stopWords);

  HashSetDispose(&stopWords);
  HashSetDispose(&readArticles);
  HashSetDispose(&keyWords);
}


/**
 * Function: loadStopWords
 * reads the stopword list from a file and loads it into a hashset
*/

static const char *const kNewLineDelimiters = "\r\n";

static void loadStopWords(hashset* stopWords, const char* stopWordListFileName){
  FILE *infile;
  streamtokenizer st;
  char buffer[64];
  HashSetNew(stopWords, sizeof(buffer), 1009, StringHash, strcasecmp, NULL);
  infile = fopen(stopWordListFileName, "r");
  assert(infile != NULL);
  STNew(&st, infile, kNewLineDelimiters, true);
  while (STNextToken(&st, buffer, sizeof(buffer))) {
    HashSetEnter(stopWords, buffer);
  }
  STDispose(&st); 
  fclose(infile); 
}

/** 
 * Function: Welcome
 * -----------------
 * Displays the contents of the specified file, which
 * holds the introductory remarks to be printed every time
 * the application launches.  This type of overhead may
 * seem silly, but by placing the text in an external file,
 * we can change the welcome text without forcing a recompilation and
 * build of the application.  It's as if welcomeTextFileName
 * is a configuration file that travels with the application.
 */
 
static void Welcome(const char *welcomeTextFileName)
{
  FILE *infile;
  streamtokenizer st;
  char buffer[1024];
  
  infile = fopen(welcomeTextFileName, "r");
  assert(infile != NULL);    
  
  STNew(&st, infile, kNewLineDelimiters, true);
  while (STNextToken(&st, buffer, sizeof(buffer))) {
    printf("%s\n", buffer);
  }
  
  printf("\n");
  STDispose(&st); // remember that STDispose doesn't close the file, since STNew doesn't open one.. 
  fclose(infile);
}

/**
 * Function: BuildIndices
 * ----------------------
 * As far as the user is concerned, BuildIndices needs to read each and every
 * one of the feeds listed in the specied feedsFileName, and for each feed parse
 * content of all referenced articles and store the content in the hashset of indices.
 * Each line of the specified feeds file looks like this:
 *
 *   <feed name>: <URL of remote xml document>
 *
 * Each iteration of the supplied while loop parses and discards the feed name (it's
 * in the file for humans to read, but our aggregator doesn't care what the name is)
 * and then extracts the URL.  It then relies on ProcessFeed to pull the remote
 * document and index its content.
 */

static void BuildIndices(const char *feedsFileName, hashset* stopWords, hashset* readArticles, hashset* keywords)
{
  FILE *infile;
  streamtokenizer st;
  char remoteFileName[1024];
  
  infile = fopen(feedsFileName, "r");
  assert(infile != NULL);
  STNew(&st, infile, kNewLineDelimiters, true);
  while (STSkipUntil(&st, ":") != EOF) { // ignore everything up to the first selicolon of the line
    STSkipOver(&st, ": ");		 // now ignore the semicolon and any whitespace directly after it
    STNextToken(&st, remoteFileName, sizeof(remoteFileName));   
    ProcessFeed(remoteFileName, stopWords, readArticles, keywords);
  }
  
  STDispose(&st);
  fclose(infile);
  printf("\n");
}


/**
 * Function: ProcessFeedFromFile
 * ---------------------
 * ProcessFeed locates the specified RSS document, from locally
 */

static void ProcessFeedFromFile(char *fileName, hashset* stopWords, hashset* readArticles, hashset* keywords)
{
  FILE *infile;
  streamtokenizer st;
  infile = fopen((const char *)fileName, "r");
  assert(infile != NULL);
  STNew(&st, infile, kTextDelimiters, true);
  ScanArticle(&st, (const char *)fileName, stopWords, readArticles, keywords);
  STDispose(&st); // remember that STDispose doesn't close the file, since STNew doesn't open one..
  fclose(infile);
}

/**
 * Function: ProcessFeed
 * ---------------------
 * ProcessFeed locates the specified RSS document and checks if it is local or not
 * we aren't supposed to work on URL-s
 */

static void ProcessFeed(const char *remoteDocumentName, hashset* stopWords, hashset* readArticles, hashset* keywords)
{
  if(!strncmp(filePrefix, remoteDocumentName, strlen(filePrefix))){
    ProcessFeedFromFile((char *)remoteDocumentName + strlen(filePrefix), stopWords, readArticles, keywords);
  }
}

// checks if this article has already been read, if not, stores it as read
bool alreadyRead(const char *articleTitle, hashset* readArticles){
  article currArticle;
  char* title = strdup(articleTitle);
  currArticle.title = title;
  currArticle.wordFrequency = 0;
  if(HashSetLookup(readArticles, &currArticle) != NULL) {
    free(title);
    return true;
  }
  HashSetEnter(readArticles, &currArticle);
  return false;
}

/**
 * Function: ScanArticle
 * ---------------------
 * Parses the specified article and counts the numbers
 * of well-formed words that could potentially serve as keys in the set of indices.
 * Once the full article has been scanned, the number of well-formed words is
 * printed, and the longest well-formed word we encountered along the way
 * is printed as well.
 */

static void ScanArticle(streamtokenizer *st, const char *articleTitle, hashset* stopWords, hashset* readArticles, hashset* keywords)
{

  if(alreadyRead(articleTitle, readArticles)) return;
  int numWords = 0;
  char word[1024];
  char longestWord[1024] = {'\0'};

  while (STNextToken(st, word, sizeof(word))) {
    if (strcasecmp(word, "<") == 0) {
      SkipIrrelevantContent(st); // in html-utls.h
    } else {
      RemoveEscapeCharacters(word);
      if (WordIsWellFormed(word)) {
        numWords++;
        if (strlen(word) > strlen(longestWord))
          strcpy(longestWord, word);
        if(HashSetLookup(stopWords, word) == NULL){
          storeWord(word, stopWords, keywords, articleTitle);
        }
      }
    }
  }

  printf("\tWe counted %d well-formed words [including duplicates].\n", numWords);
  printf("\tThe longest word scanned was \"%s\".", longestWord);
  if (strlen(longestWord) >= 15 && (strchr(longestWord, '-') == NULL)) 
    printf(" [Ooooo... long word!]");
  printf("\n");
}


//stores the given word if it is new. if not updates our data structure accordingly
static void storeWord(char* w, hashset* stopWords, hashset* keywords, const char* articleTitle){
  char* word = strdup(w);
  keyword* elem  = (keyword*)HashSetLookup(keywords, &word);
  if(elem == NULL){
    keyword key;
    initKeyWord(&key, articleTitle, word);
    HashSetEnter(keywords, &key);
  }else{
    free(word);
    article news;
    initArticle(&news, articleTitle);
    int pos = VectorSearch(&elem->articles, &news, compareArticles, 0, false);
    if(pos != -1){
      ((article*)VectorNth(&elem->articles, pos))->wordFrequency++;
      freeArticle(&news);
    }else{
      VectorAppend(&elem->articles, &news);
    }
  }
}

//initializes a keyword on given address
static void initKeyWord(keyword* key, const char* articleTitle, char* word){
    key->word = word;
    VectorNew(&key->articles, sizeof(article), freeArticle, 4);
    article news;
    initArticle(&news, articleTitle);
    VectorAppend(&key->articles, &news);
}


//initializes an article on given address
static void initArticle(article* news, const char* articleTitle){
  news->wordFrequency = 1;
  news->title = strdup(articleTitle);
}

/** 
 * Function: QueryIndices
 * ----------------------
 * Standard query loop that allows the user to specify a single search term, and
 * then proceeds (via ProcessResponse) to list up to 10 articles (sorted by relevance)
 * that contain that word.
 */

static void QueryIndices(hashset* keywords, hashset* stopwords)
{
  char response[1024];
  while (true) {
    printf("Please enter a single search term [enter to break]: ");
    fgets(response, sizeof(response), stdin);
    response[strlen(response) - 1] = '\0';
    if (strcasecmp(response, "") == 0) break;
    ProcessResponse(response, keywords, stopwords);
  }
}

/** 
 * Function: ProcessResponse
 * -------------------------
 * Placeholder implementation for what will become the search of a set of indices
 * for a list of web documents containing the specified word.
 */

static void ProcessResponse(const char *word, hashset* keywords, hashset* stopwords)
{
  if (WordIsWellFormed(word)) {
    if(HashSetLookup(stopwords, word) != NULL) {
      printf("Too common a word to be taken seriously. Try something more specific.\n");
      return;
    }
    keyword dummy;
    dummy.word = word;
    keyword* elem = (keyword*)HashSetLookup(keywords, &dummy);
    if(elem == NULL){
      printf("None of today's news articles contain the word \"%s\".", word);
    }else{
        VectorSort(&elem->articles, compareByFrequency);
        for(int i = 1; i <= VectorLength(&elem->articles); i++){
          article* curr = (article*)VectorNth(&elem->articles, i-1);
          if(curr->wordFrequency == 1)  printf("%d.) \"%s\" [search term occurs %d time]\n \"%s\"\n", i, curr->title, curr->wordFrequency, curr->title);
          else printf("%d.) \"%s\" [search term occurs %d times]\n \"%s\"\n", i, curr->title, curr->wordFrequency, curr->title);
        }
    }
  } else {
    printf("\tWe won't be allowing words like \"%s\" into our set of indices.\n", word);
  }
}

/**
 * Predicate Function: WordIsWellFormed
 * ------------------------------------
 * Before we allow a word to be inserted into our map
 * of indices, we'd like to confirm that it's a good search term.
 * One could generalize this function to allow different criteria, but
 * this version hard codes the requirement that a word begin with 
 * a letter of the alphabet and that all letters are either letters, numbers,
 * or the '-' character.  
 */

static bool WordIsWellFormed(const char *word)
{
  int i;
  if (strlen(word) == 0) return true;
  if (!isalpha((int) word[0])) return false;
  for (i = 1; i < strlen(word); i++)
    if (!isalnum((int) word[i]) && (word[i] != '-')) return false; 

  return true;
}

/** 
 * StringHash                     
 * ----------  
 * This function adapted from Eric Roberts' "The Art and Science of C"
 * It takes a string and uses it to derive a hash code, which   
 * is an integer in the range [0, numBuckets).  The hash code is computed  
 * using a method called "linear congruence."  A similar function using this     
 * method is described on page 144 of Kernighan and Ritchie.  The choice of                                                     
 * the value for the kHashMultiplier can have a significant effect on the                            
 * performance of the algorithm, but not on its correctness.                                                    
 * This hash function has the additional feature of being case-insensitive,  
 * hashing "Peter Pawlowski" and "PETER PAWLOWSKI" to the same code.  
 */  

static const signed long kHashMultiplier = -1664117991L;
static int StringHash(const void *str, int numBuckets){
  const char* s = (char*)str;         
  int i;
  unsigned long hashcode = 0;
  for (i = 0; i < strlen(s); i++)  
    hashcode = hashcode * kHashMultiplier + tolower(s[i]);  
  
  return hashcode % numBuckets;                                
}

//frees article of which address is given
static void freeArticle(void* elem){
  article* news = (article*)elem;
  free(news->title);
}

// this hash function returns hashcode for keywords using StringHash function on words they contain
// hashcode is in [0, numBuckets) range
static int KeyWordHash(const void* key, int numBuckets){
  return StringHash(((keyword*)key)->word, numBuckets);
}

// compares two keywords by the word they contain
static int compareKeyWords(const void* a, const void* b){
  const keyword* keyA = (keyword*)a;
  const keyword* keyB = (keyword*)b;
  return strcasecmp(keyA->word, keyB->word);
}

//frees keyword of which address is given
static void freeKeyWord(void* elem){
  keyword* key = (keyword*)elem;
  vector* v = &(key->articles);
  if(v) VectorDispose(v);
  if(key->word) free(key->word);
}

// compares articles by titles
static int compareArticles(const void* a, const void* b){
  article* newsA = (article*)a;
  article* newsB = (article*)b;
  return strcasecmp(newsA->title, newsB->title);
}

// this hash function returns hashcode for keywords using StringHash function on title they contain
// hashcode is in [0, numBuckets) range
static int articleHash(const void* elem, int numBuckets){
  return StringHash(((article*)elem)->title, numBuckets);
}

//compares two articles by the wordFrequency variable
static int compareByFrequency(const void* a, const void* b){
  article* newsA = (article*)a;
  article* newsB = (article*)b;
  if(newsA->wordFrequency > newsB->wordFrequency) return -1;
  if(newsA->wordFrequency < newsB->wordFrequency) return 1; 
  return 0;
}

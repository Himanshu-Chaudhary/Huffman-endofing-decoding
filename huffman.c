/**********************************************/
/*Himanshu Chaudhary                          */
/*May 11, 2016                                */
/*CS 241L    Section #003                     */
/**********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"


/*the index represents the char value of a symbol*/
unsigned long frequency[255]; 
int queueLength = 0;
struct huffTree *tree[256]; 

/* function returns the leftmost leaf from a given node*/
int minVal(struct huffTree* root)
{
  struct huffTree* current = root;
  /*moves to the left until end is reached*/
  while(current->left !=NULL)
  {
    current = current->left;
  }
  return(current->symbol);
}

/*writes the headear which contains the frequency and symbol 
 * values of each character in the hufftree*/
void WriteHeader(FILE *out, unsigned long frequency[])
{
    int i, count = 0;
    unsigned long total;

    /*countes the total number of symbols and characters*/
    for (i =0;i<256; i++) 
    {
      if(frequency[i]) 
      {
        count++;
        total+=frequency[i];
      }
    }
    /*write the total number of symbols in the fie*/
    fwrite(&count,1,1,out);
    for(i = 0; i < 256; i++)
    {  
      /* writes the symbol and frequency*/
      if(frequency[i]) 
        {  
          fwrite(&i,1,1,out);
          fwrite(&(frequency[i]),8,1,out);
        }
   }

}


/*flips the 8 bits of an unsigned int by multiplying its
 * bits values in the opposite order with 2^n*/
unsigned int flipBinary(int num)
{
  unsigned int singleBit =0, finalValue=0,i, count=0;
  int exponent=1;   /*exponent represents the 2^n value*/

  /* this loops gets the first bit of num and multiplies it
   * by the exponent value then shifts the bits up by 1 bit*/
  for(i=0;i<8;i++)
  {
    singleBit = num>>7;
    num = num<<1;
    num = num& 0x000000ff;
    finalValue += singleBit * exponent;
    exponent = exponent*2;
  }
  return finalValue;
}


/* @param char *code 
 *  is the huffman code that needs to be written
 * @param int *rem
 *  is the bits in a buffer that needs to be written before *code
 * @int *remLength
 *  is the number of bits in the buffer rem
 * @param FILE *out
 *  is the file where the code needs to be written
 * This function writes the buffer masking with char* and stores the remaing 
 * bits back in rem buffer*/
void writeChar(char *code, unsigned int *rem, int *remLength, FILE *out)
{
  unsigned int finalValue,count=0;
  unsigned int singleBit;
  unsigned int bitLength;
  int length = strlen(code);

  /*passes the value in pointer to a temp value*/
  bitLength = *remLength;
  finalValue = *rem;

  /*loops until all the characters in char *code is finished*/
  while (length!=count)
  {
    /*converts the code to an int and places it in correct position
     * to map*/
    singleBit = code[count]-'0';
    singleBit= singleBit<<(8-bitLength-1);

    /*masks a character in singleBit one by one to finalValue*/
    finalValue = finalValue|singleBit;
    bitLength++;

    /* finalValue gets written once 8 bits is reached and reuired
     * values are initialized*/
    if (bitLength==8)
    {
      /*binary values of int finalValue gets flipped to wite in out*/
      finalValue = flipBinary(finalValue);
      fwrite(&finalValue,1,1,out); 
      bitLength=0;
      finalValue='\0';

    }
    count++;
  }
  /*buffer length and remaining bits are stored back in the pointer*/

 *remLength=bitLength;
 *rem = finalValue;
}

/* builds the huffman table with symbol codes and stores them as 
 * array in table*/
void BuildHuffmanTable(char **table)
{
  int i=0;
  int index=0;
  char *prefix = (char *)calloc(1, sizeof(char));

  /*for every symbol a huffTree is created and stored in 
   * array of huffTree*/
   for (i = 0; i < 255; i++)
  {
  	if(frequency[i])
  	{
  	  struct huffTree *n = createNodes(frequency, i);	
  	  tree[index] = n;
      /*number of unique symbols is stores*/
  	  queueLength++;
  	  index++;
  	}
  }
  /* builds the ordered huffTree*/
  BuildTree();
   

  /* symbol codes for each symbol is generated from the tree
   * and stores as an array in table*/
  traverseTree(*tree, prefix,table);
}


/* encodes the input files and writes the encoded
 * code in the output file*/
void startEncode(FILE* in, FILE* out)
{
  unsigned long totalChars = 0;
  int i, j;
  int index = 0;
  char *table[256];
  unsigned int rem =0,c;
  int length =0;
  /* creates the frequency table and builds the huffman tree 
   * from the frequency table and stores the symbol codes in table*/
  totalChars = countFrequency(in);
  BuildHuffmanTable(table);
  
  /* writes the header of symbol and frequencies using the WriteHeader
   * function followed by the total number of characters*/
  WriteHeader(out,frequency);
  fwrite(&totalChars,8,1,out);

  /*brings the pointer to the front as the files needs to be read
   * again*/
  rewind(in);
  
  /*Writes the codes one by one in a loop using WriteChar function
   * WriteChar writes one symbol code and stores the returning bit 
   * which is passed again with a new symbol code*/
  while((c=fgetc(in))!=EOF)
  {
   writeChar(table[c],&rem,&length,out);
   }
  
  printTable(frequency,table);
  freeList(*tree);
}

/* reads the header and recreates the frequency table 
 * from the header*/
void readHeader(FILE* in)
{
  unsigned char length;
  int i=0;
  char symbol;
  unsigned long freq;

  /* reads the total number of unique symbol*/
  fread(&length,1,1,in);

  /*reads the symbol and its corresponding frequency
   * and recreates the frequency table*/
  for (i=0;i<length;i++)
  {
    fread(&symbol,1,1,in);
    fread(&freq,8,1,in);
    frequency[symbol]=freq;
  }

  /*reads the total number of characters and
   * positions the pointer in the beginning of encoded data*/
  fread(&freq,8,1,in);
}


/* This function returns a single bit from the FILE* in
 * an character is stored in static int a single bit is returned
 * A new character is read after returning 8 bits*/
int getBit(FILE *in)
{
  static int bits =0, bitcount=0;
  int lastBit;

  /*reads a new bits after all the bits from int
   * is returned*/
  if (bitcount == 0)
  {
    /*new character is read
     * if EOF is reached 10000 is returned as Fake EOF
     * bitcount is initialized to 8*/
    if ((bits = fgetc(in))==EOF) return 10000;
    bitcount = 8;
  }

  /* the last bit of encoded character is the first bit of decoded character
   * any int mod 2 returns its last bit as its the only value that makes an int odd */
  lastBit = bits % 2;

 /*moves the next bit to be encoded at last position*/
  bits = bits>>1;
  bitcount--;

  return lastBit;
}


/* gets the symbol from the huffman tree
 * an bit is passes from getBit function and then moves 
 * accordingly in the tree until leaf is reached
 * Returns the decoded character once leaf is reached*/

int DecodeSymbol(FILE*in, struct huffTree *tree)
{
  int next;
  /* checks if the leaf is reached*/
  while(tree->left || tree->right)
  {
    /* gets the next bit to move in the required direction*/
    next = getBit(in);

    /* checks if EOF is reached with fake EOF*/
    if (next==10000) return 10000;

    /*Moves down in the required direction*/
    if(next)
    {
      tree = tree->right;
    }
    else tree = tree->left;
  }

  /* if a leaf is reached, the appropriate symbol is returned*/
  return tree->symbol;
}

/* decodes the file and write the original data in out*/
void startDecode(FILE* in, FILE* out)
{
  int c;
  char *table[256];

  /* reads the frequency table from the header*/
  readHeader(in);

  /* reconstructs the huffTree*/
  BuildHuffmanTable(table);
  
  /* decodes the symbo which is written to out file*/
  while ((c= DecodeSymbol(in,*tree))!=10000)
        fwrite(&c,1,1,out);

  printTable(frequency,table);
}

/* creates the symbol codes from huffTree
 * the codes are stores in array of char each element containg a bit
 * This function reaches to each leaf recursively while calculating its 
 * symbol code*/
void traverseTree(struct huffTree *tree, char *prefix, char **table)
{
  /*if the leaf is reached its code in stores else it moves down*/
  if(!tree->left && !tree->right)
  {
    table[tree->symbol]=prefix;
  }
  else
  {
    if(tree->left)
    {
      traverseTree(tree->left, Concat(prefix, '0'),table);
    }
    if(tree->right)
    {
      traverseTree(tree->right, Concat(prefix, '1'),table);
    }
    free(prefix);
  }
}

/* combines a char into an array of char*/
char *Concat(char *prefix, char letter)
{
  /*size of required char is calculated and new char is inserted*/
    char *result = (char *)malloc(strlen(prefix) + 2);
    sprintf(result, "%s%c", prefix, letter);

    return result;
}

/*builds the tree from the frequency table*/
void BuildTree()
{
  /* gets the number of sumbols*/
  int length = queueLength;

  int i, j;

  /*while every element is places in the huffTree*/
  while(length > 1)
  {

    struct huffTree *node = malloc(sizeof(struct huffTree));
    struct huffTree** n = getTree(tree, 0); 
    struct huffTree** m = getTree(tree, 1); 

    /*the array of huffTree is sorted using the compareFreq comparator
     * the compareFreq compares the frequency values */
    qsort(tree, queueLength, sizeof(struct huffTree *), compareFreq);

    /* a new tree is created using the first two elements in the array
     * and places the new tree in the array with its new frequency*/
    node->left = (*n);
    node->right = (*m); 
    node->symbol = node->left->symbol;
    node->frequency = ((node->right->frequency)+(node->left->frequency));

    /*the new combined huffTree is placed in the array
     * the length gets update*/
    tree[0]= node;
    queueLength--;
    length--;

    for(i = 1; i <= queueLength; i++)
    {
      tree[i]=tree[i+1];
    } 
  }
}

int compareFreq(const void* a, const void* b)
{
   struct huffTree **first = ( struct huffTree**)a;
   struct huffTree **second = (struct huffTree**)b;

  if((*first)->frequency < (*second)->frequency)
  {
    return -1;
  }
  else if((*first)->frequency > (*second)->frequency)
  {
    return +1;
  }
  if((*first)->frequency == (*second)->frequency)
    {
      return (minVal(*first) < minVal(*second)) ? -1:1;
    }
  else return 0;
}

/* returns a tree stored in i location in huffTree array*/
struct huffTree** getTree(struct huffTree** headRef, int i)
{
  struct huffTree* temp = (*headRef)->right;
  struct huffTree** toGet = &headRef[i];
  (*toGet)->frequency = headRef[i]->frequency;
  return toGet;

}

/* creates a new struct huffTree given the character and its frequency*/
struct huffTree* createNodes(unsigned long *frequency, int i)
{
  struct huffTree *node = malloc(sizeof(struct huffTree));
  node->symbol = i;
  node->frequency = frequency[i];
  node->right = NULL;
  node->left = NULL;

  return node;
}

/* Count the frequency of each symbols form in File */
unsigned long countFrequency(FILE* in)
{
  char c;
  unsigned long i;
  unsigned long count = 0;
 
  /*initializes the frequency array*/
  for (i = 0; i < 256; i++)
  {
	frequency[i] = 0;
  }

  c = getc(in);
  /*loops throuh the in Files and increase the frequency of characters*/
  while (c != EOF)
  {
    if (c==256) frequency[0]++;
    else frequency[c]++; 
	  c = getc(in);
	  count++;
  }
 
  /*returns the total number of characters in the in File*/
  return count;
}

/* prints the symbol code stores in a char array*/
void printCode(char *code)
{
  int i=0;
  int length = strlen(code);
  int count =0;

  /*loops through each elements and prints the value*/
  for(i=0;i<length;i++)
  {
    printf("%c",code[i]);
  }
  printf("\n");
}

/*prints the frequency, symbol and its code*/
void printTable(unsigned long *frequency, char** table)
{
  int j,count =0;

  printf("Symbol\tFreq\tCode\n");
  
  /* loops through the frequency table 
   * prints the symbol with its frequency
   * char values of symbol characters is printed*/
  for (j = 0; j < 256; j++)
  {
	if(frequency[j])
	{
    count+=frequency[j];
	  if(j < 33 || j > 126)
	  {
		printf("=%d \t %lu \t" , j, frequency[j] );
	  }
	  else printf("%c \t %lu \t", j, frequency[j]);	
    printCode(table[j]);
	}
		
  }
  printf("Total chars = %d\n", count);
}


/*frees huffTree along with its branches below*/
void freeList(struct huffTree* head)
{
  struct huffTree* current = head;
  struct huffTree* temp;
  /*moves down until the leaf is reached*/
  while (current != NULL)
  {
    temp = current;
  	current = current->right;
  	free(temp);
  }
  head = NULL;
}



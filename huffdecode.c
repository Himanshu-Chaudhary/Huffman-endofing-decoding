/**********************************************/
/*Himanshu Chaudhary                          */
/*May 11, 2016                                */
/*CS 241L    Section #003                     */
/**********************************************/

#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

int main(int argc, char** argv)
{
  char* infile;
  char* outfile;
  FILE* in;
  FILE* out;
/* checking for errors in the arguments*/
/* numbers of args check*/
  if(argc != 3) 
  {
    printf("wrong arg numbers\n");
    return 1;
  }
  
  infile = argv[1];
  outfile = argv[2];
  /* checking infile */
  in = fopen(infile, "rb");
  if(in == NULL)
  {
    printf("couldn't open %s \n", infile);
    return 1;
  }
  out = fopen(outfile, "wb");
  /*checking outfile*/
  if(out == NULL)
  {
    printf("couldn't open %s \n", outfile);
    return 1;
  }
  /*starts the decoding*/
  startDecode(in, out);

  fclose(in);
  fclose(out);

  return 0;
}

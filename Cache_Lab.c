/*  
 *name: Alyssa Hsiang 
 *netId: ajh551
 *
 * cachelab.c - A cache simulator that can replay traces of memory accesses
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss. (I examined the trace,
 *  the largest request I saw was for 8 bytes).
 *  2. Instruction loads (I) are ignored.
 *  3. data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus a possible eviction. 
 */
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

//#define DEBUG_ON 
#define ADDRESS_LENGTH 64

/* Type: Memory address */
typedef unsigned long long int mem_addr_t;

/**************************************************************************/
/* Declarations of the functions currently implemented in this file */

/* 
 * printSummary - This function provides a standard way for your cache
 * simulator * to display its final hit and miss statistics
 */ 
void printSummary(int hits,  /* number of  hits */
				  int misses, /* number of misses */
				  int evictions); /* number of evictions */

/*
 * replayTrace - replays the given trace file against the cache 
 */
void replayTrace(char* trace_fn);

/* 
 * accessData - Access data at memory address addr. 
 * This is the one that you need to implement.
 */
void accessData(mem_addr_t addr);

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[]);
/**************************************************************************/


/**************************************************************************/
/* Declarations of the global variable currently used. */
/* Do NOT modify any of the following globals. 
 * But you can add your own globals if you want 
 */
/* Globals set by command line args */
int s = 0; /* set index bits */
int b = 0; /* block offset bits */
int E = 0; /* associativity */
int LRUcounter = 0;
struct Line ***cache;

char* trace_file = NULL;

/* Counters used to record cache statistics */
int miss_count = 0;     /* cache miss */
int hit_count = 0;      /* cache hit */
int eviction_count = 0; /* A block is evicted from the cache */
/**************************************************************************/


struct Line{
		char validBit;
		unsigned LRUcount;
		mem_addr_t tag;
		
	};


/* 
 * accessData - Access data at memory address addr.
 *   If it is already in cache, increast hit_count
 *   If it is not in cache, bring it in cache, increase miss count.
 *   Also increase eviction_count if a line is evicted.
 * 
 *   If you need to evict a line (i.e. block) apply least recently used 
 *   replacement policy.
 */
void accessData(mem_addr_t addr)
{
   
   /* You need to implement this function and any other function this
    * function may need 
    */


	char hit_flag = 0;
	mem_addr_t addressTag, setIndex;
	mem_addr_t maskIndex = pow(2,s) -1;
	maskIndex <<= b;
	int j;
	int dest = 0;
	int lowestCount;
	addressTag = addr >> (s + b);
	setIndex = (maskIndex & addr) >>b;

	
	LRUcounter++;
	lowestCount = LRUcounter;
	for(j = 0; j < E; j++){
		if (cache[setIndex][j] ->validBit == 1 && cache[setIndex][j] ->tag == addressTag){
			hit_flag = 1;	
			dest = j;
		}
		else if(cache[setIndex][j] -> LRUcount < lowestCount ){
			lowestCount = cache[setIndex][j] -> LRUcount; 
			if (hit_flag == 0){
				dest = j;
			}
		}
	}
		
	if(hit_flag == 1){
		hit_count++;
	}
	else{
		miss_count++;
		if(cache[setIndex][dest] -> validBit == 1){
			eviction_count++;
			
		}
	
		cache[setIndex][dest]->tag = addressTag;
		cache[setIndex][dest]->validBit = 1;
	}
	cache[setIndex][dest]->LRUcount = LRUcounter;
	

	
  
  
}







/*
 * main - Main routine 
 */
int main(int argc, char* argv[])
{
    char c;

    /* Do NOT modify anything from this point till the following comment */
    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
        switch(c){
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }
    /* From here you can make any modification, 
     * except removing the call to replayTrace */
    	int i, j;
	cache = (struct Line ***) malloc(pow(2,s) * sizeof(struct Line **));
	for( i = 0; i < pow(2,s); i++){
		cache[i] = (struct Line **) malloc(E * sizeof(struct Line *));
		for(j = 0; j < E; j++){
			cache[i][j] = (struct Line*) malloc(sizeof(struct Line));
			(cache[i][j])->validBit = 0;
           	 	(cache[i][j])->tag = 0;
           		(cache[i][j])->LRUcount = 0;
		}
	}
    
    /* Do not remove this line as it is the one calls your cache access function */
    replayTrace(trace_file);
    
  
    /* Do not modify anything from here till end of main() function */
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}


/****** Do NOT modify anything below this point ******/

/* 
 * printSummary - Summarize the cache simulation statistics. Student cache simulators
 *                must call this function in order to be properly autograded. 
 */
void printSummary(int hits, int misses, int evictions)
{
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    
}

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of blocks per set (i.e. associativity).\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t ls.trace\n", argv[0]);
    exit(0);
}

/*
 * replayTrace - replays the given trace file against the cache 
 */
void replayTrace(char* trace_fn)
{
    char buf[1000];
    mem_addr_t addr=0;
    unsigned int len=0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if(!trace_fp){
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while( fgets(buf, 1000, trace_fp) != NULL) {
        if(buf[1]=='S' || buf[1]=='L' || buf[1]=='M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);

            accessData(addr);

            /* If the instruction is R/W then access again */
            if(buf[1]=='M')
                accessData(addr);
            
        }
    }

    fclose(trace_fp);
}

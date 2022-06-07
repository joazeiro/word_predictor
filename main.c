/*                      COP 3502C Programming Assignment 5 
                This program is written by: Maria Eduarda Joazeiro Gomes
*/ 

/*
                    The objective of this program is to create a word
                    predictor just like the autofill feature on phones
*/

//pre processors
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_VALUE 26 //number of letters in the alphabet
//#include "leak_detector_c.h"

//trie_node declaration
typedef struct trie{
    
    struct trie* next[MAX_VALUE];
    int count;
    int max_next_count;
    
} trie;

//function signatures
trie* init_trie(void);
void add_word(trie*, FILE*);
void insert(trie*, char[], int, int);
void insert_max(trie*, char[], int);
void prediction(trie*, char[], int, FILE*);
void query(trie*, FILE*, FILE*);
void free_trie(trie*);

//main function
int main(){
    
    //atexit(report_mem_leak);

    //open in and out files
    FILE* infile = fopen("in.txt", "r");
    FILE* outfile = fopen("out.txt", "w");
    
    //first we have to initialize our trie
    trie* root = init_trie();

    //make sure the file you are trying to open exist
    if(infile == NULL || outfile == NULL){
        printf("File does not exist\n");
        return 1;
    }

    int n, choice;

    //getting number of lines to be read
    fscanf(infile, "%d", &n);

    for(int i = 0; i < n; i++){

        //checking which choice was picked (1 or 2)
        fscanf(infile, "%d", &choice);

        //if its one, we add the word to our dictionary
        if (choice == 1){
            add_word(root, infile);
        } 
        
        //if its two, we need to predict the next letter
        else if(choice == 2){
            query(root, infile, outfile);
        } 
        else{
            //this should not happen/debugging purposes
            printf("Something went wrong\n");
            return 1;
        }
    }
    
    //free the trie at the end and close the files
    fclose(infile);
    fclose(outfile);
    free_trie(root);
    
    return 0;
}

//allocate memory and intiialize the trie
trie* init_trie(){
    
    trie* big_head = malloc(sizeof(trie));
    big_head->count = 0;
    big_head->max_next_count = 0;
    
    //set all children to null to avoid issues
    for(int i = 0; i < MAX_VALUE; i++){
        big_head->next[i] = NULL;
    }

    return big_head;
}

//wrapper function to insertion
//we will read whatever comes after the choice in here and then pass it to the function insert
void add_word(trie* root, FILE* infile) {
    
    char word[50]; //the word itself
    int count; //how many times it appears

    fscanf(infile, "%s %d", word, &count);
    insert(root, word, 0, count);
}

//inserting the word into our dictionary
void insert(trie* root, char word[], int k, int count) {
   
    //base case for recursive call
    if(k == strlen(word)){
        return;
    }

    //the next index has to be recalculated at every recursive call
    int next_idx = word[k] - 'a';

    //if we do not have a node for the next letter we
    //need to initialize it
    //if we do, we just add its count to our general count (not to max_count)
    if(root->next[next_idx] == NULL){
        root->next[next_idx] = init_trie();
    }

    root->next[next_idx]->count += count;

    //then proceed to the next letter of the word
    insert(root->next[next_idx], word, k+1, count);
}

//wrapper function to the second option
void query(trie* root, FILE* infile, FILE* outfile) {
    
    //same concept as in the first one --> read the word in here
    //no number needs to be read in here
    char word[50];
    fscanf(infile, "%s", word);

    //first we need to find the letter/word with the highest chance to show up
    insert_max(root, word, 0);

    //then we output the prediction
    prediction(root, word, 0, outfile);
}

//look for the letter/word with the highest chance to appear
void insert_max(trie* root, char word[], int k) {
    
    int next_idx = word[k] - 'a';
    
    //if we reached the end of the word that was read
    //we need to look for whatever next possible letter is not NULL
    //and then update the highest count based on the count of the possible next letters
    if(strlen(word) == k){
        for(int i = 0; i < MAX_VALUE; i++){
            if(root->next[i] != NULL && root->next[i]->count > root->max_next_count){
                root->max_next_count = root->next[i]->count; //update happens here
            }
        }
    } 
    
    //if we are not at the end of the word already, we keep calling the function 
    //recursively to get there
    else if(root->next[next_idx] != NULL){
        insert_max(root->next[next_idx], word, k+1);
    }
}

//this is where the actual predict happens and the predicted char
//is printed to the output file
void prediction(trie* root, char word[], int k, FILE* outfile){
    
    int next_idx = word[k] - 'a';
    if(k == strlen(word)){
        
        //initialize the predicted char to the equivalent to null in char
        char predicted_char = '\0';
        for(int i = 0; i < MAX_VALUE; i++){

            //if once we get to the end of the world, we have a next char, we need to check
            //if that char's count matches with our max count
            //if it does, then this is the predicted char
            //the max count was set up in the insert_max function
            if(root->next[i] != NULL && root->max_next_count == root->next[i]->count){
                
                //updating the predicted char and printing it
                predicted_char = i + 'a';
                fprintf(outfile, "%c", predicted_char);
                printf("%c", predicted_char);
            }
        }

        //if our char never gets updated, it means that we do not know this word
        //than according to instructions we have to print "unknown word"
        if(predicted_char == '\0'){
            printf("unknown word");
            fprintf(outfile, "unknown word");
        }
        fprintf(outfile, "\n");
        printf("\n");
        return;
    } 

    //if we to not have anything at the next index, it means that we do not know this word
    //than according to instructions we have to print "unknown word"
    else if(root->next[next_idx] == NULL){
        printf("unknown word\n");
        fprintf(outfile, "unknown word\n");
        return;
    }

    //we haven't reached the end of the word yet, keep going recursively
    else{
        prediction(root->next[next_idx], word, k+1, outfile);
    }
}

//freeing the memory of the trie
void free_trie(trie* root) {

    for(int i = 0; i < MAX_VALUE; i++){
        if (root->next[i] != NULL){
            free_trie(root->next[i]);
        }
    }
    
    free(root);
}
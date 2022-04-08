#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef const char Face[4];
// Any move order works (only no_move must be last), so set by preference
// on the order to search
typedef enum Move {U_prime, M, U, M_prime, no_move} Move;

typedef struct State {
    unsigned char edges_position[6];
    unsigned char edges_orientation[6];
    unsigned M_position:2;
    unsigned U_position:2;
} State;

// they make a tree of states
typedef struct StateNode {
    State state;
    Move generator_move;  //move that creates the state from the parent
    struct StateNode* parent;
} StateNode;

typedef struct QueueNode {
    StateNode* state_node;
    struct QueueNode* next;
    struct QueueNode* prev;
} QueueNode;

typedef struct StateCache {
    bool* array;
    size_t length;
    unsigned char perms[720][6];
} StateCache;

typedef struct Queue {
    QueueNode tail;
    QueueNode head;
} Queue;

Move reverse_move(Move move);

State rotate_edges(State initial_state, Face face, bool prime); 
State make_move(State initial_state, Move move);

bool compare_states(State A, State B);

void print_state(State state);
void print_moves_traceback(StateNode initial_node); 
void get_new_neighbors(StateCache*, StateNode, State*);

/////////////////////////////////////////q

Move reverse_move(Move);
State rotate_edges(State, Face, bool prime);
State make_move(State, Move move);
bool compare_states(State, State);
void print_state(State);
void print_queue_addresses(Queue*);
bool is_nostate(State state);
bool is_in_cache(StateCache*, State);
void get_new_neighbors(StateCache*, StateNode, State* new_neighbors);
int cache_append(StateCache*, State);
void cache_free(StateCache*);
Queue* new_queue(void);
bool queue_is_empty(Queue*);
StateNode* queue_pop(Queue*);
void print_bin(unsigned n);

void swap(unsigned char* a, unsigned char* b);

// Generating permutation using Heap Algorithm
void heap_permutation(
    unsigned char* a, int size, size_t* index, unsigned char permutations[720][6]);

uint32_t encode_eo(unsigned char* eo);

void decode_eo(uint32_t eo_id, unsigned char* eo);

bool ep_is_equal(unsigned char* A, unsigned char* B);

uint32_t encode_state(State state, unsigned char permutations[720][6]);

State decode_state(uint32_t code, unsigned char permutations[720][6]);

int queue_append(
    Queue* queue, State new_state, StateNode* parent, Move gen_move,
    StateCache* state_cache);

void queue_free(Queue*);

void print_moves_traceback(StateNode initial_node);

size_t queue_length(Queue*);

StateCache* new_cache(void);

void test_state_encoding(State state, unsigned char permutations[720][6]);

uint32_t get_max_state(void);

/* Find a solution to the last six edges step (LSE) of roux method for solving
 * the Rubik's cube.
 *
 * To specify a state of the cube, one must provide the current edge permuta-
 * tion, the edges' orientation and the positions of the middle and top layers.
 *
 * For edges permutation, provide numbers 0 to 5 in the order they appear
 * in the cube, 0 to 5 representing the edges DF, DB, UB, UF, UL and UR, res-
 * pectively. So a sequence 425130 means UL is in DF's place, UB is in DB's
 * place, UR is in UB's place and so on.
 *
 * For edge orientation, provide a 6-digit binary sequence, one digit for each
 * edge in the same order mentioned above, DF, DB, UB, UF, UL and UR. 0 means
 * the edge is oriented, 1 means it is misoriented. Note: the position of each
 * digit refer to the edge (ehich can be anywhere in the cube), NOT an edge
 * position. So 110000 means that the edges which should have been in the spots
 * DF and DB are misoriented.
 *
 * The middle and upper layer positions are simply two integers from 0 to 3,
 * 0 indicating that the layer is correctly aligned, 1 meaning the layer is a
 * single move (U or M, depending on the layer) apart from the solved position.
 * So an U move brings the top layer position to 1, another to 2, U' back to 1,
 * and so on, the same applying to the M layer.
 *
 * Therefore, one should run the program with the arguments in the following
 * order:
 *
 * $ ./roux_lse <edges position> <edges orientation> <M position> <U position>
 * 
 *
 * Run examples
 * ============
 *
 * $ ./roux_lse 012345 000011 2 2
 *      Target state: 012345 000011 2 2
 *      Visited 178936 states.
 *      Solution found!
 *      M U M' U M U' M U M2 U' M' U' M' U2
 *
 * $ ./roux_lse 251304 100001 2 2
 *      Target state: 251304 100001 2 2
 *      Visited 157352 states.
 *      Solution found! 
 *      U2 M' U' M2 U2 M' U2 M' U M'
 *
 * $ ./roux_lse 503124 000011 0 1
 *      Target state: 503124 000011 0 1
 *      Visited 103567 states.
 *      Solution found! 
 *      U' M U M U M' U' M U M2 U2 
 *
 * $ ./roux_lse 130542 010111 1 3
 *     Target state: 130542 010111 1 3
 *     Visited 163445 states.
 *     Solution found! 
 *     M' U' M' U M' U M' U2 M U' M2 U'
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "roux_lse.h"
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
#define MAX_CACHE_SIZE 368639

const State MAXSTATE = {{5, 4, 3, 2, 1, 0}, {1, 1, 1, 1, 1, 1}, 3, 3};
const State SOLVED = {{0, 1, 2, 3, 4, 5}, {0, 0, 0, 0, 0, 0}, 0, 0};
const State NOSTATE = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, 0, 0};

Face U_face = {3, 4, 2, 5};
Face M_face = {0, 1, 2, 3};


Move reverse_move(Move move) {
    switch (move) {
        case (U): return U_prime;
        case (U_prime): return U;
        case (M): return M_prime;
        case (M_prime): return M;
        case (no_move): return no_move;
        default: return no_move;  //should never run!
    }
}


State rotate_edges(State initial_state, Face face, bool prime) {
    State final_state = initial_state;
    int pos, new_pos;

    for (size_t i=0; i<4; i++) {
        if (prime) {
            pos = face[(i+1)%4];
            new_pos = face[i];
        }
        else {
            pos = face[i];
            new_pos = face[(i+1)%4];
        }
        final_state.edges_position[new_pos] = initial_state.edges_position[pos];
    }

    return final_state;
}


State make_move(State initial_state, Move move) {
    bool prime = (move == M_prime || move == U_prime);
    bool is_U = (move == U || move == U_prime);
    bool BF_up = initial_state.M_position % 2;  //back or front center is up
    size_t pos, edge;
    State final_state;

    if (move == no_move)
        return NOSTATE;

    if (is_U) {
        final_state = rotate_edges(initial_state, U_face, prime);
        (prime) ? --final_state.U_position : ++final_state.U_position;

        if (BF_up) {
            //flip edges orientation
            //TODO use Edges enum
            for (size_t i=0; i<4; i++) {
                pos = U_face[i]; 
                edge = final_state.edges_position[pos];
                final_state.edges_orientation[edge] ^= 1;  // flip boolean
            }
        }
    }
    else {  //move is M or M'
        final_state = rotate_edges(initial_state, M_face, prime);
        (prime) ? --final_state.M_position : ++final_state.M_position;
    }

    return final_state;
}


bool compare_states(State A, State B) {
    if (A.M_position != B.M_position)
        return false;
    if (A.U_position != B.U_position)
        return false;
    for (size_t i=0; i<6; i++) {
        if (A.edges_position[i] != B.edges_position[i])
            return false;
        if (A.edges_orientation[i] != B.edges_orientation[i])
            return false;
    }
    return true;
}


void print_state(State state) {
    for (size_t i=0; i<6; i++)
        printf("%zu", state.edges_position[i]);
    printf(" ");
    for (size_t i=0; i<6; i++)
        printf("%zu", state.edges_orientation[i]);
    printf(" %zu %zu\n", state.M_position, state.U_position);
}


void print_queue_addresses(Queue* queue) {
    QueueNode* queue_node = &queue->tail;
    printf("Q addr.\n");
    while (queue_node) {
        printf("%zu\n", queue_node);
        queue_node = queue_node->next;
    }
}


bool is_nostate(State state) {
    return state.edges_position[0] == state.edges_position[1];
}


bool is_in_cache(StateCache* cache, State state) {
    uint32_t state_code = encode_state(state, cache->perms);
    return cache->array[state_code];
}


void get_new_neighbors(
    StateCache* cache, StateNode state_node,
    State* new_neighbors)
{
    State neighbor;
    /* The parent state obviously exists, so do not consider it.
    move_to_parent is the move that takes the state back to
    its parent. */
    Move move_to_parent = reverse_move(state_node.generator_move);

    for (Move m=0; m<4; m++) {
        if (m == move_to_parent) {
            new_neighbors[m] = NOSTATE;
            continue;
        }
        neighbor = make_move(state_node.state, m);
        new_neighbors[m] = is_in_cache(cache, neighbor) ? NOSTATE : neighbor;
    }
}


int cache_append(StateCache* cache, State state) {
    cache->array[encode_state(state, cache->perms)] = true;
    cache->length++;
    return 0;
}


void cache_free(StateCache* cache) {
    free(cache->array);
    free(cache);
}


Queue* new_queue(void) {
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue->tail.prev=NULL;
    queue->tail.next=&queue->head;
    queue->head.prev=&queue->tail;
    queue->head.next=NULL;
    return queue;
}


bool queue_is_empty(Queue* queue) {
    return queue->tail.next == &queue->head;
}


StateNode* queue_pop(Queue* queue) {
    if (queue_is_empty(queue)) {
        printf("Error: popping empty queue!\n");
        return NULL;
    }
    StateNode* node;
    QueueNode* queue_node = queue->head.prev;
    queue->head.prev = queue_node->prev;
    queue->head.prev->next = &queue->head;

    node = queue_node->state_node;
    free(queue_node);
    return node;
}


void print_bin(unsigned n) {
    while (n) {
        printf("%d", n & 1);
        n >>= 1;
    }
    printf("\n");
}

uint32_t bin_count(uint32_t n) {
    uint32_t count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

void swap(unsigned char* a, unsigned char* b) {
    unsigned char tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

// Generating permutation using Heap Algorithm
void heap_permutation(
    unsigned char* a, int size, size_t* index, unsigned char permutations[720][6])
{
    // if size becomes 1 then prints the obtained
    // permutation
    if (size == 1) {
        for (size_t i=0; i<6; i++)
            permutations[*index][i] = a[i];
        ++*index;
        return;
    }

    for (size_t i = 0; i < size; i++) {
        heap_permutation(a, size - 1, index, permutations);

        // if size is odd, swap 0th i.e (first) and
        // (size-1)th i.e (last) element
        if (size % 2)
            swap(a, a+size-1);

        // If size is even, swap ith and
        // (size-1)th i.e (last) element
        else
            swap(a+i, a+size-1);
    }
}


uint32_t encode_eo(unsigned char* eo) {
    //result occupies 5 bits at most
    uint32_t int_eo=0, count_valid=0;

    for (size_t i=0; i<6; i++) {
        int_eo <<= 1;
        int_eo |= eo[i];
    }
    if (__builtin_popcount(int_eo) % 2) {
        printf("\nError: invalid edge orientation: ");
        print_bin(int_eo);
        return 1;
    }
    for (size_t i=0; i<64; i++) {
        if (!(__builtin_popcount(i)%2)) {
            if (i == int_eo)
                return count_valid;
            ++count_valid;
        }
    }
}


void decode_eo(uint32_t eo_id, unsigned char* eo) {
    //result occupies 5 bits at most
    uint32_t count_valid=0, int_eo;

    for (size_t i=0; i<64; i++) {
        if (!(__builtin_popcount(i)%2)) {
            if (eo_id == count_valid) {
                int_eo = i;
                break;
            }
            ++count_valid;
        }
    }
    for (size_t i=5; i<6; i--) {
        eo[i] = int_eo & 1;
        int_eo >>= 1;
    }
}


bool ep_is_equal(unsigned char* A, unsigned char* B) {
    for (size_t i=0; i<6; i++)
        if (A[i] != B[i])
            return false;
    return true;
}


uint32_t encode_state(State state, unsigned char permutations[720][6]) {
    uint32_t code = 0;
    uint32_t int_ep;
    for (int_ep=0; int_ep<720; ++int_ep)
        if (ep_is_equal(state.edges_position, permutations[int_ep]))
            break;
    code |= int_ep;
    code <<= 5;
    code |= encode_eo(state.edges_orientation);
    code <<= 2;
    code |= state.M_position;
    code <<= 2;
    code |= state.U_position;
    return code;
}


State decode_state(uint32_t code, unsigned char permutations[720][6]) {
    State state;

    state.U_position = code & 3;
    code >>= 2;
    state.M_position = code & 3;
    code >>= 2;
    decode_eo(code & 31, state.edges_orientation);
    code >>= 5;
    for (size_t i=0; i<6; ++i)
         state.edges_position[i] = permutations[code][i];

    return state;
}


int queue_append(
    Queue* queue, State new_state, StateNode* parent, Move gen_move,
    StateCache* state_cache
) {
    QueueNode* queue_node = (QueueNode*) malloc(sizeof(QueueNode));
    StateNode* state_node = (StateNode*) malloc(sizeof(StateNode));
    state_node->state = new_state;
    state_node->parent = parent;
    state_node->generator_move = gen_move;

    //printf("Appending state %zu ", state_node);
    //print_state(new_state);
    if (cache_append(state_cache, new_state)) {
        free(state_node);
        free(queue_node);
        return 1;
    }

    queue_node->state_node = state_node;
    queue_node->prev = &queue->tail;
    queue_node->next = queue->tail.next;

    queue->tail.next->prev = queue_node;
    queue->tail.next = queue_node;
    return 0;
}


void queue_free(Queue* queue) {
    QueueNode* current_node = queue->head.prev;
    QueueNode* last_node;

    while (current_node->prev) {
        last_node = current_node;
        current_node = current_node->prev;
        free(last_node);
    }
    free(queue);  //this frees tail and head
}


void print_moves_traceback(StateNode initial_node) {
    //print moves that took us from root to here
    char* move_names[5];
    StateNode current_node = initial_node;
    Move current_move = no_move, prev_move;
    bool quiet_next = true;

    move_names[U] = "U";
    move_names[U_prime] = "U'";
    move_names[M] = "M";
    move_names[M_prime] = "M'";
    move_names[no_move] = ".";

    printf("\n");

    while (current_node.parent) {
        prev_move = current_move;
        current_move = reverse_move(current_node.generator_move);
        current_node = *current_node.parent;

        if (quiet_next) {
            quiet_next = false;
            continue;
        }
        if (current_move == prev_move) {
            printf("%c2 ", move_names[prev_move][0]);
            quiet_next = true;
        }
        else
            printf("%s ", move_names[prev_move]);

    }

    if (!quiet_next)
        printf("%s", move_names[current_move]);
}


size_t queue_length(Queue* queue) {
    size_t length = 0;
    QueueNode* current_node = queue->head.prev;

    while (current_node != &queue->tail) {
        current_node = current_node->prev;
        ++length;
    }
    return length;
}

StateCache* new_cache(void) {
    size_t ind = 0;
    StateCache* cache = (StateCache*) malloc(sizeof(StateCache));
    unsigned char ini[] = {0,1,2,3,4,5};

    bool* array = (bool*) malloc(MAX_CACHE_SIZE*sizeof(bool));
    //unsigned char* perms = (unsigned char*) malloc(6*720*sizeof(unsigned char));

    for (size_t i=0; i<MAX_CACHE_SIZE; i++)
        array[i] = false;

    cache->array=array;
    cache->length=0;
    heap_permutation(ini, 6, &ind, cache->perms);

    return cache;
}


void test_state_encoding(State state, unsigned char permutations[720][6]) {
    uint32_t enc_state = encode_state(state, permutations);
    print_state(state);
    printf("State encoding: %d\n", enc_state);
    print_state(decode_state(enc_state, permutations));
}


uint32_t get_max_state(void) {
    State mstate;
    StateCache* cache = new_cache();

    for (size_t i=0; i<6; i++) {
        mstate.edges_position[i] = cache->perms[719][i];
        mstate.edges_orientation[i] = 1;
    }

    mstate.M_position = 3;
    mstate.U_position = 3;
    return encode_state(mstate, cache->perms);
}


State get_state(char* argv[]) {
    State state;
    for (size_t i=0; i<6; i++) {
        state.edges_position[i] = argv[1][i]-48;
        state.edges_orientation[i] = argv[2][i]-48;
    }
    state.M_position = argv[3][0]-48;
    state.U_position = argv[4][0]-48;
    return state;
}


int main(int argc, char* argv[]) {
    // MAX STATE CODE = 368639
    // Total 184320 states.
    if (argc != 5) {
        printf("Error: 4 arguments must be provided.\n");
        return EXIT_FAILURE;
    }

    int rc = 0;  //return code
    Queue* queue = new_queue();
    StateCache* cache = new_cache();

    StateNode* current_node;
    State next_states[4];
    State new_state;

    State target_state = get_state(argv);
    printf("Target state: ");
    print_state(target_state);


    //Queue* queue, State new_state, StateNode* parent, Move gen_move,
    //StateCache* state_cache 
    queue_append(queue, SOLVED, NULL, no_move, cache);

    while (!queue_is_empty(queue)) {
        printf("Visited %zu states.\r", cache->length);
        //printf("Queue length: %zu\n", queue_length(queue));
        //printf("Visited ");
        //print_state(current_node->state);
        current_node = queue_pop(queue);

        if (compare_states(target_state, current_node->state)) {
            printf("\nSolution found! ");
            print_moves_traceback(*current_node);
            printf("\n");
            rc = EXIT_SUCCESS;
            goto END;
        }

        //store new neighbors in next_states
        get_new_neighbors(cache, *current_node, next_states);

        for (Move m=0; m<4; m++) {
            new_state = next_states[m];
            if (!is_nostate(new_state) &&
                    queue_append(queue, new_state, current_node, m, cache)) {
                printf("\nError: exausted cache.\n");
                rc = EXIT_FAILURE;
                goto END;
            }
        }
    }
    rc = EXIT_FAILURE;
    printf("\nNot found.\n");

    END: {
        queue_free(queue);
        cache_free(cache);
    }
    return rc;
}

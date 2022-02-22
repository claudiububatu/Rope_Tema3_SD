#include "./rope.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
extern char* strdup(const char*);
#define EMPTY ""



void myfreeRopeNode(RopeNode* rn) {
    if (rn == NULL)
        return;

    free((void*)rn->str);
    myfreeRopeNode(rn->left);
    myfreeRopeNode(rn->right);
    free(rn);
}

void myfreeRopeTree(RopeTree* rt) {
    if (!rt)
       return;

    myfreeRopeNode(rt->root);
}

RopeNode* mycopyRopeNode(RopeNode* rn) {
    if (!rn)
        return NULL;
    RopeNode* new_rn =  makeRopeNode(strdup(rn->str));
    new_rn->weight = rn->weight;
    new_rn->left = mycopyRopeNode(rn->left);
    new_rn->right = mycopyRopeNode(rn->right);
    return new_rn;
}

RopeTree* mycopyRopeTree(RopeTree* rt) {
    if (!rt)
        return NULL;

    RopeTree* new_rt = makeRopeTree(mycopyRopeNode(rt->root));
    return new_rt;
}

RopeNode* makeRopeNode(const char* str) {
    RopeNode *node = calloc(1, sizeof(RopeNode));
    node->str = str;
    node->weight = 0;
    return node;
}

RopeTree* makeRopeTree(RopeNode* root) {
    RopeTree *tree = calloc(1, sizeof(RopeTree));
    tree->root = root;
    return tree;
}

void printRopeNode(RopeNode* rn) {
    if (!rn)
        return;

    if (!(rn->left) && !(rn->right)) {
        printf("%s", rn->str);
        return;
    }

    printRopeNode(rn->left);
    printRopeNode(rn->right);
}

void printRopeTree(RopeTree* rt) {
    if (rt && rt->root) {
        printRopeNode(rt->root);
        printf("%s", "\n");
    }
}

void debugRopeNode(RopeNode* rn, int indent) {
    if (!rn)
        return;

    for (int i = 0; i < indent; ++i)
        printf("%s", " ");

    if (!strcmp(rn->str, EMPTY))
        printf("# %d\n", rn->weight);
    else
        printf("%s %d\n", rn->str, rn->weight);

    debugRopeNode(rn->left, indent+2);
    debugRopeNode(rn->right, indent+2);
}

int get_total_length(RopeNode *rt) {
    if (rt == NULL)
        return 0;

    if (rt->weight == 0)
        return strlen(rt->str);
    return get_total_length(rt->left)+ get_total_length(rt->right);
}


int getTotalWeight(RopeNode* rt) {
    if (!rt)
        return 0;

    return rt->weight + getTotalWeight(rt->right);
}

int update_weight(RopeNode *node) {
    if (node == NULL) {
        return 0;
    }

    if (node->left == NULL && node->right == NULL) {
        return strlen(node->str);
    }

    if (node->left) {
        if (node->left->right) {
            node->weight = update_weight(node->left) +
                        update_weight(node->left->right) +
                        update_weight(node->left->right->right);
        } else {
            node->weight = update_weight(node->left)
                        + update_weight(node->left->right);
        }

        return node->weight;
    }

    return -1;
}

// am creat un nou arbore a carui radacina are drept
// copil stang radacina arborelui rt1 si drept copil drept
// radacina arborelui rt2; am actualizat greutatea noii radacini
RopeTree* concat(RopeTree* rt1, RopeTree* rt2) {
    RopeNode *new_root = makeRopeNode(strdup(EMPTY));
    RopeTree *new_tree = makeRopeTree(new_root);
    new_tree->root->left = rt1->root;
    new_tree->root->right = rt2->root;
    update_weight(new_tree->root);
    return new_tree;
}

// am parcurs arborele pana am ajuns la frunze
// cand am ajuns acolo, intorc caracterul ce se afla pe pozitia idx
char nodeIndexRope(RopeNode *rad, int idx) {
    if (rad->weight <= idx && rad->right) {
        return nodeIndexRope(rad->right, idx - rad->weight);
    } else if (rad->left) {
        return nodeIndexRope(rad->left, idx);
    }
    return rad->str[idx];
}

// am parcurs arborele pana am ajuns la frunze
// cand am ajuns acolo, intorc caracterul ce se afla pe pozitia idx
char indexRope(RopeTree* rt, int idx) {
    RopeNode *rad = rt->root;
    if (rad->weight <= idx && rad->right) {
        return nodeIndexRope(rad->right, idx - rad->weight);
    } else if (rad->left) {
        return nodeIndexRope(rad->left, idx);
    }

    return -1;
}

// am folosit functioa indexRope in intervalul dat
// si am creat un vector de litere determinand fiecare litera
// prin apelul functiei de mai sus
char* search(RopeTree* rt, int start, int end) {
    int i = 0;
    char *res = calloc((end - start + 1), sizeof(char));
    while (start != end) {
        res[i] = indexRope(rt, start);
        i++;
        start++;
    }
    return res;
}


void debug_current_node(RopeNode *node) {
    printRopeNode(node);
}

void print_index(RopeNode *node, int index) {
    printf("index: %d\n", index);
    printf("potential index difference: %d\n", index - node->weight);
}

RopeTree *splitNode(RopeTree *rt, RopeNode *node, int index) {
    // daca suntem in cazul in care unul din copii este o frunza
    if ((node->left != NULL && node->left->weight == 0 &&
        node->weight > index) || (node->right != NULL &&
        node->right->weight == 0 && node->weight < index)) {
        RopeNode *leaf;
        int correct_index;
        if (node->left->weight == 0 && node->weight > index) {
            // setam frunza intr o noua variabila
            // si modificam index-ul daca e cazul
            leaf = node->left;
            correct_index = index;
        } else {
            leaf = node->right;
            correct_index = index - node->weight;
        }

        // extragem cele 2 substring-uri din nod

        char *new_string = (char *)malloc((correct_index) * sizeof(char) + 1);
        char *end_string = (char *)malloc((strlen(leaf->str) -
                                    correct_index + 1) * sizeof(char) + 1);

        strncpy(new_string, leaf->str, correct_index);
        strcpy(end_string, &leaf->str[correct_index]);
        new_string[correct_index] = '\0';

        // cream cele 2 noduri auxiliare si parintele acestora
        RopeNode *first = makeRopeNode(new_string);
        RopeNode *second = makeRopeNode(end_string);
        RopeNode *new_node = makeRopeNode(strdup(EMPTY));

        new_node->left = first;
        new_node->right = second;
        new_node->weight = strlen(new_string);

        // dam free vechiului nod de unde am extras
        // substring-urile si setam legatura cu parintele creat
        if (node->left->weight == 0 && node->weight > index) {
            free((void *)node->left->str);
            free(node->left);
            node->left = new_node;
        } else {
            free((void *)node->right->str);
            free(node->right);
            node->right = new_node;
        }
    }

    // daca nodul din stanga are nodul din dreapte
    // de la index-ul de la care trebuie sa "taiem"
     if (node->left && node->left->weight == index) {
        // cream un nou tree din subarborele drept
        // de la indexul cautat + "fratele drept" al nodului stang
        RopeNode *parent = node->left;
        RopeNode *right_child = parent->right;
        parent->right = NULL;
        if (node->right) {
            RopeTree *right_cutted_subtree = makeRopeTree(node->right);
            node->right = NULL;
            RopeTree *orphanNodes = makeRopeTree(right_child);
            RopeTree *concatenated_tree = concat(orphanNodes,
                                        right_cutted_subtree);
            free(right_cutted_subtree);
            free(orphanNodes);

            return concatenated_tree;
        } else {
            RopeTree *orphanNodes = makeRopeTree(right_child);
            return orphanNodes;
        }

        // daca nodul din dreapta are nodul din
        // dreapta de la index-ul de la care trebuie sa taiem
    } else if (node->right != NULL &&
            node->right->weight == index - node->weight) {
        RopeNode *parent = node->right;
        RopeNode *right_child = parent->right;
        parent->right = NULL;
        RopeTree *orphanNodes = makeRopeTree(right_child);
        return orphanNodes;
    } else if (node->weight > index) {
        if (node->right) {
            // am un tree auxiliar cu ce urmeaza sa tai din dreapta
            RopeTree *right_cutted_subtree = makeRopeTree(node->right);
            // tai intotdeauna legatura dreapta daca merg la stanga
            node->right = NULL;
            // copacul obtinut dupa gasirea elementului de pe index
            RopeTree *orphan_tree = splitNode(rt, node->left, index);
            // crearea unui nou copac
            RopeTree *concatenated_tree = concat(orphan_tree,
                                        right_cutted_subtree);

            free(right_cutted_subtree);
            free(orphan_tree);

            return concatenated_tree;

        } else {
            // tai intotdeauna legatura dreapta daca merg la stanga
            node->right = NULL;
            // copacul obtinut dupa gasirea elementului de pe index
            RopeTree *orphan_tree = splitNode(rt, node->left, index);
            return orphan_tree;
        }
    } else if (node->weight < index) {
        RopeTree *orphan_tree = splitNode(rt, node->right,
                                        index - node->weight);
        return orphan_tree;
    }

    return NULL;
}



SplitPair split(RopeTree* rt, int idx) {
    RopeTree *my_rt = mycopyRopeTree(rt);

    if (idx == 0) {
        RopeNode *new_root = makeRopeNode(strdup(EMPTY));
        RopeTree* new_tree = makeRopeTree(new_root);
        SplitPair sp;
        sp.left = new_tree->root;
        sp.right = my_rt->root;
        free(my_rt);
        free(new_tree);
        return sp;
    }


    RopeTree *new_rope_tree;
    if (my_rt->root->weight == idx) {
        RopeNode *right_node = my_rt->root->right;
        my_rt->root->right = NULL;
        new_rope_tree = makeRopeTree(right_node);
    } else {
        new_rope_tree = splitNode(my_rt, my_rt->root, idx);
    }

    SplitPair sp;
    sp.left = my_rt->root;
    sp.right = new_rope_tree->root;
    update_weight(my_rt->root);
    update_weight(new_rope_tree->root);
    free(new_rope_tree);

    free(my_rt);
    return sp;
}

RopeTree* insert(RopeTree* rt, int idx, const char* str) {
    SplitPair sp;
    sp = split(rt, idx);
    RopeTree *rp1 = makeRopeTree(sp.left);

    RopeNode *new_rp_root = makeRopeNode(str);
    RopeTree *new_rp = makeRopeTree(new_rp_root);

    RopeTree *rp2 = makeRopeTree(sp.right);

    RopeTree *rp_concat1 = concat(rp1, new_rp);
    free(rp1);
    free(new_rp);

    RopeTree *rp_concat2 = concat(rp_concat1, rp2);
    free(rp_concat1);
    free(rp2);

    return rp_concat2;
}

RopeTree* delete(RopeTree* rt, int start, int len) {
    RopeTree *my_rt = mycopyRopeTree(rt);

    if (start == 0) {
        SplitPair sp2 = split(my_rt, len);
        free(my_rt);
        RopeTree *final_tree = makeRopeTree(sp2.right);
        return final_tree;
    }

    SplitPair sp1;
    sp1 = split(my_rt, start);

    RopeTree *rp1 = makeRopeTree(sp1.left);
    RopeTree *rp2 = makeRopeTree(sp1.right);

    if (rp2->root->weight == 0) {
        RopeNode *aux_node = makeRopeNode(strdup(EMPTY));
        aux_node->left = rp2->root;
        aux_node->weight = strlen(rp2->root->str);
        rp2->root = aux_node;
    }

    SplitPair sp2;

    int max_len = get_total_length(rt->root);
    if (start + len > max_len) {
        len = max_len - start;
    }

    sp2 = split(rp2, len);
    free(rp2);

    RopeTree *rp3 = makeRopeTree(sp2.right);

    RopeTree *final_tree = concat(rp1, rp3);

    fflush(stdout);
    free(rp1);
    free(rp3);
    myfreeRopeTree(my_rt);
    free(my_rt);
    return final_tree;
}

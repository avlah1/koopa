#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/CommandChain.h"


bool CommandChain_Allocate(CommandChain** chain_ret) {
  CommandChain* chain = (CommandChain*) malloc(sizeof(CommandChain));
  if (chain == NULL) {
    perror("malloc failed in CommandChain_Allocate");
    return false;
  }
  chain->head = chain->tail = NULL;
  chain->num_commands = 0;
  *chain_ret = chain;
  return true;
}

bool CommandChain_Append(CommandChain* chain, Command* cmd) {
  if (chain->num_commands == 0) {
    chain->head = chain->tail = cmd;
  } else {
    chain->tail->next = (struct Command*) cmd;
    chain->tail = cmd;
  }
  chain->num_commands++;
  return true;
}

void CommandChain_Free(CommandChain* chain) {
  Command* curr = chain->head;
  while (curr != NULL) {
    struct Command* next = curr->next;
    CommandChain_FreeCommand(curr);
    curr = (Command*) next;
  }
  free(chain);
}

void CommandChain_FreeCommand(Command* cmd) {
  int num_args = cmd->num_args;
  if (cmd->args != NULL) {
    for (int i = 0; i < num_args; i++) {
      free(cmd->args[i]);
    }
  }
  if (cmd->input_file != NULL) {
    free(cmd->input_file);
  }
  if (cmd ->output_file != NULL) {
    free(cmd->output_file);
  }
  free(cmd->args);
  free(cmd);
}

void CommandChain_PrintChain(CommandChain* chain) {
  printf("head: %p, with %d commands\n", chain->head, chain->num_commands);
  Command* curr = chain->head;
  while (curr != NULL) {
    printf("%d args:\n", curr->num_args);
    for (int i = 0; i < curr->num_args; i++) {
      printf("  arg[%d]: %s\n", i, curr->args[i]);
    }
    printf("input_file: %s\n", curr->input_file);
    printf("output_file: %s\n", curr->output_file);
    printf("append status: ");
    if (curr->append) {
      printf("true\n");
    } else {
      printf("false\n");
    }
    printf("cond op: ");
    if (curr->cond_op == OP_AND) {
      printf("and\n");
    } else if (curr->cond_op == OP_OR) { 
      printf("or\n");
    } else if (curr->cond_op == OP_SEP) {
      printf("unconditional\n");
    } else {
       printf("none\n");
    }
    printf("piped: ");
    if (curr->pipe_next) {
      printf("true\n");
    } else {
      printf("false\n");
    }
    curr = (Command*) curr->next;
  }
}
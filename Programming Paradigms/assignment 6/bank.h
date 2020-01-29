#ifndef _BANK_H
#define _BANK_H

#include <semaphore.h>

typedef struct Bank {
  unsigned int numberBranches;
  struct Branch *branches;
  struct Report *report;
  int activeWorkers; // to keep track of how many workers have to finish their job until daily report is done
  sem_t checkHelper; // helps checking happen atomically
  sem_t* workersFinished; // keeps track of how many workers are ready to start a new day
  sem_t transferPadlock; //to make transfers happen atomically
} Bank;

#include "account.h"

int Bank_Balance(Bank *bank, AccountAmount *balance);

Bank *Bank_Init(int numBranches, int numAccounts, AccountAmount initAmount, AccountAmount reportingAmount, int numWorkers);

int Bank_Validate(Bank *bank);

int Bank_Compare(Bank *bank1, Bank *bank2);

#endif /* _BANK_H */
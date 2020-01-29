#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include "teller.h"
#include "account.h"
#include "error.h"
#include "debug.h"

void lockThreads(Account* GreaterAccount, Account* LesserAcount, Bank* bank, short sameBranches);
void unlockThreads(Account* GreaterAccount, Account* LesserAcount, Bank* bank, short sameBranches);

/*
 * deposit money into an account
 */
int Teller_DoDeposit(Bank *bank, AccountNumber accountNum, AccountAmount amount){
  assert(amount >= 0);
  DPRINTF('t', ("Teller_DoDeposit(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));
  Account *account = Account_LookupByNumber(bank, accountNum);
  if (account == NULL) return ERROR_ACCOUNT_NOT_FOUND;
  lockThreads(account, NULL, bank, 0); //we must protect account before changing its state
  Account_Adjust(bank, account, amount, 1);
  unlockThreads(account, NULL, bank, 0); //unlocking threads 
  return ERROR_SUCCESS;
}

/*
 * withdraw money from an account
 */
int Teller_DoWithdraw(Bank *bank, AccountNumber accountNum, AccountAmount amount){ 
  assert(amount >= 0);
  DPRINTF('t', ("Teller_DoWithdraw(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));
  Account *account = Account_LookupByNumber(bank, accountNum);
  if (account == NULL) return ERROR_ACCOUNT_NOT_FOUND;
  lockThreads(account, NULL, bank, 0); //we must protect account before changing its state
  if (amount > Account_Balance(account)) {
    unlockThreads(account, NULL, bank, 0); // so that threads dont stay locked
    return ERROR_INSUFFICIENT_FUNDS;
  }
  Account_Adjust(bank,account, -amount, 1);
  unlockThreads(account, NULL, bank, 0); // so that threads dont stay locked
  return ERROR_SUCCESS;
}

/*
 * do a tranfer from one account to another account
 */
int Teller_DoTransfer(Bank *bank, AccountNumber srcAccountNum, AccountNumber dstAccountNum, AccountAmount amount){
  assert(amount >= 0);
  DPRINTF('t', ("Teller_DoTransfer(src 0x%"PRIx64", dst 0x%"PRIx64
                ", amount %"PRId64")\n",
                srcAccountNum, dstAccountNum, amount));
  Account *srcAccount = Account_LookupByNumber(bank, srcAccountNum);
  if (srcAccount == NULL) return ERROR_ACCOUNT_NOT_FOUND;
  Account *dstAccount = Account_LookupByNumber(bank, dstAccountNum);
  if (dstAccount == NULL) return ERROR_ACCOUNT_NOT_FOUND;
  //if destination and source is same we don't need to change anything
  if(dstAccountNum == srcAccountNum) return ERROR_SUCCESS;
  /*
   * If we are doing a transfer within the branch, we tell the Account module to
   * not bother updating the branch balance since the net change for the
   * branch is 0.
   */
  int updateBranch = !Account_IsSameBranch(srcAccountNum, dstAccountNum);
  if(!updateBranch){
    //threads should be locked according to their accountNum
    //threads with lesser accountNum should be locked first
    if(srcAccountNum < dstAccountNum) lockThreads(srcAccount, dstAccount, bank, 1);
    else lockThreads(dstAccount, srcAccount, bank, 1); //we must protect accounts before changing its state
    if (amount > Account_Balance(srcAccount)) {
      unlockThreads(srcAccount, dstAccount, bank, 1);  // so that threads dont stay locked
      return ERROR_INSUFFICIENT_FUNDS;
    }
    Account_Adjust(bank, srcAccount, -amount, updateBranch);
    Account_Adjust(bank, dstAccount, amount, updateBranch);
    unlockThreads(srcAccount, dstAccount, bank, 1);  // so that threads dont stay locked
    return ERROR_SUCCESS;
  }else{
    BranchID sourceID = AccountNum_GetBranchID(srcAccount->accountNumber);
    BranchID destID = AccountNum_GetBranchID(dstAccount->accountNumber);
    if(sourceID > destID) lockThreads(srcAccount, dstAccount, bank, 0);
    else lockThreads(dstAccount, srcAccount, bank, 0); //we must protect accounts before changing its state
    if (amount > Account_Balance(srcAccount)){
      unlockThreads(srcAccount, dstAccount, bank, 0);  // so that threads dont stay locked
      return ERROR_INSUFFICIENT_FUNDS;
    }
    Account_Adjust(bank, srcAccount, -amount, updateBranch);
    Account_Adjust(bank, dstAccount, amount, updateBranch);
    unlockThreads(srcAccount, dstAccount, bank, 0);  // so that threads dont stay locked
    return ERROR_SUCCESS;
  }
}

//locks threads according to passed information
void lockThreads(Account* GreaterAccount, Account* LesserAccount, Bank* bank, short sameBranches){
  if(LesserAccount == NULL){
    sem_wait(&GreaterAccount->padlock);
    BranchID branchID = AccountNum_GetBranchID(GreaterAccount->accountNumber);
    sem_wait(&bank->branches[branchID].BranchLocker);
  }else{
    sem_wait(&LesserAccount->padlock);
    sem_wait(&GreaterAccount->padlock);
    if(!sameBranches){
      BranchID LesserID = AccountNum_GetBranchID(LesserAccount->accountNumber);
      BranchID GreaterID = AccountNum_GetBranchID(GreaterAccount->accountNumber);
      sem_wait(&bank->branches[LesserID].BranchLocker);
      sem_wait(&bank->branches[GreaterID].BranchLocker);
     }
  }
}

//unlocks threads according to passed information
void unlockThreads(Account* GreaterAccount, Account* LesserAccount, Bank* bank, short sameBranches){
  if(LesserAccount == NULL){
    sem_post(&GreaterAccount->padlock);
    BranchID branchID = AccountNum_GetBranchID(GreaterAccount->accountNumber);
    sem_post(&bank->branches[branchID].BranchLocker);
  }else{
    sem_post(&LesserAccount->padlock);
    sem_post(&GreaterAccount->padlock);
    if(!sameBranches){
      BranchID LesserID = AccountNum_GetBranchID(LesserAccount->accountNumber);
      BranchID GreaterID = AccountNum_GetBranchID(GreaterAccount->accountNumber);
      sem_post(&bank->branches[LesserID].BranchLocker);
      sem_post(&bank->branches[GreaterID].BranchLocker);
    }
  }
}
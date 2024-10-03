#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_SYMBOLS 4
#define NUM_REELS 3
// this is simple usage examle without raylib
const char symbols[NUM_SYMBOLS] = {'.', '*', '?', '@'};
const int payouts[NUM_SYMBOLS] = {2, 5, 10, 20};

typedef struct {
    char name[50];
    int balance;
    int current_bet;
} User;

typedef struct {
    char reels[NUM_REELS];
} SlotMachine;

void initialize_user(User* user) {
    printf("Enter your name: ");
    scanf("%49s", user->name);
    printf("Enter your deposit money: ");
    scanf("%d", &user->balance);
    printf("Enter your bet: ");
    scanf("%d", &user->current_bet);
}

void spin_slot_machine(SlotMachine* machine) {
    for (int i = 0; i < NUM_REELS; i++) {
        int rand_index = rand() % NUM_SYMBOLS;
        machine->reels[i] = symbols[rand_index];
    }
}

void display_reels(SlotMachine* machine) {
    for (int i = 0; i < NUM_REELS; i++) {
        printf("%c ", machine->reels[i]);
    }
    printf("\n");
}



int calculate_payout(SlotMachine* machine, int bet) {
    int payout = 0;

    if (machine->reels[0] == machine->reels[1] && machine->reels[1] == machine->reels[2]) {
        for (int i = 0; i < NUM_SYMBOLS; i++) {
            if (machine->reels[0] == symbols[i]) {
                payout = bet * payouts[i];
                break;
            }
        }
    }
    else if (machine->reels[0] == machine->reels[1]) {
        for (int i = 0; i < NUM_SYMBOLS; i++) {
            if (machine->reels[0] == symbols[i]) {
                payout = bet * payouts[i] / 2;
                break;
            }
        }
    }

    return payout;
}

void update_balance(User* user, int payout) {
    user->balance += payout;
}

void change_bet(User* user) {
    printf("Enter your new bet: ");
    scanf("%d", &user->current_bet);
}


int main() {
    srand(time(NULL));

    User user;
    initialize_user(&user);

    SlotMachine machine;
    char choice[10];

    do {
        spin_slot_machine(&machine);
        display_reels(&machine);

        int payout = calculate_payout(&machine, user.current_bet);
        if (payout > 0) {
            printf("You won %d!\n", payout);
        } else {
            printf("You lost your bet.\n");
            payout = -user.current_bet;
        }
        update_balance(&user, payout);

        printf("Current balance: %d\n", user.balance);
        
        printf("Do you want to spin again? (yes/no/change): ");
        scanf("%s", choice);

        if (strcmp(choice, "change") == 0) {
            change_bet(&user);
        }
    } while ((strcmp(choice, "yes") == 0 || (strcmp(choice, "change") == 0 )) && user.balance > 0);

    printf("Final balance: %d\n", user.balance);
    return 0;
}


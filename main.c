/*************************
Vypracoval: Martin Hric
DSA Spravca pamate
***********************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<time.h>

void *memory = NULL;

void memory_init(void *ptr, unsigned int size){

    if(size%4==1) size=size-1;
    if(size%4==2) size=size-2;             //zaokruhlujem to na 4
    if(size%4==3) size=size-3;

    memset(ptr,0,size);
    memory = ptr;

    *(int*)ptr=size-sizeof(int);                      //zaciatok bloku
    *(int*)(ptr+size -2*sizeof(int))=size-sizeof(int);   //koniec bloku
    *(int*)(ptr+size -sizeof(int))='k';                   //koniec regionu
}

void *memory_alloc(unsigned int size) {

    if (memory == NULL) return NULL;
    if(size==0) return NULL;

    if(size%4==1) size=size+3;
    if(size%4==2) size=size+2;             //zaokruhlujem to na 4
    if(size%4==3) size=size+1;

    void *tptr = memory;

    while(*(int *)tptr != 'k') {
        while ((*(int *) tptr < -2) ) {
            tptr = tptr - *(int *) tptr;
            if(*(int *)tptr == 'k') return NULL;                 //kym hlavicka je zaporna, posuva sa mi to na nasledujuce hlavicky
        }
        if (*(int *) tptr >= size + 2* sizeof(int)) {                           //ak je velkost bloku >= ako potrebujem
            if (*(int *) tptr - size - 2* sizeof(int) <=8){
                size=size+(*(int *) tptr) - size - 2* sizeof(int);             //ak ostane potom velkost <=8, navysi mi to velkost
            }
            if (*(int*)(tptr+size+ 2* sizeof(int))==0){
                unsigned int next_block=*(int *) (tptr + size + 2 * sizeof(int)) = *(int*)tptr - size - 2 * sizeof(int);  // zapisem za tym velkost bloku aky ostane po alokovani
                *(int*)(tptr + *(int*)tptr - sizeof(int))=next_block;
            }

            *(int *) tptr = -size - 2 * sizeof(int);                              // hlavicka
            *(int *) (tptr + size + sizeof(int)) = -size - 2 * sizeof(int);       // paticka
            return tptr ;
        }

        if((*(int*)tptr < size + 2* sizeof(int)) && (*(int*)tptr >0)){          //ak je velkost bloku mensia ako potrebujem
            tptr = tptr + *(int*)tptr;
        }
    }
    return NULL;
}

int memory_check(void *ptr){
    if(*(int*)ptr<-3 && *(int*)(ptr+sizeof(int))==0) return 1;               // ak hodnota v ptr je < - 3 (hlavicka alebo paticka) && hodnota v ptr + sizeof(int) ==0, cize to nieje paticka
    else return 0;
}

int memory_free(void *valid_ptr) {

    if(memory_check(valid_ptr)==0) return 1;

    if (valid_ptr == memory) {                               //ak uvolnujem blok na zaciatku regionu

        if ((*(int *) (valid_ptr - *(int *) valid_ptr) < -2)) {
            *(int *) valid_ptr = -*(int *) valid_ptr;                      //zmena znamienka v hlavicke
            valid_ptr = valid_ptr + *(int *) valid_ptr - sizeof(int);       // posuniem pointer nech ukazuje na paticku
            *(int *) valid_ptr = -*(int *) valid_ptr;                      //paticku nastavim na opacne znamienko
            return 0;
        }

        else if ((*(int *) (valid_ptr - *(int *) valid_ptr) > 2)) {
            unsigned int size_next = (*(int *) (valid_ptr - *(int *) valid_ptr));   //ulozim si tu velkost dalsieho bloku, kery je volny ze ich potom scitam spolu
            *(int *) (valid_ptr - *(int *) valid_ptr - sizeof(int)) = 0;         //zapisem tam 0 nech mi to nerobi bordel
            *(int *) (valid_ptr - *(int *) valid_ptr) = 0;
            *(int *) valid_ptr = -*(int *) valid_ptr + size_next; //hlavicka
            *(int *) (valid_ptr + *(int *) valid_ptr - sizeof(int)) = *(int *) valid_ptr; // paticka
            return 0;
        }
    }
    if ( valid_ptr!=memory && *(int*)(valid_ptr -*(int*)valid_ptr)=='k') {        //ak uvolnujem blok na konci regionu a nieje to aj zaroven zaciatok

        if (*(int *) (valid_ptr - sizeof(int)) > 2) {

            unsigned int size_prev = *(int *) (valid_ptr - sizeof(int));          //pomocne premenne kde si ulozim velkosti danych blokov
            unsigned int size_curr = -*(int *) valid_ptr;
            valid_ptr = valid_ptr - size_prev;                                //posun pointera
            *(int *) valid_ptr += size_curr;
            unsigned int size_new = *(int *) valid_ptr;                       // proste to tam ponastavujem secko
            *(int *) (valid_ptr + size_new - sizeof(int)) = size_new;
            *(int *) (valid_ptr + size_prev - sizeof(int)) = 0;
            *(int *) (valid_ptr + size_prev) = 0;
            return 0;
        }
        else if (*(int *) (valid_ptr - sizeof(int)) < -2) {
            *(int *) valid_ptr = -*(int *) valid_ptr;                      //zmenim znamienko v hlavicke
            valid_ptr = valid_ptr + *(int *) valid_ptr - sizeof(int);       //posun pointra na paticku
            *(int *) valid_ptr = -*(int *) valid_ptr;                     //zmena znamienka
            return 0;
        }
    }

    if(valid_ptr!=memory && *(int*)(valid_ptr -*(int*)valid_ptr)!='k'){   //ak ju uvolnujem hockde v strede

        if ((*(int *) (valid_ptr - *(int *) valid_ptr) < -2) && (*(int *) (valid_ptr -sizeof(int)) < -2)) {
            *(int *) valid_ptr = -*(int *) valid_ptr;                      //zmena znamienka v hlavicke
            valid_ptr = valid_ptr + *(int *) valid_ptr - sizeof(int);       // posuniem pointer nech ukazuje na paticku
            *(int *) valid_ptr = -*(int *) valid_ptr;                      //paticku nastavim na opacne znamienko
            return 0;
        }
        else if ((*(int *) (valid_ptr - *(int *) valid_ptr) > 2) && (*(int *) (valid_ptr -sizeof(int)) < -2)) {
            unsigned int size_next = (*(int *) (valid_ptr - *(int *) valid_ptr));   //ulozim si tu velkost dalsieho bloku, kery je volny ze ich potom scitam spolu
            *(int *) (valid_ptr - *(int *) valid_ptr - sizeof(int)) = 0;           //zapisem tam 0 nech mi to nerobi bordel
            *(int *) (valid_ptr - *(int *) valid_ptr) = 0;
            *(int *) valid_ptr = -*(int *) valid_ptr + size_next;                  //hlavicka
            *(int *) (valid_ptr + *(int *) valid_ptr - sizeof(int)) = *(int *) valid_ptr;       // paticka

            return 0;
        }

        if (*(int *) (valid_ptr - sizeof(int)) > 2 && (*(int *) (valid_ptr - *(int *) valid_ptr) < -2)) {
            unsigned int size_prev = *(int *) (valid_ptr - sizeof(int));          //pomocne premenne kde si ulozim velkosti danych blokov
            unsigned int size_curr = -*(int *) valid_ptr;
            valid_ptr = valid_ptr - size_prev;                                //posun pointera
            *(int *) valid_ptr += size_curr;
            unsigned int size_new = *(int *) valid_ptr;                          // proste to tam ponastavujem secko
            *(int *) (valid_ptr + size_new - sizeof(int)) = size_new;
            *(int *) (valid_ptr + size_prev - sizeof(int)) = 0;
            *(int *) (valid_ptr + size_prev) = 0;
            return 0;
        }

        else if ((*(int *) (valid_ptr - sizeof(int)) > 2) && (*(int *) (valid_ptr - *(int *) valid_ptr) > 2)) { //posledna situacia ak oba susedne bloky su volne
           unsigned int size_next = *(int*)(valid_ptr - *(int*)valid_ptr);
           unsigned int size_prev = *(int*)(valid_ptr - sizeof(int));                 //pomocne premenne kde budu ulozene velkosti
           unsigned int size_curr = - *(int*)valid_ptr;
           unsigned int size_new = size_prev + size_curr + size_next;
           valid_ptr = valid_ptr - size_prev;
           *(int*)valid_ptr = size_new;                                                // hlavicka
           *(int*)(valid_ptr + size_new - sizeof(int))= size_new;                     //paticka
           *(int*)(valid_ptr + size_prev - sizeof(int))=0;
           *(int*)(valid_ptr + size_prev )=0;
           *(int*)(valid_ptr + size_prev + size_curr - sizeof(int))=0;               // nastavim cely blok na 0 aby mi tam nic nespatilo
           *(int*)(valid_ptr + size_prev + size_curr)=0;
           return 0;
        }
    }
    return 1;
}

/*TEST1 - PAMAT 50 - ROVNAKE BLOKY 8 A ICH UVOLNENIE, POTOM ALOKOVANIE 24, 8, 8, UVOLNENIE 24 A ALOKOVANIE ZASE 8*/
void test1(void)
{
    char region[50];
    memory_init(region, 50);

    char* list[5];
    int x;
    for (int i = 0; i < 5; i++) {
        list[i] = (char*)memory_alloc(8);//tu
    }
    for (int i = 0; i < 5; i++) {
        if (list[i] != NULL) x=memory_free(list[i]);
    }

    char *pointer = (char *) memory_alloc(24);
    x=memory_check(pointer);
    x=memory_check(region+200);
    x=memory_check(pointer+1);
    x =memory_free(pointer);
    x=memory_check(pointer);
    x=memory_check(pointer+1);
    pointer = (char *) memory_alloc(24);
    char *pointer2 = (char *) memory_alloc(1);
    char *pointer3 = (char *) memory_alloc(8);
    char *pointer4 = (char *) memory_alloc(8);

    x =memory_free(pointer);
    pointer = (char *) memory_alloc(8);
    char *pointer6 = (char *) memory_alloc(8);

}

/*TEST2 - PAMAT 500 - BLOKY NAHODNEJ VELKOSTI(8-24) A ICH UVOLNENIE, POTOM ALOKOVANIE 80*/
void test2(void){
    char region[500];
    memory_init(region, 500);
    char* list[30];
    srand(time(0));
    int x ;
    for (int i = 0; i < 30; i++) {
        x = rand() % 16 + 8;
        list[i] = (char*)memory_alloc(x);
    }

    for (int i = 0; i < 30; i++) {
        if (list[i] != NULL)
            x = memory_free(list[i]);
    }

    char* pointer = (char*)memory_alloc(450);
}

/*TEST3 - PAMAT 2000 - BLOKY NAHODNEJ VELKOSTI(300-1000) A ICH UVOLNENIE, POTOM ALOKOVANIE 1500*/
void test3(void){
    char region[2000];
    memory_init(region,2000);
    char *list[10];
    srand(time(0));
    int x;
    for (int i = 0; i < 10; i++) {
        x = rand() % 700 + 300;
        list[i] = (char*)memory_alloc(x);
    }
    for (int i = 0; i < 10; i++) {
        if (list[i] != NULL)
            x = memory_free(list[i]);
    }
    char* pointer = (char*)memory_alloc(1500);
}

/*TEST3 - PAMAT 1000 - BLOKY NAHODNEJ VELKOSTI(8-500) A ICH UVOLNENIE*/
void test4(void){
    char region[1000];
    memory_init(region,1000);
    char *list[10];
    srand(time(0));
    int x;
    for (int i = 0; i < 10; i++) {
        x = rand() % 492 + 8;
        list[i] = (char*)memory_alloc(x);
    }
    for (int i = 0; i < 10; i++) {
        if (list[i] != NULL)
            x = memory_free(list[i]);
    }
}

int main() {
    char region[50];
    memory_init(region,50);
    char *p1=memory_alloc(32);
    /*test1();
    test2();
    test3();
    test4();*/
    return 0;
}

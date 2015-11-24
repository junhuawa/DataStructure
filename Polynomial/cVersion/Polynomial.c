#include <stdio.h>
#include <stdlib.h>
typedef struct Node *PtrToNode;
struct Node  {
    int Coefficient;
    int Exponent;
    PtrToNode Next;
};

typedef PtrToNode Polynomial;

Polynomial Read(); /* details omitted */
void Print( Polynomial p ); /* details omitted */
Polynomial Add( Polynomial a, Polynomial b );


int DestroyPolynomial(PtrToNode Ptr);

int main()
{
    Polynomial a, b, s;
    a = Read();
    b = Read();
    s = Add(a, b);
    Print(s);
    DestroyPolynomial(a);
    DestroyPolynomial(s);
    DestroyPolynomial(b);
    return 0;
}


#include <stdbool.h>

int DestroyPolynomial(PtrToNode Ptr){
    PtrToNode p, tmpPtr;

    p = Ptr;
    while(p){
        tmpPtr = p->Next;
        free(p);
        p = tmpPtr;
    }
    return 0;
}

bool CreatePolynomial(PtrToNode ptr, int num){
    int coef;
    int expo;
    PtrToNode p = ptr;
    PtrToNode tmpPtr = NULL ;
    while(num > 0){
        scanf("%d %d", &coef, &expo);

        tmpPtr = (PtrToNode)malloc(sizeof(struct Node)); 
        if(!tmpPtr){
            printf("malloc failed\n");
            DestroyPolynomial(ptr->Next);
            ptr->Next = NULL;
            return false;
        }
        tmpPtr->Coefficient = coef;
        tmpPtr->Exponent = expo;
        tmpPtr->Next = NULL;
        p->Next = tmpPtr; 
        p = p->Next;
        num --;
    }
    return true;
}

Polynomial Initialize(void);

Polynomial Read(){
    int num = 0;

    Polynomial ptr = Initialize();

    scanf("%d", &num);
    if(num < 0) {
        free(ptr);
        return NULL;
    }

    if(num == 0) {
        return ptr;
    }

    if(CreatePolynomial(ptr, num) == false) {
        free(ptr);
        return NULL;
    }
    return ptr;
}

void Print(Polynomial p){
	PtrToNode ptr;
	int count  = 0;

	if(!p){
		return;
	}

    if(!p->Next) {
        printf("0 0\n");
        return;
    }

	ptr = p->Next;
	while(ptr){
		printf("%d %d", ptr->Coefficient, ptr->Exponent);
		ptr = ptr->Next;
		count ++;
		if(ptr){
			printf(" ");
		}
	}
	printf("\n");
}

PtrToNode dupNode(PtrToNode p){
	PtrToNode ptr = NULL;

	ptr = (PtrToNode)malloc(sizeof(struct Node)); 
	if(!ptr){
		printf("malloc failed\n");
        exit(1);
	}
	ptr->Coefficient = p->Coefficient;	
	ptr->Exponent = p->Exponent;
	ptr->Next = NULL;

	return ptr;
}

Polynomial Initialize(void)
{
    Polynomial ptr = NULL;

    ptr = (PtrToNode)malloc(sizeof(struct Node));
    if(!ptr){
        printf("malloc failed\n");
        exit(-1);
    }
    ptr->Next= NULL;
    ptr->Coefficient = -1;
    ptr->Exponent = -1;

    return ptr;
}

void Attach(int coef, int expo, PtrToNode *ptr)
{
    PtrToNode tmp;
    tmp = (PtrToNode)malloc(sizeof(struct Node));
    if(!tmp){
        printf("malloc failed\n");
        exit(-1);
    }
    tmp->Next= NULL;
    tmp->Coefficient = coef;
    tmp->Exponent = expo;

    (*ptr)->Next = tmp;
    (*ptr) = (*ptr)->Next;
}

Polynomial Add(Polynomial Ptra, Polynomial Ptrb){
    PtrToNode p1, p2, tmpPtr, p;

    if((!Ptra) || (!Ptrb)) return NULL;

    tmpPtr = Initialize();

    p1 = Ptra->Next;
    p2 = Ptrb->Next;
    p = tmpPtr;

    while(p1 || p2) {
        if(p1 && p2) {
            if(p1->Exponent == p2->Exponent) {
                if(p1->Coefficient + p2->Coefficient != 0) {
                    Attach(p1->Coefficient+p2->Coefficient, p1->Exponent, &p);
                }
                p1 = p1->Next;
                p2 = p2->Next;
                continue;
            }

            if(p1->Exponent > p2->Exponent) {
                Attach(p1->Coefficient, p1->Exponent, &p);
                p1 = p1->Next;
                continue;
            }

            Attach(p2->Coefficient, p2->Exponent, &p);
            p2 = p2->Next;
            continue;
        }

        if(p1) {
            Attach(p1->Coefficient, p1->Exponent, &p);
            p1 = p1->Next;
            continue;
        }else if(p2) {
            Attach(p2->Coefficient, p2->Exponent, &p);
            p2 = p2->Next;
            continue;
        }
    }
    p->Next = NULL;
    return tmpPtr;
}

/*
#define COMPARE(numa,numb) (numa>numb?-1:(numa<numb?1:0))
void Attach(int coefficient, int exponent, PtrToNode * ptr) 
{
    PtrToNode temp; 

    temp = (PtrToNode)malloc(sizeof(struct Node)); 
    if (temp == NULL) 
    {
        printf("The memory is full!\n"); 
        exit(1); 
    } 

    temp->Coefficient = coefficient; 
    temp->Exponent = exponent; 
    temp->Next = NULL; 

    (*ptr)->Next = temp; 
    *ptr = temp; 
} 

Polynomial Add(Polynomial polya, Polynomial polyb) 
{ 
    PtrToNode front, tail; 
    int sum; 

    tail = (PtrToNode)malloc(sizeof(struct Node)); 
    if (tail == NULL) 
    { 
        printf("The memory is full!\n"); 
        exit(1); 
    } 

    front = tail; 
    front->Exponent = -1; 

    polya = polya->Next; 
    polyb = polyb->Next; 

    while (polya && polyb) 
    { 

        switch (COMPARE(polya->Exponent, polyb->Exponent)) 
        { 
            case -1: 
                Attach(polya->Coefficient, polya->Exponent, &tail); 
                polya = polya->Next; 
                break; 
            case 1: 
                Attach(polyb->Coefficient, polyb->Exponent, &tail); 
                polyb = polyb->Next; 
                break; 

            case 0: 
                sum = polya->Coefficient + polyb->Coefficient;  
                if (sum) 
                    Attach(sum, polya->Exponent, &tail); 
                polya = polya->Next; 
                polyb = polyb->Next; 
                break; 

        } 
    } 

    if(polya) {
        for (;polya; polya = polya-> Next ) 
            Attach(polya-> Coefficient, polya-> Exponent, &tail); 
    }
    if(polyb) {
        for (;polyb; polyb = polyb-> Next ) 
            Attach(polyb-> Coefficient, polyb-> Exponent, &tail); 
    }

    tail->Next = NULL; 

    return  front; 
}

*/

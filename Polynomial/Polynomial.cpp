#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>


using namespace std;

typedef struct Node *PtrToNode;

struct Node{
    int Coefficient;
    int Exponent;
    PtrToNode Next;
};

bool DestroyPolynomial(PtrToNode Ptr);

class Polynomial{
    struct Node header;
    public:
    Polynomial(){
        header.Coefficient = -1;
        header.Exponent = -1;
        header.Next = NULL;
    }

    Polynomial(Polynomial &poly);

    ~Polynomial(){
        DestroyPolynomial(header.Next);
        header.Next = NULL;
        header.Coefficient = -1;
        header.Exponent = -1;
    }
    bool Initialize(void);
    void Print(void);

    bool CreatePolynomial(int num);
    PtrToNode GetHeader(void);
};

PtrToNode dupNode(PtrToNode p);

Polynomial::Polynomial(Polynomial &poly){
    PtrToNode p, tmpPtr;
    p = poly.GetHeader()->Next;
    tmpPtr = &header;

    header.Next = NULL;
    header.Coefficient = -1;
    header.Exponent = -1;

    while(p){
        tmpPtr->Next = dupNode(p);
        tmpPtr= tmpPtr->Next;
        p = p->Next;
    }
    tmpPtr->Next = NULL;
}

bool DestroyPolynomial(PtrToNode Ptr){
    PtrToNode p, tmpPtr;

    p = Ptr;
    while(p){
        tmpPtr = p->Next;
        free(p);
        p = tmpPtr;
    }
    return true;
}

PtrToNode Polynomial::GetHeader(void){
	return &header;
}

bool Polynomial::CreatePolynomial(int num){
	int coef;
	int expo;

	PtrToNode p = &header;
	PtrToNode tmpPtr = NULL ;

	while(num > 0){
		scanf("%d %d", &coef, &expo);

		tmpPtr = (PtrToNode)malloc(sizeof(struct Node)); 
		if(!p){
			printf("malloc failed\n");
			DestroyPolynomial(header.Next);
			header.Next = NULL;
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

bool Polynomial::Initialize(void){
    int num = 0;
    header.Next= NULL;
    header.Coefficient = -1;
    header.Exponent = -1;

    scanf("%d", &num);
    if(num <= 0) 
        return false;

    CreatePolynomial(num);
    return false;
}

void Polynomial::Print(void){
    PtrToNode ptr = header.Next;
    int count  = 0;

    if(!ptr){
        cout<<"0 0";
    }
    while(ptr){
        cout<<ptr->Coefficient<<" "<<ptr->Exponent;
        ptr = ptr->Next;
        if(ptr){
            cout<<" ";
        }
        count ++;
    }
    cout<<endl;
    //cout<<"The Polynomial have "<<count<<" members!"<<endl;
}

PtrToNode dupNode(PtrToNode p){
    PtrToNode ptr = NULL;

    ptr = (PtrToNode)malloc(sizeof(struct Node)); 
    if(!ptr){
        printf("malloc failed\n");
    }
    ptr->Coefficient = p->Coefficient;	
    ptr->Exponent = p->Exponent;
    ptr->Next = NULL;

    return ptr;
}

Polynomial * Add(Polynomial &poly1, Polynomial &poly2) {
    PtrToNode p1, p2, tmpPtr;
    Polynomial *PolyNewPtr = new Polynomial;

    p1 = poly1.GetHeader()->Next;
    p2 = poly2.GetHeader()->Next;
    tmpPtr = PolyNewPtr->GetHeader();

    if((!p1) && (!p2)) return PolyNewPtr;
    while((p1) ||(p2)){
        if(p1 && p2){
            if(p1->Exponent == p2->Exponent){
                if(p1->Coefficient + p2->Coefficient != 0) {
                    tmpPtr->Next = dupNode(p1);
                    (tmpPtr->Next)->Coefficient += p2->Coefficient;
                    tmpPtr = tmpPtr->Next;
                }
                p1 = p1->Next;
                p2 = p2->Next;
                continue;
            }

            if(p1->Exponent > p2->Exponent){
                tmpPtr->Next = dupNode(p1);
                tmpPtr = tmpPtr->Next;
                p1 = p1->Next;
                continue;
            }

            tmpPtr->Next = dupNode(p2);
            tmpPtr= tmpPtr->Next;
            p2 = p2->Next;
            continue;
        }		

        if(p1){
            tmpPtr->Next = dupNode(p1);
            tmpPtr= tmpPtr->Next;
            p1 = p1->Next;
            continue;
        }

        if(p2){
            tmpPtr->Next = dupNode(p2);
            tmpPtr= tmpPtr->Next;
            p2 = p2->Next;
            continue;
        }
    }
    tmpPtr->Next = NULL;
    return PolyNewPtr;
}

Polynomial * MultiplyOneItem(int Coef, int Expo, Polynomial &Poly) {
    Polynomial *PolyPtr = new Polynomial(Poly); 
	PtrToNode ptr = PolyPtr->GetHeader()->Next;

	while(ptr){
		ptr->Coefficient *= Coef;
		ptr->Exponent += Expo;
		//cout<<ptr->Coefficient<<" "<<ptr->Exponent<<endl;
		ptr = ptr->Next;
	}

	return PolyPtr;
}

Polynomial * Multiply(Polynomial &poly1, Polynomial &poly2) {

    Polynomial *PolyPtr = new Polynomial; 
    Polynomial *tmpPtr = NULL; 
    Polynomial *sumPtr= PolyPtr; 
	PtrToNode ptr = (poly2.GetHeader())->Next;

	while(ptr){
		tmpPtr = MultiplyOneItem(ptr->Coefficient, ptr->Exponent, poly1); 
        sumPtr = Add(*PolyPtr, *tmpPtr);
        delete PolyPtr;
        delete tmpPtr;
        PolyPtr = sumPtr;
		ptr = ptr->Next;
	}

	return PolyPtr;
}

int main(void){
    Polynomial Poly1;
    Polynomial Poly2;
    Polynomial *PolyPtr;

    Poly1.Initialize();
    Poly2.Initialize();

    PolyPtr = Multiply(Poly1, Poly2);
    PolyPtr->Print();
    delete PolyPtr;

    PolyPtr = Add(Poly1, Poly2);
    PolyPtr->Print();
    delete PolyPtr;

    return 0;
}

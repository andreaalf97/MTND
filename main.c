// Lettura della transizioni
//tr
//0 a c R 1
//...
//2 b d L 3
//acc

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define NCARATTERI 256

typedef struct transizioni_s {
	int inizio, fine;
	char letto, scritto, mossa;
} transizione;

typedef struct listaTransizioni_s {
	struct listaTransizioni_s *next;	//puntatore alla transizione successiva
	char scritto, mossa;							//SCRITTO = carattere da scrivere; MOSSA = R, L o STOP
	int fine;													//stato in cui andare
}	listaTr;

typedef struct listaInt_s {
	struct listaInt_s *next;
	int pid;
}	listaInt;

typedef struct nastro_s {
	char *left, *right;
	int dimLeft, dimRight;
	listaInt *whoShares;
} nstr;

typedef struct processo_s {
	nstr nastro;
	int testina;
	int stato;
} processo;

void stampaTransizione(transizione);
void stampaNastro(nstr, int);
int pos(int, int, int);			//int pos(int i, int j, int height);
//funzione che ritorna l'elemento nella cella <i, j> in una matrice alta height
listaTr *push(listaTr *, transizione);
//funzione che aggiunge in testa alla lista in posizione <i, j> la transizione specificata nel secondo parametro
void stampaLista(listaTr *);
//semplice funzione che stampa la lista in input
char executeMachine(listaTr **, int, int, bool *, int, char *);


//char eseguiMacchina(listaTr **, char *, int *, int, int);
//char eseguiMacchina(listaTr **matrice, char * stringaInput, int * statiAccettazione, int nStati, int maxTransizioni)
//funzione che rivece in ingresso tutte le specifiche della macchina di Touring richiesta e stampa '1', '0', o 'U' a seconda che
//la macchina accetti la stringa in ingresso, lo rifiuti o non termini.

int main(int argc, char *argv[])
{
	// FASE DI INPUT
	int i, j;

	char *temp;	//Qui salvo le singole linee di input per lavorarci sopra
	size_t llinea = 0;	//dimensione della singola linea, da aggiornare ogni volta

	//variabili per leggere la prima parte di input
	//vett e' il vettore che contiene tutte le transizioni
	transizione *vett = (transizione *)malloc(2 * sizeof(transizione));	//Alloco subito 2 transizioni
	size_t dimVett = 2;								//DIMENSIONE DELL'ARRAY
	int nTransizioni = 0;								//PRIMA CELLA LIBERA

	//variabili per trovare lo stato piu' grande
	int statoMassimo = 0;

	//Variabili per la creazione della tabella che rappresenta la macchina di Touring del file
	listaTr **matrice;
	int dim, posizione;

	//variabili per riconoscere gli stati di accettazione
	bool *statiAccettazione;
	int t;

	//variabili per il numero massimo di mosse effettuabili
	int max;

	llinea = getline(&temp, &llinea, stdin);
	//Prende una linea intera dallo stdin e la mette nel vettore temp
	//alloca automaticamente la memoria e salva in n la lunghezza dell'array (compreso \n escluso \0)
	//Ora 'temp' contiene tutta la linea compresa del \n e infine il terminatore

	if(strcmp("tr\n", temp) != 0){
		fprintf(stderr, "Il file non inizia per tr\n");
		return -1;
	}	//controlla che il file input inizi per TR

	llinea = getline(&temp, &llinea, stdin);
	while(strcmp(temp, "acc\n")){		//finche' non trova acc
		//Riallocazione di memoria -- Se serve piu' memoria alloco il doppio di quella occupata
		if(nTransizioni >= dimVett){
			vett = (transizione *)realloc(vett, (dimVett * sizeof(transizione)) * 2);
			dimVett *= 2;
		}

		sscanf(temp, "%d%*c%c%*c%c%*c%c%d", &(vett[nTransizioni].inizio), &(vett[nTransizioni].letto), &(vett[nTransizioni].scritto), &(vett[nTransizioni].mossa), &(vett[nTransizioni].fine));
		//Questa sscanf legge la linea e mette i vari parametri al posto giusto

		//qui sotto calcolo quale sia lo stato piu' grande
		if(vett[nTransizioni].inizio > statoMassimo)
			statoMassimo = vett[nTransizioni].inizio;

		if(vett[nTransizioni].fine > statoMassimo)
			statoMassimo = vett[nTransizioni].fine;

		nTransizioni++;
		llinea = getline(&temp, &llinea, stdin);
	}


	//ORA DENTRO A VETT HO TUTTE LE POSSIBILI TRANSIZIONI
	//dimVett contiene la quantita' di memoria occupata da vett
	//nTransizioni contiene il numero di transizioni effettive contenute in vett
	//statoMassimo contiene quale stato e' il piu' grande numericamente, quindi se il piu' grande e' k,
	//esistono sicuramente tutti gli stati 0, 1, ... , k-1,


	//Ora devo creare una matrice di liste in cui per ogni coppia STATO - INPUT
	//ho una sequenza di possibili transizioni

	//Per una matrice MATRIX di dimensione AxB alloco un vettore di dimensione A*B e trovo
	//l'elemento [i][j] come MATRIX[ i*B + j]

	//devo fare una tabella di puntatori a NULL grande = (statoMassimo+1) x NCARATTERI
	dim = (statoMassimo + 1) * NCARATTERI;
	matrice = (listaTr **)malloc(dim * sizeof(listaTr *));
	for(i = 0; i < dim; i++)		//inizializzo a zero la mia matrice delle transizioni
		matrice[i] = NULL;

	//Per ogni transizione che parte dallo stato x leggendo y aggiungo alla lista corrispondente
	//tale transizione.
	for(i = 0; i < nTransizioni; i++){
		posizione = pos(vett[i].inizio, (int)vett[i].letto, NCARATTERI);
		matrice[posizione] = push(matrice[posizione], vett[i]);
		//qui inserisco nella posizione <i, j> = <stato, carattere in input> la transizione
	}

	/*for(i = 0; i < statoMassimo + 1; i++)
		for(j = 0; j < NCARATTERI; j++)
			if(matrice[pos(i, j, NCARATTERI)] != NULL){
				printf("Dallo stato %d, leggendo %c:\n", i, (char)j);
				stampaLista(matrice[pos(i, j, NCARATTERI)]);
			}*/

	//A questo punto dentro la matrice ho tutto cio' che mi serve e posso andare avanti a leggere il file di input
	//Il vettore delle transizioni non serve piu'

	//**********************************************************

		//PRIMA DELLA CHIAMATA A free(vett) HO SIA LA MATRICE DELLE TRANSIZIONI
		//CHE VETT ALLOCATI --> POTREBBERO OCCUPARE TROPPA MEMORIA


	//**********************************************************
	free(vett);



	//Ora devo leggere quali sono gli stati di accettazione
	//getline aveva gia' letto 'acc' quindi con questa chiamata legge il primo numero dopo acc
	statiAccettazione = (bool *)calloc((statoMassimo + 1), sizeof(bool));	//creo un vettore di booleani che dice se lo stato alla posizione 'i' e' di accettazione
	llinea = getline(&temp, &llinea, stdin);
	while(strcmp(temp, "max\n")){	//leggo tutte le linee fino a quando non leggo 'max'
		sscanf(temp, "%d", &t);			//leggo un intero dalla linea appena letta e salvata in 'temp'
		statiAccettazione[t] = 1;		//"pongo" lo stato di accettazione a 1
		llinea = getline(&temp, &llinea, stdin);
	}


	//Ora ho la matrice pronta e un vettore chiamato statiAccettazione che contiene
	//0 o 1 a seconda che la posizione i sia uno stato di accettazione o meno
	//C'e' anche corrispondenza tra la posizione i e la posizione della colonna della matrice

	//Ora leggo il numero massimo di mosse effettuabili
	llinea = getline(&temp, &llinea, stdin);
	sscanf(temp, "%d", &max);

	llinea = getline(&temp, &llinea, stdin);	//ora temp dovrebbe contenere la parola 'run'
	if(strcmp(temp, "run\n") != 0){
		fprintf(stderr, "Non ho letto la parola 'run'\n");
		return -1;
	}

	llinea = getline(&temp, &llinea, stdin);	//leggo una linea di input, che dovrebbe
	//corrispondere proprio alla prima stringa in ingresso
	for(i = 0; temp[i] != '\n'; i++);	//ciclo fino allo \n
	temp[i] = '\0';										//sostituisco lo \n con il terminatore


	//ORA E' TUTTO PRONTO PER L'ESECUZIONE
	//matrice = 						matrice che contiene per ogni i(stato) e j (carattere letto) la lista delle transizioni
	//											che devo possono essere eseguite
	//statiAccettazione = 	vettore di 0 e 1 che e' a 1 solo se i (indice) e' uno stato di accettazione
	//max = 								variabile che contiene il numero massimo di mosse effettuabili dalla macchina di Touring.
	//temp = stringa da porre sul nastro


	while(!feof(stdin)){
		printf("%c\n", executeMachine(matrice, statoMassimo+1, NCARATTERI, statiAccettazione, max, temp));
		llinea = getline(&temp, &llinea, stdin);
		for(i = 0; temp[i] != '\n' && temp[i] != '\0'; i++);
		temp[i] = '\0';
	}



















	free(temp);		//libero temp perche' non devo piu' leggere stringhe dall'input

	//**********************************************************
		//PARTE DA RIVEDERE

		//IN REALTA' TEMP MI SERVIRA' ANCORA!!

	//**********************************************************
	free(statiAccettazione);
	free(matrice);

	//FASE DI OUTPUT
	return 0;
}

void stampaTransizione(transizione t){
	printf("Inizio: %d, fine: %d\n", t.inizio, t.fine);
	printf("Letto: %c, scritto: %c, mossa: %c\n", t.letto, t.scritto, t.mossa);
	printf("*********************\n");
}


int pos(int i, int j, int B) {return ((i * B) + j);}

listaTr *push(listaTr *head, transizione t){
	listaTr *nuovo;
	if(nuovo = (listaTr *)malloc(sizeof(listaTr))){
		nuovo->scritto =	t.scritto;
		nuovo->mossa = t.mossa;
		nuovo->fine = t.fine;
		nuovo->next = head;
		head = nuovo;
	}
	else
		fprintf(stderr, "Errore allocazione memoria lista\n");

	return head;
}

void stampaLista(listaTr *head){
	listaTr *p = head;
	while(p){
		printf("Scritto: %c, Mossa: %c, Vai allo stato %d\n", p->scritto, p->mossa, p->fine);
		p = p->next;
	}
	printf("--------------\n");
}

char executeMachine(listaTr **matrice, int width, int height, bool *statiAccettazione, int max, char *input){
	int i, posizione;


	//FASE DI INIZIALIZZAZIONE:
	//creo un processo iniziale (cioe' una configurazione della MT da cui partire) dal
	//quale forkero' ogni volta che incontro un non determinismo
	//bisogna capire come gestire la copia del nastro in quanto deve essere condiviso
	//nel caso in cui venga modificato da un sottoprocesso
	processo init;
	init.stato = 0;
	init.testina = 0;
	init.nastro.right = (char *)malloc(max * sizeof(char));
	init.nastro.dimRight = max;
	init.nastro.left = (char *)malloc(max * sizeof(char));
	init.nastro.dimLeft = max;

	for(i = 0; i < init.nastro.dimLeft; i++)
		init.nastro.left[i] = '_';
	for(i = 0; input[i] != '\0'; i++)
		init.nastro.right[i] = input[i];
	for(; i < init.nastro.dimRight; i++)
		init.nastro.right[i] = '_';

	stampaNastro(init.nastro, init.testina);

	return '1';

}

void stampaNastro(nstr nastro, int testina) {
	int i;
	for(i = 0; i < nastro.dimLeft; i++)
		printf("%c ", nastro.left[i]);
	for(i = 0; i < nastro.dimRight; i++)
		printf("%c ", nastro.right[i]);
	printf("\n");

	if(testina >= 0){
		for(i = 0; i < nastro.dimLeft; i++)
			printf("  ");
		for(i = 0; i < testina; i++)
			printf("  ");
		printf("^");
	}
	else{
		testina = -testina;
		for(i = 0; i < testina; i++)
			printf("  ");
		printf(" ^");
	}
	printf("\n");
	return;
}

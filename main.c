#define DIMNASTRO 1024

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>


typedef struct transizioni_s {
	unsigned int inizio, fine;
	char letto, scritto, mossa;
} transizione;

typedef struct listaTransizioni_s {
	struct listaTransizioni_s *next;	//puntatore alla transizione successiva
	char scritto, mossa;							//SCRITTO = carattere da scrivere; MOSSA = R, L o STOP
	unsigned int fine;													//stato in cui andare
}	listaTr;

typedef struct nastro_s {
	char *left, *right;
	unsigned int dimLeft, dimRight;
	unsigned int whoShares;
} nstr;

typedef struct processo_s {
	nstr *nastro;
	int testina;
	unsigned int stato;
	unsigned int pid;
	unsigned int nMosseFatte;
} processo;

typedef struct listaProcessi_s {
	struct listaProcessi_s *next;
	processo *p;
} listaProcessi;



//FUNZIONI DI LETTURA INPUT:
transizione *leggiTransizioni(transizione *, unsigned int *, unsigned int *, int *, unsigned int *); //legge le transizioni dallo stdin e le salva in vettoreTransizioni, salvando anche tutti gli altri parametri che deduce leggendo l'input
void creaRigheCaratteri(int *); //trasforma il vettore da [0 / 1] a [-1 / 0, 1, 2, ...]
void leggiStatiAccettazione(bool *); //prende un vettore e pone a 1 v[i] se i è uno stato di accettazione
void leggiMax(unsigned int *);
listaTr **creaMatrice(listaTr **, transizione *, unsigned int, unsigned int, int *, unsigned int);

unsigned int pos(unsigned int, int, int);	//ritorna la posizione <i, j> nella matrice
listaTr *pushTransizione(listaTr *, transizione);	//push nella lista transizioni (per lettura file input)

char executeMachine(listaTr **, unsigned int, bool *, unsigned int, char *, int *, size_t);	//esegue la macchina

processo *createProcess(processo *, int, unsigned int, unsigned int, nstr *);	//crea un nuovo processo
nstr *createNastroInit(nstr *, char *, unsigned int);	//data la stringa input e MAX, costruisce il nastro
listaProcessi *pushListaProcessi(listaProcessi *, processo *);	//push nella lista dei processi attivi
listaProcessi *popListaProcessi(listaProcessi *, processo *);	//pop dalla lista dei processi attivi
void freeElementoListaProcessi(listaProcessi *);
void freeListaProcessi(listaProcessi *);	//elimina la lista dei processi attivi
char carattereLetto(processo *);	//ritorna il carattere puntato dalla testina sul nastro
void scriviSuNastro(processo *, char);	//scrive sul nastro del processo indicato
void muoviTestina(processo *, char, size_t, char *);	//sposta la testina
void copyOwnNastro(processo *);	//crea un nuovo nastro e lo assegna al processo in input, copiando il nastro precedente
listaProcessi *copyProcesso(listaProcessi *, processo *, int, listaTr *, size_t, char *);	//copia un processo

void freeListaTr(listaTr *);








int main(int argc, char *argv[]){
	unsigned int i;

	size_t llinea = 0;
	ssize_t nread;
	char *temp = NULL;	//llinea e temp servono per la lettura dell'input attraverso la funzione 'getline', nread è il numero di caratteri letti
	transizione *vettoreTransizioni = NULL;	//vettore che contiene tutte le transizioni possibili
	unsigned int nTransizioni = 0;	//contatore per il numero di transizioni, e quindi la dimensione di vettoreTransizioni
	unsigned int statoMassimo = 0;	//indica quale stato e' il piu' grande
	int *righeCaratteri = (int *)calloc(256, sizeof(int));	//indica ogni carattere in quale riga della matrice puo' essere trovato [-1 se non presente]
	//prima pero' uso lo stesso vettore per indicare se un carattere e' presente tra quelli possibili o no
	unsigned int nCaratteriPresenti = 0;	//indica quanti caratteri diversi sono presenti

	bool *statiAccettazione = NULL;	//indica ogni stato se lo stato i-esimo e' di accettazione (1) o meno (0)

	unsigned int max;	//indica il numero massimo di mosse effettuabili dalla macchina

	listaTr **matrice = NULL;

	//***********************************************
	vettoreTransizioni = leggiTransizioni(vettoreTransizioni, &nTransizioni, &statoMassimo, righeCaratteri, &nCaratteriPresenti);
	//legge le transizioni dallo stdin e le salva in vettoreTransizioni, salvando anche tutti gli altri parametri che deduce leggendo l'input
	creaRigheCaratteri(righeCaratteri);	//trasforma il vettore dei caratteri presenti in quello che indica ogni carattere a che riga corrisponde
	matrice = creaMatrice(matrice, vettoreTransizioni, nTransizioni, statoMassimo, righeCaratteri, nCaratteriPresenti);//crea la matrice delle transizioni

	free(vettoreTransizioni); //a questo punto non mi serve più il vettore delle transizioni e posso liberarlo
	vettoreTransizioni = NULL;

	statiAccettazione = (bool *)calloc(statoMassimo+1, sizeof(bool));	//alloco un vettore lungo quanto il numero di stati
	leggiStatiAccettazione(statiAccettazione);	//legge gli stati di accettazione e li setta a 1 nel vettore corrispondente

	leggiMax(&max);	//legge max e lo salva




	//********************************************************



	nread = getline(&temp, &llinea, stdin);
	if(strcmp("run\n", temp) != 0){
		fprintf(stderr, "Non ho letto run\n");
		return 0;
	}	//controlla che la riga letta sia run

	while((nread = getline(&temp, &llinea, stdin)) != -1 && temp[0] != '\n'){
		if(temp[nread - 1] == '\n'){
			nread--;
			temp[nread] = '\0';	//sostituisco lo \n con il terminatore
		}
		else
			temp[nread] = '\0';

		printf("%c\n", executeMachine(matrice, nCaratteriPresenti, statiAccettazione, max, temp, righeCaratteri, nread));
	}


	return 0;
}


//*****************************************************************


transizione *leggiTransizioni(transizione *vettoreTransizioni, unsigned int *nTransizioni, unsigned int *statoMassimo, int *caratteriPresenti, unsigned int *nCaratteriPresenti) {
	char *temp = NULL; //per lettura tramite getline
	char blank = '_';	//per leggibilità
	size_t llinea = 0;	//per getline

	unsigned int dimVett = 2;	//dimensione iniziale del vettore delle transizioni
	*nTransizioni = 0;	//numero di transizioni lette
	vettoreTransizioni = (transizione *)malloc(2 * sizeof(transizione)); //alloco subito 2 celle per il vettore della transizioni possibili

	getline(&temp, &llinea, stdin);	//legge la prima linea del file di input
	if(strcmp("tr\n", temp) != 0){
		fprintf(stderr, "Il file non inizia per tr\n");
		return 0;
	}	//controlla che il file input inizi per TR, si potrebbe anche omettere ma tanto è O(1)

	getline(&temp, &llinea, stdin);	//legge la seconda linea dell'input
	while(strcmp(temp, "acc\n")){		//finche' non trova acc
		//Riallocazione di memoria -- Se serve piu' memoria rialloco il doppio di quella occupata
		if(*nTransizioni >= dimVett){
			vettoreTransizioni = (transizione *)realloc(vettoreTransizioni, (dimVett * sizeof(transizione)) * 2);
			dimVett *= 2;
		}

		sscanf(temp, "%u%s%s%s%u", &(vettoreTransizioni[*nTransizioni].inizio), &(vettoreTransizioni[*nTransizioni].letto), &(vettoreTransizioni[*nTransizioni].scritto), &(vettoreTransizioni[*nTransizioni].mossa), &(vettoreTransizioni[*nTransizioni].fine));
		//Questa sscanf legge la linea e mette i vari parametri al posto giusto

		//qui sotto calcolo quale sia lo stato piu' grande
		if(vettoreTransizioni[*nTransizioni].inizio > *statoMassimo)
			*statoMassimo = vettoreTransizioni[*nTransizioni].inizio;

		if(vettoreTransizioni[*nTransizioni].fine > *statoMassimo)
			*statoMassimo = vettoreTransizioni[*nTransizioni].fine;

		if(!caratteriPresenti[(int)vettoreTransizioni[*nTransizioni].letto]){
			caratteriPresenti[(int)vettoreTransizioni[*nTransizioni].letto] = 1;
			(*nCaratteriPresenti)++;
		}

		(*nTransizioni)++;
		getline(&temp, &llinea, stdin);
	}

	//se non ho trovato il blank tra i caratteri presenti nelle transizioni lo aggiungo al vettore
	if(!caratteriPresenti[(int)blank]){
		caratteriPresenti[(int)blank] = 1;
		(*nCaratteriPresenti)++;
	}

	free(temp);
	temp = NULL;
	return vettoreTransizioni;
}

void leggiStatiAccettazione(bool *statiAccettazione){
	unsigned int i;
	char *temp = NULL;
	size_t llinea = 0;

	getline(&temp, &llinea, stdin);
	while(strcmp(temp, "max\n")){	//leggo tutte le linee fino a quando non leggo 'max'
		sscanf(temp, "%u", &i);			//leggo un intero dalla linea appena letta e salvata in 'temp'
		statiAccettazione[i] = 1;		//"pongo" lo stato di accettazione a 1
		getline(&temp, &llinea, stdin);
	}

	free(temp);
	temp = NULL;
	return;
}

void leggiMax(unsigned int *max){
	char *temp = NULL;
	size_t llinea = 0;

	getline(&temp, &llinea, stdin);
	sscanf(temp, "%u", max);

	free(temp);
	temp = NULL;
	return;
}

void creaRigheCaratteri(int *righeCaratteri){
	unsigned int i;
	int count = 0;	//contatore per ricordarsi a che riga sono arrivato
	for(i = 0; i < 256; i++){	//scansiona tutti i 256 caratteri e vede se sono presenti
		if(righeCaratteri[i]){
			righeCaratteri[i] = count;	//gli assegna la riga che gli spetta
			count++;
		}
		else
			righeCaratteri[i] = -1; //altrimenti gli assegna -1
	}

	return;
}

listaTr **creaMatrice(listaTr **matrice, transizione *vettoreTransizioni, unsigned int nTransizioni, unsigned int statoMassimo, int *righeCaratteri, unsigned int nCaratteriPresenti){
	unsigned int i;
	unsigned int dim, //dimensione della matrice (vista linearmente)
		posizione;	//equivalenza tra tupla <i, j> e intero singolo

	//devo fare una tabella di puntatori a NULL grande = (statoMassimo+1) x nCaratteriPresenti
	dim = (statoMassimo + 1) * nCaratteriPresenti; //dimensione lineare
	matrice = (listaTr **)calloc(dim, sizeof(listaTr *)); //non so se 'calloc' vale anche per inizializzare a NULL, ma nei test funziona quindi sticazzi
	// for(i = 0; i < dim; i++)
	// 	matrice[i] = NULL;

	//Per ogni transizione che parte dallo stato x leggendo y aggiungo alla lista corrispondente
	//tale transizione.
	for(i = 0; i < nTransizioni; i++){
		posizione = pos(vettoreTransizioni[i].inizio, righeCaratteri[(int)vettoreTransizioni[i].letto], nCaratteriPresenti); //posizione(stato in cui mi trovo, carattere che sto leggendo, altezza della matrice)
		matrice[posizione] = pushTransizione(matrice[posizione], vettoreTransizioni[i]); //aggiungo la transizione "alleggerita" alla lista delle transizioni possibili
		//qui inserisco nella posizione <i, j> = <stato, carattere in input> la transizione
	}

	return matrice; //ridondanza, credo
}


char executeMachine(listaTr **matrice, unsigned int nCaratteriPresenti, bool *statiAccettazione, unsigned int max, char *input, int *righeCaratteri, size_t dimensioneStringa){
	char exitStatus = '0';

	//****************************
	processo *init = NULL;
	nstr *nastroInit = NULL;
	listaProcessi *processiAttiviHead = NULL;
	listaProcessi *indice = NULL;
	processo *indiceProcesso = NULL;
	int newPidCounter;
	listaTr *headTransizione = NULL;
	listaTr *indiceTransizione = NULL;

	unsigned int posizione;
	char carattere;
	//****************************


	nastroInit = createNastroInit(nastroInit, input, DIMNASTRO);
	init = createProcess(init, 0, 0, 0, nastroInit);

	processiAttiviHead = pushListaProcessi(processiAttiviHead, init);
	newPidCounter = 1;



	while(processiAttiviHead){ //finche' ci sono processi attivi
		indice = processiAttiviHead; //punta all'elemento di tipo listaProcessi che sto considerando
		while(indice){	//scorri tutta la lista dei processi attivi
			indiceProcesso = indice->p;	//punta alla struttura 'processo' che sto considerando

			if(statiAccettazione[indiceProcesso->stato]){ //se sono in uno stato di accettazione ho finito
				freeListaProcessi(processiAttiviHead);
				return '1';
			}

			if(indiceProcesso->nMosseFatte > max){
				exitStatus = 'U';
				processiAttiviHead = popListaProcessi(processiAttiviHead, indiceProcesso);
				break;
			}


			carattere = carattereLetto(indiceProcesso);
			if(righeCaratteri[(int)carattere] == -1){
				processiAttiviHead = popListaProcessi(processiAttiviHead, indiceProcesso);
				break;
			}

			posizione = pos(indiceProcesso->stato, righeCaratteri[(int)carattere], nCaratteriPresenti);
			headTransizione = matrice[posizione]; //testa della lista di transizioni


			if(headTransizione != NULL){	//se esiste almeno una transizione possibile
				indiceTransizione = headTransizione->next;
				while(indiceTransizione){ //se c'e' piu' di una mossa possibile
					//Crea un nuovo processo identico e mettilo in lista con il nastro in condivisione
					if(indiceProcesso->stato != indiceTransizione->fine || indiceTransizione->mossa != 'S' || indiceTransizione->scritto != carattere){
						processiAttiviHead = copyProcesso(processiAttiviHead, indiceProcesso, newPidCounter, indiceTransizione, dimensioneStringa, input);
						newPidCounter++;
					}
					else
						exitStatus = 'U';

					indiceTransizione = indiceTransizione->next;
				}

				//eseguo la transizione:
				if(headTransizione->scritto == carattere && headTransizione->mossa == 'S' && indiceProcesso->stato == headTransizione->fine){
					exitStatus = 'U';
					processiAttiviHead = popListaProcessi(processiAttiviHead, indiceProcesso);
					break;
				}

				if(headTransizione->scritto != carattere && indiceProcesso->nastro->whoShares > 1){	//se devo scrivere ma il nastro e' in condivisione lo copio
					//COW
					(indiceProcesso->nastro->whoShares)--; //elimino questo processo da tutte le liste di condivisione

					copyOwnNastro(indiceProcesso);
				}

				scriviSuNastro(indiceProcesso, headTransizione->scritto); //scrivo sul nastro
				muoviTestina(indiceProcesso, headTransizione->mossa, dimensioneStringa, input); //sposto la testina

				indiceProcesso->stato = headTransizione->fine; //cambio lo stato del processo
			}
			else{	//se non ci sono transizioni possibili
				//indice = indice->next;
				processiAttiviHead = popListaProcessi(processiAttiviHead, indiceProcesso);
				break;
			}



			(indiceProcesso->nMosseFatte)++; //se ho eseguito una mossa aggiorno il contatore del processo
			indice = indice->next;
		}
	}


	return exitStatus;
}

//funzione che ritorna l'elemento nella cella <i, j> in una matrice alta height
unsigned int pos(unsigned int i, int j, int B) {
	return ((i * B) + j);
}

//funzione che aggiunge in testa alla lista in posizione <i, j> la transizione specificata nel secondo parametro
listaTr *pushTransizione(listaTr *head, transizione t){
	listaTr *nuovo;
	if((nuovo = (listaTr *)malloc(sizeof(listaTr)))){
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

//Execute Machine Functions

processo *createProcess(processo *p, int testina, unsigned int stato, unsigned int pid, nstr *nastro){

	p = (processo *)malloc(sizeof(processo));	//alloca il nuovo processo
	if(!p){
		fprintf(stderr, "Errore allocazione memoria nuovo nastro\n");
		return p;
	}
	//assegnazione del PID
	p->pid = pid;
	p->testina = testina;
	p->stato = stato;
	p->nastro = nastro;
	p->nMosseFatte = 0;

	return p;
}

nstr *createNastroInit(nstr *n, char *stringa, unsigned int dimNastro){
	unsigned int i;
	n = (nstr *)malloc(sizeof(nstr));

	n->left = (char *)malloc(dimNastro * sizeof(char));
	n->dimLeft = dimNastro;
	n->right = (char *)malloc(dimNastro * sizeof(char));
	n->dimRight = dimNastro;

	for(i = 0; i < n->dimLeft; i++)
		(n->left)[i] = '_';
	for(i = 0; stringa[i] != '\0' && i < n->dimRight; i++)
		(n->right)[i] = stringa[i];
	for(; i < n->dimRight; i++)
		(n->right)[i] = '_';

	n->whoShares = 1;

	return n;
}

listaProcessi *pushListaProcessi(listaProcessi *head, processo *p){
	listaProcessi *nuovo;
	nuovo = (listaProcessi *)malloc(sizeof(listaProcessi));

	if(!nuovo){
		fprintf(stderr, "Errore allocazione memoria pushListaProcessi\n");
		return head;
	}

	nuovo->p = p;
	nuovo->next = head;

	head = nuovo;

	return head;
}

listaProcessi *popListaProcessi(listaProcessi *head, processo *p){
    // Store head node
    listaProcessi *temp = head;
		listaProcessi *prev;

    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->p == p)
    {
        head = head->next;	// Changed head
        freeElementoListaProcessi(temp);	// free old head
        return head;
    }

    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->p != p){
        prev = temp;
        temp = temp->next;
    }

    // If key was not present in linked list
    if (temp == NULL) return head;

    // Unlink the node from linked list
    prev->next = temp->next;

    freeElementoListaProcessi(temp);  // Free memory
		return head;
}

void freeElementoListaProcessi(listaProcessi *el){

	if(el->p->nastro->whoShares == 1){ //se e' l'unico condivisore
		free(el->p->nastro->left);
		el->p->nastro->left = NULL;
		free(el->p->nastro->right);
		el->p->nastro->right = NULL;
		free(el->p->nastro);
		el->p->nastro = NULL;
	}
	else
		(el->p->nastro->whoShares)--;

	free(el->p);
	el->p = NULL;
	free(el);
	el = NULL;

	return;
}

void freeListaProcessi(listaProcessi *head){
	while(head)
		head = popListaProcessi(head, head->p);
	return;
}

char carattereLetto(processo *p){
	if(p->testina >= 0)
		return (p->nastro->right)[p->testina];

	return (p->nastro->left)[-(p->testina) - 1];
}

void scriviSuNastro(processo *p, char toWrite){
	if(p->testina >= 0)
		(p->nastro->right)[p->testina] = toWrite;
	else
		(p->nastro->left)[-(p->testina) - 1] = toWrite;
	return;
}

void muoviTestina(processo *p, char mossa, size_t dimensioneStringa, char *input){
	unsigned int i;
	if(mossa == 'R'){	//se devo spostarmi a destra
		(p->testina)++;	//muovo la testina
		if(p->testina > 0 && p->testina >=  p->nastro->dimRight){	//se sto puntando a una cella del nastro non ancora allocata a destra
			i = p->nastro->dimRight; //punto alla prima cella non allocata del nastro
			p->nastro->right = (char *)realloc(p->nastro->right, (p->nastro->dimRight * sizeof(char)) + DIMNASTRO); //raddoppio il nastro
			p->nastro->dimRight = (p->nastro->dimRight) + DIMNASTRO;	//raddoppio il contatore della dimensione

			for(; i < dimensioneStringa && i < p->nastro->dimRight; i++)	//finisco di copiare la stringa
				(p->nastro->right)[i] = input[i];
			for(; i < p->nastro->dimRight; i++) //aggiungo tanti _ quanti ne servono
				(p->nastro->right)[i] = '_';
		}
		return;
	}

	if(mossa == 'L'){
		(p->testina)--;
		if(p->testina < 0 && -(p->testina) >=  p->nastro->dimLeft){
			i = p->nastro->dimLeft;
			p->nastro->left = (char *)realloc(p->nastro->left, (p->nastro->dimLeft * sizeof(char)) + DIMNASTRO);
			p->nastro->dimLeft = (p->nastro->dimLeft) + DIMNASTRO;

			for(; i < p->nastro->dimLeft; i++)
				(p->nastro->left)[i] = '_';
		}
		return;
	}

	return;
}

void copyOwnNastro(processo *p){
	unsigned int i;
	nstr *nuovo;
	nuovo = (nstr *)malloc(sizeof(nstr));

	nuovo->dimLeft = p->nastro->dimLeft;
	nuovo->dimRight = p->nastro->dimRight;

	nuovo->left = (char *)malloc(nuovo->dimLeft * sizeof(char));
	nuovo->right = (char *)malloc(nuovo->dimRight * sizeof(char));

	for(i = 0; i < nuovo->dimLeft; i++)
		(nuovo->left)[i] = (p->nastro->left)[i];
	for(i = 0; i < nuovo->dimRight; i++)
		(nuovo->right)[i] = (p->nastro->right)[i];

	nuovo->whoShares = 1;

	p->nastro = nuovo;
	return;
}

listaProcessi *copyProcesso(listaProcessi *processiAttiviHead, processo *toCopy, int newPid, listaTr *transizione, size_t dimensioneStringa, char *input){
	processo *nuovo;
	char carattere;

	nuovo = (processo *)malloc(sizeof(processo));
	nuovo->pid = newPid;
	nuovo->nMosseFatte = toCopy->nMosseFatte + 1;

	(toCopy->nastro->whoShares)++;
	nuovo->stato = toCopy->stato;
	nuovo->nastro = toCopy->nastro;
	nuovo->testina = toCopy->testina;


	carattere = carattereLetto(nuovo);
	if(transizione->scritto != carattere && nuovo->nastro->whoShares > 1){
		//printf("Il nastro e' condiviso\n");
		(nuovo->nastro->whoShares)--; //elimino questo processo da tutte le liste di condivisione
		//printf("Eseguito popWhoShares\n");
		copyOwnNastro(nuovo);
		//printf("Copiato il mio nastro\n");

		scriviSuNastro(nuovo, transizione->scritto);
		//printf("Ho scritto %c sul mio nastro\n", transizione->scritto);
	}

	nuovo->stato = transizione->fine;
	muoviTestina(nuovo, transizione->mossa, dimensioneStringa, input);

	processiAttiviHead = pushListaProcessi(processiAttiviHead, nuovo);
	return processiAttiviHead;
}

void freeListaTr(listaTr *head){
   listaTr *tmp;

   while (head != NULL){
	   tmp = head;
	   head = head->next;
	   free(tmp);
		 tmp = NULL;
    }

		return;
}

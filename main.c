#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>


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
	nstr *nastro;
	int testina;
	int stato;
	int pid;
	int nMosseFatte;
} processo;

typedef struct listaProcessi_s {
	struct listaProcessi_s *next;
	processo *p;
} listaProcessi;


int pos(int, int, int);	//ritorna la posizione <i, j> nella matrice
listaTr *pushTransizione(listaTr *, transizione);	//push nella lista transizioni
char executeMachine(listaTr **, int, bool *, int, char *, int *);	//esegue la macchina

//FUNZIONI DI LETTURA INPUT:
transizione *leggiTransizioni(transizione *, size_t *, int *, int *, int *);
void leggiStatiAccettazione(bool *);
void leggiMax(int *);
void creaRigheCaratteri(int *);
listaTr **creaMatrice(listaTr **, transizione *, int, int, int *, int);

//FUNZIONI PER DEBUGGING
char rigaToCarattere(int, int *);
void stampaLista(listaTr *);
void showMatrix(listaTr **, int, int, int *);
void stampaListaProcessiAttivi(listaProcessi *);


processo *createProcess(processo *, int, int, int, nstr *);	//crea un nuovo processo (usata solo per init)
nstr *createNastroInit(nstr *, char *, int);	//data la stringa input e MAX, costruisce il nastro
listaInt *pushListaInt(listaInt *, int);	//push nella lista whoShares
listaProcessi *pushListaProcessi(listaProcessi *, processo *);	//push nella lista dei processi attivi
listaProcessi *popListaProcessi(listaProcessi *, processo *);	//pop dalla lista dei processi attivi
void deleteListaInt(listaInt *);	//elimina la lista whoShares

void freeListaProcessi(listaProcessi *);	//elimina la lista dei processi attivi
char carattereLetto(processo *);	//ritorna il carattere puntato dalla testina sul nastro
bool nastroIsShared(processo *);	//1: il nastro è condiviso, altrimenti 0
void scriviSuNastro(processo *, char);	//scrive sul nastro del processo indicato
void muoviTestina(processo *, char);	//sposta la testina
void popWhoShares(processo *, listaProcessi *);	//pop dalla lista dei condivisori (controlla TUTTI i processi attivi)
listaInt *popListaInt(listaInt *, int);	//pop dalla lista dei condivisori
void copyOwnNastro(processo *);	//crea un nuovo nastro e lo assegna al processo in input, copiando il nastro precedente
listaProcessi *copyProcesso(listaProcessi *, processo *, int, listaTr *);	//copia un processo

void freeListaTr(listaTr *);








int main(int argc, char *argv[]){
	int i;

	size_t llinea = 0;
	size_t nread;
	char *temp = NULL;	//llinea e temp servono per la lettura dell'input attraverso la funzione 'getline'
	transizione *vettoreTransizioni = NULL;	//vettore che contiene tutte le transizioni possibili
	size_t nTransizioni = 0;	//contatore per il numero di transizioni
	int statoMassimo = 0;	//indica quale stato e' il piu' grande
	int *righeCaratteri = (int *)calloc(256, sizeof(int));	//indica ogni carattere in quale riga della matrice puo' essere trovato
	//prima pero' usa lo stesso vettore per indicare se un carattere e' presente tra quelli possibili o no
	int nCaratteriPresenti = 0;	//indica quanti caratteri diversi sono presenti

	bool *statiAccettazione = NULL;	//indica ogni stato se e' di accettazione (1) o meno (0)

	int max;	//indica il numero massimo di mosse effettuabili dalla macchina

	listaTr **matrice = NULL;

	//***********************************************

	vettoreTransizioni = leggiTransizioni(vettoreTransizioni, &nTransizioni, &statoMassimo, righeCaratteri, &nCaratteriPresenti);
	//legge le transizioni dallo stdin e le salva in vettoreTransizioni, salvando anche tutti gli altri parametri che deduce leggendo l'input
	creaRigheCaratteri(righeCaratteri);	//trasforma il vettore dei caratteri presenti in quello che indica ogni carattere a che riga corrisponde
	matrice = creaMatrice(matrice, vettoreTransizioni, nTransizioni, statoMassimo, righeCaratteri, nCaratteriPresenti);
	//crea la matrice delle transizioni
	free(vettoreTransizioni);

	statiAccettazione = (bool *)calloc(statoMassimo+1, sizeof(bool));	//alloco un vettore lungo quanto il numero di stati
	leggiStatiAccettazione(statiAccettazione);	//legge gli stati di accettazione e li setta a 1 nel vettore corrispondente

	leggiMax(&max);	//legge max e lo salva




	//********************************************************



	nread = getline(&temp, &llinea, stdin);
	if(strcmp("run\n", temp) != 0){
		fprintf(stderr, "Non ho letto run\n");
		return 0;
	}	//controlla che la riga letta sia run

	nread = getline(&temp, &llinea, stdin);
	while(!feof(stdin) != 0){
		temp[nread - 1] = '\0';	//sostituisco lo \n con il terminatore
		//printf("Eseguo stringa %s\n", temp);
		printf("%c\n", executeMachine(matrice, nCaratteriPresenti, statiAccettazione, max, temp, righeCaratteri));
		nread = getline(&temp, &llinea, stdin);
	}

	//printf("Eseguo stringa %s\n", temp);
	printf("%c\n", executeMachine(matrice, nCaratteriPresenti, statiAccettazione, max, temp, righeCaratteri));

	for(i = 0; i < ((statoMassimo + 1) * nCaratteriPresenti); i++){
		freeListaTr(matrice[i]);
	}

	free(matrice);

	free(temp);
	free(righeCaratteri);
	free(statiAccettazione);
	//FASE DI OUTPUT
	return 0;
}


//*****************************************************************


transizione *leggiTransizioni(transizione *vettoreTransizioni, size_t *nTransizioni, int *statoMassimo, int *caratteriPresenti, int *nCaratteriPresenti) {
	char *temp = NULL;
	size_t llinea = 0;
	size_t nread;

	size_t dimVett = 2;
	*nTransizioni = 0;
	vettoreTransizioni = (transizione *)malloc(2 * sizeof(transizione));

	nread = getline(&temp, &llinea, stdin);	//legge la prima linea dell'input
	if(strcmp("tr\n", temp) != 0){
		fprintf(stderr, "Il file non inizia per tr\n");
		return 0;
	}	//controlla che il file input inizi per TR

	nread = getline(&temp, &llinea, stdin);	//legge la seconda linea dell'input
	while(strcmp(temp, "acc\n")){		//finche' non trova acc
		//Riallocazione di memoria -- Se serve piu' memoria alloco il doppio di quella occupata
		if(*nTransizioni >= dimVett){
			vettoreTransizioni = (transizione *)realloc(vettoreTransizioni, (dimVett * sizeof(transizione)) * 2);
			dimVett *= 2;
		}

		sscanf(temp, "%d%*c%c%*c%c%*c%c%d", &(vettoreTransizioni[*nTransizioni].inizio), &(vettoreTransizioni[*nTransizioni].letto), &(vettoreTransizioni[*nTransizioni].scritto), &(vettoreTransizioni[*nTransizioni].mossa), &(vettoreTransizioni[*nTransizioni].fine));
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
		nread = getline(&temp, &llinea, stdin);
	}

	free(temp);
	return vettoreTransizioni;
}
void leggiStatiAccettazione(bool *statiAccettazione){
	int i;
	char *temp = NULL;
	size_t llinea = 0;
	size_t nread;

	nread = getline(&temp, &llinea, stdin);
	while(strcmp(temp, "max\n")){	//leggo tutte le linee fino a quando non leggo 'max'
		sscanf(temp, "%d", &i);			//leggo un intero dalla linea appena letta e salvata in 'temp'
		statiAccettazione[i] = 1;		//"pongo" lo stato di accettazione a 1
		nread = getline(&temp, &llinea, stdin);
	}

	free(temp);
	return;
}
void leggiMax(int *max){
	char *temp = NULL;
	size_t llinea = 0;
	size_t nread;

	nread = getline(&temp, &llinea, stdin);
	sscanf(temp, "%d", max);

	free(temp);
	return;
}
void creaRigheCaratteri(int *righeCaratteri){
	int i;
	int count = 0;
	for(i = 0; i < 256; i++){
		if(righeCaratteri[i]){
			righeCaratteri[i] = count;
			count++;
		}
		else
			righeCaratteri[i] = -1;
	}

	return;
}
listaTr **creaMatrice(listaTr **matrice, transizione *vettoreTransizioni, int nTransizioni, int statoMassimo, int *righeCaratteri, int nCaratteriPresenti){
	int i;
	int dim, posizione;

	//devo fare una tabella di puntatori a NULL grande = (statoMassimo+1) x nCaratteriPresenti
	dim = (statoMassimo + 1) * nCaratteriPresenti;
	matrice = (listaTr **)calloc(dim, sizeof(listaTr *));
	// for(i = 0; i < dim; i++)
	// 	matrice[i] = NULL;

	//Per ogni transizione che parte dallo stato x leggendo y aggiungo alla lista corrispondente
	//tale transizione.
	for(i = 0; i < nTransizioni; i++){
		posizione = pos(vettoreTransizioni[i].inizio, righeCaratteri[(int)vettoreTransizioni[i].letto], nCaratteriPresenti);
		matrice[posizione] = pushTransizione(matrice[posizione], vettoreTransizioni[i]);
		//qui inserisco nella posizione <i, j> = <stato, carattere in input> la transizione
	}

	return matrice;
}



//*****************************************************************
//funzione che esegue la macchina sull'input dato
char executeMachine(listaTr **matrice, int nCaratteriPresenti, bool *statiAccettazione, int max, char *input, int *righeCaratteri){
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

	int posizione;
	char carattere;
	//****************************
	//L'esecuzione della macchina si basa sulla creazione di processi ogni volta che incontro un NON determinismo

	//printf("Inizio esecuzione macchina\n");

	//creazione processo iniziale:
	nastroInit = createNastroInit(nastroInit, input, max);
	//printf("Ho creato il nastro per init\n");
	init = createProcess(init, 0, 0, 0, nastroInit);
	//printf("Ho creato il processo INIT\n");

	processiAttiviHead = pushListaProcessi(processiAttiviHead, init);
	//printf("Ho inserito INIT nella lista processi attivi\n");
	newPidCounter = 1;



	while(processiAttiviHead){
		//printf("La lista processi attivi non e' vuota\n");
		indice = processiAttiviHead;
		while(indice){
			indiceProcesso = indice->p;
			//printf("Sto eseguendo il processo %d\n", indiceProcesso->pid);

			if(statiAccettazione[indiceProcesso->stato]){ //se sono in uno stato di accettazione ho finito
				//printf("Mi trovo in uno stato di accettazione\n");
				freeListaProcessi(processiAttiviHead);
				return '1';
			}


			carattere = carattereLetto(indiceProcesso);
			//printf("Ho letto il carattere %c\n", carattere);
			posizione = pos(indiceProcesso->stato, righeCaratteri[(int)carattere], nCaratteriPresenti);
			//printf("La posizione nella matrice e' %d\n", posizione);
			headTransizione = matrice[posizione]; //testa della lista di transizioni



			if(indiceProcesso->nMosseFatte > max){
				//printf("Ho eseguito piu' mosse di max\n");
				exitStatus = 'U';
				indice = indice->next;
				processiAttiviHead = popListaProcessi(processiAttiviHead, indiceProcesso);
				//printf("**************************\n");
				//stampaListaProcessiAttivi(processiAttiviHead);
				//printf("**************************\n");
				break;
			}
			else{
				if(headTransizione){
					//printf("Ho trovato una transizione da eseguire\n");
					indiceTransizione = headTransizione->next;
					while(indiceTransizione){
						//printf("C'e' una mossa non deterministica\n");
						//Crea un nuovo processo identico e mettilo in lista con il nastro in condivisione
						processiAttiviHead = copyProcesso(processiAttiviHead, indiceProcesso, newPidCounter, indiceTransizione);
						//printf("Ho copiato il processo\n");
						newPidCounter++;
						//...
						indiceTransizione = indiceTransizione->next;
					}

					//eseguo la transizione:
					//printf("Eseguo la transizione:\n");
					if(headTransizione->scritto != carattere && nastroIsShared(indiceProcesso)){
						//printf("Il nastro e' condiviso\n");
						popWhoShares(indiceProcesso, processiAttiviHead); //elimino questo processo da tutte le liste di condivisione
						//printf("Eseguito popWhoShares\n");
						copyOwnNastro(indiceProcesso);
						//printf("Copiato il mio nastro\n");
					}

					scriviSuNastro(indiceProcesso, headTransizione->scritto);
					//printf("Ho scritto %c sul mio nastro\n", headTransizione->scritto);
					muoviTestina(indiceProcesso, headTransizione->mossa);
					//printf("Ho mosso la testina: %c\n", headTransizione->mossa);

					indiceProcesso->stato = headTransizione->fine;
				}
				else{
					//printf("Non ci sono transizioni da eseguire\n");
					indice = indice->next;
					processiAttiviHead = popListaProcessi(processiAttiviHead, indiceProcesso);
					//printf("**************************\n");
					//stampaListaProcessiAttivi(processiAttiviHead);
					//printf("**************************\n");
					break;
				}
			}



			(indiceProcesso->nMosseFatte)++;
			indice = indice->next;
			//printf("**************************\n");
			//stampaListaProcessiAttivi(processiAttiviHead);
			//printf("**************************\n");
		}
	}


	return exitStatus;
}

//funzione che ritorna l'elemento nella cella <i, j> in una matrice alta height
int pos(int i, int j, int B) {
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

processo *createProcess(processo *p, int testina, int stato, int pid, nstr *nastro){

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

nstr *createNastroInit(nstr *n, char *stringa, int max){
	int i;
	n = (nstr *)malloc(sizeof(nstr));

	n->left = (char *)malloc(max * sizeof(char));
	n->dimLeft = max;
	n->right = (char *)malloc(max * sizeof(char));
	n->dimRight = max;

	for(i = 0; i < n->dimLeft; i++)
		(n->left)[i] = '_';
	for(i = 0; stringa[i] != '\0' && i < n->dimRight; i++)
		(n->right)[i] = stringa[i];
	for(; i < n->dimRight; i++)
		(n->right)[i] = '_';

	n->whoShares = NULL;
	n->whoShares = pushListaInt(n->whoShares, 0);

	return n;
}

listaInt *pushListaInt(listaInt *head, int value){
	listaInt *nuovo;
	nuovo = (listaInt *)malloc(sizeof(listaInt));

	if(!nuovo){
		fprintf(stderr, "Errore allocazione memoria pushListaInt\n");
		return head;
	}
	nuovo->pid = value;
	nuovo->next = head;

	head = nuovo;
	return head;
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

listaProcessi *popListaProcessi(listaProcessi *processiAttiviHead, processo *p){
	listaProcessi *temp;

	if(processiAttiviHead == NULL)
		return processiAttiviHead;

	if(processiAttiviHead->p == p){
		//printf("Sto eliminando il processo %d dalla lista processi attivi -- processiAttiviHead: %p\n", p->pid, processiAttiviHead);
		temp = processiAttiviHead;
		processiAttiviHead = processiAttiviHead->next;

		free(temp->p->nastro->left);
		//printf("Liberato il nastro sinistro\n");
		free(temp->p->nastro->right);
		//printf("Liberato il nastro destro\n");
		deleteListaInt(temp->p->nastro->whoShares);
		//printf("Liberato la lista condivisori\n");
		free(temp->p);
		//printf("Liberato il processo\n");
		free(temp);
		//printf("Liberato l'elemento della lista\n");
	}
	else{
		processiAttiviHead->next = popListaProcessi(processiAttiviHead->next, p);
	}

	//printf("Ora processiAttiviHead vale %p\n", processiAttiviHead);
	return processiAttiviHead;
}

void deleteListaInt(listaInt *head){
	listaInt *temp;
	if(!head)
		return;
	temp = head;
	head = head->next;
	free(temp);
	deleteListaInt(head);
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

bool nastroIsShared(processo *p){
	if((p->nastro->whoShares)->next)
		return 1;
	return 0;
}

void scriviSuNastro(processo *p, char toWrite){
	if(p->testina >= 0)
		(p->nastro->right)[p->testina] = toWrite;
	else
		(p->nastro->left)[-(p->testina) - 1] = toWrite;
	return;
}

void muoviTestina(processo *p, char mossa){
	if(mossa == 'R'){
		(p->testina)++;
		return;
	}

	if(mossa == 'L'){
		(p->testina)--;
		return;
	}

	return;
}

void popWhoShares(processo *indiceProcesso, listaProcessi *processiAttiviHead){
	int pid = indiceProcesso->pid;
	listaProcessi *temp;
	processo *processoTemp;

	temp = processiAttiviHead;
	while(temp){
		processoTemp = temp->p;
		processoTemp->nastro->whoShares = popListaInt(processoTemp->nastro->whoShares, pid);
		temp = temp->next;
	}

	return;
}

listaInt *popListaInt(listaInt *head, int pid){
	listaInt *temp;
	if(!head)
		return head;

	if(head->pid == pid){
		temp = head;
		head = head->next;

		free(temp);
		return head;
	}
	else{
		head->next = popListaInt(head->next, pid);
		return head;
	}
}

void copyOwnNastro(processo *p){
	int i;
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

	nuovo->whoShares = NULL;
	nuovo->whoShares = pushListaInt(nuovo->whoShares, p->pid);

	p->nastro = nuovo;
	return;
}

listaProcessi *copyProcesso(listaProcessi *processiAttiviHead, processo *toCopy, int newPid, listaTr *transizione){
	processo *nuovo;
	char carattere;

	nuovo = (processo *)malloc(sizeof(processo));
	nuovo->pid = newPid;
	nuovo->nMosseFatte = toCopy->nMosseFatte + 1;

	toCopy->nastro->whoShares = pushListaInt(toCopy->nastro->whoShares, newPid);
	nuovo->stato = toCopy->stato;
	nuovo->nastro = toCopy->nastro;
	nuovo->testina = toCopy->testina;


	carattere = carattereLetto(nuovo);
	if(transizione->scritto != carattere && nastroIsShared(nuovo)){
		//printf("Il nastro e' condiviso\n");
		popWhoShares(nuovo, processiAttiviHead); //elimino questo processo da tutte le liste di condivisione
		//printf("Eseguito popWhoShares\n");
		copyOwnNastro(nuovo);
		//printf("Copiato il mio nastro\n");

		scriviSuNastro(nuovo, transizione->scritto);
		//printf("Ho scritto %c sul mio nastro\n", transizione->scritto);
	}

	nuovo->stato = transizione->fine;
	muoviTestina(nuovo, transizione->mossa);

	processiAttiviHead = pushListaProcessi(processiAttiviHead, nuovo);
	return processiAttiviHead;
}

void freeListaTr(listaTr *head){
   listaTr *tmp;

   while (head != NULL){
	   tmp = head;
	   head = head->next;
	   free(tmp);
    }

		return;
}
//*****************TESTING*****************************************

char rigaToCarattere(int j, int* righeCaratteri){
	int i;
	for(i = 0; i < 256; i++){
		if(righeCaratteri[i] == j)
			return (char)i;
	}
	return '^';
}
void stampaLista(listaTr *head){
	if(!head)
		return;
	printf("Scritto: %c, Mossa: %c, Fine: %d\n", head->scritto, head->mossa, head->fine);
	stampaLista(head->next);
	return;
}
void showMatrix(listaTr **matrice, int statoMassimo, int nCaratteriPresenti, int *righeCaratteri){
	int i, j;
	printf("  ");
	for(i = 0; i < nCaratteriPresenti; i++)
		printf("%c ", rigaToCarattere(i, righeCaratteri));
	printf("\n");

	for(i = 0; i < statoMassimo+1; i++){
		printf("%d ", i);
		for(j = 0; j < nCaratteriPresenti; j++){
			if(matrice[pos(j, i, nCaratteriPresenti)])
				printf("O ");
			else
				printf("X ");
		}
		printf("\n");
	}

	return;
}

void stampaListaProcessiAttivi(listaProcessi *head){
	listaInt *temp;

	if(!head)
		return;

	temp = head->p->nastro->whoShares;
	printf("Il processo %d si trova nello stato %d e sta leggendo %c\n", head->p->pid, head->p->stato, carattereLetto(head->p));
	printf("Il nastro e' condiviso con: ");
	while(temp){
		printf("%d ", temp->pid);
		temp = temp->next;
	}
	printf("\n");
	stampaListaProcessiAttivi(head->next);
	return;
}
/*
int leggiInput(listaTr **matrice, bool *statiAccettazione, int *max, int *caratteriPresenti){
	int i, j;

	char *temp;	//Qui salvo le singole linee di input per lavorarci sopra
	size_t llinea = 0;	//dimensione della singola linea, da aggiornare ogni volta

	transizione *vett = (transizione *)malloc(2 * sizeof(transizione));	//Alloco subito 2 transizioni
	size_t dimVett = 2;								//DIMENSIONE DELL'ARRAY
	int nTransizioni = 0;								//PRIMA CELLA LIBERA

	int statoMassimo = 0; //tiene conto dello stato piu' grande trovato
	//se esiste lo stato x, allora esistono tutti gli stati 0, 1, ..., x-1

	int nCaratteriPresenti = 0;

	int dim, posizione;


	//ORA DENTRO A VETT HO TUTTE LE POSSIBILI TRANSIZIONI
	//dimVett contiene la quantita' di memoria occupata da vett
	//nTransizioni contiene il numero di transizioni effettive contenute in vett
	//statoMassimo contiene quale stato e' il piu' grande numericamente, quindi se il piu' grande e' k,
	//esistono sicuramente tutti gli stati 0, 1, ... , k-1,

	//Per quanto riguarda i caratteri:
	//caratteriPresenti: vettore 'booleano' in cui la cella 'i' e' a 1 sse il carattere (char)i e' presente tra quelli da leggere
	//nCaratteriPresenti: intero che rappresenta quanti caratteri sono presenti in lettura


	//Ora devo creare una matrice di liste in cui per ogni coppia STATO - INPUT
	//ho una sequenza di possibili transizioni

	//Per una matrice MATRIX di dimensione AxB alloco un vettore di dimensione A*B e trovo
	//l'elemento [i][j] come MATRIX[ i*B + j]

	//devo fare una tabella di puntatori a NULL grande = (statoMassimo+1) x nCaratteriPresenti
	dim = (statoMassimo + 1) * nCaratteriPresenti;
	matrice = (listaTr **)malloc(dim * sizeof(listaTr *));
	for(i = 0; i < dim; i++)
		matrice[i] = NULL;

	//Ora trasformo il vettore caratteriPresenti in un vettore che nella posizione i-esima rappresenta la riga in
	//cui e' presente quel carattere nella tabella
	for(i = 0, j = 0; i < 256; i++){
		if(caratteriPresenti[i]){
			caratteriPresenti[i] = j;
			j++;
		}
		else
			caratteriPresenti[i] = -1;
	}

	//Per ogni transizione che parte dallo stato x leggendo y aggiungo alla lista corrispondente
	//tale transizione.
	for(i = 0; i < nTransizioni; i++){
		posizione = pos(vett[i].inizio, caratteriPresenti[(int)vett[i].letto], nCaratteriPresenti);
		matrice[posizione] = pushTransizione(matrice[posizione], vett[i]);
		//qui inserisco nella posizione <i, j> = <stato, carattere in input> la transizione
	}

	free(vett);

	//Ora ho la matrice pronta e un vettore chiamato statiAccettazione che contiene
	//0 o 1 a seconda che la posizione i sia uno stato di accettazione o meno
	//C'e' anche corrispondenza tra la posizione i e la posizione della colonna della matrice


	llinea = getline(&temp, &llinea, stdin);	//ora temp dovrebbe contenere la parola 'run'
	if(strcmp(temp, "run\n") != 0){
		fprintf(stderr, "Non ho letto la parola 'run'\n");
		return 0;
	}


	return statoMassimo;
}
*/

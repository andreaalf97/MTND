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
	nstr nastro;
	int testina;
	int stato;
	int pid;
} processo;

typedef struct listaProcessi_s {
	struct listaProcessi_s *next;
	processo *p;
} listaProcessi;


int pos(int, int, int);
listaTr *pushTransizione(listaTr *, transizione);
char executeMachine(listaTr **, int, int, bool *, int, char *, int *);

transizione *leggiTransizioni(transizione *, int *, int *, int *, int *);
void leggiStatiAccettazione(bool *);
void leggiMax(int *);
void creaRigheCaratteri(int *);
listaTr **creaMatrice(listaTr **, transizione *, int, int, int *, int);

char rigaToCarattere(int, int *);
void stampaLista(listaTr *);
void showMatrix(listaTr **, int, int, int *);

processo *createProcess(processo *, int, int, int, nstr);
nstr *createNastro(nstr *, char *, int);
listaInt *pushListaInt(listaInt *, int);
listaProcessi *pushListaProcessi(listaProcessi *, processo *);
listaProcessi *popListaProcessi(listaProcessi *, processo *);
void deleteListaInt(listaInt *);

int main(int argc, char *argv[]){
	//**********VARIABILI PER LETTURA INPUT**********
	transizione *vettoreTransizioni;
	int nTransizioni = 0;
	int statoMassimo = 0;
	int *righeCaratteri = (int *)calloc(256, sizeof(int));
	int nCaratteriPresenti = 0;

	bool *statiAccettazione;

	int max;

	listaTr **matrice;
	//***********************************************

	int i, j;
	size_t llinea;
	char *temp;

	//**********LETTURA INPUT*******************
	vettoreTransizioni = leggiTransizioni(vettoreTransizioni, &nTransizioni, &statoMassimo, righeCaratteri, &nCaratteriPresenti);
	statiAccettazione = (bool *)calloc(statoMassimo+1, sizeof(bool));
	leggiStatiAccettazione(statiAccettazione);
	leggiMax(&max);

	/*printf("Ci sono %d transizioni\n", nTransizioni);
	for(i = 0; i < nTransizioni; i++)
		printf("Dallo stato %d allo stato %d ---> leggo %c scrivo %c mossa %c\n", vettoreTransizioni[i].inizio, vettoreTransizioni[i].fine, vettoreTransizioni[i].letto, vettoreTransizioni[i].scritto, vettoreTransizioni[i].mossa);
	printf("****************************\n");

	printf("Stato massimo: %d\n", statoMassimo);
	printf("****************************\n");

	printf("Ci sono %d caratteri\n", nCaratteriPresenti);
	for(i = 0; i < 256; i++)
		if(righeCaratteri[i])
			printf("%c\n", (char)i);
	printf("****************************\n");

	printf("Stati accettazione:\n");
	for(i = 0; i < statoMassimo+1; i++)
		if(statiAccettazione[i])
			printf("%d\n", i);
	printf("****************************\n");*/

	creaRigheCaratteri(righeCaratteri);

	/*for(i = 0; i < 256; i++)
		if(righeCaratteri[i] >= 0)
			printf("Il carattere %c si trova alla riga %d\n", (char)i, righeCaratteri[i]);
	printf("****************************\n");*/

	matrice = creaMatrice(matrice, vettoreTransizioni, nTransizioni, statoMassimo, righeCaratteri, nCaratteriPresenti);
	
	/*for(i = 0; i < statoMassimo+1; i++)
		for(j = 0; j < nCaratteriPresenti; j++){
			if(matrice[pos(i, j, nCaratteriPresenti)]){
				printf("Dallo stato %d, leggendo %c:\n", i, rigaToCarattere(j, righeCaratteri));
				stampaLista(matrice[pos(i, j, nCaratteriPresenti)]);
				printf("*************************************\n");
			}
		}*/
	//********************************************************

	llinea = getline(&temp, &llinea, stdin);
	if(strcmp("run\n", temp) != 0){
		fprintf(stderr, "Non ho letto run\n");
		return 0;
	}	//controlla che la riga letta sia run

		//dovrebbe corrispondere alla prima stringa in ingresso

	//ORA E' TUTTO PRONTO PER L'ESECUZIONE
	//matrice = 						matrice che contiene per ogni i(stato) e j (carattere letto) la lista delle transizioni
	//											che devo possono essere eseguite
	//statiAccettazione = 	vettore di 0 e 1 che e' a 1 solo se i (indice) e' uno stato di accettazione
	//max = 								variabile che contiene il numero massimo di mosse effettuabili dalla macchina di Touring.

	llinea = getline(&temp, &llinea, stdin);
	while(!feof(stdin) && strcmp("\n", temp) != 0){
		for(i = 0; temp[i] != '\n' && temp[i] != '\0'; i++);	//ciclo fino allo \n
		temp[i] = '\0';	//sostituisco lo \n con il terminatore
		printf("Stringa %s\n", temp);
		printf("%c\n", executeMachine(matrice, statoMassimo, nCaratteriPresenti, statiAccettazione, max, temp, righeCaratteri));
		llinea = getline(&temp, &llinea, stdin);
	}

	//FASE DI OUTPUT
	return 0;
}


//*****************************************************************


transizione *leggiTransizioni(transizione *vettoreTransizioni, int *nTransizioni, int *statoMassimo, int *caratteriPresenti, int *nCaratteriPresenti) {
	char *temp;
	size_t llinea = 0;

	size_t dimVett = 2;
	*nTransizioni = 0;
	vettoreTransizioni = (transizione *)malloc(2 * sizeof(transizione));

	llinea = getline(&temp, &llinea, stdin);	//legge la prima linea dell'input
	if(strcmp("tr\n", temp) != 0){
		fprintf(stderr, "Il file non inizia per tr\n");
		return 0;
	}	//controlla che il file input inizi per TR

	llinea = getline(&temp, &llinea, stdin);	//legge la seconda linea dell'input
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
		llinea = getline(&temp, &llinea, stdin);
	}

	free(temp);
	return vettoreTransizioni;
}
void leggiStatiAccettazione(bool *statiAccettazione){
	int i;
	char *temp;
	size_t llinea = 0;

	llinea = getline(&temp, &llinea, stdin);
	while(strcmp(temp, "max\n")){	//leggo tutte le linee fino a quando non leggo 'max'
		sscanf(temp, "%d", &i);			//leggo un intero dalla linea appena letta e salvata in 'temp'
		statiAccettazione[i] = 1;		//"pongo" lo stato di accettazione a 1
		llinea = getline(&temp, &llinea, stdin);
	}

	free(temp);
	return;
}
void leggiMax(int *max){
	char *temp;
	size_t llinea = 0;

	llinea = getline(&temp, &llinea, stdin);
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
	int i, j;
	int dim, posizione;

	//devo fare una tabella di puntatori a NULL grande = (statoMassimo+1) x nCaratteriPresenti
	dim = (statoMassimo + 1) * nCaratteriPresenti;
	matrice = (listaTr **)malloc(dim * sizeof(listaTr *));
	for(i = 0; i < dim; i++)
		matrice[i] = NULL;

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
char executeMachine(listaTr **matrice, int statoMassimo, int nCaratteriPresenti, bool *statiAccettazione, int max, char *input, int *righeCaratteri){
	char exitStatus = '0';
	int nMosseFatte = 0;

	//****************************
	processo *init = NULL;
	nstr *nastroInit = NULL;
	nstr *nastroTemp = NULL;
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

	//creazione processo iniziale:
	nastroInit = createNastro(nastroInit, input, max);
	init = createProcess(init, 0, 0, 0, *nastroInit);

	processiAttiviHead = pushListaProcessi(processiAttiviHead, init);
	newPidCounter = 1;



	while(processiAttiviHead){
		indice = processiAttiviHead;
		while(indice){
			indiceProcesso = indice->p;
			printf("Sto eseguendo il processo %d\n", indiceProcesso->pid);

			if(statiAccettazione[indiceProcesso.stato]){ //se sono in uno stato di accettazione ho finito
				freeListaProcessi(processiAttiviHead);
				return '1';
			}


			carattere = carattereLetto(indiceProcesso);
			posizione = pos(indiceProcesso.stato, righeCaratteri[(int)carattere], nCaratteriPresenti);
			indiceTransizione = matrice[posizione]; //testa della lista di transizioni


			if(nMosseFatte > max){
				exitStatus = 'U';
				processiAttiviHead = popListaProcessi(processiAttiviHead, indiceProcesso);
			}
			else{
				if(headTransizione){
					indiceTransizione = headTransizione->next;
					while(indiceTransizione){
						//Crea un nuovo processo identico e mettilo in lista con il nastro in condivisione
						processiAttiviHead = copyProcesso(processiAttiviHead, indiceProcesso, newPidCounter);
						newPidCounter++;
						//...
						indiceTransizione = indiceTransizione->next;
					}

					//eseguo la transizione:
					if(headTransizione->scritto != carattere && nastroIsShared(indiceProcesso)){
						popWhoShares(indiceProcesso, processiAttiviHead); //elimino questo processo da tutte le liste di condivisione
						copyOwnNastro(indiceProcesso);
					}

					scriviSuNastro(indiceProcesso, headTransizione->scritto);
					muoviTestina(indiceProcesso, headTransizione->mossa);
				}
				else
					processiAttiviHead = popListaProcessi(processiAttiviHead, indiceProcesso);
			}



			nMosseFatte++;
			indice = indice->next;
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

//Execute Machine Functions

processo *createProcess(processo *p, int testina, int stato, int pid, nstr nastro){

	p = (processo *)malloc(sizeof(processo));
	if(!p){
		fprintf(stderr, "Errore allocazione memoria nuovo nastro\n");
		return p;
	}
	//assegnazione del PID
	p->pid = pid;
	p->testina = testina;
	p->stato = stato;
	p->nastro = nastro;

	return p;
}

nstr *createNastro(nstr *n, char *stringa, int max){
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

	if(!processiAttiviHead)
		return processiAttiviHead;

	if(processiAttiviHead->p == p){
		temp = processiAttiviHead;
		processiAttiviHead = processiAttiviHead->next;

		free(temp->p->nastro.left);
		free(temp->p->nastro.right);
		deleteListaInt(temp->p->nastro.whoShares);
		free(temp->p);
		free(temp);
	}
	else{
		processiAttiviHead->next = popListaProcessi(processiAttiviHead->next, p);
	}
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
	listaProcessi *temp = head;
	popListaProcessi(temp, temp->p);
	freeListaProcessi(head);
}

char carattereLetto(processo *p){
	if(p->testina >= 0)
		return (p->nastro.right)[testina];
	
	return (p->nastro.left)[-testina - 1];
}

bool nastroIsShared(processo *p){
	if((p->nastro.whoShares)->next)
		return 1;
	return 0;
}

void scriviSuNastro(processo *p, char toWrite){
	if(testina >= 0)
		(p->nastro.right)[testina] = toWrite;
	else
		(p->nastro.left)[-testina - 1] = toWrite;
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
		processoTemp->nastro.whoShares = popListaInt(processoTemp->nastro.whoShares, pid);
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

	nuovo.dimLeft = p->nastro.dimLeft;
	nuovo.dimRight = p->nastro.dimRight;

	nuovo.left = (char *)malloc(dimLeft * sizeof(char));
	nuovo.right = (char *)malloc(dimRight * sizeof(char));

	for(i = 0; i < nuovo.dimLeft; i++)
		(nuovo.left)[i] = (p->nastro.left)[i];
	for(i = 0; i < nuovo.dimRight; i++)
		(nuovo.right)[i] = (p->nastro.right)[i];

	nuovo.whoShares = NULL;
	nuovo.whoShares = pushListaInt(nuovo.whoShares, p->pid);

	&(p->nastro) = nuovo;
	return;
}

listaProcessi *copyProcesso(listaProcessi *processiAttiviHead, processo *toCopy, int newPid){
	processo *nuovo;

	nuovo = (processo *)malloc(sizeof(processo));
	nuovo->pid = newPid;

	toCopy->nastro.whoShares = pushListaInt(toCopy->nastro.whoShares, newPid);
	nuovo->stato = toCopy->stato;
	nuovo->testina = toCopy->testina;

	&(nuovo->nastro) = &(toCopy->nastro);

	processiAttiviHead = pushListaProcessi(processiAttiviHead, nuovo);
	return processiAttiviHead;
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
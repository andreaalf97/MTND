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

typedef struct listaPid_s {
	struct listaPid_s *next;
	int pid;
}	listaPid;

typedef struct nastro_s {
	char *left, *right;
	int dimLeft, dimRight;
	listaPid *whoShares;
} nstr;

typedef struct processo_s {
	nstr nastro;
	int testina;
	int stato;
	int pid;
	int nMosseFatte;
} processo;

typedef struct listaProcessi_s {
	struct listaProcessi_s *next;
	processo p;
} listaProcessi;


int pos(int, int, int);
listaTr *pushTransizione(listaTr *, transizione);
listaPid *pushListaInt(listaPid *, int);
char executeMachine(listaTr **, int, int, bool *, int, char *, int *);
listaProcessi *pushProcesso(listaProcessi *, processo);
listaProcessi *removeProcesso(listaProcessi *, int);
int copiaNastro(nstr, nstr *);
listaPid *rimuoviPidDaWhoshares(listaPid *, int);

transizione *leggiTransizioni(transizione *, int *, int *, int *);
void leggiStatiAccettazione(bool *);
void leggiMax(int *);

int main(int argc, char *argv[]){
	//**********VARIABILI PER LETTURA INPUT**********
	transizione *vettoreTransizioni;
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
	vettoreTransizioni = leggiTransizioni(vettoreTransizioni, &statoMassimo, righeCaratteri, &nCaratteriPresenti);
	statiAccettazione = (bool *)calloc(statoMassimo+1, sizeof(bool));
	leggiStatiAccettazione(statiAccettazione);
	leggiMax(&max);


	printf("Max: %d\n", max);
	return 0;
	matrice = creaMatrice(matrice, vettoreTransizioni, statoMassimo, righeCaratteri, nCaratteriPresenti);
	//********************************************************

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
		printf("Stringa %s\n", temp);
		printf("%c\n", executeMachine(matrice, statoMassimo+1, nCaratteriPresenti, statiAccettazione, max, temp, righeCaratteri));
		llinea = getline(&temp, &llinea, stdin);
		for(i = 0; temp[i] != '\n' && temp[i] != '\0'; i++);
		temp[i] = '\0';
	}


	free(temp);		//libero temp perche' non devo piu' leggere stringhe dall'input
	free(statiAccettazione);
	free(matrice);

	//FASE DI OUTPUT
	return 0;
}
//*****************************************************************
transizione *leggiTransizioni(transizione *vettoreTransizioni, int *statoMassimo, int *caratteriPresenti, int *nCaratteriPresenti) {
	char *temp;
	size_t llinea = 0;

	size_t dimVett = 2;
	int nTransizioni = 0;
	vettoreTransizioni = (transizione *)malloc(2 * sizeof(transizione));

	llinea = getline(&temp, &llinea, stdin);	//legge la prima linea dell'input
	if(strcmp("tr\n", temp) != 0){
		fprintf(stderr, "Il file non inizia per tr\n");
		return 0;
	}	//controlla che il file input inizi per TR

	llinea = getline(&temp, &llinea, stdin);	//legge la seconda linea dell'input
	while(strcmp(temp, "acc\n")){		//finche' non trova acc
		//Riallocazione di memoria -- Se serve piu' memoria alloco il doppio di quella occupata
		if(nTransizioni >= dimVett){
			vettoreTransizioni = (transizione *)realloc(vettoreTransizioni, (dimVett * sizeof(transizione)) * 2);
			dimVett *= 2;
		}

		sscanf(temp, "%d%*c%c%*c%c%*c%c%d", &(vettoreTransizioni[nTransizioni].inizio), &(vettoreTransizioni[nTransizioni].letto), &(vettoreTransizioni[nTransizioni].scritto), &(vettoreTransizioni[nTransizioni].mossa), &(vettoreTransizioni[nTransizioni].fine));
		//Questa sscanf legge la linea e mette i vari parametri al posto giusto

		//qui sotto calcolo quale sia lo stato piu' grande
		if(vettoreTransizioni[nTransizioni].inizio > *statoMassimo)
			*statoMassimo = vettoreTransizioni[nTransizioni].inizio;

		if(vettoreTransizioni[nTransizioni].fine > *statoMassimo)
			*statoMassimo = vettoreTransizioni[nTransizioni].fine;

		if(!caratteriPresenti[(int)vettoreTransizioni[nTransizioni].letto]){
			caratteriPresenti[(int)vettoreTransizioni[nTransizioni].letto] = 1;
			*nCaratteriPresenti++;
		}

		nTransizioni++;
		llinea = getline(&temp, &llinea, stdin);
	}

	free(temp);
	return vettoreTransizioni;
}
void leggiStatiAccettazione(bool *statiAccettazione){
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
//*****************************************************************
/*int leggiInput(listaTr **matrice, bool *statiAccettazione, int *max, int *caratteriPresenti){
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
}*/

//funzione che esegue la macchina sull'input dato
char executeMachine(listaTr **matrice, int width, int nCaratteriPresenti, bool *statiAccettazione, int max, char *input, int *righeCaratteri){
	int i,	//per i loop
	posizione,	//per calcolare la posizione <i, j> nella matrice
	pidCounter;	//contiene il valore da assegnare al prossimo processo che verra' creato

	char *tempChar;	//usato per calcoli intermedi
	int tempInt;		//usato per calcoli intermedi
	listaTr *tempListaTrHead;

	nstr *nuovoNastro;

	//FASE DI INIZIALIZZAZIONE:
	//creo un processo iniziale init(cioe' una configurazione della MT da cui partire) dal
	//quale forkero' ogni volta che incontro un non determinismo
	//bisogna capire come gestire la copia del nastro nel caso in cui venga modificato
	//da un sottoprocesso --> deve essere condiviso

	processo init;	//processo iniziale
	listaProcessi *processiAttiviHead = NULL;	//lista che tiene traccia di quanti processi non hanno ancora terminato
	listaProcessi *indice;	//serve a scorrere all'interno della lista dei processi
	char exitStatus;	//variabile per controllare alla fine se ritornare 0 o U (deve essere 0 di default)


	pidCounter = 0; //init e' sempre il processo 0
	//FASE DI INIZIALIZZAZIONE DI INIT:
	//assegnazione del PID
	init.pid = pidCounter; // = 0

	//Costruzione del nastro
	init.nastro.right = (char *)malloc(max * sizeof(char));
	init.nastro.dimRight = max;
	init.nastro.left = (char *)malloc(max * sizeof(char));
	init.nastro.dimLeft = max;
	init.nastro.whoShares = pushListaInt(init.nastro.whoShares, init.pid);

	for(i = 0; i < init.nastro.dimLeft; i++)
		init.nastro.left[i] = '_';
	for(i = 0; input[i] != '\0' && i < init.nastro.dimRight; i++)
		init.nastro.right[i] = input[i];
	for(; i < init.nastro.dimRight; i++)
		init.nastro.right[i] = '_';

	//inizializzazione testina
	init.testina = 0;

	//inizializzazione stato
	init.stato = 0;

	//inizializzazione mosse fatte
	init.nMosseFatte = 0;


	processiAttiviHead = pushProcesso(processiAttiviHead, init);
	exitStatus = '0'; //0 di default; se trovo un U diventa U, se trovo 1 ritorno subito 1

	printf("Inizializzato INIT, inizio l'esecuzione\n");
	//INIZIO DELL'ESECUZIONE
	while(processiAttiviHead){		//finche' ci sono processi attivi
		indice = processiAttiviHead;
		while(indice){	//per ogni processo ancora in esecuzione
			//4 casi:
			// -lo stato in cui mi trovo e' finale e quindi ritorno subito 1
			// -non ci sono piu' transizioni di uscita
			// -c'e' una sola transizione possibile
			// -ci sono due o piu' transizioni possibili

			printf("Sto eseguendo il processo %d\n", indice->p.pid);

			if(statiAccettazione[indice->p.stato]){	//se mi trovo in uno stato di accettazione per uno qualsiasi dei sottoprocessi ritorno subito 1
				//LIBERA TUTTA LA MEMORIA CHE NON VIENE LIBERATA DALLA RETURN
				return '1';
			}

			//guardo sul nastro a che carattere sta puntando la testina
			if(indice->p.testina >= 0)
				tempChar = &(indice->p.nastro.right[indice->p.testina]);
			else
				tempChar = &(indice->p.nastro.left[-(indice->p.testina) -1]);

			tempInt = righeCaratteri[(int)*tempChar];
			//dentro a tempInt ho la riga della matrice a cui si trova il carattere appena letto dal nastro
			//dentro a tempChar ho il carattere appena letto dal nastro

			printf("Sono nello stato %d, sto leggendo %c\n", indice->p.stato, *tempChar);
			posizione = pos(indice->p.stato, tempInt, nCaratteriPresenti);
			tempListaTrHead = matrice[posizione]; //ora ho un riferimento alla lista delle possibili transizioni partendo da questo stato
			if(tempListaTrHead){
				if(!tempListaTrHead->next){	//se c'e' solo una transizione possibile
					//COSE DA FARE:
					//1 - scrivere sul nastro quello che la transizione vuole che scriva
						//SE non devo cambiare il valore che c'e' sul nastro --> NON FARE NULLA
						//ALTRIMENTI -->
							//SE il nastro NON e' condiviso --> scrivi e basta
							//ALTRIMENTI --> creane una copia e scrivi

					//questa parte e' commentata perche' in teoria tempChar e tempInt vanno gia' bene
					/*if(indice->p.testina >= 0)
						tempChar = &(indice->p.nastro.right[indice->p.testina]);
					else
						tempChar = &(indice->p.nastro.left[-(indice->p.testina) - 1]);
					//tempChar e' il carattere appena letto sul nastro
					*/

					if(*tempChar != tempListaTrHead->scritto){	//se quello che devo scrivere e' diverso da quello che c'e' gia' scritto sul nastro
						if(indice->p.nastro.whoShares->next){	//cioe' e' condiviso da piu' di un processo
							//COPIA DEL NASTRO
							if(!copiaNastro(indice->p.nastro, nuovoNastro)){fprintf(stderr, "Errore durante la copia del nastro\n");}
							//ho allocato e creato il nuovo nastro
							rimuoviPidDaWhoshares(indice->p.nastro.whoShares, indice->p.pid);
							//rimuovo questo pid dalla lista di quelli che condividono il nastro vecchio
							(*nuovoNastro).whoShares = pushListaInt((*nuovoNastro).whoShares, indice->p.pid);
							//aggiungo questo pid alla lista di quelli che condividono il nastro nuovo
							indice->p.nastro = *nuovoNastro;
							//cambio il riferimento al nastro di questo processo
						}
						else{	//se il nastro non e' condiviso posso scrivere e basta
							*tempChar = tempListaTrHead->scritto;
						}
					}
					//else{NON FARE NULLA}


									/*if(indice->p.testina >= 0)
										indice->p.nastro.right[indice->p.testina] = tempListaTrHead->scritto;
									else
										indice->p.nastro.left[-(indice->p.testina) -1] = tempListaTrHead->scritto;*/

					//2 - spostare la testina
					if(tempListaTrHead->mossa == 'R')
						indice->p.testina++;
					else if(tempListaTrHead->mossa == 'L')
						indice->p.testina--;
					//3 - aggiornare lo stato
					indice->p.stato = tempListaTrHead->fine;
					//Aumentare il numero di mosse eseguite
					indice->p.nMosseFatte++;
				}
				else{//se ci sono piu' transizioni possibili
					//COSA DEVO FARE:
					//eseguire la transizione del processo che sto eseguendo
					//creare un nuovo processo per ogni transizione in più che trovo
				}
			}
			else{	//cioe' da questo stato con questo carattere non ci sono transizioni possibili
				//devo rimuovere il processo da quelli in esecuzione
				//RIMUOVI IL PROCESSO DALLA LISTA DI QUELLI CHE CONDIVIDONO UN DETERMINATO NASTRO
				//LIBERA UN PO' DI MEMORIA MA NON SO ANCORA QUALE
				processiAttiviHead = removeProcesso(processiAttiviHead, indice->p.pid);
			}


			if(indice->p.nMosseFatte > max){
				processiAttiviHead = removeProcesso(processiAttiviHead, indice->p.pid);
				exitStatus = 'U';
			}


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


listaProcessi *pushProcesso(listaProcessi *head, processo t){
	listaProcessi *nuovo;
	if(nuovo = (listaProcessi *)malloc(sizeof(listaProcessi))){
		nuovo->p = t;
		nuovo->next = head;
		head = nuovo;
	}
	else
		fprintf(stderr, "Errore allocazione memoria lista\n");

	return head;
}

listaProcessi *removeProcesso(listaProcessi *currP, int pid){
	if(currP == NULL)
		return NULL;

	if(currP->p.pid == pid){
		listaProcessi *tempNextP = currP->next;
		free(currP);
		return tempNextP;
	}

	currP->next = removeProcesso(currP->next, pid);
	return currP;
}

listaPid *rimuoviPidDaWhoshares(listaPid *currP, int pid){
	if(currP == NULL)
		return NULL;

	if(currP->pid == pid){
		listaPid *tempNextP = currP->next;
		free(currP);
		return tempNextP;
	}

	currP->next = rimuoviPidDaWhoshares(currP->next, pid);
	return currP;
}

char carattereSulNastro(nstr nastro, int testina){
	if(testina >= 0)
		return nastro.right[testina];
	testina = -testina;
	return nastro.left[testina];
}

//push per la lista di chi condivide un determinato nastro
listaPid *pushListaInt(listaPid *head, int pid){
	listaPid *nuovo;
	if(nuovo = (listaPid *)malloc(sizeof(listaPid))){
		nuovo->pid = pid;
		nuovo->next = head;
		head = nuovo;
	}
	else
		fprintf(stderr, "Errore allocazione memoria lista\n");

	return head;
}

//copia il vecchio nastro su quello nuovo
int copiaNastro(nstr vecchioNastro, nstr *nuovoNastro){
	int i;

	if(!(nuovoNastro = (nstr *)malloc(sizeof(nstr))))
		return 0;

	if(!((*nuovoNastro).left = (char *)malloc(sizeof(char) * vecchioNastro.dimLeft)))
		return 0;
	if(!((*nuovoNastro).right = (char *)malloc(sizeof(char) * vecchioNastro.dimRight)))
		return 0;

	for(i = 0; i < vecchioNastro.dimLeft; i++)
		(*nuovoNastro).left[i] = vecchioNastro.left[i];

	for(i = 0; i < vecchioNastro.dimRight; i++)
		(*nuovoNastro).right[i] = vecchioNastro.right[i];

	(*nuovoNastro).dimLeft = vecchioNastro.dimLeft;
	(*nuovoNastro).dimRight = vecchioNastro.dimRight;

	return 1;
}

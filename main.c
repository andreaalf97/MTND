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
	int pid;
	int nMosseFatte;
} processo;

typedef struct listaProcessi_s {
	struct listaProcessi_s *next;
	processo p;
} listaProcessi;

void stampaTransizione(transizione);
void stampaNastro(nstr, int);
//funzione che ritorna l'elemento nella cella <i, j> in una matrice alta height
int pos(int, int, int);			//int pos(int i, int j, int height);
//funzione che aggiunge in testa alla lista in posizione <i, j> la transizione specificata nel secondo parametro
listaTr *push(listaTr *, transizione);
//push per la lista di chi condivide un determinato nastro
listaInt *pushListaInt(listaInt *, int);
//semplice funzione che stampa la lista in input
void stampaLista(listaTr *);
char executeMachine(listaTr **, int, int, bool *, int, char *, int *);
listaProcessi *pushProcesso(listaProcessi *, processo);
listaProcessi *removeProcess(listaProcessi *, unsigned long );
char carattereSulNastro(nstr, int);





//char eseguiMacchina(listaTr **, char *, int *, int, int);
//char eseguiMacchina(listaTr **matrice, char * stringaInput, int * statiAccettazione, int nStati, int maxTransizioni)
//funzione che rivece in ingresso tutte le specifiche della macchina di Touring richiesta e stampa '1', '0', o 'U' a seconda che
//la macchina accetti la stringa in ingresso, lo rifiuti o non termini.

int main(int argc, char *argv[])
{
	// FASE DI INPUT
	int i, j;

	char *temp;	//Qui salvo le singole linee di input per lavorarci sopra
	//infatti leggo l'input una riga per volta e ne salvo le informazioni che mi servono
	size_t llinea = 0;	//dimensione della singola linea, da aggiornare ogni volta

	//variabili per leggere la prima parte di input
	//vett e' il vettore che contiene tutte le transizioni
	transizione *vett = (transizione *)malloc(2 * sizeof(transizione));	//Alloco subito 2 transizioni
	size_t dimVett = 2;								//DIMENSIONE DELL'ARRAY
	int nTransizioni = 0;								//PRIMA CELLA LIBERA

	//variabili per trovare lo stato piu' grande
	int statoMassimo = 0;

	int nCaratteriPresenti;
	int *caratteriPresenti; //questo vettore viene utilizzato due volte:
	//prima lo utilizzo quando leggo l'input riempiendolo di 0 o 1 a seconda che il carattere sia presente o meno nelle transizioni
	//nella seconda parte lo utilizzo per indicare quale riga della matrice della transizioni e' quella che rappresenta quel carattere
	//ad es. se il carattere '%' (ASCII 37) è presente e caratteriPresenti[37] e' uguale a 4, sapro' che la 5^ riga della matrice rappresenta '%'

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
	//alloca automaticamente la memoria e salva in llinea la lunghezza dell'array (compreso \n escluso \0)
	//Ora 'temp' contiene tutta la linea compresa del \n e infine il terminatore

	nCaratteriPresenti = 0;
	caratteriPresenti = (int *)calloc(256, sizeof(int)); //inizializzo a 0 il vettore dei caratteri presenti

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

		if(!caratteriPresenti[(int)vett[nTransizioni].letto]){
			caratteriPresenti[(int)vett[nTransizioni].letto] = 1;
			nCaratteriPresenti++;
		}

		nTransizioni++;
		llinea = getline(&temp, &llinea, stdin);
	}


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
	matrice = (listaTr **)calloc(dim, sizeof(listaTr *));

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
		//posizione = pos(vett[i].inizio, (int)vett[i].letto, NCARATTERI);
		posizione = pos(vett[i].inizio, caratteriPresenti[(int)vett[i].letto], nCaratteriPresenti);
		matrice[posizione] = push(matrice[posizione], vett[i]);
		//qui inserisco nella posizione <i, j> = <stato, carattere in input> la transizione
	}

/*
	for(i = 0; i < statoMassimo; i++){
		for(j = 0; j < nCaratteriPresenti; j++){
			if(matrice[pos(i, j, nCaratteriPresenti)] != NULL){
				for(k = 0; k < 256; k++){
					if(caratteriPresenti[k] == j){
						rowToChar = (char)k;
						break;
					}
				}
				printf("Dallo stato %d, leggendo %c:\n", i, rowToChar);
				stampaLista(matrice[pos(i, j, nCaratteriPresenti)]);
			}
		}
	}
*/

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
		printf("Stringa %s\n", temp);
		printf("%c\n", executeMachine(matrice, statoMassimo+1, nCaratteriPresenti, statiAccettazione, max, temp, caratteriPresenti));
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

//executeMachine(matrice, statoMassimo+1, NCARATTERI, statiAccettazione, max, temp)
char executeMachine(listaTr **matrice, int width, int nCaratteriPresenti, bool *statiAccettazione, int max, char *input, int *righeCaratteri){
	int i,	//per i loop
	posizione,	//per calcolare la posizione <i, j> nella matrice
	pidCounter;	//contiene il valore da assegnare al prossimo processo che verra' creato

	char *tempChar;	//usato per calcoli intermedi
	int tempInt;		//usato per calcoli intermedi
	listaTr *tempListaTrHead;

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

			printf("Sono nello stato %d, sto leggendo %c\n", indice->p.stato, *tempChar);
			posizione = pos(indice->p.stato, tempInt, nCaratteriPresenti);
			tempListaTrHead = matrice[posizione];
			if(tempListaTrHead){
				if(!tempListaTrHead->next){	//se c'e' solo una transizione possibile
					//COSE DA FARE:
					//1 - scrivere sul nastro quella che la transizione vuole che scriva
						//SE non devo cambiare il valore che c'e' sul nastro --> NON FARE NULLA
						//ALTRIMENTI --> 
							//SE il nastro NON e' condiviso --> scrivi e basta
							//ALTRIMENTI --> creane una copia e scrivi
					if(indice->p.testina >= 0)
						tempChar = &(indice->p.nastro.right[indice->p.testina]);	
					else
						tempChar = &(indice->p.nastro.left[-(indice->p.testina) -1]);
					//tempChar e' il carattere appena letto sul nastro
					if(*tempChar != tempListaTrHead->scritto){
						if(0){

						}
						else{	//se il nastro non e' condiviso posso scrivere e basta
							*tempChar = tempListaTrHead->scritto;
						}
					}


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
				//RIMUOVI IL PROCESSO DALLA LISTA DI QUELLI CHE CONDIVIDONO UN TEREMINATO NASTRO
				//LIBERA UN PO' DI MEMORIA MA NON SO ANCORA QUALE
				processiAttiviHead = removeProcess(processiAttiviHead, indice->p.pid);
			}


			if(indice->p.nMosseFatte > max){
				processiAttiviHead = removeProcess(processiAttiviHead, indice->p.pid);
				exitStatus = 'U';
			}


			indice = indice->next;
		}
	}




	return exitStatus;
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

listaProcessi *removeProcess(listaProcessi *currP, unsigned long pid){
	if(currP == NULL)
		return NULL;

	if(currP->p.pid == pid){
		listaProcessi *tempNextP = currP->next;
		free(currP);
		return tempNextP;
	}

	currP->next = removeProcess(currP->next, pid);
	return currP;
}

char carattereSulNastro(nstr nastro, int testina){
	if(testina >= 0)
		return nastro.right[testina];
	testina = -testina;
	return nastro.left[testina];
}

listaInt *pushListaInt(listaInt *head, int pid){
	listaInt *nuovo;
	if(nuovo = (listaInt *)malloc(sizeof(listaInt))){
		nuovo->pid = pid;
		nuovo->next = head;
		head = nuovo;
	}
	else
		fprintf(stderr, "Errore allocazione memoria lista\n");

	return head;
}
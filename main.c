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
	unsigned long pid;
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
//semplice funzione che stampa la lista in input
void stampaLista(listaTr *);
char executeMachine(listaTr **, int, int, bool *, int, char *);
listaProcessi *pushProcesso(listaProcessi *, processo);
listaProcessi *removeProcess(listaProcessi *, unsigned long );
char carattereSulNatro(nstr, int);





//char eseguiMacchina(listaTr **, char *, int *, int, int);
//char eseguiMacchina(listaTr **matrice, char * stringaInput, int * statiAccettazione, int nStati, int maxTransizioni)
//funzione che rivece in ingresso tutte le specifiche della macchina di Touring richiesta e stampa '1', '0', o 'U' a seconda che
//la macchina accetti la stringa in ingresso, lo rifiuti o non termini.

int main(int argc, char *argv[])
{
	char rowToChar;
	int k;

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
		//printf("%c\n", executeMachine(matrice, statoMassimo+1, NCARATTERI, statiAccettazione, max, temp));
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
	int i, posizione, pidCounter;


	//FASE DI INIZIALIZZAZIONE:
	//creo un processo iniziale init(cioe' una configurazione della MT da cui partire) dal
	//quale forkero' ogni volta che incontro un non determinismo
	//bisogna capire come gestire la copia del nastro in quanto deve essere condiviso
	//nel caso in cui venga modificato da un sottoprocesso
	processo init;	//processo iniziale
	listaProcessi *list = NULL;	//lista che controlla ogni istante quanti processi stanno lavorando con la macchina
	listaProcessi *indice;	//serve a scorrere all'interno della lista dei processi
	char exitStatus = '0';	//variabile per controllare alla fine se ritornare 0 o U


	init.stato = 0;
	init.testina = 0;
	init.pid = 0;
	pidCounter = 0;
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

	list = pushProcesso(list, init);

	//INIZIO DELL'ESECUZIONE
	while(list){		//finche' ci sono processi da eseguire
		indice = list;
		while(indice){	//per ogni processo ancora in esecuzione
			//4 casi:
			// -lo stato in cui mi trovo e' finale e quindi ritorno subito 1
			// -non ci sono piu' transizioni di uscita
			// -c'e' una sola transizione possibile
			// -ci sono due o piu' transizioni possibili

			if(statiAccettazione[indice->p.stato])	//se mi trovo in uno stato di accettazione per uno qualsiasi dei sottoprocessi ritorno subito 1
				return '1';

			posizione = pos(indice->p.stato, carattereSulNatro(indice->p.nastro, indice->p.testina), NCARATTERI);
			if(matrice[posizione]){

			}
			else{	//cioe' da questo stato con questo carattere non ci sono transizioni possibili
				//devo rimuovere il processo da quelli in esecuzione
				list = removeProcess(list, indice->p.pid);
			}





			indice = indice->next;
		}
	}




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

char carattereSulNatro(nstr nastro, int testina){
	if(testina >= 0)
		return nastro.right[testina];
	testina = -testina;
	return nastro.left[testina];
}

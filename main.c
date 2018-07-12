// Lettura della transizioni
//tr
//0 a c R 1
//...
//2 b d L 3
//acc

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define NCARATTERI 256

typedef struct transizioni_s {
	int inizio, fine;
	char letto, scritto, mossa;
} transizioni;

typedef struct listaTransizioni_s {
	struct listaTransizioni_s *next;	//puntatore alla transizione successiva
	char scritto, mossa;							
	int fine;
}	listaTr;

int pos(int, int, int);
listaTr *push(listaTr *, transizioni);
void stampaLista(listaTr *);

int main(int argc, char *argv[])
{
	// FASE DI INPUT

	int i, j;

	char *temp;	//Qui salvo le singole linee di input per lavorarci sopra
	size_t llinea = 0;	//dimensione della singola linea, da aggiornare ogni volta

	//variabili per leggere la prima parte di input
	//vett è il vettore che contiene tutte le transizioni
	transizioni *vett = (transizioni *)malloc(10 * sizeof(transizioni));	//Alloco subito 10 transizioni
	size_t dimVett = 10;	//DIMENSIONE DELL'ARRAY
	int nTransizioni = 0;								//PRIMA CELLA LIBERA

	//variabili per trovare lo stato più grande
	int statoMassimo = 0;

	//Variabili per la creazione della tabella che rappresenta la macchina di Touring del file
	listaTr **matrice;
	int dim, posizione;

	//variabili per riconoscere gli stati di accettazione
	int *statiAccettazione;
	int t;

	//variabili per il numero massimo di mosse effettuabili
	int max;

	llinea = getline(&temp, &llinea, stdin);	
	//Prende una linea intera dallo stdin e la mette nel vettore temp
	//alloca automaticamente la memoria e salva in n la lunghezza dell'array (compreso \n escluso \0)
	//Ora 'temp' contiene tutta la linea compresa del \n e infine il terminatore

	if(strcmp("tr\n", temp) != 0){
		fprintf(stderr, "Il file non inizia per err\n");
		return -1;
	}	//controlla che il file input inizi per TR

	llinea = getline(&temp, &llinea, stdin);
	while(strcmp(temp, "acc\n")){		//finchè non trova acc
		//operazioni sulle TRANSIZIONI

		//Riallocazione di memoria -- Se serve più memoria alloco altri 10 spazi e vado avanti
		if(nTransizioni >= dimVett){
			vett = (transizioni *)realloc(vett, (dimVett * sizeof(transizioni)) + (10 * sizeof(transizioni)));
			dimVett += 10;
		}

		sscanf(temp, "%d%*c%c%*c%c%*c%c%d", &(vett[nTransizioni].inizio), &(vett[nTransizioni].letto), &(vett[nTransizioni].scritto), &(vett[nTransizioni].mossa), &(vett[nTransizioni].fine));
		//Questa sscanf legge la linea e mette i vari parametri al posto giusto

		//qui sotto calcolo quale sia lo stato più grande
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
	//statoMassimo contiene quale stato è il più grande numericamente, quindi se il più grande è k,
	//esistono sicuramente tutti gli stati 0, 1, ... , k-1, 

	//Ora devo creare una matrice di liste in cui per ogni coppia STATO - INPUT
	//ho una sequenza di possibili transizioni

	//Per una matrice MATRIX di dimensione AxB alloco un vettore di dimensione A*B e trovo
	//l'elemento [i][j] come MATRIX[ i*B + j]

	//devo fare una tabella di puntatori a NULL grande = (statoMassimo+1) x NCARATTERI
	dim = (statoMassimo + 1) * NCARATTERI; 
	matrice = (listaTr **)malloc(dim * sizeof(listaTr *));
	for(i = 0; i < dim; i++)
		matrice[i] = NULL;

	//Per ogni transizione che parte dallo stato x leggendo y aggiungo alla lista corrispondente
	//tale transizione.
	for(i = 0; i < nTransizioni; i++){
		posizione = pos(vett[i].inizio, (int)vett[i].letto, NCARATTERI);
		matrice[posizione] = push(matrice[posizione], vett[i]);
	}

	/*for(i = 0; i < statoMassimo + 1; i++)
		for(j = 0; j < NCARATTERI; j++)
			if(matrice[pos(i, j, NCARATTERI)] != NULL){
				printf("Dallo stato %d, leggendo %c:\n", i, (char)j);
				stampaLista(matrice[pos(i, j, NCARATTERI)]);
			}*/

	//A questo punto dentro la matrice ho tutto ciò che mi serve e posso andare avanti a leggere il file di input
	//Il vettore delle transizioni non serve più
	free(vett);


	//Ora devo leggere quali sono gli stati di accettazione
	//getline aveva già letto 'acc' quindi con questa chiamata legge il primo numero dopo acc
	statiAccettazione = (int *)calloc((statoMassimo + 1), sizeof(int));
	llinea = getline(&temp, &llinea, stdin);	
	while(strcmp(temp, "max\n")){
		sscanf(temp, "%d", &t);
		statiAccettazione[t] = 1;
		llinea = getline(&temp, &llinea, stdin);	
	}

	//Ora ho la matrice pronta e un vettore chiamato statiAccettazione che contiene
	//0 o 1 a seconda che la posizione i è uno stato di accettazione o meno
	//C'è anche corrispondenza tra la posizione i e la posizione della colonna della matrice

	//Ora leggo il numero massimo di mosse effettuabili
	llinea = getline(&temp, &llinea, stdin);	
	sscanf(temp, "%d", &max);

	llinea = getline(&temp, &llinea, stdin);	//ora temp dovrebbe contenere la parola 'run'
	if(strcmp(temp, "run\n") != 0){
		fprintf(stderr, "Non ho letto la parola 'run'\n");
		return -1;
	}

	free(temp);

	//ORA E' TUTTO PRONTO PER L'ESECUZIONE
	//matrice = 						matrice che contiene per ogni i(stato) e j (carattere letto) la lista delle transizioni
	//											che devo possono essere eseguite
	//statiAccettazione = 	vettore di 0 e 1 che è a 1 solo se i (indice) è uno stato di accettazione
	//max = 								variabile che contiene il numero massimo di mosse effettuabili dalla macchina di Touring.
	
	//DEVO LEGGERE LA PROSSIMA LINEA ED ESEGUIRE LA MACCHINA SU DI ESSA


	free(statiAccettazione);
	free(matrice);

	//FASE DI OUTPUT
	return 0;
}


int pos(int i, int j, int B)
{
	return ((i * B) + j);
}

listaTr *push(listaTr *head, transizioni transizione){
	listaTr *nuovo;
	if(nuovo = (listaTr *)malloc(sizeof(listaTr))){
		nuovo->scritto =	transizione.scritto;
		nuovo->mossa = transizione.mossa;
		nuovo->fine = transizione.fine;
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
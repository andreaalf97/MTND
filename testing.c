typedef struct listaProcessi_s {
	struct listaProcessi_s *next;
	processo *p;
} listaProcessi;

char executeMachine(listaTr **matrice, int statoMassimo, int nCaratteriPresenti, bool *statiAccettazione, int max, char *input, int *righeCaratteri){
	char exitStatus = '0';

	int testCounter = 0;

	//****************************
	processo *init = NULL;
	nstr *nastroInit = NULL;
	listaProcessi *processiAttiviHead = NULL;
	listaProcessi *indice = NULL;
	processo *indiceProcesso = NULL;
	int newPidCounter;
	//****************************
	//L'esecuzione della macchina si basa sulla creazione di processi ogni volta che incontro un NON determinismo

	//creazione processo iniziale:
	nastroInit = createNastro(nastroInit, stringa, max);
	init = createProcess(init, 0, 0, 0, nastroInit);

	processiAttiviHead = pushListaProcessi(processiAttiviHead, init);
	newPidCounter = 1;



	while(processiAttiviHead){
		indice = processiAttiviHead;
		while(indice){
			indiceProcesso = indice->p;
			printf("Sto eseguendo il processo %d\n", indiceProcesso->pid);

			if(testCounter > 10)
				processiAttiviHead = popListaProcessi(processiAttiviHead, processo *p);

			testCounter++;
			indice = indice->next;
		}
	}


	return exitStatus;
}

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

		free(temp->p->nastro->left);
		free(temp->p->nastro->right);
		deleteListaInt(temp->p->nastro->whoShares);
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
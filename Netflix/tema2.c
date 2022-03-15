/*	Voinea Raul Cristian - 314CB	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "TListaGenerica.h"
#include "TCoada.h"
#include "TStiva.h"

#define MAX_line 1024
#define MAX_seriale 2048
#define MAX_sezoane 10
#define MAX_episoade 20
#define MAX_durata 120
#define MAX_nume 33
#define MAX_top 10

/* numar maxim elemente coada / stiva */

typedef struct {
	char nume[MAX_nume];
	double rating;
	TCoada *sezoane;
	int dime;
} TSerial;

TSerial *InitSerial(char *nume, double rating)
{
	TSerial *serial = malloc(sizeof(TSerial));
	if (serial == NULL)
		return NULL;
	serial->dime = 0; 
	serial->sezoane = InitQ();
	strcpy(serial->nume, nume);
	serial->rating = rating;
	return serial;
}


// elibereaza memoria unui serial
void Elib_serial(TSerial *serial)
{
	// extragem din coada sezeoane coada sezon
	TCoada *sezon = ExtrQ(serial->sezoane);
	int *episod = NULL;

	while (sezon != NULL)
	{
		// etragerile din coada automate elibereaza celulele
		episod = ExtrQ(sezon);
		// episode este tot o coada formata din inturi
		while (episod != NULL)
		{
			// eliberam int
			free(episod);
			// la extragere se elibereaza celula
			episod = ExtrQ(sezon);
		}
		// eliberam TCoada de inturi
		free(sezon);
		sezon = ExtrQ(serial->sezoane);
	}
	// elibeream TCoada de cozi
	free(serial->sezoane);
}

// functia gaseste serialul cu numele specifica si il returneaza
// print scoatere din lista sau referinta (sterge 0/1)

TSerial *Gaseste_serial(TCelulaG *categorii[4], TCoada *watch_later,
				TStiva *currently_watching, char *nume, int sterge)
{
	// cautam main intai prin categorii
	int i;
	for (i = 1; i <= 4; i++)
	{
		TCelulaG *curent = categorii[i - 1];
		TCelulaG *precedent = NULL;
		while (curent != NULL)
		{
			TSerial *serial = curent->data;
			// daca am gasit serialul in lista il vom returna
			// in acest if
			if (strcmp(serial->nume, nume) == 0)
			{
				// daca sterge este setat - extragem celula din lsta
				if (sterge == 1)
				{
					if (precedent == NULL)
					{
						categorii[i - 1] = curent->urm;
					}
					else
					{
						precedent->urm = curent->urm;
					}
					// eliberam celula
					free(curent);
				}
				// returnam referinta
				return serial;
			}
			precedent = curent;
			curent = curent->urm;
		}
	}

	int ok = 0;
	// daca nu s-a gasit in categorii cautam in coada watch_later si
	// coada currently_watching
	TCoada *coada = InitQ();
	TStiva *stiva = InitS();
	TSerial *gasit = NULL;
	TSerial *serial = ExtrQ(watch_later);

	// cautam in coada watch later
	while (serial != NULL) {
		if (strcmp(serial->nume, nume) == 0)
		{
			gasit = serial;
			// setam variablia ok sa stim ca l-am gasit
			ok = 1;
		}
		if (strcmp(serial->nume, nume) != 0 || sterge != 1)
		{
			// salvam serialul extras in coada temporara "coada"
			IntrQ(coada, serial);
		}
		serial = ExtrQ(watch_later);
	}
	// cautam in stiva currently_watching
	serial = Pop(currently_watching);
	while (serial != NULL)
	{
		if (strcmp(serial->nume, nume) == 0)
		{
			gasit = serial;
			// setam variablia ok sa stim ca l-am gasit
			ok = 1;
		}
		if (strcmp(serial->nume, nume) != 0 || sterge != 1)
		{
			// salvam serialul extras in stiva temporara "stiva"
			Push(stiva, serial);
		}
		serial = Pop(currently_watching);
	}

	// intoarcem cozile(coada nu se roteste) si stivele(stiva se roteste)

	// extragem din coada temporara si bagam in coada initiala watch_later
	serial = ExtrQ(coada);
	while (serial != NULL)
	{
		IntrQ(watch_later, serial);
		serial = ExtrQ(coada);
	}
	// extragem din stiva temporara si bagam in stiva initiala currently_watching
	serial = Pop(stiva);
	while (serial != NULL)
	{
		Push(currently_watching, serial);
		serial = Pop(stiva);
	}

	free(coada);
	free(stiva);
	// returnam serialul daca l-am gasit
	if (ok == 1)
		return gasit;
	return NULL;
}

//adauga un sezon la un serial citind datele utilizand strtok

void Add_sez(TSerial *serial) {
	TCoada *sezon = InitQ(); // e o simpla coada de inturi
	int nr_episoade = atoi(strtok(NULL, " ")); // citim numarul de episoade
	int j;
	
	for(j = 1; j <= nr_episoade; j++)
	{
		// pentru fiecare episod alocam o zona de memorie pentru a fi
		// bagat in campul data a unui nod de lista gestionata de o coada
		int *durata = malloc(sizeof(int));
		*durata = atoi(strtok(NULL, " "));
		// introducem episod(durata) in coada sezon
		IntrQ(sezon, durata);
	}
	// introducem coada sezon in coada sezoane
 	IntrQ(serial->sezoane, sezon);
}


// afisare nume si rating serial

void Afi_serial(FILE *outputFile, TSerial *serial)
{
	if (serial == NULL)
		return;
	fprintf(outputFile, "(");
	fprintf(outputFile, "%s", serial->nume);
	fprintf(outputFile, ", ");
	fprintf(outputFile, "%.1lf", serial->rating);
	fprintf(outputFile, ")");
}

// gaseste pozitia serialului in lista

int Pos_serial(TCelulaG *lista, char *nume)
{
	int poz = 1;
	while (lista != NULL)
	{
		TSerial *serial = lista->data;
		// returnam pozitia cand ii corspunde numele
		if (strcmp(serial->nume, nume) == 0)
			return poz;
		poz++;
		lista = lista->urm;
	}
	return -1;
}

//  Adaugare serial intr-o anumita categorie
void Add(FILE *outputFile, TCelulaG *categorii[4], int top)
{
	int ID = atoi(strtok(NULL, " ")) - 1;
	char *nume = strtok(NULL, " ");
	double rating = strtod(strtok(NULL, " "), NULL);
	TSerial *serial = InitSerial(nume, rating);
	TCelulaG *celula = InitL(serial);
	TCelulaG *precedent = NULL;
	TCelulaG *curent = NULL;
	int nr_sezoane = atoi(strtok(NULL, " "));
	int i;
	for (i = 1; i <= nr_sezoane; i++)
	{
		Add_sez(serial);
	}
	if (top == 1)
	{
		// daca top e pe 1 trebuie introdus serialul la pozitia specificata
		// de ID
		int poz = 0;
		// categoria[3] este categoria top10
		curent = categorii[3];
		// navigam prin lista pana nu se termina ori pozitia corespunde
		while (curent != NULL && poz < ID)
		{
			poz++;
			precedent = curent;
			curent = curent->urm;
		}
		// daca precedentul nu a reusit sa fie setat inseram la inceputul
		// listei
		if (precedent == NULL)
		{
			celula->urm = categorii[3];
			categorii[3] = celula;
		}
		else
		{
			celula->urm = precedent->urm;
			precedent->urm = celula;
		}
		// aici verificam daca lista nu a depasit lungimea top10 MAX_top
		curent = categorii[3];
		poz = 0;
		while (curent != NULL && poz < MAX_top)
		{
			if (poz + 1 == MAX_top && curent->urm != NULL)
			{
				// daca exista celula dupa limita superioara
				// eliminam celula si serialul din campul data
				Elib_serial(curent->urm->data);
				free(curent->urm->data);
				free(curent->urm);
			 	curent->urm = NULL;
			}
			poz++;
			curent = curent->urm;
		}
		curent = categorii[3];
		// facem o afisare a listei top10 din categoria[3]
		fprintf(outputFile, "Categoria top10: [");
		while (curent != NULL)
		{
			serial = curent->data;
			Afi_serial(outputFile, serial);
			if (curent->urm != NULL)
				fprintf(outputFile, ", ");
			curent = curent->urm;
		}
		fprintf(outputFile, "].\n");
	}
	else
	{
		// in cazul in care top e pe 0 inserearea are loc pe una
		// din pozitiile 1 sau 2 sau 3 determinat de ID
		// iar inserarea va fi sortata dupa rating apoi in cazul
		// in care ratingurile corespund - dupa nume
		curent = categorii[ID];
		// navigam print lista pana cand nu dam de o pozitie potrivita
		while (curent != NULL)
		{
			// navigam cat timp ratingul e mai mic deoarce ne asteptam
			// la o pozitie unde ratingul va fi mai mare
			double rating_curent = ((TSerial *)curent->data)->rating;

			if (rating_curent <= rating)
				break;

			precedent = curent;
			curent = curent->urm;
		}
		// dupa ce s-a facut prima navigare trebuie sa navigam inca
		// o data dar de data asta navigam cat timp ratingurile sunt
		// egale (un motiv din care putea sa se opreasca while-ul de sus) si
		// nuemele celulei se afla in ordine lexicografica inversa
		while (curent != NULL)
		{
			double rating_curent = ((TSerial *)curent->data)->rating;
			char *nume_curent = ((TSerial *)curent->data)->nume;

			if (rating_curent != rating || strcmp(nume_curent, nume) >= 0)
				break;

			precedent = curent;
			curent = curent->urm;
		}
		// dupa ce am terminat navigarea de sus (motivele opririi de sus:
		// s-a terminat lista, lista era inexistenta, s-a oprit ca
		// s-a realizat conditia ca ratingul e egal dar numele deja
		// este in pozitie corecta lexicografic) inseram elementul
		// elementul in lista folosindu-ne de variabelele de parcurgere
		if (precedent == NULL)
		{
			celula->urm = categorii[ID];
			categorii[ID] = celula;
		}
		else
		{
			celula->urm = precedent->urm;
			precedent->urm = celula;
		}
		fprintf(outputFile,
				"Serialul %s a fost adaugat la pozitia %d.\n", nume,
				Pos_serial(categorii[ID], nume));
	}
}

// functia raspunde de afisarea listelor, cozilor, si stivelor
// ce contin ca informatie serialele
void Afiseaza(FILE *outputFile, TCelulaG *categorii[4], TCoada *watch_later,
					TStiva *currently_watching, TStiva *history)
{
	char *X = strtok(NULL, " ");
	int ID = atoi(X) - 1;
	fprintf(outputFile, "Categoria %s: [", X);
	// ID va fi -1 deoarece atoi de un string care nu contine cifre la
	// inceput va fi 
	// daca e un id valid sau top afisam lista
	if (ID > -1 || strcmp(X, "top10") == 0)
	{
		TCelulaG *curent;
		if (ID == -1)
			// categorii[3] e lista top10
			curent = categorii[3];
		else
			curent = categorii[ID];
		// navigam prin lista si afisam fiecare serial din campul data
		while (curent != NULL)
		{
			TSerial *serial = curent->data;
			Afi_serial(outputFile, serial);
			if (curent->urm != NULL)
				fprintf(outputFile, ", ");
			curent = curent->urm;
		}
	}
	// daca se cere de afisam coada later
	// pasii sunt umratorii:
	// extragem fiecare serial, il afisam, il bagam in coada temporara
	// dupa care din coada temporara bagam inapoi
	if (strcmp(X, "later") == 0)
	{
		TCoada *coada = InitQ();
		TSerial *serial = ExtrQ(watch_later);

		while (serial != NULL)
		{
			Afi_serial(outputFile, serial);
			IntrQ(coada, serial);
			serial = ExtrQ(watch_later);
			if (serial != NULL)
				fprintf(outputFile, ", ");
		}
		serial = ExtrQ(coada);
		while (serial != NULL)
		{
			IntrQ(watch_later, serial);
			serial = ExtrQ(coada);
		}
		free(coada);
	}
	// pentru a afisa o stiva vom face asemanator ca la coada
	// extragem fiecare serial, il afisam, il bagam in stiva temporara
	// dupa care din stiva temporara bagam inapoi
	if (strcmp(X, "history") == 0 || strcmp(X, "watching") == 0)
	{
		TStiva *stiva = InitS();
		TStiva *sursa = NULL;

		// procedura e aceeasi pentru stiva history si stiva currently_watching
		if (strcmp(X, "history") == 0)
			sursa = history;
		if (strcmp(X, "watching") == 0)
			sursa = currently_watching;

		TSerial *serial = Pop(sursa);
		while (serial != NULL)
		{
			Afi_serial(outputFile, serial);
			Push(stiva, serial);
			serial = Pop(sursa);
			if (serial != NULL)
				fprintf(outputFile, ", ");
		}
		serial = Pop(stiva);
		while (serial != NULL)
		{
			Push(sursa, serial);
			serial = Pop(stiva);
		}
		free(stiva);

	}
	fprintf(outputFile, "].\n");
} 


// o procedura de introducere a unui serial intr-o coada
//  cu afisarea pozitiei lui in ea
void Add_later(FILE *outputFile, TCoada *watch_later, TSerial *serial)
{
	IntrQ(watch_later, serial);
	fprintf(outputFile,
		"Serialul %s se afla in coada de asteptare pe pozitia %d.\n",
		serial->nume, DimQ(watch_later));
}

// functia raspunde de vizualizarea serialelor

void Watch(FILE *outputFile, TCelulaG *categorii[4],
						TCoada *watch_later, TStiva *currently_watching,
						TStiva *history)
{
	// gasim numele serialului si durata vizualizarii
	char *nume = strtok(NULL, " ");
	int durata = atoi(strtok(NULL, " "));
	// cautam serialul in unele din listele, cozile sau stivele specificate
	TSerial *serial = Gaseste_serial(categorii, watch_later,
			 							currently_watching, nume, 1);
	// extragem primul sezon
	TCoada *sezon = ExtrQ(serial->sezoane);
	// serial_nou este coada temporara de sezoane
	// uenele sezoane vor fi eliminate dupa vizualizare
	TCoada *serial_nou = InitQ();
	while (sezon != NULL && durata > 0)
	{
		// extragem din sezon primul episod(durata)
		TCoada *sezon_nou = InitQ();
		int *episod = ExtrQ(sezon);
		while (episod != NULL)
		{
			// gasim minumul dintre durata episodului si timpul
			// acordat vizualizarii
			int min = 0;
			if (*episod > durata)
				min = durata;
			if (*episod <= durata)
				min = *episod;
			// extragem acest timp din unul din operanzi
			// cel putin unul din ei va fi setat pe zero
			*episod -= min;
			durata -= min;
			// daca episodul curent a fost epuizat nu mai 
			// are sens sa-l bagam inapoi in coada sezon
			if (*episod > 0)
			{
				IntrQ(sezon_nou, episod);
			}
			if (*episod <= 0)
			{
				
				free(episod);
			}

			episod = ExtrQ(sezon); 
		}
		// daca sa ajuns la situatia ca nu am bagat macar un episod in
		// sezon -> sezonul este gol -> deci a fost privit in totalmente
		if (VidaQ(sezon_nou) == 1)
		{
			free(sezon_nou);
		}
		else
		{
			// in cazul inc care sezonul nou are macar un episode
			// se introduce sezonul in coada temporara de sezoane
			IntrQ(serial_nou, sezon_nou);
		}
		free(sezon);
		sezon = ExtrQ(serial->sezoane);
	}
	// bagam din coada temporara de sezoane in coada serialului
	while (sezon != NULL)
	{
		IntrQ(serial_nou, sezon);
		sezon = ExtrQ(serial->sezoane);
	}
	free(serial->sezoane);
	// daca serialul a fost vizionat complet il bagam in stiva
	// history in caz contrar il bagam in stia currently watching
	serial->sezoane = serial_nou;
	if (!VidaQ(serial_nou))
	{
		Push(currently_watching, serial);
	} 
	else
	{
		// afisam rezultatul in cazul in care a fost vizionat
		// complet
		fprintf(outputFile, "Serialul %s a fost vizionat integral.\n", nume);
		Push(history, serial);
	}
}

// eliberearea memoriei pentru fiecare lsita, coada si stiva
void Elibereaza(TCelulaG *categorii[4], TCoada *watch_later,
				TStiva *currently_watching, TStiva *history)
{
	// eliberam stiva history
	TSerial *serial = Pop(history);
	while (serial != NULL)
	{
		Elib_serial(serial);
		free(serial);
		serial = Pop(history);
	}
	// eliberam stiva currently_watching
	serial = Pop(currently_watching);
	while (serial != NULL)
	{
		Elib_serial(serial);
		free(serial);
		serial = Pop(currently_watching);
	}
	// eliberam coada watch_later
	serial = ExtrQ(watch_later);
	while (serial != NULL) {
		Elib_serial(serial);
		free(serial);
		serial = ExtrQ(watch_later);
	}
	// eliberam fiecare categorie
	int i;
	for (i = 1; i <= 4; i++)
	{
		TCelulaG *lista = categorii[i - 1], *urm;
		while (lista != NULL)
		{
			urm = lista->urm;
			Elib_serial(lista->data);
			free(lista->data);
			free(lista);
			lista = urm;
		}
	}
	free(currently_watching);
	free(history);
	free(watch_later);
}

int main(int argc, char *argv[])
{ 
	char *inputFileName = strdup(argv[1]);
	char *outputFileName = strdup(argv[2]);

	FILE *inputFile = fopen(inputFileName, "rt");
	FILE *outputFile = fopen(outputFileName, "wt");

	char *line = NULL;
    size_t len = 0;

	TCoada *watch_later = InitQ();
	TStiva *history = InitS();
	TStiva *currently_watching = InitS();

	TCelulaG *categorii[4] = {NULL, NULL, NULL, NULL};
	// apelam comenzile corespunzatoare

	while (getline(&line, &len, inputFile) != -1)
	{
		line[strlen(line) - 1] = 0;
		char *command = strtok(line, " ");
		
		if (strcmp(command, "add") == 0)
		{
			Add(outputFile, categorii, 0);
		}
		if (strcmp(command, "add_sez") == 0)
		{
			char *nume = strtok(NULL, " ");
			TSerial *serial = Gaseste_serial(categorii, watch_later,
										currently_watching, nume, 0);
			Add_sez(serial);
			fprintf(outputFile, "Serialul %s are un sezon nou.\n", nume);
		}
		if (strcmp(command, "add_top") == 0)
		{
			Add(outputFile, categorii, 1);
		}
		if (strcmp(command, "later") == 0)
		{
			// scoatem serialul si il introducem in coada later
			char *nume = strtok(NULL, " ");
			TSerial *serial = Gaseste_serial(categorii, watch_later,
			 							currently_watching, nume, 1);
			Add_later(outputFile, watch_later, serial);
		}
		if (strcmp(command, "show") == 0)
		{
			Afiseaza(outputFile, categorii, watch_later,
						currently_watching, history);
		}
		if (strcmp(command, "watch") == 0)
		{
			Watch(outputFile, categorii, watch_later,
						currently_watching, history);
		}
	}

	Elibereaza(categorii, watch_later, currently_watching, history);



	free(line);
	free(inputFileName);
	free(outputFileName);
	fclose(inputFile);
	fclose(outputFile);
	return 0;
}

/*	Voinea Raul Cristian - 314CB	*/

#ifndef COADA
#define COADA

typedef struct {
	TCelulaG *fata, *spate;
	int dime;
} TCoada;

TCoada *InitQ() {
	TCoada *coada = malloc(sizeof(TCoada));
	coada->dime = 0;
	coada->fata = coada->spate = NULL;
	return coada;
}

int VidaQ(TCoada *coada) {
	if (coada->dime == 0)
		return 1;
	return 0;
}

void *ExtrQ(TCoada *coada) {
	if (VidaQ(coada) == 1)
		return NULL;
	TCelulaG *celula = coada->fata;
	void *data = celula->data;
	coada->fata = coada->fata->urm;
	coada->dime--;
	free(celula);
	return data;
}

void IntrQ(TCoada *coada, void *data) {

	TCelulaG *celula = InitL(data);
	if (VidaQ(coada) == 0)
	{
		coada->spate->urm = celula;
		coada->spate = celula;
	}
	if (VidaQ(coada) == 1)
	{
		coada->fata = celula;
		coada->spate = celula;
	}
	coada->dime++;
}

int DimQ(TCoada *coada) {
	return coada->dime;
}



#endif
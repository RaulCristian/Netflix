/*	Voinea Raul Cristian - 314CB	*/

#ifndef STIVA
#define STIVA

typedef struct {
	TCelulaG *virf;
	int dime;
} TStiva;

TStiva *InitS()
{
	TStiva *stiva = malloc(sizeof(TStiva));
	stiva->dime = 0;
	stiva->virf = NULL;
	return stiva;
}

int VidaS(TStiva *stiva)
{
	return (stiva->virf == NULL);
}

void *Pop(TStiva *stiva)
{
	if (stiva->virf == NULL)
		return NULL;
	TCelulaG *celula = stiva->virf;

	stiva->virf = celula->urm;
	stiva->dime--;
	
	void *data = celula->data;
	free(celula);

	return data;
}

void Push(TStiva *stiva, void *data)
{
	TCelulaG *node = IntrL(stiva->virf, data);
	
	stiva->virf = node;
	stiva->dime++;

}

int DimS(TStiva *stiva)
{
	return stiva->dime;
}

#endif

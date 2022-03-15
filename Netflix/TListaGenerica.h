/*	Voinea Raul Cristian - 314CB	*/

#ifndef LISTA_GENERICA
#define LISTA_GENERICA

typedef struct node {
	struct node *urm;
	void *data;
} TCelulaG;

TCelulaG *InitL(void *data) {
	TCelulaG *celula = malloc(sizeof(TCelulaG));
	if (celula != NULL) {
		celula->data = data;
		celula->urm = NULL;
		return celula;
	}
	return NULL;
}

TCelulaG *IntrL(TCelulaG *lista, void *data) {
	TCelulaG *celula = InitL(data);

	if (celula == NULL)
		return lista;

	celula->urm = lista;

	return celula;
}

#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "/home/lucassleoni/Desktop/FIUBA/Algoritmos y Programación/Algo2/TP's/TDA's/TDA Lista (TP N° 4)/lista.h"
#include "hash.h"
#include "hash_iterador.h"

#define ERROR -1
#define EXITO 0

typedef struct nodo{
	void* elemento;
	const char* clave;	// Modificación agregada para el TDA Hash
	struct nodo* siguiente;
} nodo_t;

struct hash{
	lista_t** vector_hash;
	int capacidad;
	int cantidad_elementos;
	hash_destruir_dato_t destructor;
};

/*
 * Crea el hash reservando la memoria necesaria para él.
 * Destruir_elemento es un destructor que se utilizará para liberar
 * los elementos que se eliminen del hash.
 * Capacidad indica la capacidad minima inicial con la que se crea el hash.
 * Devuelve un puntero al hash creado o NULL en caso de no poder crearlo.
 */
hash_t* hash_crear(hash_destruir_dato_t destruir_elemento, size_t capacidad){
	if(destruir_elemento == NULL){
		return NULL;
	}

	hash_t* hash = malloc(sizeof(hash_t));
	if(hash == NULL){
		return NULL;
	}

	hash->vector_hash = malloc(sizeof(lista_t*) * capacidad);
	if(hash->vector_hash = NULL){
		free(hash);
		return NULL;
	}

	int i = 0;
	bool hay_lista_nula = false;

	while((i < capacidad) && (!hay_lista_nula)){
		hash->vector_hash[i] = lista_crear();

		if(hash->vector_hash[i] == NULL){
			hay_lista_nula = true;
			for(int j = 0; j < i; j++){
				lista_destruir(hash->vector_hash[j]);
			}
		}

		i++;
	}

	if(hay_lista_nula){
		free(hash->vector_hash);
		free(hash);
		return NULL;
	}

	hash->capacidad = capacidad;
	hash->cantidad_elementos = 0;
	hash->destructor = destruir_elemento;

	return hash;
}

// Pre C.: Recibe un puntero al Hash.
// Post C.: Devuelve 'true' si la estructura o el vector Hash son nulas.
bool hay_error(hash_t* hash){
	return((hash == NULL) || (hash->vector_hash == NULL));
}

// Pre C.: Recibe un puntero al Hash.
// Post C.: Devuelve 'true' si la la cantidad de elementos del Hash es igual a '0'.
bool hash_vacio(hash_t* hash){
	return(hash->cantidad_elementos == 0);
}

// Pre C.: Recibe la clave sobre la que se va a calcular el valor de Hash.
// Post C.: Devuelve el valor de Hash correspondiente a la clave recibida.
int hashear(const char* clave){
	int valor_clave = 0;

	for(int i = 0; i < strlen(clave); i++){
		valor_clave += (int)clave[i];
	}

	return(valor_clave % (hash->capacidad));
}

/*
 * Inserta un elemento reservando la memoria necesaria para el mismo.
 * Devuelve 0 si pudo guardarlo o -1 si no pudo.
 */
int hash_insertar(hash_t* hash, const char* clave, void* elemento){
	if(hay_error(hash)){
		return ERROR;
	}

	int posicion_hash = hashear(clave);
	lista_insertar(hash->vector_hash[posicion_hash], clave, elemento);

	return EXITO;
}

// Pre C.: Recibe un puntero a la lista y a la clave buscada.
// Post C.: Devuelve un puntero al nodo con la clave buscada o NULL en caso de error.
nodo_t* buscar_nodo_por_clave(nodo_t* nodo_inicio, const char* clave){	// Función agregada para el TDA Hash
	if(lista_vacia(lista)){
		return NULL;
	}

	bool nodo_encontrado = false;
	nodo_t* nodo_buscado = NULL;
	nodo_t* nodo_aux = nodo_inicio;
	
	while(!nodo_encontrado){
		if(nodo_aux->clave == clave){
			nodo_buscado = nodo_aux;
			nodo_encontrado = true;
		}
		else{
			nodo_aux = nodo_aux->siguiente;
		}
	}

	return nodo_buscado;
}

// Pre C.: Recibe un puntero al nodo que se quiere borrar y un puntero a la clave del mismo.
// Post C.: Devuelve '0' si se borró correctamente o '-1' en caso de error.
int borrar_nodo_por_clave(lista_t* lista, const char* clave, hash_destruir_dato_t destructor){
	if(lista_vacia(lista)){
		return ERROR;
	}

	nodo_t* nodo_buscado = buscar_nodo_por_clave(lista->nodo_inicio, clave);	// Está mal (intentar con iterador)
	destructor(nodo_buscado->elemento);

	nodo_t* nodo_aux = nodo_buscado->siguiente->siguiente;
	free(nodo_buscado->siguiente);
	nodo_buscado->siguiente = nodo_aux;

	(lista->cantidad)--;

	return EXITO;
}
/*
 * Quita un elemento del hash e invoca la funcion destructora
 * pasandole dicho elemento.
 * Devuelve 0 si pudo eliminar el elemento o -1 si no pudo.
 */
int hash_quitar(hash_t* hash, const char* clave){
	if((hay_error(hash)) || (hash_vacio(hash))){
		return ERROR;
	}

	int posicion_hash = hashear(clave);
	nodo_t* nodo_borrar = borrar_nodo_por_clave(hash->vector_hash[posicion_hash], clave, hash->destructor);

	return EXITO;
}

/*
 * Devuelve un elemento del hash con la clave dada o NULL si dicho
 * elemento no existe.
 */
void* hash_obtener(hash_t* hash, const char* clave){

}

/*
 * Devuelve true si el hash contiene un elemento almacenado con la
 * clave dada o false en caso contrario.
 */
bool hash_contiene(hash_t* hash, const char* clave){
	
}

/*
 * Devuelve la cantidad de elementos almacenados en el hash.
 */
size_t hash_cantidad(hash_t* hash){
	if((hay_error(hash)) || (hash_vacio(hash))){
		return 0;
	}

	return(hash->cantidad_elementos);
}

/*
 * Destruye el hash liberando la memoria reservada y asegurandose de
 * invocar la funcion destructora con cada elemento almacenado en el
 * hash.
 */
void hash_destruir(hash_t* hash){
	if(hay_error(hash)){
		return;
	}

	while(!hash_vacio(hash)){
		for(int i = 0; i < (hash->capacidad); i++){
			lista_destruir(hash->vector_hash[i]);
		}
	}

	free(hash->vector_hash);
	free(hash);
}
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
	void* clave;	// Modificación agregada para el TDA Hash
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

	nodo_t* nuevo_nodo = malloc(sizeof(nodo_t));
	if(nuevo_nodo == NULL){
		return ERROR;
	}

	int posicion_hash = hashear(clave);

	
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
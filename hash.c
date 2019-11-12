#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "lista.h"
#include "hash.h"
#include "hash_iterador.h"

#define ERROR -1
#define EXITO 0
#define FACTOR_MAX 0.75
#define COEFICIENTE_REHASH 1.25

typedef struct bloque{
	void* elemento;
	char* clave;
} bloque_t;

struct hash{
	lista_t** vector_listas_hash;
	size_t capacidad;
	size_t cantidad_elementos;
	size_t factor_de_carga;
	hash_destruir_dato_t destructor;
};

// Pre C.: Recibe un puntero al vector de listas, la capacidad inicial del hash y un puntero a una variable
//		   booleana que vierifica el éxito/error del procedimiento.
// Post C.: Crea la listas correspondientes a cada lugar del vector Hash. En caso de un error antes de finalizar, destuye las listas creadas y retorna.
void hash_crear_listas(lista_t** vector_listas_hash, size_t capacidad, bool* hay_lista_nula){
	int i = 0;

	while((i < capacidad) && (!(*hay_lista_nula))){
		vector_listas_hash[i] = lista_crear();

		if(vector_listas_hash[i] == NULL){
			(*hay_lista_nula) = true;

			for(int j = 0; j < i; j++){
				lista_destruir(vector_listas_hash[j]);
			}
		}
		i++;
	}
}

/*
 * Crea el hash reservando la memoria necesaria para él.
 * Destruir_elemento es un destructor que se utilizará para liberar
 * los elementos que se eliminen del hash.
 * Capacidad indica la capacidad minima inicial con la que se crea el hash.
 * Devuelve un puntero al hash creado o NULL en caso de no poder crearlo.
 */
hash_t* hash_crear(hash_destruir_dato_t destruir_cada_elemento, size_t capacidad){
	if(destruir_cada_elemento == NULL){
		return NULL;
	}

	hash_t* hash = malloc(sizeof(hash_t));
	if(hash == NULL){
		return NULL;
	}

	hash->vector_listas_hash = malloc(sizeof(lista_t*) * capacidad);
	if((hash->vector_listas_hash) == NULL){
		free(hash);
		return NULL;
	}

	bool hay_lista_nula = false;
	hash_crear_listas(hash->vector_listas_hash, capacidad, &(hay_lista_nula));

	if(hay_lista_nula){
		free(hash->vector_listas_hash);
		free(hash);
		return NULL;
	}

	hash->capacidad = capacidad;
	hash->cantidad_elementos = 0;
	hash->factor_de_carga = 0;
	hash->destructor = destruir_cada_elemento;

	return hash;
}

// Pre C.: Recibe un puntero al Hash.
// Post C.: Devuelve 'true' si la estructura o el vector Hash son nulas.
bool hay_error_hash(hash_t* hash){
	return((hash == NULL) || (hash->vector_listas_hash == NULL));
}

// Pre C.: Recibe un puntero al Hash.
// Post C.: Devuelve 'true' si la la cantidad de elementos del Hash es igual a '0'.
bool hash_vacio(hash_t* hash){
	return(hash->cantidad_elementos == 0);
}

// Pre C.: Recibe la clave sobre la que se va a calcular el valor de Hash.
// Post C.: Devuelve el valor de Hash correspondiente a la clave recibida.
size_t hashear(const char* clave, size_t capacidad){
	size_t valor_clave = 0;

	for(int i = 0; i < strlen(clave); i++){
		valor_clave += (size_t)clave[i];
	}

	return (valor_clave % capacidad);
}

// Pre C.: Recibe un puntero al Hash.
// Post C.: Devuelve el valor de Hash correspondiente a la clave recibida.
/*int rehashear(hash_t* hash){
	if(hay_error_hash(hash)){
		return ERROR;
	}

	lista_t** vector_hash_aux = realloc(hash->vector_listas_hash, sizeof(lista_t*) * hash->capacidad * COEFICIENTE_REHASH);
	if(vector_hash_aux == NULL){
		return ERROR;
	}

	hash->vector_listas_hash = vector_hash_aux;
}*/

/*
 * Inserta un elemento reservando la memoria necesaria para el mismo.
 * Devuelve 0 si pudo guardarlo o -1 si no pudo.
 */
int hash_insertar(hash_t* hash, const char* clave, void* elemento){
	if(hay_error_hash(hash)){
		return ERROR;
	}

	bloque_t* bloque = malloc(sizeof(bloque_t));
	if(bloque == NULL){
		return ERROR;
	}

	int estado = ERROR;
	bloque->clave = (char*)clave;
	bloque->elemento = elemento;

	/*if((hash->factor_de_carga) >= FACTOR_MAX){
		if(rehashear(hash) == ERROR){
			(hash->destructor)(bloque->elemento);
			free(bloque);
			return ERROR;
		}
	}*/

	size_t posicion_hash = hashear(clave, hash->capacidad);
	lista_t* lista = hash->vector_listas_hash[posicion_hash];

	if(hash_contiene(hash, clave)){
		hash_quitar(hash, clave);
	}

	if(lista_insertar(lista, bloque) == EXITO){
		estado = EXITO;
		(hash->cantidad_elementos)++;
		hash->factor_de_carga = hash->cantidad_elementos / hash->capacidad;
	}

	return estado;
}

/*
 * Quita un elemento del hash e invoca la funcion destructora
 * pasandole dicho elemento.
 * Devuelve 0 si pudo eliminar el elemento o -1 si no pudo.
 */
int hash_quitar(hash_t* hash, const char* clave){
	if((hay_error_hash(hash)) || (hash_vacio(hash))){
		return ERROR;
	}

	size_t posicion_hash = hashear(clave, hash->capacidad);
	lista_t* lista = hash->vector_listas_hash[posicion_hash];
	
	int estado = ERROR;
	size_t contador_posicion = 0;
	bool bloque_borrado = false;
	bloque_t* bloque_aux = NULL;

	lista_iterador_t* iterador = lista_iterador_crear(lista);
	if(iterador == NULL){
		return ERROR;
	}

	while((!bloque_borrado) && (lista_iterador_tiene_siguiente(iterador))){
		bloque_aux = lista_iterador_siguiente(iterador);

		if(strcmp(bloque_aux->clave, clave) == 0){
			(hash->destructor)(bloque_aux->elemento);
			free(bloque_aux);
			estado = lista_borrar_de_posicion(lista, contador_posicion);
			bloque_borrado = true;
		}
		contador_posicion++;
	}

	lista_iterador_destruir(iterador);
	(hash->cantidad_elementos)--;

	return estado;
}

/*
 * Devuelve un elemento del hash con la clave dada o NULL si dicho
 * elemento no existe.
 */
void* hash_obtener(hash_t* hash, const char* clave){
	if((hay_error_hash(hash)) || (hash_vacio(hash))){
		return NULL;
	}

	size_t posicion_hash = hashear(clave, hash->capacidad);
	lista_t* lista = hash->vector_listas_hash[posicion_hash];

	bool elemento_encontrado = false;
	void* elemento_buscado = NULL;
	bloque_t* bloque_aux = NULL;

	lista_iterador_t* iterador = lista_iterador_crear(lista);
	if(iterador == NULL){
		return NULL;
	}

	while((lista_iterador_tiene_siguiente(iterador)) && (!elemento_encontrado)){
		bloque_aux = lista_iterador_siguiente(iterador);

		if(strcmp(bloque_aux->clave, clave) == 0){
			elemento_buscado = bloque_aux->elemento;
			elemento_encontrado = true;
		}
	}

	lista_iterador_destruir(iterador);

	return elemento_buscado;
}

/*
 * Devuelve true si el hash contiene un elemento almacenado con la
 * clave dada o false en caso contrario.
 */
bool hash_contiene(hash_t* hash, const char* clave){
	if((hay_error_hash(hash)) || (hash_vacio(hash))){
		return false;
	}

	size_t posicion_hash = hashear(clave, hash->capacidad);
	lista_t* lista = hash->vector_listas_hash[posicion_hash];

	bool elemento_encontrado = false;
	bloque_t* bloque_aux = NULL;

	lista_iterador_t* iterador = lista_iterador_crear(lista);
	if(iterador == NULL){
		return ERROR;
	}

	while((lista_iterador_tiene_siguiente(iterador)) && (!elemento_encontrado)){
		bloque_aux = lista_iterador_siguiente(iterador);

		if(strcmp(bloque_aux->clave, clave) == 0){
			elemento_encontrado = true;
		}
	}
	
	lista_iterador_destruir(iterador);

	return elemento_encontrado;
}

/*
 * Devuelve la cantidad de elementos almacenados en el hash.
 */
size_t hash_cantidad(hash_t* hash){
	if((hay_error_hash(hash)) || (hash_vacio(hash))){
		return 0;
	}

	return (hash->cantidad_elementos);
}

// Pre C.: Recibe un puntero al Hash.
// Post C.: Invoca a la función destructora con cada elemento del Hash, utilizando el iterador externo de la lista.
int destruir_cada_elemento(hash_t* hash){
	if(hay_error_hash(hash)){
		return ERROR;
	}

	int estado = EXITO;
	bloque_t* bloque = NULL;
	lista_t* lista = NULL;

	for(int i = 0; i < (hash->capacidad); i++){
		lista = hash->vector_listas_hash[i];
		
		while((estado == EXITO) && (!lista_vacia(lista))){
			
			lista_iterador_t* iterador = lista_iterador_crear(lista);
			if(iterador == NULL){
				estado = ERROR;
			}

			bloque = lista_iterador_siguiente(iterador);

			if(bloque != NULL){
				(hash->destructor)(bloque->elemento);
				free(bloque);
				(hash->cantidad_elementos)--;
				lista_borrar_de_posicion(lista, 0);
			}

			lista_iterador_destruir(iterador);
		}
	}

	return estado;
}

/*
 * Destruye el hash liberando la memoria reservada y asegurandose de
 * invocar la funcion destructora con cada elemento almacenado en el
 * hash.
 */
void hash_destruir(hash_t* hash){
	if(hay_error_hash(hash)){
		return;
	}

	if(destruir_cada_elemento(hash) == EXITO){
		for(int i = 0; i < (hash->capacidad); i++){
			lista_destruir(hash->vector_listas_hash[i]);
		}
	}

	free(hash->vector_listas_hash);
	free(hash);
}



/* ------------------------------------------------------------------------------------------------------------------------------------------------ */



/* Iterador externo para el HASH */
struct hash_iter{
	hash_t* hash;
	size_t cantidad_listas_por_recorrer;
	size_t lista_corriente;
	lista_iterador_t* lista_iterador;
};

// Pre C.: Recibe la estructura del iterador externo.
// Post C.: Devuelve 'true' si la lista a la que apunta el iterador es nula y 'false' en caso contrario.
bool hay_error_iterador_hash(hash_iterador_t* iterador){
	return ((iterador == NULL) || (iterador->hash->vector_listas_hash == NULL) || (iterador->lista_iterador == NULL));
}

/*
 * Crea un iterador de claves para el hash reservando la memoria
 * necesaria para el mismo. El iterador creado es válido desde su
 * creación hasta que se modifique la tabla de hash (insertando o
 * removiendo elementos).
 *
 * Devuelve el puntero al iterador creado o NULL en caso de error.
 */
hash_iterador_t* hash_iterador_crear(hash_t* hash){
	if(hay_error_hash(hash)){
		return NULL;
	}

	hash_iterador_t* iterador = malloc(sizeof(hash_iterador_t));
	if(iterador == NULL){
		return NULL;
	}

	iterador->lista_corriente = 0;
	iterador->cantidad_listas_por_recorrer = (hash->capacidad)-1;
	iterador->hash = hash;
	
	iterador->lista_iterador = lista_iterador_crear(iterador->hash->vector_listas_hash[iterador->lista_corriente]);
	if((iterador->lista_iterador) == NULL){
		return NULL;
	}

	return iterador;
}

/*
 * Devuelve la próxima clave almacenada en el hash y avanza el iterador.
 * Devuelve la clave o NULL si no habia mas.
 */
void* hash_iterador_siguiente(hash_iterador_t* iterador){
	if(hay_error_iterador_hash(iterador) || (!hash_iterador_tiene_siguiente(iterador))){
		return NULL;
	}

	bloque_t* bloque_aux = NULL;

	if(lista_iterador_tiene_siguiente(iterador->lista_iterador)){
		bloque_aux = lista_iterador_siguiente(iterador->lista_iterador);
	}
	else if((!lista_iterador_tiene_siguiente(iterador->lista_iterador)) && (iterador->cantidad_listas_por_recorrer > 0)){
		lista_iterador_destruir(iterador->lista_iterador);

		(iterador->lista_corriente)++;
		(iterador->cantidad_listas_por_recorrer)--;

		iterador->lista_iterador = lista_iterador_crear(iterador->hash->vector_listas_hash[iterador->lista_corriente]);
		if((iterador->lista_iterador) == NULL){
			return NULL;
		}
		bloque_aux = lista_iterador_siguiente(iterador->lista_iterador);
	}
	
	void* clave = (void*)(bloque_aux->clave);

	return clave;
}

/*
 * Devuelve true si quedan claves por recorrer o false en caso
 * contrario.
 */
bool hash_iterador_tiene_siguiente(hash_iterador_t* iterador){
	if((hay_error_iterador_hash(iterador)) || ((iterador->cantidad_listas_por_recorrer) == 0)){
		return false;
	}

	if(lista_iterador_tiene_siguiente(iterador->lista_iterador)){
		return true;
	}

	if((!lista_iterador_tiene_siguiente(iterador->lista_iterador)) && (iterador->cantidad_listas_por_recorrer > 0)){
		bool tiene_siguiente = false;
		size_t lista_corriente = iterador->lista_corriente;
		lista_t* proxima_lista = iterador->hash->vector_listas_hash[lista_corriente + 1];

		while((!tiene_siguiente) && (lista_corriente <= (iterador->hash->capacidad)-1)){
			tiene_siguiente = !lista_vacia(proxima_lista);
			lista_corriente++;
			proxima_lista = iterador->hash->vector_listas_hash[lista_corriente + 1];
		}
		
		return tiene_siguiente;
	}

	return (iterador->cantidad_listas_por_recorrer > 0);
}

/*
 * Destruye el iterador del hash.
 */
void hash_iterador_destruir(hash_iterador_t* iterador){
	if(hay_error_iterador_hash(iterador)){
		return;
	}

	lista_iterador_destruir(iterador->lista_iterador);
	free(iterador);
}
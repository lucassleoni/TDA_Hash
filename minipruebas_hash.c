#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "hash_iterador.h"

#define EXITO  0
#define ERROR -1

typedef struct bloque{
    void* elemento;
    char* clave;
} bloque_t;

// Necesario porque strdup es POSIX pero no C99
extern char* strdup(const char*);

void destruir_string(void* elemento){
    if(elemento){
        //printf("(Destructor) Libero el vehiculo: %s\n", (char*)elemento);
        free(elemento);
    }
}

void guardar_vehiculo(hash_t* garage, const char* patente, const char* descripcion){
    int retorno = hash_insertar(garage, patente, strdup(descripcion));
    printf("Guardando vehiculo patente %s (%s): ", patente, descripcion);
    printf("%s\n", retorno == 0?"OK":"ERROR");
}

void quitar_vehiculo(hash_t* garage, const char* patente){
    int retorno = hash_quitar(garage, patente);
    printf("Retirando vehiculo patente %s: ", patente);
    printf("%s\n", retorno == 0?"OK":"ERROR");
}

void verificar_vehiculo(hash_t* garage, const char* patente, bool deberia_existir){
    printf("Verifico el vehiculo patente %s: ", patente);
    bool retorno = hash_contiene(garage, patente);
    printf("%s\n", (retorno == deberia_existir)?"OK":"ERROR");
}

void ejecutar_caso_feliz(){
    hash_t* garage = hash_crear(destruir_string, 5);

    printf("Agrego autos al garage\n");

    guardar_vehiculo(garage, "AC123BD", "Auto de Mariano");
    guardar_vehiculo(garage, "OPQ976", "Auto de Lucas");
    guardar_vehiculo(garage, "A421ACB", "Bici de Manu");
    guardar_vehiculo(garage, "AA442CD", "Auto de Guido");
    guardar_vehiculo(garage, "AC152AD", "Auto de Agustina");
    guardar_vehiculo(garage, "DZE443", "Auto de Jonathan");
    guardar_vehiculo(garage, "AA436BA", "Auto de Gonzalo");
    guardar_vehiculo(garage, "QDM443", "Auto de Daniela");
    guardar_vehiculo(garage, "BD123AC", "Auto de Pablo");
    guardar_vehiculo(garage, "CD442AA", "Auto de Micaela");
    guardar_vehiculo(garage, "PQO697", "Auto de Juan");
    guardar_vehiculo(garage, "DZE443", "Auto de Jonathan otra vez");
    guardar_vehiculo(garage, "AC152AD", "Auto de Agustina otra vez");

    verificar_vehiculo(garage, "QDM443", true);
    verificar_vehiculo(garage, "PQO697", true);

    quitar_vehiculo(garage, "QDM443");
    quitar_vehiculo(garage, "PQO697");

    verificar_vehiculo(garage, "QDM443", false);
    verificar_vehiculo(garage, "PQO697", false);

    hash_iterador_t* iter = hash_iterador_crear(garage);
    size_t listados = 0;

    while(hash_iterador_tiene_siguiente(iter)){
        const char* clave = hash_iterador_siguiente(iter);
        if(clave){
            listados++;
            printf("Patente: %s -- Vehiculo: %s\n", clave, (char*)hash_obtener(garage, clave));
        }
    }

    printf("Cantidad de autos guardados: %zu. Cantidad de autos listados: %zu -- %s\n", hash_cantidad(garage), listados, (hash_cantidad(garage)==listados)?"OK":"ERROR");


    hash_iterador_destruir(iter);
    hash_destruir(garage);

    printf("\n");
}

// Pre C.: ---
// Post C.: Intenta eliminar un elemento de un Hash vacío y verifica si esto devuelve NULL, imprimiendo el resultado de la prueba por pantalla.
void con_hash_vacio_hash_quitar_deberia_devolver_error(){
    hash_t* garage = hash_crear(destruir_string, 5);

    printf("Prueba #1: Con Hash vacío, hash_quitar debería devolver error.\n");

    char* clave = "AD578GS";
    int estado = hash_quitar(garage, clave);

    printf("\t%s: Si el Hash está vacío, hash_quitar %s error. \n",
        (estado == ERROR)?"EXITO":"FALLO",
        (estado == ERROR)?"devuelve":"no devuelve");

    hash_destruir(garage);
}

// Pre C.: ---
// Post C.: Inserta una cantidad de elementos que supere el factor máximo de carga y verifica si se modifica la capacidad
//          (conservando los elementos), imprimiendo el resultado de la prueba por pantalla.
void sobrepasar_el_factor_maximo_de_carga_deberia_rehashear_y_conservar_los_elementos(){
    hash_t* garage = hash_crear(destruir_string, 5);

    printf("\nPrueba #2: Al sobrepasar el factor de carga, hash_insertar debería rehashear (verificar rehasheo por pantalla).\n");

    char* claves[16] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P"};
    char* elementos[16] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"};

    for(int i = 0; i < 16; i++){
        hash_insertar(garage, claves[i], strdup(elementos[i]));
    }

    int cantidad_coincidencias = 0;
    for(int i = 0; i < 16; i++){
        if(strcmp(hash_obtener(garage, claves[0]), elementos[0]) == 0){
            cantidad_coincidencias++;
        }
    }

    printf("\t%s: Si sobrepaso el factor máximo de carga, la función hash_insertar %s (y todos los elementos quedan presentes en el Hash). \n",
          (cantidad_coincidencias == hash_cantidad(garage))?"EXITO":"FALLO",
          (cantidad_coincidencias == hash_cantidad(garage))?"rehashea":"no rehashea");

    hash_destruir(garage);
}

// Pre C.: ---
// Post C.: Envía una clave no existente y verifica que el elemento devuelto sea NULL, imprimiendo el resultado de la prueba por pantalla.
void si_pido_un_elemento_con_clave_no_existente_deberia_devolver_NULL(){
    hash_t* garage = hash_crear(destruir_string, 2);

    printf("\nPrueba #3: Al enviar una clave no presente en el Hash, la función hash_obtener debería devolver NULL.\n");

    hash_insertar(garage, "AC123BD", strdup("Auto de Mariano"));

    void* elemento = hash_obtener(garage, "A421ACB");

    printf("\t%s: Enviando una clave no existente, hash_obtener %s NULL. \n",
        (elemento == NULL)?"EXITO":"FALLO",
        (elemento == NULL)?"devuelve":"no devuelve");

    hash_destruir(garage);
}

// Pre C.: ---
// Post C.: Crea un Hash vacío y verifica que la cantidad de elementos sea '0', imprimiendo el resultado de la prueba por pantalla.
void con_hash_vacio_la_cantidad_de_elementos_deberia_ser_cero(){
    hash_t* garage = hash_crear(destruir_string, 2);
    
    printf("\nPrueba #4: Con Hash vacío, la cantidad de elementos debería ser '0'.\n");

    printf("\t%s: Si el Hash está vacío, la cantidad de elementos %s '0'. \n",
          (hash_cantidad(garage) == 0)?"EXITO":"FALLO",
          (hash_cantidad(garage) == 0)?"es":"no es");

    hash_destruir(garage);
}

// Pre C.: ---
// Post C.: Crea y avanza el iterador externo hasta la última posición válida posible y verifica si avanzar nuevamente devuelve NULL,
//          imprimiendo el resultado de la prueba por pantalla.
void ejecutar_iterador_siguiente_sobre_el_ultimo_elemento_deberia_devolver_NULL(){
    hash_t* garage = hash_crear(destruir_string, 2);
    hash_iterador_t* iterador = hash_iterador_crear(garage);

    printf("\nPrueba #5: Al estar apuntado al último elemento, el iterador no debería tener siguiente.\n");

    hash_insertar(garage, "DZE443", strdup("Auto de Jonathan"));
    hash_insertar(garage, "QDM443", strdup("Auto de Daniela"));
    hash_insertar(garage, "KMW765", strdup("Auto de Charly"));

    while(hash_iterador_tiene_siguiente(iterador)){
        hash_iterador_siguiente(iterador);
    }

    void* elemento = hash_iterador_siguiente(iterador);

    printf("\t%s: Avanzar el iterador sobre el último elemento %s NULL. \n",
        (elemento == NULL)?"EXITO":"FALLO",
        (elemento == NULL)?"devuelve":"no devuelve");

    hash_iterador_destruir(iterador);
    hash_destruir(garage);
}

// Pre C.: ---
// Post C.: Crea un Hash nulo y verifica si al intentar insertar un elemento, la funcion hash_insertar devulve NULL,
//          imprimiendo el resultado de la prueba por pantalla.
void insertar_elemento_sobre_hash_nulo_deberia_devolver_error(){
    hash_t* garage = NULL;

    printf("\nPrueba #6: Con Hash nulo, hash_insertar debería devolver error.\n");

    int estado = hash_insertar(garage, "AC123BD", "Auto de Mariano");
    printf("\t%s: Si el Hash es nulo, hash_insertar %s error. \n",
        (estado == ERROR)?"EXITO":"FALLO",
        (estado == ERROR)?"devuelve":"no devuelve");
}

// Pre C.: ---
// Post C.: Crea un Hash vacío y verifica que el iterador no tenga siguiente, imprimiendo el resultado de la prueba por pantalla.
void con_hash_vacio_el_iterador_externo_no_deberia_tener_siguiente(){
    hash_t* garage = hash_crear(destruir_string, 2);
    hash_iterador_t* iterador = hash_iterador_crear(garage);

    printf("\nPrueba #7: Con Hash vacío, el iterador no debería tener siguiente.\n");

    bool tiene_siguiente = hash_iterador_tiene_siguiente(iterador);

    printf("\t%s: Si el Hash está vacío, el iterador %s siguiente. \n",
        (!tiene_siguiente)?"EXITO":"FALLO",
        (!tiene_siguiente)?"no tiene":"tiene");

    hash_iterador_destruir(iterador);
    hash_destruir(garage);
}

// Pre C.: ---
// Post C.: Verifica que al insertar una clave nula se obtenga el código de error establecido '-1', imprimiendo el resultado de la prueba por pantalla.
void insertar_elemento_con_clave_nula_deberia_devolver_error(){
    hash_t* garage = hash_crear(destruir_string, 2);
    char* clave = NULL;

    printf("\nPrueba #8: Insertar una clave nula debería devolver error.\n");

    int estado = hash_insertar(garage, clave, strdup("Auto de Luke"));

    printf("\t%s: Insertar clave nula %s ERROR. \n",
        (estado == ERROR)?"EXITO":"FALLO",
        (estado == ERROR)?"devuelve":"no devuelve");

    hash_destruir(garage);
}

// Pre C.: ---
// Post C.: Verifica que borrar un elemento no presente en el Hash devuelva error, imprimiendo el resultado de la prueba por pantalla.
void quitar_elemento_no_existente_deberia_devolver_error(){
    hash_t* garage = hash_crear(destruir_string, 2);

    printf("\nPrueba #9: Quitar elemento ya borrado debería devolver error.\n");

    hash_insertar(garage, "DZE443", strdup("Auto de Jonathan"));
    hash_insertar(garage, "QDM443", strdup("Auto de Daniela"));
    hash_insertar(garage, "KMW765", strdup("Auto de Charly"));

    hash_quitar(garage, "QDM443");

    int estado = hash_quitar(garage, "QDM443");

    printf("\t%s: Quitar elemento ya borrado %s ERROR. \n",
        (estado == ERROR)?"EXITO":"FALLO",
        (estado == ERROR)?"devuelve":"no devuelve");

    hash_destruir(garage);
}

// Pre C.: ---
// Post C.: Verifica que insertar un elemento nulo (pero con clave válida) devuelva éxito, imprimiendo el resultado de la prueba por pantalla.
void insertar_elemento_nulo_con_clave_valida_deberia_devolver_exito(){
    hash_t* garage = hash_crear(destruir_string, 2);

    printf("\nPrueba #10: Insertar un elemento nulo con clave válida debería devolver éxito.\n");

    int estado = hash_insertar(garage, "KMW765", NULL);

    printf("\t%s: Insertar elemento nulo con clave válida %s ERROR. \n",
        (estado == EXITO)?"EXITO":"FALLO",
        (estado == EXITO)?"devuelve":"no devuelve");

    hash_destruir(garage);
}


int main(){
    system("clear");

    // Tests provistos por la cátedra:
    ejecutar_caso_feliz();

    // Tests propios:
    con_hash_vacio_hash_quitar_deberia_devolver_error(); // Hash nulo es considerado vacío, por lo que 'vacío' y 'nulo' son equivalentes en esta prueba.
    sobrepasar_el_factor_maximo_de_carga_deberia_rehashear_y_conservar_los_elementos();
    si_pido_un_elemento_con_clave_no_existente_deberia_devolver_NULL();
    con_hash_vacio_la_cantidad_de_elementos_deberia_ser_cero();
    ejecutar_iterador_siguiente_sobre_el_ultimo_elemento_deberia_devolver_NULL();
    insertar_elemento_sobre_hash_nulo_deberia_devolver_error();
    con_hash_vacio_el_iterador_externo_no_deberia_tener_siguiente();
    insertar_elemento_con_clave_nula_deberia_devolver_error();
    quitar_elemento_no_existente_deberia_devolver_error();
    insertar_elemento_nulo_con_clave_valida_deberia_devolver_exito();

    return 0;
}
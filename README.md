TDA Hash (Abierto - Con listas)

La implementación del TDA Hash incluído en 'TDA Hash (Lucas Leoni).zip' posee los siguientes programas:
    1) 'hash.h' es la biblioteca que incluye las firmas de las funciones primitivas para el funcionamiento del Hash.
    2) 'hash_iterador.h' es la biblioteca que incluye las firmas de las funciones primitivas para el funcionamiento del
        iterador externo del Hash.
    3) 'hash.c' es el programa con la implementación del Hash.
    4) 'lista.h' es la biblioteca que incluye las firmas de las funciones primitivas de las listas utilizadas en la
        implementanción del Hash.
    5) 'lista.c' es el programa con la implementación de las funciones halladas en 'lista.h'.
    6) 'minipruebas_hash.c' es el programa que contiene las pruebas que testean el funcionamiento del Hash implementado.
    7) 'makefile' es el programa que facilita el acceso a las líneas de compilación y ejecución del Hash, las pruebas y Valgrind.
    8) 'Consigna' es la consigna del trabajo pŕactico.


Estructuras utilizadas:

1) Bloque: cada bloque posee un puntero 'void*' al elemento que apunta y un puntero a la clave (string) que lo indentifica.
           La estructura 'bloque_t bloque' utilizada es la siguiente:

typedef struct bloque{
	void* elemento;
	char* clave;
} bloque_t;


2) Hash: Hash posee un vector de punteros a las listas sobre las que se insertará cada bloque, junto con 3 variables de tipo
         'size_t' para llevar un recuento de la capacidad (cantidad de listas), cantidad de elementos/bloques y el factor de carga
         (para determinar cuándo rehashear). A su vez, contiene un puntero a la función destructora de elementos.
         La estructura 'hash_t hash' utilizada es la siguiente:

struct hash{
	lista_t** vector_listas_hash;
	size_t capacidad;
	size_t cantidad_elementos;
	size_t factor_de_carga;
	hash_destruir_dato_t destructor;
};


3) Iterador externo: El iterador externo posee punteros al Hash sobre el que se está ultilizando y a un iterador de listas.
                     A su vez, posee la variable 'size_t lista corriente', que indica el número de la lista sobre la que se está iterando
                     (siendo '0' la primera lista del vector de listas del Hash).
                     La estructura 'hash_iterador_t iterador' utilizada es la siguiente:

struct hash_iter{
	hash_t* hash;
	lista_iterador_t* lista_iterador;
	size_t lista_corriente;
};


Funciones del TDA Hash

Aclaración: todas las funciones utilizadas verifican (previo a ser ejecutadas) que no haya ningún error,
            (ver 'Funciones Propias > Verificación de errores generales').

1) Creación del Hash ---> hash_t* hash_crear(hash_destruir_dato_t destruir_elemento, size_t capacidad);
     Crea la estructra 'hash_t hash'.
     Devuelve un puntero a la estructura 'hash_t hash' y reserva la memoria necesaria para su almacenamiento en el Heap.
     La variable 'capacidad' determina cuántas listas serán inicializadas al crear el Hash.
     Se asigna el puntero a la función destructora y se inicializan en '0' la cantidad de elementos y el factor de carga.
     Devuelve NULL en caso de haber habido un error al crear la estructura.

2) Insertar elementos ---> int hash_insertar(hash_t* hash, const char* clave, void* elemento);
     Inserta el bloque/elemento recibido en la lista correspondiente según el valor de hasheo obtenido por medio de la clave,
     reservando la memoria necesaria para el mismo.
     En caso de ser igualado (o superado) el factor de carga, se invoca a la funcion de rehash.
     En caso de ya exisitir la clave recibida, se reemplaza el elemento existente por el nuevo.
     Aumenta la cantidad de elementos del Hash y actualiza el factor de carga.
     Devuelve '0' si la operación tuvo éxito y '-1' si falló.

3) Borrar elementos ---> int hash_quitar(hash_t* hash, const char* clave);
     Borra el elemento correspondiente a la clave recibida, liberando la memoria ocupada por el nodo borrado.
     Disminuye la cantidad de elementos del Hash.
     Devuelve '0' si la operación tuvo éxito y '-1' si falló.

4) Obtener elemento con una clave determinada ---> void* hash_obtener(hash_t* hash, const char* clave);
     Devuelve un puntero al elemento correspondiente a la clave indicada.
     Si la clave no existe o el Hash está vacío/es nulo, devuelve NULL.

5) Elemento contenido ---> bool hash_contiene(hash_t* hash, const char* clave);
     Devuelve 'true' si existe un elemento correspondiente a la clave recibida.
     Si la clave no existe o el Hash está vacío/es nulo, devuelve 'false'.

6) Cantidad de elementos ---> size_t hash_cantidad(hash_t* hash);
     Devuelve la cantidad de elementos/nodos presentes en el Hash.
     Si el Hash está vacío/es nulo, devuelve '0'.

7) Destrucción del Hash ---> void hash_destruir(hash_t* hash);
     En caso de no estar vacío, borra todos los nodos del Hash (invocando a la función destructora con cada elemento)
     hasta que no tenga ninguno, liberando cada lista y luego el vector_hash.
     Finalmente, destruye la estructura 'hash_t hash' (liberando la memoria ocupada por ella).


Funciones primitivas del TDA iterador

1) Crear iterador ---> hash_iterador_t* hash_iterador_crear(hash_t* hash);
     Crea la estructura 'hash_iterador_t iterador', inicializando sus campos y asociándolo a la primera lista del vector_hash.
     El iterador externo es válido desde su creación hasta que se modifique la tabla de hash.
     Devuelve un puntero a la estructura 'hash_iterador_t iterador' y reserva la memoria necesaria para su almacenamiento en el Heap.
     Devuelve NULL en caso de haber habido un error al crear el iterador.

2) Verificar si hay nodo siguiente ---> bool hash_iterador_tiene_siguiente(hash_iterador_t* iterador);
     Determina si hay un elemento (presente en la lista) luego del que está siendo apuntado por el iterador al momento ser invocada esta función.
     Si el iterador no comenzó a recorrer la primera lista (y la misma no está vacía), el elemento siguiente será el nodo inicial.
     Si el iterador no comenzó a recorrer la primera lista (y la misma está vacía), el elemento siguiente será el primer elemento
     de la próxima no vacía (en caso que haya).
     Si hay listas vacías en el medio, la función verifica si queda alguna próxima lista no vacía para devolver 'true' en ese caso.
     Devuelve 'true' si hay nodo siguiente, y 'false' en cualquier otro caso.

3) Avanzar un elemento ---> void* hash_iterador_siguiente(hash_iterador_t* iterador);
     Avanza el iterador al próximo elemento y lo devuelve.
     Devuelve NULL si la lista a la que apunta el iterador no existe/está vacía o si es invocada cuando ya no quedan elementos por recorrer.

4) Destruir iterador ---> void hash_iterador_destruir(hash_iterador_t* iterador);
     Libera el 'lista_iterador_t' y la memoria reservada por el iterador del Hash.


Funciones Propias (para modularizar la implementacíon de la lista/iterador)

1) Verificación de errores generales (Hash) ---> bool hay_error_hash(hash_t* hash)
     Devuelve 'true' si:
        1.1) La estructura 'hash_t hash' es nula.
        1.2) El vector de listas del Hash es nulo.

2) Creación de listas ---> void hash_crear_listas(lista_t** vector_listas_hash, size_t capacidad, bool* hay_lista_nula);
     Crea la listas correspondientes a cada lugar del vector Hash.
     En caso de haber un error antes de finalizar, destruye las listas creadas y retorna.

3) Hash vacío ---> bool hash_vacio(hash_t* hash);
     Devuelve 'true' si la cantidad de elemementos es '0' o si hay algún error en el Hash.

4) Función hash ---> size_t hashear(const char* clave, size_t capacidad);
     Devuelve el valor de Hash (posición del vector de listas del Hash) correspondiente a la clave recibida.

5) Guardar copia de los elementos (para Rehash) ---> void guardar_elementos(hash_t* hash, bloque_t* vector[], int* posicion_elemento)
     Almacena todos los elementos del Hash en un vector estático.

6) Rehash ---> int rehashear(hash_t* hash);
     Redimensiona el vector_listas_hash, actualizando la capacidad del Hash y volviendo a insertar todos los elementos.

7) Destrucción de cada elemento (para hash_destruir) ---> int destruir_cada_elemento(hash_t* hash);
     Invoca a la función destructora con cada elemento del Hash, utilizando el iterador externo de la lista.

8) Buscar bloque por clave ---> bloque_t* buscar_nodo_por_clave(lista_t* lista, const char* clave, size_t* contador_posicion);
     Devuelve un puntero al bloque con la clave recibida.

9) Verificación de errores generales (iterador) ---> bool hay_error_iterador_hash(hash_iterador_t* iterador);
     Devuelve 'true' si la lista a la que apunta el iterador es nula.


Compilación y Convenciones

1) Tamaño del Hash
     Inicialmente, el Hash se creará con la capacidad (cantidad de listas) solicitada por el usuario.

2) Makefile
   La carpeta cuenta con un programa 'makefile' que posee los siguientes comandos:
     3.1) 'make compile' compilará el programa 'hash.c' junto con las pruebas que testean su funcionamiento, ejecutando la línea de compilación:
              gcc -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O0 lista.c minipruebas_lista.c -o hash -g

     3.2) 'make hash' compilará el programa con la línea mencionada en el punto (3.1) y luego ejecutará las pruebas (incluídas en la carpeta) con la línea:
              ./hash

     3.2) 'make valgrind' ejecutrá Valgrind (mostrando los resultados de las pruebas por pantalla) con la línea:
              valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./hash

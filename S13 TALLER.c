#include <stdio.h>
#include <string.h>

// Definiciones de archivos
#define FILE_INV "inventario_ruedas.txt"
#define FILE_VENTAS "registro_ventas.txt"

// Estructura principal
struct Auto {
    int id;
    char marca[30];
    char tipo[30];
    char estado[15];
    float precio;
    int es_vendido;     // 0 = disponible, 1 = vendido
};

// Prototipos
void agregar_carro();
void ver_todo();
void buscar_especifico();
void realizar_venta();
void borrar_todo();

// Funciones auxiliares
void pedir_texto(char *destino, const char *mensaje);
float pedir_dinero(const char *mensaje);

int main() {
    int opc;
    
    do {
        printf("\nRUEDAS DE ORO\n");
        printf("1. Ingresar nuevo vehiculo\n");
        printf("2. Ver lista de vehiculos\n");
        printf("3. Buscar (Caso Ruben)\n");
        printf("4. Vender vehiculo\n");
        printf("5. Reiniciar archivos (Borrar todo)\n");
        printf("0. Salir\n");
        printf("Elige una opcion: ");
        
        // Validacion del menu
        if (scanf("%d", &opc) != 1) {
            printf("Opcion no valida.\n");
            while(getchar() != '\n'); // Limpiar buffer
            opc = -1; 
        } else {
            switch(opc) {
                case 1: agregar_carro(); 
                break;
                case 2: ver_todo(); 
                break;
                case 3: buscar_especifico(); 
                break;
                case 4: realizar_venta(); 
                break;
                case 5: borrar_todo(); 
                break;
                case 0: printf("Cerrando programa\n"); 
                break;
                default: printf("Esa opcion no existe.\n");
            }
        }
    } while(opc != 0);

    return 0;
}

void pedir_texto(char *destino, const char *mensaje) {
    int es_valido;
    int i; 
    
    do {
        es_valido = 1;
        printf("%s", mensaje);
        scanf("%s", destino);
        
        for(i = 0; i < strlen(destino); i++) {
            if(destino[i] >= '0' && destino[i] <= '9') {
                es_valido = 0;
                printf("Error: No puedes poner numeros aqui.\n");
                break;
            }
        }
    } while(es_valido == 0);
}

float pedir_dinero(const char *mensaje) {
    float valor;
    int leido;
    do {
        printf("%s", mensaje);
        leido = scanf("%f", &valor);
        if(leido != 1 || valor < 0) {
            printf("Error: Ingresa un monto valido (positivo).\n");
            while(getchar() != '\n'); 
            valor = -1; 
        }
    } while(valor < 0);
    return valor;
}

void agregar_carro() {
    FILE *fp = fopen(FILE_INV, "a");
    if(fp == NULL) {
        printf("Error con el archivo.\n");
        return;
    }
    
    struct Auto c;
    int estado_valido = 0;
    c.es_vendido = 0; 
    
    printf("\nNuevo Ingreso\n");
    
    printf("ID unico: ");
    while(scanf("%d", &c.id) != 1 || c.id < 0) {
        printf("ID invalido. Intenta de nuevo: ");
        while(getchar() != '\n');
    }

    pedir_texto(c.marca, "Marca: ");
    pedir_texto(c.tipo, "Tipo (ej. Camioneta): ");
    
    // Validacion de estado
    do {
        pedir_texto(c.estado, "Condicion (Nuevo/Usado): ");
        
        if (strcmp(c.estado, "Nuevo") == 0 || strcmp(c.estado, "nuevo") == 0 ||
            strcmp(c.estado, "Usado") == 0 || strcmp(c.estado, "usado") == 0) {
            estado_valido = 1; 
        } else {
            printf("Error: Solo se permite Nuevo o Usado. Intentar otra vez.\n");
            estado_valido = 0;
        }
    } while (estado_valido == 0);

    c.precio = pedir_dinero("Precio ($): ");
    
    fprintf(fp, "%d %s %s %s %.2f %d\n", c.id, c.marca, c.tipo, c.estado, c.precio, c.es_vendido);
    
    fclose(fp);
    printf("Guardado exitosamente.\n");
}

void ver_todo() {
    FILE *fp = fopen(FILE_INV, "r");
    struct Auto c;
    
    if(fp == NULL) {
        printf("Inventario vacio.\n");
        return;
    }
    
    printf("\nID\tMarca\t\tTipo\t\tPrecio\t\tEstado\n");
    printf("------------------------------------------------------------\n");
    
    while(fscanf(fp, "%d %s %s %s %f %d", &c.id, c.marca, c.tipo, c.estado, &c.precio, &c.es_vendido) != EOF) {
        printf("%d\t%-10s\t%-10s\t$%.2f\t%s\n", 
            c.id, c.marca, c.tipo, c.precio, 
            (c.es_vendido == 1 ? "VENDIDO" : "Disponible"));
    }
    fclose(fp);
}

void buscar_especifico() {
    FILE *fp = fopen(FILE_INV, "r");
    struct Auto c;
    char busqueda_marca[30];
    float max_presupuesto;
    int contador = 0;
    
    if(fp == NULL) return;
    
    printf("\nBusqueda (Caso Ruben)\n");
    pedir_texto(busqueda_marca, "Marca preferida: ");
    max_presupuesto = pedir_dinero("Presupuesto maximo: ");
    
    printf("\nResultados:\n");
    while(fscanf(fp, "%d %s %s %s %f %d", &c.id, c.marca, c.tipo, c.estado, &c.precio, &c.es_vendido) != EOF) {
        if(strcmp(c.marca, busqueda_marca) == 0 && c.precio <= max_presupuesto && c.es_vendido == 0) {
            printf("- [ID: %d] %s %s (%s) a $%.2f\n", c.id, c.marca, c.tipo, c.estado, c.precio);
            contador++;
        }
    }
    
    if(contador == 0) printf("No se encontraron vehiculos disponibles con esos datos.\n");
    fclose(fp);
}

void realizar_venta() {
    FILE *fp = fopen(FILE_INV, "r");
    FILE *temp = fopen("temp.txt", "w");
    FILE *ventas = fopen(FILE_VENTAS, "a");
    
    struct Auto c;
    int id_venta, encontrado = 0;
    char cliente[50];
    
    if(fp == NULL) {
        printf("No hay datos.\n");
        return;
    }
    
    printf("ID del vehiculo a vender: ");
    scanf("%d", &id_venta);
    
    while(fscanf(fp, "%d %s %s %s %f %d", &c.id, c.marca, c.tipo, c.estado, &c.precio, &c.es_vendido) != EOF) {
        if(c.id == id_venta && c.es_vendido == 0) {
            c.es_vendido = 1; 
            encontrado = 1;
            
            pedir_texto(cliente, "Nombre del cliente: ");
            
            fprintf(ventas, "Cliente: %s compro ID %d (%s) por $%.2f\n", cliente, c.id, c.marca, c.precio);
            printf("Venta exitosa.\n");
        }
        fprintf(temp, "%d %s %s %s %.2f %d\n", c.id, c.marca, c.tipo, c.estado, c.precio, c.es_vendido);
    }
    
    fclose(fp);
    fclose(temp);
    fclose(ventas);
    
    remove(FILE_INV);
    rename("temp.txt", FILE_INV);
    
    if(!encontrado) printf("ID no encontrado o ya vendido.\n");
}

void borrar_todo() {
    char confirmacion;
    printf("¿Borrar todo el inventario? (s/n): ");
    scanf(" %c", &confirmacion); 
    
    if(confirmacion == 's' || confirmacion == 'S') {
        FILE *fp = fopen(FILE_INV, "w"); 
        if(fp != NULL) fclose(fp);
        printf("Archivos reiniciados.\n");
    }
}
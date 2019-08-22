#include <stdio.h>
#include <string.h>
#include "var/globals.h"
#include "interpreter.h"

/**
 * @brief Método principal encargado de recibir las peticiones del usuario
 * 
 * @return int 
 */
int main() 
{
    initDisksMount();
    clearValues();
    printf("---> Disk Parted <---\n");
    char input[999] = "";
    int exit = 0;
    while (exit >= 0)
    {
        fgets(input, 999, stdin);
        exit = loadCommand(input);
    }
    
    return 0;
}